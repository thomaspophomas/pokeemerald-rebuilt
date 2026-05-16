#include "global.h"
#include "multiplayer/battle.h"
#include "multiplayer/commit.h"
#include "multiplayer/session.h"
#include "multiplayer/transport.h"

#if FEATURE_MULTIPLAYER
static EWRAM_DATA bool8 sMultiplayerBattleActive = FALSE;
#endif

static bool8 RequestHasUniquePlayers(const struct MultiplayerBattleRequest *request)
{
    u8 i;
    u8 j;

    for (i = 0; i < request->playerCount; i++)
    {
        if (request->players[i] >= MAX_NET_PLAYERS)
            return FALSE;

        for (j = i + 1; j < request->playerCount; j++)
        {
            if (request->players[i] == request->players[j])
                return FALSE;
        }
    }

    return TRUE;
}

bool8 MultiplayerBattle_CanStart(const struct MultiplayerBattleRequest *request)
{
#if FEATURE_MULTIPLAYER
    u8 i;

    if (request == NULL)
        return FALSE;
    if (!MultiplayerSession_IsOnline())
        return FALSE;
    if (request->playerCount == 0 || request->playerCount > MAX_NET_BATTLE_PLAYERS)
        return FALSE;
    if (!RequestHasUniquePlayers(request))
        return FALSE;
    if (!MultiplayerSession_ArePlayersWithinRange(request->playerCount, request->players, NET_PLAYER_INTERACTION_RANGE_TILES))
        return FALSE;
    for (i = 0; i < request->playerCount; i++)
    {
        if (MultiplayerSession_IsPlayerBusy(request->players[i]))
            return FALSE;
    }

    switch (request->type)
    {
    case MULTIPLAYER_SUBSESSION_PVE_BATTLE:
        return request->playerCount > 0 && request->playerCount <= MAX_NET_PVE_PLAYERS;
    case MULTIPLAYER_SUBSESSION_PVP_BATTLE:
        return request->playerCount == MAX_NET_BATTLE_PLAYERS;
    default:
        return FALSE;
    }
#else
    (void)subsessionId;
    (void)battlerSlot;
    (void)action;
    (void)target;
    (void)parameter;
    return FALSE;
#endif
}

bool8 MultiplayerBattle_Start(const struct MultiplayerBattleRequest *request)
{
#if FEATURE_MULTIPLAYER
    if (!MultiplayerBattle_CanStart(request))
        return FALSE;

    return MultiplayerSession_StartSubsession(request->type, request->playerCount, request->players);
#else
    return FALSE;
#endif
}

bool8 MultiplayerBattle_SendAction(u8 subsessionId, u8 battlerSlot, u8 action, u8 target, u16 parameter)
{
#if FEATURE_MULTIPLAYER
    struct NetBattleAction packet;
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
    if (!MultiplayerSession_BuildTransactionKey(&key, NET_PACKET_BATTLE_ACTION, subsessionId, actionSequence))
        return FALSE;
    packet.header.clientFrame = session->localClientFrame;
    packet.header.serverTickSeen = session->bridgeTick;
    packet.header.actionSequence = actionSequence;
    packet.header.transactionId = MultiplayerCommit_GetTransactionId(&key);
    packet.subsessionId = subsessionId;
    packet.battlerSlot = battlerSlot;
    packet.action = action;
    packet.target = target;
    packet.parameter = parameter;
    MultiplayerCommit_Prepare(&key, MULTIPLAYER_COMMIT_BATTLE, &packet, sizeof(packet), NULL);
    if (!NetTransport_SendPacket(NET_PACKET_BATTLE_ACTION, &packet, sizeof(packet)))
    {
        MultiplayerCommit_Rollback(&key, MULTIPLAYER_COMMIT_BATTLE, &packet, sizeof(packet), NULL);
        return FALSE;
    }

    return TRUE;
#else
    return FALSE;
#endif
}

void MultiplayerBattle_OnBattleStart(u32 battleTypeFlags)
{
#if FEATURE_MULTIPLAYER
    (void)battleTypeFlags;

    if (!MultiplayerSession_IsOnline())
        return;
    if (sMultiplayerBattleActive)
        return;

    sMultiplayerBattleActive = TRUE;
#else
    (void)battleTypeFlags;
#endif
}

void MultiplayerBattle_OnBattleEnd(u32 battleOutcome)
{
#if FEATURE_MULTIPLAYER
    (void)battleOutcome;

    if (!sMultiplayerBattleActive)
        return;

    sMultiplayerBattleActive = FALSE;
#else
    (void)battleOutcome;
#endif
}
