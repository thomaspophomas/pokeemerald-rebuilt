#include "global.h"
#include "multiplayer/session.h"
#include "multiplayer/transport.h"
#include "multiplayer/overworld.h"
#include "multiplayer/battle.h"

#if FEATURE_MULTIPLAYER

static EWRAM_DATA struct MultiplayerSession sSession = {0};
static EWRAM_DATA bool8 sConnectRequested = FALSE;

static void ResetSession(void)
{
    memset(&sSession, 0, sizeof(sSession));
    sSession.state = MULTIPLAYER_SESSION_OFFLINE;
    sSession.localPlayerId = NET_PLAYER_NONE;
    sSession.hostPlayerId = NET_PLAYER_NONE;
}

static u8 GetMaxPlayersForSubsession(u8 type)
{
    switch (type)
    {
    case MULTIPLAYER_SUBSESSION_PVE_BATTLE:
        return MAX_NET_PVE_PLAYERS;
    case MULTIPLAYER_SUBSESSION_PVP_BATTLE:
        return MAX_NET_BATTLE_PLAYERS;
    case MULTIPLAYER_SUBSESSION_TRADE:
        return MAX_NET_TRADE_PLAYERS;
    default:
        return 0;
    }
}

static bool8 ViewHasActiveLocalSubsession(const struct NetTransportSessionView *view)
{
    u8 i;
    u8 j;

    for (i = 0; i < MAX_NET_SUBSESSIONS; i++)
    {
        if (!view->subsessions[i].active)
            continue;

        for (j = 0; j < view->subsessions[i].playerCount; j++)
        {
            if (view->subsessions[i].players[j] == view->localPlayerId)
                return TRUE;
        }
    }

    return FALSE;
}

static void CopyViewIntoSession(const struct NetTransportSessionView *view)
{
    sSession.localPlayerId = view->localPlayerId;
    sSession.hostPlayerId = view->hostPlayerId;
    sSession.playerCount = view->playerCount;
    sSession.sessionId = view->sessionId;
    sSession.tick++;
    memcpy(sSession.players, view->players, sizeof(sSession.players));
    memcpy(sSession.subsessions, view->subsessions, sizeof(sSession.subsessions));

    if (ViewHasActiveLocalSubsession(view))
        sSession.state = MULTIPLAYER_SESSION_SUBSESSION;
    else if (view->playerCount > 1)
        sSession.state = MULTIPLAYER_SESSION_OVERWORLD_SYNC;
    else
        sSession.state = MULTIPLAYER_SESSION_LOBBY;
}

static void PublishLocalSnapshot(void)
{
    struct NetPlayerSnapshot snapshot;

    if (sSession.localPlayerId >= MAX_NET_PLAYERS)
        return;

    MultiplayerOverworld_BuildLocalSnapshot(&snapshot, sSession.localPlayerId, sSession.tick);
    if (MultiplayerSession_IsPlayerInSubsession(sSession.localPlayerId))
    {
        snapshot.flags |= NET_PLAYER_FLAG_BUSY | NET_PLAYER_FLAG_IN_SUBSESSION;
        snapshot.interactionState = MULTIPLAYER_INTERACTION_BATTLE;
    }
    NetTransport_WriteLocalSnapshot(&snapshot);
    sSession.players[sSession.localPlayerId] = snapshot;
}

#endif

void MultiplayerSession_Init(void)
{
#if FEATURE_MULTIPLAYER
    ResetSession();
    NetTransport_Init();
    MultiplayerOverworld_Init();
#if FEATURE_MULTIPLAYER_AUTOCONNECT
    MultiplayerSession_RequestConnect();
#endif
#endif
}

void MultiplayerSession_Tick(void)
{
#if FEATURE_MULTIPLAYER
    struct NetTransportSessionView view;

    if (!sConnectRequested && sSession.state == MULTIPLAYER_SESSION_OFFLINE)
        return;

    NetTransport_Tick();
    if (!NetTransport_ReadSessionView(&view))
    {
        if (sConnectRequested)
            sSession.state = MULTIPLAYER_SESSION_CONNECTING;
        return;
    }

    CopyViewIntoSession(&view);
    PublishLocalSnapshot();
    MultiplayerOverworld_Tick(&sSession);
#endif
}

void MultiplayerSession_OnMapLoad(void)
{
#if FEATURE_MULTIPLAYER
    MultiplayerOverworld_OnMapLoad();
#endif
}

void MultiplayerSession_OnPlayerStep(u8 direction, u16 newKeys, u16 heldKeys)
{
#if FEATURE_MULTIPLAYER
    MultiplayerOverworld_OnPlayerStep(direction, newKeys, heldKeys);
#endif
}

void MultiplayerSession_OnBattleStart(u32 battleTypeFlags)
{
#if FEATURE_MULTIPLAYER
    MultiplayerBattle_OnBattleStart(battleTypeFlags);
#endif
}

