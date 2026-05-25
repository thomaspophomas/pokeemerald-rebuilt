#include "global.h"
#include "money.h"
#include "multiplayer/trade.h"
#include "multiplayer/commit.h"
#include "multiplayer/session.h"
#include "multiplayer/transport.h"

#if FEATURE_MULTIPLAYER
static EWRAM_DATA bool8 sPendingItemTrade = FALSE;
static EWRAM_DATA u8 sPendingTradePlayerA = NET_PLAYER_NONE;
static EWRAM_DATA u8 sPendingTradePlayerB = NET_PLAYER_NONE;
static EWRAM_DATA u8 sPendingTradeSubsessionId = NET_SUBSESSION_NONE;
static EWRAM_DATA u32 sPendingTradeMoneyAmount = 0;
static EWRAM_DATA u32 sPendingTradeTransactionId = 0;
static EWRAM_DATA u32 sLastRemoteTradeTransactionIds[MAX_NET_PLAYERS] = {0};

static void ClearPendingItemTrade(void)
{
    sPendingItemTrade = FALSE;
    sPendingTradePlayerA = NET_PLAYER_NONE;
    sPendingTradePlayerB = NET_PLAYER_NONE;
    sPendingTradeSubsessionId = NET_SUBSESSION_NONE;
    sPendingTradeMoneyAmount = 0;
    sPendingTradeTransactionId = 0;
}

static bool8 SubsessionIncludesPlayer(const struct MultiplayerSubsession *subsession, u8 player_id)
{
    u8 player_index_in_subsession;

    if (subsession == NULL || !subsession->active || player_id >= MAX_NET_PLAYERS)
        return FALSE;

    for (player_index_in_subsession = 0; player_index_in_subsession < subsession->player_count; player_index_in_subsession++)
    {
        if (subsession->players[player_index_in_subsession] == player_id)
            return TRUE;
    }

    return FALSE;
}

static bool8 SubsessionMatchesPendingTrade(const struct MultiplayerSubsession *subsession, u8 local_player_id)
{
    if (subsession == NULL || !subsession->active)
        return FALSE;
    if (subsession->type != MULTIPLAYER_SUBSESSION_TRADE)
        return FALSE;
    if (subsession->state != MULTIPLAYER_SUBSESSION_STATE_INVITING
     && subsession->state != MULTIPLAYER_SUBSESSION_STATE_READY
     && subsession->state != MULTIPLAYER_SUBSESSION_STATE_ACTIVE)
        return FALSE;
    if (!SubsessionIncludesPlayer(subsession, local_player_id))
        return FALSE;
    if (!SubsessionIncludesPlayer(subsession, sPendingTradePlayerA)
     || !SubsessionIncludesPlayer(subsession, sPendingTradePlayerB))
        return FALSE;

    return TRUE;
}

static const struct MultiplayerSubsession *FindPendingTradeSubsession(const struct MultiplayerSession *session)
{
    u8 subsession_index;

    if (session == NULL || session->local_player_id >= MAX_NET_PLAYERS)
        return NULL;

    for (subsession_index = 0; subsession_index < MAX_NET_SUBSESSIONS; subsession_index++)
    {
        const struct MultiplayerSubsession *subsession = &session->subsessions[subsession_index];

        if (SubsessionMatchesPendingTrade(subsession, session->local_player_id))
            return subsession;
    }

    return NULL;
}
#endif

bool8 MultiplayerTrade_CanStart(const struct MultiplayerTradeRequest *request)
{
#if FEATURE_MULTIPLAYER
    u8 players[MAX_NET_TRADE_PLAYERS];

    if (request == NULL)
        return FALSE;
    if (!MultiplayerSession_IsOnline())
        return FALSE;
    if (request->player_a >= MAX_NET_PLAYERS || request->player_b >= MAX_NET_PLAYERS)
        return FALSE;
    if (request->player_a == request->player_b)
        return FALSE;
    if (MultiplayerSession_IsPlayerBusy(request->player_a) || MultiplayerSession_IsPlayerBusy(request->player_b))
        return FALSE;

    players[0] = request->player_a;
    players[1] = request->player_b;
    if (!MultiplayerSession_ArePlayersWithinRange(MAX_NET_TRADE_PLAYERS, players, NET_PLAYER_INTERACTION_RANGE_TILES))
        return FALSE;

    return TRUE;
#else
    (void)request;
    return FALSE;
#endif
}

