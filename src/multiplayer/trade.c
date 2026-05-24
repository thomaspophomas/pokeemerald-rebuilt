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

static bool8 SubsessionIncludesPlayer(const struct MultiplayerSubsession *subsession, u8 playerId)
{
    u8 i;

    if (subsession == NULL || !subsession->active || playerId >= MAX_NET_PLAYERS)
        return FALSE;

    for (i = 0; i < subsession->playerCount; i++)
    {
        if (subsession->players[i] == playerId)
            return TRUE;
    }

    return FALSE;
}

static bool8 SubsessionMatchesPendingTrade(const struct MultiplayerSubsession *subsession, u8 localPlayerId)
{
    if (subsession == NULL || !subsession->active)
        return FALSE;
    if (subsession->type != MULTIPLAYER_SUBSESSION_TRADE)
        return FALSE;
    if (subsession->state != MULTIPLAYER_SUBSESSION_STATE_INVITING
     && subsession->state != MULTIPLAYER_SUBSESSION_STATE_READY
     && subsession->state != MULTIPLAYER_SUBSESSION_STATE_ACTIVE)
        return FALSE;
    if (!SubsessionIncludesPlayer(subsession, localPlayerId))
        return FALSE;
    if (!SubsessionIncludesPlayer(subsession, sPendingTradePlayerA)
     || !SubsessionIncludesPlayer(subsession, sPendingTradePlayerB))
        return FALSE;

    return TRUE;
}

static const struct MultiplayerSubsession *FindPendingTradeSubsession(const struct MultiplayerSession *session)
{
    u8 i;

    if (session == NULL || session->localPlayerId >= MAX_NET_PLAYERS)
        return NULL;

    for (i = 0; i < MAX_NET_SUBSESSIONS; i++)
    {
        const struct MultiplayerSubsession *subsession = &session->subsessions[i];

        if (SubsessionMatchesPendingTrade(subsession, session->localPlayerId))
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
    if (request->playerA >= MAX_NET_PLAYERS || request->playerB >= MAX_NET_PLAYERS)
        return FALSE;
    if (request->playerA == request->playerB)
        return FALSE;
    if (MultiplayerSession_IsPlayerBusy(request->playerA) || MultiplayerSession_IsPlayerBusy(request->playerB))
        return FALSE;

    players[0] = request->playerA;
    players[1] = request->playerB;
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

    players[0] = request->playerA;
    players[1] = request->playerB;
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

bool8 MultiplayerTrade_StartItemTrade(u8 otherPlayerId, u32 moneyAmount)
{
#if FEATURE_MULTIPLAYER
    struct MultiplayerTradeRequest request;
    u8 localPlayerId = MultiplayerSession_GetLocalPlayerId();

    if (localPlayerId >= MAX_NET_PLAYERS || otherPlayerId >= MAX_NET_PLAYERS)
        return FALSE;
    if (localPlayerId == otherPlayerId)
        return FALSE;
    if (moneyAmount > GetMoney(&gSaveBlock1Ptr->money))
        return FALSE;

    request.playerA = localPlayerId;
    request.playerB = otherPlayerId;
    if (!MultiplayerTrade_Start(&request))
        return FALSE;

    sPendingItemTrade = TRUE;
    sPendingTradePlayerA = request.playerA;
    sPendingTradePlayerB = request.playerB;
    sPendingTradeSubsessionId = NET_SUBSESSION_NONE;
    sPendingTradeMoneyAmount = moneyAmount;
    sPendingTradeTransactionId = 0;
    return TRUE;
#else
    (void)otherPlayerId;
    (void)moneyAmount;
    return FALSE;
#endif
}

bool8 MultiplayerTrade_SendAction(u8 subsessionId, u8 action, u16 partySlot, u32 tradeChecksum, u32 moneyAmount)
{
#if FEATURE_MULTIPLAYER
    struct NetTradeAction packet;
    struct MultiplayerTransactionKey key;
    const struct MultiplayerSession *session;
    u32 actionSequence;

    if (!MultiplayerSession_IsOnline())
        return FALSE;
    if (subsessionId == NET_SUBSESSION_NONE || subsessionId > MAX_NET_SUBSESSIONS)
        return FALSE;

    session = MultiplayerSession_Get();
    if (session == NULL)
        return FALSE;

    memset(&packet, 0, sizeof(packet));
    actionSequence = MultiplayerSession_NextActionSequence();
    if (!MultiplayerSession_BuildTransactionKey(&key, NET_PACKET_TRADE_ACTION, subsessionId, actionSequence))
        return FALSE;
    packet.header.clientFrame = session->localClientFrame;
    packet.header.serverTickSeen = session->bridgeTick;
    packet.header.actionSequence = actionSequence;
    packet.header.transactionId = MultiplayerCommit_GetTransactionId(&key);
    packet.subsessionId = subsessionId;
    packet.action = action;
    packet.partySlot = partySlot;
    packet.tradeChecksum = tradeChecksum;
    packet.moneyAmount = moneyAmount;
    if (action == MULTIPLAYER_TRADE_ACTION_ITEMTRADE_OFFER)
    {
        sPendingTradeSubsessionId = subsessionId;
        sPendingTradeTransactionId = packet.header.transactionId;
    }
    MultiplayerCommit_Prepare(&key, MULTIPLAYER_COMMIT_TRADE, &packet, sizeof(packet), NULL);
    if (!NetTransport_SendPacket(NET_PACKET_TRADE_ACTION, &packet, sizeof(packet)))
    {
        MultiplayerCommit_Rollback(&key, MULTIPLAYER_COMMIT_TRADE, &packet, sizeof(packet), NULL);
        return FALSE;
    }

    return TRUE;
#else
    (void)subsessionId;
    (void)action;
    (void)partySlot;
    (void)tradeChecksum;
    (void)moneyAmount;
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
        subsession->id,
        MULTIPLAYER_TRADE_ACTION_ITEMTRADE_OFFER,
        0xFFFF,
        0,
        sPendingTradeMoneyAmount))
    {
        MultiplayerSession_EndSubsession(subsession->id, MULTIPLAYER_SUBSESSION_STATE_ERROR);
    }
#else
    (void)session;
#endif
}