void MultiplayerSession_OnBattleEnd(u32 battleOutcome)
{
#if FEATURE_MULTIPLAYER
    MultiplayerBattle_OnBattleEnd(battleOutcome);
#endif
}

void MultiplayerSession_RequestConnect(void)
{
#if FEATURE_MULTIPLAYER
    sConnectRequested = TRUE;
    if (sSession.state == MULTIPLAYER_SESSION_OFFLINE)
        sSession.state = MULTIPLAYER_SESSION_CONNECTING;
#endif
}

void MultiplayerSession_RequestDisconnect(void)
{
#if FEATURE_MULTIPLAYER
    sConnectRequested = FALSE;
    sSession.state = MULTIPLAYER_SESSION_DISCONNECTING;
    ResetSession();
#endif
}

u8 MultiplayerSession_GetState(void)
{
#if FEATURE_MULTIPLAYER
    return sSession.state;
#else
    return MULTIPLAYER_SESSION_OFFLINE;
#endif
}

u8 MultiplayerSession_GetLocalPlayerId(void)
{
#if FEATURE_MULTIPLAYER
    return sSession.localPlayerId;
#else
    return NET_PLAYER_NONE;
#endif
}

u8 MultiplayerSession_GetPlayerCount(void)
{
#if FEATURE_MULTIPLAYER
    return sSession.playerCount;
#else
    return 0;
#endif
}

const struct MultiplayerSession *MultiplayerSession_Get(void)
{
#if FEATURE_MULTIPLAYER
    return &sSession;
#else
    return NULL;
#endif
}

bool8 MultiplayerSession_IsOnline(void)
{
#if FEATURE_MULTIPLAYER
    return sSession.state == MULTIPLAYER_SESSION_LOBBY
        || sSession.state == MULTIPLAYER_SESSION_OVERWORLD_SYNC
        || sSession.state == MULTIPLAYER_SESSION_SUBSESSION;
#else
    return FALSE;
#endif
}

bool8 MultiplayerSession_IsHost(void)
{
#if FEATURE_MULTIPLAYER
    return sSession.localPlayerId != NET_PLAYER_NONE && sSession.localPlayerId == sSession.hostPlayerId;
#else
    return FALSE;
#endif
}

bool8 MultiplayerSession_IsPlayerInSubsession(u8 playerId)
{
#if FEATURE_MULTIPLAYER
    u8 i;
    u8 j;

    for (i = 0; i < MAX_NET_SUBSESSIONS; i++)
    {
        if (!sSession.subsessions[i].active)
            continue;

        for (j = 0; j < sSession.subsessions[i].playerCount; j++)
        {
            if (sSession.subsessions[i].players[j] == playerId)
                return TRUE;
        }
    }
#endif
    return FALSE;
}

bool8 MultiplayerSession_StartSubsession(u8 type, u8 playerCount, const u8 *players)
{
#if FEATURE_MULTIPLAYER
    u8 i;
    u8 maxPlayers;

    maxPlayers = GetMaxPlayersForSubsession(type);
    if (maxPlayers == 0 || playerCount == 0 || playerCount > maxPlayers)
        return FALSE;
    if (!MultiplayerSession_IsHost())
        return FALSE;

    for (i = 0; i < MAX_NET_SUBSESSIONS; i++)
    {
        if (!sSession.subsessions[i].active)
        {
            memset(&sSession.subsessions[i], 0, sizeof(sSession.subsessions[i]));
            sSession.subsessions[i].active = TRUE;
            sSession.subsessions[i].id = i + 1;
            sSession.subsessions[i].type = type;
            sSession.subsessions[i].state = MULTIPLAYER_SUBSESSION_STATE_INVITING;
            sSession.subsessions[i].hostPlayerId = sSession.localPlayerId;
            sSession.subsessions[i].playerCount = playerCount;
            sSession.subsessions[i].timeoutFrames = NET_SUBSESSION_TIMEOUT_FRAMES;
            memcpy(sSession.subsessions[i].players, players, playerCount);
            NetTransport_SendPacket(NET_PACKET_SUBSESSION_INVITE, &sSession.subsessions[i], sizeof(sSession.subsessions[i]));
            return TRUE;
        }
    }
#endif
    return FALSE;
}

void MultiplayerSession_EndSubsession(u8 subsessionId, u8 state)
{
#if FEATURE_MULTIPLAYER
    u8 i;

    for (i = 0; i < MAX_NET_SUBSESSIONS; i++)
    {
        if (sSession.subsessions[i].active && sSession.subsessions[i].id == subsessionId)
        {
            sSession.subsessions[i].state = state;
            NetTransport_SendPacket(NET_PACKET_SUBSESSION_ABORT, &sSession.subsessions[i], sizeof(sSession.subsessions[i]));
            memset(&sSession.subsessions[i], 0, sizeof(sSession.subsessions[i]));
            return;
        }
    }
#endif
}