bool8 MultiplayerTrade_Start(const struct MultiplayerTradeRequest *request)
{
#if FEATURE_MULTIPLAYER
    u8 players[MAX_NET_TRADE_PLAYERS];

    if (!MultiplayerTrade_CanStart(request))
        return FALSE;

    players[0] = request->player_a;
    players[1] = request->player_b;
    return MultiplayerSession_StartSubsession(MULTIPLAYER_SUBSESSION_TRADE, MAX_NET_TRADE_PLAYERS, players);
#else
    return FALSE;
#endif
}

void MultiplayerTrade_Reset(void)
{
#if FEATURE_MULTIPLAYER
    ClearPendingItemTrade();
    memset(sLastRemoteTradeTransactionIds, 0, sizeof(sLastRemoteTradeTransactionIds));
#endif
}

bool8 MultiplayerTrade_StartItemTrade(u8 other_player_id, u32 money_amount)
{
#if FEATURE_MULTIPLAYER
    struct MultiplayerTradeRequest request;
    u8 local_player_id = MultiplayerSession_GetLocalPlayerId();

    if (local_player_id >= MAX_NET_PLAYERS || other_player_id >= MAX_NET_PLAYERS)
        return FALSE;
    if (local_player_id == other_player_id)
        return FALSE;
    if (money_amount > GetMoney(&gSaveBlock1Ptr->money))
        return FALSE;

    request.player_a = local_player_id;
    request.player_b = other_player_id;
    if (!MultiplayerTrade_Start(&request))
        return FALSE;

    sPendingItemTrade = TRUE;
    sPendingTradePlayerA = request.player_a;
    sPendingTradePlayerB = request.player_b;
    sPendingTradeSubsessionId = NET_SUBSESSION_NONE;
    sPendingTradeMoneyAmount = money_amount;
    sPendingTradeTransactionId = 0;
    return TRUE;
#else
    (void)other_player_id;
    (void)money_amount;
    return FALSE;
#endif
}

bool8 MultiplayerTrade_SendAction(u8 subsession_id, u8 action, u16 party_slot, u32 trade_checksum, u32 money_amount)
{
#if FEATURE_MULTIPLAYER
    struct NetTradeAction trade_action_packet;
    struct MultiplayerTransactionKey transaction_key;
    const struct MultiplayerSession *session;
    u32 action_sequence;

    if (!MultiplayerSession_IsOnline())
        return FALSE;
    if (subsession_id == NET_SUBSESSION_NONE || subsession_id > MAX_NET_SUBSESSIONS)
        return FALSE;

    session = MultiplayerSession_Get();
    if (session == NULL)
        return FALSE;

    memset(&trade_action_packet, 0, sizeof(trade_action_packet));
    action_sequence = MultiplayerSession_NextActionSequence();
    if (!MultiplayerSession_BuildTransactionKey(&transaction_key, NET_PACKET_TRADE_ACTION, subsession_id, action_sequence))
        return FALSE;
    trade_action_packet.header.client_frame = session->local_client_frame;
    trade_action_packet.header.server_tick_seen = session->bridge_tick;
    trade_action_packet.header.action_sequence = action_sequence;
    trade_action_packet.header.transaction_id = MultiplayerCommit_GetTransactionId(&transaction_key);
    trade_action_packet.subsession_id = subsession_id;
    trade_action_packet.action = action;
    trade_action_packet.party_slot = party_slot;
    trade_action_packet.trade_checksum = trade_checksum;
    trade_action_packet.money_amount = money_amount;
    if (action == MULTIPLAYER_TRADE_ACTION_ITEMTRADE_OFFER)
    {
        sPendingTradeSubsessionId = subsession_id;
        sPendingTradeTransactionId = trade_action_packet.header.transaction_id;
    }
    MultiplayerCommit_Prepare(&transaction_key, MULTIPLAYER_COMMIT_TRADE, &trade_action_packet, sizeof(trade_action_packet), NULL);
    if (!NetTransport_SendPacket(NET_PACKET_TRADE_ACTION, &trade_action_packet, sizeof(trade_action_packet)))
    {
        MultiplayerCommit_Rollback(&transaction_key, MULTIPLAYER_COMMIT_TRADE, &trade_action_packet, sizeof(trade_action_packet), NULL);
        return FALSE;
    }

    return TRUE;
#else
    (void)subsession_id;
    (void)action;
    (void)party_slot;
    (void)trade_checksum;
    (void)money_amount;
    return FALSE;
#endif
}