void MultiplayerTrade_OnCommitResult(const struct NetCommitResult *result)
{
#if FEATURE_MULTIPLAYER
    if (result == NULL || result->commitType != MULTIPLAYER_COMMIT_TRADE)
        return;
    if (sPendingTradeTransactionId == 0 || result->transactionId != sPendingTradeTransactionId)
        return;

    if (result->result == MULTIPLAYER_COMMIT_RESULT_OK && sPendingTradeMoneyAmount != 0)
        MultiplayerCommit_PayMoney(sPendingTradeMoneyAmount);

    ClearPendingItemTrade();
#else
    (void)result;
#endif
}

void MultiplayerTrade_ApplyRemoteAction(u8 senderPlayerId, const struct NetTradeAction *action)
{
#if FEATURE_MULTIPLAYER
    if (action == NULL || senderPlayerId >= MAX_NET_PLAYERS)
        return;
    if (senderPlayerId == MultiplayerSession_GetLocalPlayerId())
        return;
    if (action->action != MULTIPLAYER_TRADE_ACTION_ITEMTRADE_OFFER)
        return;
    if (action->header.transactionId == 0 || action->moneyAmount == 0)
        return;
    if (sLastRemoteTradeTransactionIds[senderPlayerId] == action->header.transactionId)
        return;

    sLastRemoteTradeTransactionIds[senderPlayerId] = action->header.transactionId;
    MultiplayerCommit_ReceiveMoney(action->moneyAmount);
#else
    (void)senderPlayerId;
    (void)action;
#endif
}

void MultiplayerTrade_End(u8 subsessionId, bool8 committed)
{
#if FEATURE_MULTIPLAYER
    if (committed)
        committed = FALSE;

    MultiplayerSession_EndSubsession(
        subsessionId,
        committed ? MULTIPLAYER_SUBSESSION_STATE_ENDING : MULTIPLAYER_SUBSESSION_STATE_ERROR);
#endif
}
