#include "global.h"
#include "multiplayer/trade.h"
#include "multiplayer/commit.h"
#include "multiplayer/session.h"
#include "multiplayer/transport.h"

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

bool8 MultiplayerTrade_SendAction(u8 subsessionId, u8 action, u16 partySlot, u32 tradeChecksum)
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
    MultiplayerCommit_Prepare(&key, MULTIPLAYER_COMMIT_TRADE, &packet, sizeof(packet), NULL);
    if (!NetTransport_SendPacket(NET_PACKET_TRADE_ACTION, &packet, sizeof(packet)))
    {
        MultiplayerCommit_Rollback(&key, MULTIPLAYER_COMMIT_TRADE, &packet, sizeof(packet), NULL);
        return FALSE;
    }

    return TRUE;
#else
    return FALSE;
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