void MultiplayerTrade_Tick(const struct MultiplayerSession *session)
{
#if FEATURE_MULTIPLAYER
    const struct MultiplayerSubsession *subsession;

    if (!MultiplayerSession_IsOnline())
    {
        ClearPendingItemTrade();
        return;
    }
    if (!sPendingItemTrade || sPendingTradeTransactionId != 0)
        return;

    subsession = FindPendingTradeSubsession(session);
    if (subsession == NULL)
        return;

    if (MultiplayerTrade_SendAction(
        subsession->subsession_id,
        MULTIPLAYER_TRADE_ACTION_ITEMTRADE_OFFER,
        0xFFFF,
        0,
        sPendingTradeMoneyAmount))
    {
        MultiplayerSession_EndSubsession(subsession->subsession_id, MULTIPLAYER_SUBSESSION_STATE_ERROR);
    }
#else
    (void)session;
#endif
}

void MultiplayerTrade_OnCommitResult(const struct NetCommitResult *commit_result)
{
#if FEATURE_MULTIPLAYER
    if (commit_result == NULL || commit_result->commit_type != MULTIPLAYER_COMMIT_TRADE)
        return;
    if (sPendingTradeTransactionId == 0 || commit_result->transaction_id != sPendingTradeTransactionId)
        return;

    if (commit_result->result_code == MULTIPLAYER_COMMIT_RESULT_OK && sPendingTradeMoneyAmount != 0)
        MultiplayerCommit_PayMoney(sPendingTradeMoneyAmount);

    ClearPendingItemTrade();
#else
    (void)commit_result;
#endif
}

void MultiplayerTrade_ApplyRemoteAction(u8 sender_player_id, const struct NetTradeAction *trade_action)
{
#if FEATURE_MULTIPLAYER
    if (trade_action == NULL || sender_player_id >= MAX_NET_PLAYERS)
        return;
    if (sender_player_id == MultiplayerSession_GetLocalPlayerId())
        return;
    if (trade_action->action != MULTIPLAYER_TRADE_ACTION_ITEMTRADE_OFFER)
        return;
    if (trade_action->header.transaction_id == 0 || trade_action->money_amount == 0)
        return;
    if (sLastRemoteTradeTransactionIds[sender_player_id] == trade_action->header.transaction_id)
        return;

    sLastRemoteTradeTransactionIds[sender_player_id] = trade_action->header.transaction_id;
    MultiplayerCommit_ReceiveMoney(trade_action->money_amount);
#else
    (void)sender_player_id;
    (void)trade_action;
#endif
}

void MultiplayerTrade_End(u8 subsession_id, bool8 committed)
{
#if FEATURE_MULTIPLAYER
    if (committed)
        committed = FALSE;

    MultiplayerSession_EndSubsession(
        subsession_id,
        committed ? MULTIPLAYER_SUBSESSION_STATE_ENDING : MULTIPLAYER_SUBSESSION_STATE_ERROR);
#endif
}
