#include "global.h"
#include "engine/runtime_state.h"
#include "generated/mod_registry.h"
#include "global.fieldmap.h"
#include "multiplayer/session.h"
#include "multiplayer/companion_save_beacon.h"
#include "multiplayer/commit.h"
#include "multiplayer/transport.h"
#include "multiplayer/overworld.h"
#include "multiplayer/battle.h"
#include "mod/runtime_profile.h"

#if FEATURE_MULTIPLAYER_SMOKE_STATUS
#include "multiplayer/smoke_status.h"
#endif

#if FEATURE_MULTIPLAYER

static EWRAM_DATA struct MultiplayerSession sSession = {0};
static EWRAM_DATA bool8 sConnectRequested = FALSE;
static EWRAM_DATA bool8 sClientHelloSent = FALSE;
static EWRAM_DATA u16 sHeartbeatTimer = 0;
static EWRAM_DATA u16 sTransportLossFrames = 0;

#if FEATURE_MULTIPLAYER_SMOKE_STATUS
EWRAM_DATA volatile struct NetMultiplayerSmokeStatus gNetMultiplayerSmokeStatus = {0};
static EWRAM_DATA u32 sLastSmokeProfileAckHash = 0;
static EWRAM_DATA u8 sLastSmokeProfileAckResult = 0;

static void UpdateSmokeStatus(void);
#endif

static bool8 RuntimeAllowsOnline(void)
{
    return EngineRuntimeState_IsMultiplayerOnlineEnabled();
}

static void ResetSession(void)
{
    memset(&sSession, 0, sizeof(sSession));
    sSession.state = MULTIPLAYER_SESSION_OFFLINE;
    sSession.localPlayerId = NET_PLAYER_NONE;
    sSession.hostPlayerId = NET_PLAYER_NONE;
    sSession.transportMode = NET_ACTIVE_TRANSPORT_MODE;
    sSession.healthState = MULTIPLAYER_HEALTH_DISCONNECTED;
    sClientHelloSent = FALSE;
    sHeartbeatTimer = 0;
    sTransportLossFrames = 0;
    ModRuntimeProfile_Clear();
#if FEATURE_MULTIPLAYER_SMOKE_STATUS
    sLastSmokeProfileAckHash = 0;
    sLastSmokeProfileAckResult = 0;
    UpdateSmokeStatus();
#endif
}

#if FEATURE_MULTIPLAYER_SMOKE_STATUS
static void UpdateSmokeStatus(void)
{
    gNetMultiplayerSmokeStatus.magic = NET_SMOKE_STATUS_MAGIC;
    gNetMultiplayerSmokeStatus.sessionState = sSession.state;
    gNetMultiplayerSmokeStatus.healthState = sSession.healthState;
    gNetMultiplayerSmokeStatus.localPlayerId = sSession.localPlayerId;
    gNetMultiplayerSmokeStatus.playerCount = sSession.playerCount;
    gNetMultiplayerSmokeStatus.sessionEpoch = sSession.sessionEpoch;
    gNetMultiplayerSmokeStatus.activeProfileHash = ModRuntimeProfile_GetActiveHash();
    gNetMultiplayerSmokeStatus.lastProfileAckHash = sLastSmokeProfileAckHash;
    gNetMultiplayerSmokeStatus.lastProfileAckResult = sLastSmokeProfileAckResult;
}
#endif

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

static bool8 IsValidPlayerId(u8 playerId)
{
    return playerId < MAX_NET_PLAYERS;
}

static bool8 IsValidSubsessionState(u8 state)
{
    return state == MULTIPLAYER_SUBSESSION_STATE_INVITING
        || state == MULTIPLAYER_SUBSESSION_STATE_READY
        || state == MULTIPLAYER_SUBSESSION_STATE_ACTIVE
        || state == MULTIPLAYER_SUBSESSION_STATE_ENDING
        || state == MULTIPLAYER_SUBSESSION_STATE_ERROR;
}

static bool8 IsValidInteractionState(u8 state)
{
    return state <= MULTIPLAYER_INTERACTION_TRADE;
}

static bool8 DirectionIsValid(u8 direction)
{
    return direction == DIR_NORTH
        || direction == DIR_SOUTH
        || direction == DIR_WEST
        || direction == DIR_EAST;
}

static bool8 ElevationIsValid(u8 elevation)
{
    return elevation == ELEVATION_TRANSITION
        || elevation == ELEVATION_SURF
        || elevation == ELEVATION_DEFAULT
        || elevation == ELEVATION_MULTI_LEVEL;
}

static bool8 MapLocationIsValid(u8 mapGroup, u8 mapNum)
{
    return mapGroup != 0xFF && mapNum != 0xFF;
}

static bool8 SessionIdentityIsValid(const struct NetTransportSessionView *view)
{
    return view->transportMode == NET_ACTIVE_TRANSPORT_MODE
        && view->sessionId != 0
        && view->sessionEpoch != 0
        && view->playerToken != 0
        && view->joinNonce != 0;
}

static bool8 SnapshotCoordsAreSane(const struct NetPlayerSnapshot *snapshot)
{
    return snapshot->x >= NET_PLAYER_COORD_MIN
        && snapshot->x <= NET_PLAYER_COORD_MAX
        && snapshot->y >= NET_PLAYER_COORD_MIN
        && snapshot->y <= NET_PLAYER_COORD_MAX;
}

static bool8 SubsessionStateBlocksInteraction(u8 state)
{
    return state == MULTIPLAYER_SUBSESSION_STATE_INVITING
        || state == MULTIPLAYER_SUBSESSION_STATE_READY
        || state == MULTIPLAYER_SUBSESSION_STATE_ACTIVE;
}

static bool8 SubsessionStateIsTerminal(u8 state)
{
    return state == MULTIPLAYER_SUBSESSION_STATE_ENDING
        || state == MULTIPLAYER_SUBSESSION_STATE_ERROR;
}

static bool8 BarrierTypeIsValid(u8 type)
{
    return type == MULTIPLAYER_BARRIER_SCRIPT
        || type == MULTIPLAYER_BARRIER_WARP
        || type == MULTIPLAYER_BARRIER_BATTLE_INVITE
        || type == MULTIPLAYER_BARRIER_TRADE_INVITE;
}

static bool8 SnapshotTickIsFresh(const struct NetPlayerSnapshot *snapshot, u32 currentTick)
{
    if (snapshot->tick == 0 || currentTick == 0)
        return TRUE;
    if (snapshot->tick > currentTick)
        return snapshot->tick - currentTick <= NET_PLAYER_SNAPSHOT_FUTURE_SKEW_FRAMES;

    return currentTick - snapshot->tick <= NET_PLAYER_SNAPSHOT_TTL_FRAMES;
}

static bool8 SnapshotIsStale(const struct NetPlayerSnapshot *snapshot)
{
    return (snapshot->flags & (NET_PLAYER_FLAG_STALE | NET_PLAYER_FLAG_RESYNC))
        || snapshot->staleFrames >= NET_PLAYER_STALE_FRAMES;
}

static bool8 SnapshotHasValidOnlineIdentity(const struct NetPlayerSnapshot *snapshot, const struct NetTransportSessionView *view)
{
    return snapshot->sessionEpoch == view->sessionEpoch
        && snapshot->sequence != 0;
}

static bool8 SnapshotServerTickIsFresh(const struct NetPlayerSnapshot *snapshot, u32 currentTick)
{
    if (snapshot->serverTickSeen == 0 || currentTick == 0)
        return TRUE;
    if (snapshot->serverTickSeen > currentTick)
        return snapshot->serverTickSeen - currentTick <= NET_PLAYER_SNAPSHOT_FUTURE_SKEW_FRAMES;

    return currentTick - snapshot->serverTickSeen <= NET_PLAYER_DISCONNECT_FRAMES;
}

static bool8 SnapshotIsValid(const struct NetPlayerSnapshot *snapshot, u8 slot, const struct NetTransportSessionView *view)
{
    if (!snapshot->active)
        return TRUE;
    if (snapshot->playerId != slot)
        return FALSE;
    if (!SnapshotHasValidOnlineIdentity(snapshot, view))
        return FALSE;
    if (!MapLocationIsValid(snapshot->mapGroup, snapshot->mapNum))
        return FALSE;
    if (!SnapshotCoordsAreSane(snapshot))
        return FALSE;
    if (!ElevationIsValid(snapshot->elevation))
        return FALSE;
    if (!DirectionIsValid(snapshot->facingDirection))
        return FALSE;
    if (snapshot->flags & ~NET_PLAYER_FLAGS_MASK)
        return FALSE;
    if (!IsValidInteractionState(snapshot->interactionState))
        return FALSE;
    if (snapshot->subsessionState > MULTIPLAYER_SUBSESSION_STATE_ERROR)
        return FALSE;
    if (snapshot->subsessionId > MAX_NET_SUBSESSIONS)
        return FALSE;
    if ((snapshot->flags & NET_PLAYER_FLAG_IN_SUBSESSION) && snapshot->subsessionId == NET_SUBSESSION_NONE)
        return FALSE;
    if ((snapshot->flags & NET_PLAYER_FLAG_IN_SUBSESSION) && !IsValidSubsessionState(snapshot->subsessionState))
        return FALSE;
    if (!(snapshot->flags & NET_PLAYER_FLAG_IN_SUBSESSION)
     && (snapshot->subsessionId != NET_SUBSESSION_NONE || snapshot->subsessionState != MULTIPLAYER_SUBSESSION_STATE_NONE))
        return FALSE;
    if ((snapshot->flags & NET_PLAYER_FLAG_IN_SUBSESSION)
     && snapshot->interactionState != MULTIPLAYER_INTERACTION_BATTLE
     && snapshot->interactionState != MULTIPLAYER_INTERACTION_TRADE)
        return FALSE;
    if ((snapshot->interactionState == MULTIPLAYER_INTERACTION_BATTLE
      || snapshot->interactionState == MULTIPLAYER_INTERACTION_TRADE)
     && !(snapshot->flags & NET_PLAYER_FLAG_IN_SUBSESSION))
        return FALSE;
    if ((snapshot->interactionState == MULTIPLAYER_INTERACTION_SCRIPT
      || snapshot->interactionState == MULTIPLAYER_INTERACTION_WARP)
     && !(snapshot->flags & NET_PLAYER_FLAG_BUSY))
        return FALSE;
    if (snapshot->staleFrames > NET_PLAYER_DISCONNECT_FRAMES)
        return FALSE;
    if (snapshot->anomalyScore >= NET_ANOMALY_THRESHOLD_KICK)
        return FALSE;
    if (!SnapshotTickIsFresh(snapshot, view->bridgeTick))
        return FALSE;
    if (!SnapshotServerTickIsFresh(snapshot, view->bridgeTick))
        return FALSE;

    return TRUE;
}

static bool8 SubsessionIncludesPlayer(const struct MultiplayerSubsession *subsession, u8 playerId)
{
    u8 j;
    u8 maxPlayers;

    if (!subsession->active)
        return FALSE;

    maxPlayers = GetMaxPlayersForSubsession(subsession->type);
    if (maxPlayers == 0 || subsession->playerCount > maxPlayers)
        return FALSE;

    for (j = 0; j < subsession->playerCount; j++)
    {
        if (subsession->players[j] == playerId)
            return TRUE;
    }

    return FALSE;
}

static bool8 BlockingSubsessionIncludesPlayer(const struct MultiplayerSubsession *subsession, u8 playerId)
{
    return SubsessionStateBlocksInteraction(subsession->state)
        && SubsessionIncludesPlayer(subsession, playerId);
}

static bool8 SubsessionIsValid(const struct MultiplayerSubsession *subsession, const struct NetPlayerSnapshot *players)
{
    u8 i;
    u8 j;
    u8 maxPlayers;

    if (!subsession->active)
        return TRUE;
    if (subsession->id == NET_SUBSESSION_NONE || subsession->id > MAX_NET_SUBSESSIONS)
        return FALSE;
    if (!IsValidSubsessionState(subsession->state))
        return FALSE;
    if (SubsessionStateBlocksInteraction(subsession->state) && subsession->timeoutFrames == 0)
        return FALSE;
    if (!IsValidPlayerId(subsession->hostPlayerId) || !players[subsession->hostPlayerId].active)
        return FALSE;

    maxPlayers = GetMaxPlayersForSubsession(subsession->type);
    if (maxPlayers == 0 || subsession->playerCount == 0 || subsession->playerCount > maxPlayers)
        return FALSE;

    for (i = 0; i < subsession->playerCount; i++)
    {
        if (!IsValidPlayerId(subsession->players[i])
         || !players[subsession->players[i]].active
         || SnapshotIsStale(&players[subsession->players[i]]))
            return FALSE;

        for (j = i + 1; j < subsession->playerCount; j++)
        {
            if (subsession->players[i] == subsession->players[j])
                return FALSE;
        }
    }

    return TRUE;
}

static bool8 SanitizeSessionView(const struct NetTransportSessionView *view, struct NetTransportSessionView *sanitized)
{
    u8 i;
    u8 j;
    bool8 claimedPlayers[MAX_NET_PLAYERS] = {0};

    memset(sanitized, 0, sizeof(*sanitized));
    if (!view->connected)
        return FALSE;
    if (!IsValidPlayerId(view->localPlayerId))
        return FALSE;
    if (!IsValidPlayerId(view->hostPlayerId))
        return FALSE;
    if (view->playerCount == 0 || view->playerCount > MAX_NET_PLAYERS)
        return FALSE;
    if (!SessionIdentityIsValid(view))
        return FALSE;

    sanitized->connected = TRUE;
    sanitized->localPlayerId = view->localPlayerId;
    sanitized->hostPlayerId = view->hostPlayerId;
    sanitized->playerCount = view->playerCount;
    sanitized->transportMode = view->transportMode;
    sanitized->sessionId = view->sessionId;
    sanitized->sessionEpoch = view->sessionEpoch;
    sanitized->playerToken = view->playerToken;
    sanitized->joinNonce = view->joinNonce;
    sanitized->bridgeTick = view->bridgeTick;
    sanitized->serverClockSeconds = view->serverClockSeconds;

    for (i = 0; i < MAX_NET_PLAYERS; i++)
    {
        if (SnapshotIsValid(&view->players[i], i, view))
            sanitized->players[i] = view->players[i];
    }

    for (i = 0; i < MAX_NET_SUBSESSIONS; i++)
    {
        if (!view->subsessions[i].active)
            continue;
        if (!SubsessionIsValid(&view->subsessions[i], sanitized->players))
            continue;

        if (SubsessionStateBlocksInteraction(view->subsessions[i].state))
        {
            for (j = 0; j < view->subsessions[i].playerCount; j++)
            {
                if (claimedPlayers[view->subsessions[i].players[j]])
                    break;
            }
            if (j != view->subsessions[i].playerCount)
                continue;
        }

        sanitized->subsessions[i] = view->subsessions[i];
        if (SubsessionStateBlocksInteraction(sanitized->subsessions[i].state))
        {
            for (j = 0; j < sanitized->subsessions[i].playerCount; j++)
                claimedPlayers[sanitized->subsessions[i].players[j]] = TRUE;
        }
    }

    return TRUE;
}

static bool8 BarrierIncludesPlayer(const struct MultiplayerInteractionBarrier *barrier, u8 playerId)
{
    u8 i;

    if (!barrier->active)
        return FALSE;

    for (i = 0; i < barrier->playerCount; i++)
    {
        if (barrier->players[i] == playerId)
            return TRUE;
    }

    return FALSE;
}

static u8 GetBarrierTypeForSubsession(u8 subsessionType)
{
    if (subsessionType == MULTIPLAYER_SUBSESSION_TRADE)
        return MULTIPLAYER_BARRIER_TRADE_INVITE;
    if (subsessionType == MULTIPLAYER_SUBSESSION_PVE_BATTLE
     || subsessionType == MULTIPLAYER_SUBSESSION_PVP_BATTLE)
        return MULTIPLAYER_BARRIER_BATTLE_INVITE;

    return MULTIPLAYER_BARRIER_NONE;
}

static bool8 SubsessionMatchesBarrier(const struct MultiplayerSubsession *subsession, const struct MultiplayerInteractionBarrier *barrier)
{
    u8 i;

    if (!subsession->active || !barrier->active)
        return FALSE;
    if (GetBarrierTypeForSubsession(subsession->type) != barrier->type)
        return FALSE;
    if (subsession->playerCount != barrier->playerCount)
        return FALSE;

    for (i = 0; i < barrier->playerCount; i++)
    {
        if (!SubsessionIncludesPlayer(subsession, barrier->players[i]))
            return FALSE;
    }

    return TRUE;
}

static void ResetInteractionBarrier(void)
{
    memset(&sSession.interactionBarrier, 0, sizeof(sSession.interactionBarrier));
}

static void ClearInviteBarrierWhenSubsessionLeavesInvite(void)
{
    u8 i;

    if (sSession.interactionBarrier.type != MULTIPLAYER_BARRIER_BATTLE_INVITE
     && sSession.interactionBarrier.type != MULTIPLAYER_BARRIER_TRADE_INVITE)
        return;

    for (i = 0; i < MAX_NET_SUBSESSIONS; i++)
    {
        if (!SubsessionMatchesBarrier(&sSession.subsessions[i], &sSession.interactionBarrier))
            continue;
        if (sSession.subsessions[i].state == MULTIPLAYER_SUBSESSION_STATE_INVITING
         || sSession.subsessions[i].state == MULTIPLAYER_SUBSESSION_STATE_READY)
            return;

        ResetInteractionBarrier();
        return;
    }
}

static void ExpireInteractionBarrier(void)
{
    if (!sSession.interactionBarrier.active)
        return;
    if (sSession.interactionBarrier.timeoutFrames != 0)
        sSession.interactionBarrier.timeoutFrames--;
    if (sSession.interactionBarrier.timeoutFrames == 0)
        ResetInteractionBarrier();
}

static bool8 SnapshotInteractionBlocks(const struct NetPlayerSnapshot *snapshot)
{
    if (!snapshot->active)
        return FALSE;
    if (SnapshotIsStale(snapshot))
        return TRUE;
    if (snapshot->interactionState == MULTIPLAYER_INTERACTION_SCRIPT
     || snapshot->interactionState == MULTIPLAYER_INTERACTION_WARP)
        return TRUE;
    if ((snapshot->flags & NET_PLAYER_FLAG_IN_SUBSESSION)
     && SubsessionStateBlocksInteraction(snapshot->subsessionState))
        return TRUE;

    return FALSE;
}

static bool8 SessionHasActiveLocalSubsession(void)
{
    u8 i;

    for (i = 0; i < MAX_NET_SUBSESSIONS; i++)
    {
        if (BlockingSubsessionIncludesPlayer(&sSession.subsessions[i], sSession.localPlayerId))
            return TRUE;
    }

    return FALSE;
}

static const struct MultiplayerSubsession *GetLocalSubsession(void)
{
    u8 i;

    for (i = 0; i < MAX_NET_SUBSESSIONS; i++)
    {
        if (BlockingSubsessionIncludesPlayer(&sSession.subsessions[i], sSession.localPlayerId))
            return &sSession.subsessions[i];
    }

    return NULL;
}

static void AbortLocalSubsessionsForDisconnect(void)
{
    u8 i;

    for (i = 0; i < MAX_NET_SUBSESSIONS; i++)
    {
        if (!sSession.subsessions[i].active)
            continue;
        if (!SubsessionIncludesPlayer(&sSession.subsessions[i], sSession.localPlayerId))
            continue;

        sSession.subsessions[i].state = MULTIPLAYER_SUBSESSION_STATE_ERROR;
        memset(&sSession.subsessions[i], 0, sizeof(sSession.subsessions[i]));
    }

    ResetInteractionBarrier();
}

static void ExpireLocalSubsessions(void)
{
    u8 i;

    ExpireInteractionBarrier();

    if (sSession.localPlayerId == NET_PLAYER_NONE || sSession.localPlayerId != sSession.hostPlayerId)
        return;

    for (i = 0; i < MAX_NET_SUBSESSIONS; i++)
    {
        if (!sSession.subsessions[i].active)
            continue;
        if (!SubsessionStateBlocksInteraction(sSession.subsessions[i].state))
            continue;

        if (sSession.subsessions[i].timeoutFrames != 0)
            sSession.subsessions[i].timeoutFrames--;
        if (sSession.subsessions[i].timeoutFrames == 0)
        {
            sSession.subsessions[i].state = MULTIPLAYER_SUBSESSION_STATE_ERROR;
            NetTransport_SendPacket(NET_PACKET_SUBSESSION_ABORT, &sSession.subsessions[i], sizeof(sSession.subsessions[i]));
            memset(&sSession.subsessions[i], 0, sizeof(sSession.subsessions[i]));
            ResetInteractionBarrier();
        }
    }
}

static void CopyViewIntoSession(const struct NetTransportSessionView *view)
{
    u32 oldSessionEpoch = sSession.sessionEpoch;

    sSession.localPlayerId = view->localPlayerId;
    sSession.hostPlayerId = view->hostPlayerId;
    sSession.playerCount = view->playerCount;
    sSession.transportMode = view->transportMode;
    sSession.sessionId = view->sessionId;
    sSession.sessionEpoch = view->sessionEpoch;
    sSession.playerToken = view->playerToken;
    sSession.joinNonce = view->joinNonce;
    sSession.bridgeTick = view->bridgeTick;
    sSession.serverClockSeconds = view->serverClockSeconds;
    if (oldSessionEpoch != sSession.sessionEpoch)
    {
        sClientHelloSent = FALSE;
        sHeartbeatTimer = 0;
        sSession.localClientFrame = 0;
        sSession.localSnapshotSequence = 0;
        sSession.localActionSequence = 0;
        MultiplayerCommit_Init();
        ModRuntimeProfile_Clear();
    }
    sSession.healthState = MULTIPLAYER_HEALTH_HEALTHY;
    sTransportLossFrames = 0;
    if (view->bridgeTick != 0)
        sSession.tick = view->bridgeTick;
    else
        sSession.tick++;
    memcpy(sSession.players, view->players, sizeof(sSession.players));
    memcpy(sSession.subsessions, view->subsessions, sizeof(sSession.subsessions));
    ClearInviteBarrierWhenSubsessionLeavesInvite();
    ExpireLocalSubsessions();

    if (SessionHasActiveLocalSubsession())
        sSession.state = MULTIPLAYER_SESSION_SUBSESSION;
    else if (view->playerCount > 1)
        sSession.state = MULTIPLAYER_SESSION_OVERWORLD_SYNC;
    else
        sSession.state = MULTIPLAYER_SESSION_LOBBY;
}

static u32 BuildFeatureFlags(void)
{
    u32 flags = 0;

#if FEATURE_MULTIPLAYER
    flags |= NET_FEATURE_FLAG_MULTIPLAYER;
#endif
#if FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT
    flags |= NET_FEATURE_FLAG_EMULATOR_TRANSPORT;
#endif
#if FEATURE_MULTIPLAYER_LINK_TRANSPORT
    flags |= NET_FEATURE_FLAG_LINK_TRANSPORT;
#endif
#if FEATURE_ENGINE_GEN1
    flags |= NET_FEATURE_FLAG_ENGINE_GEN1;
#endif
#if FEATURE_ENGINE_GEN2
    flags |= NET_FEATURE_FLAG_ENGINE_GEN2;
#endif
#if FEATURE_ENGINE_GEN3
    flags |= NET_FEATURE_FLAG_ENGINE_GEN3;
#endif

    return flags;
}

static bool8 PublishClientHello(void)
{
    struct NetClientHello hello;

    if (sClientHelloSent)
        return FALSE;
    if (sSession.sessionEpoch == 0 || sSession.playerToken == 0 || sSession.joinNonce == 0)
        return FALSE;

    memset(&hello, 0, sizeof(hello));
    hello.protocolVersion = NET_PROTOCOL_VERSION;
    hello.bridgeVersion = NET_EMULATOR_BRIDGE_VERSION;
    hello.buildId = NET_PROTOCOL_BUILD_ID;
    hello.rulesetHash = NET_RULESET_HASH;
    hello.featureFlags = BuildFeatureFlags();
    hello.profileProtocolVersion = MOD_RUNTIME_PROFILE_PROTOCOL_VERSION;
    hello.profileCapabilityFlags = MOD_RUNTIME_PROFILE_CAPABILITIES;
    hello.profileCapabilityHash = MOD_RUNTIME_PROFILE_CAPABILITY_HASH;
    hello.modCatalogHash = gModCatalogHash;
    hello.modCatalogCount = gModCatalogEntryCount;
    hello.transportMode = NET_ACTIVE_TRANSPORT_MODE;

    if (NetTransport_SendPacket(NET_PACKET_CLIENT_HELLO, &hello, sizeof(hello)))
    {
        sClientHelloSent = TRUE;
        sHeartbeatTimer = NET_HEARTBEAT_INTERVAL_FRAMES;
        return TRUE;
    }

    return FALSE;
}

static u32 NextActionSequence(void)
{
    sSession.localActionSequence++;
    if (sSession.localActionSequence == 0)
        sSession.localActionSequence++;

    return sSession.localActionSequence;
}

static void PublishHeartbeat(void)
{
    struct NetHeartbeat heartbeat;

    if (sSession.sessionEpoch == 0 || sSession.playerToken == 0 || sSession.joinNonce == 0)
        return;
    if (sHeartbeatTimer != 0)
    {
        sHeartbeatTimer--;
        return;
    }

    memset(&heartbeat, 0, sizeof(heartbeat));
    heartbeat.clientFrame = sSession.localClientFrame;
    heartbeat.serverTickSeen = sSession.bridgeTick;
    heartbeat.sessionEpoch = sSession.sessionEpoch;
    heartbeat.playerToken = sSession.playerToken;
    heartbeat.joinNonce = sSession.joinNonce;
    if (NetTransport_SendPacket(NET_PACKET_HEARTBEAT, &heartbeat, sizeof(heartbeat)))
        sHeartbeatTimer = NET_HEARTBEAT_INTERVAL_FRAMES;
}

static void PublishLocalSnapshot(void)
{
    struct NetPlayerSnapshot snapshot;
    const struct MultiplayerSubsession *subsession;

    if (sSession.localPlayerId >= MAX_NET_PLAYERS)
        return;
    if (!MultiplayerOverworld_CanTick())
        return;

    MultiplayerOverworld_BuildLocalSnapshot(&snapshot, sSession.localPlayerId, sSession.tick);
    sSession.localClientFrame++;
    if (sSession.localClientFrame == 0)
        sSession.localClientFrame++;
    sSession.localSnapshotSequence++;
    if (sSession.localSnapshotSequence == 0)
        sSession.localSnapshotSequence++;
    snapshot.clientFrame = sSession.localClientFrame;
    snapshot.serverTickSeen = sSession.bridgeTick;
    snapshot.sequence = sSession.localSnapshotSequence;
    snapshot.sessionEpoch = sSession.sessionEpoch;
    snapshot.playerToken = sSession.playerToken;
    snapshot.joinNonce = sSession.joinNonce;
    snapshot.staleFrames = 0;
    snapshot.anomalyScore = sSession.anomalyScore;
    subsession = GetLocalSubsession();
    if (subsession != NULL)
    {
        snapshot.flags |= NET_PLAYER_FLAG_BUSY | NET_PLAYER_FLAG_IN_SUBSESSION;
        snapshot.subsessionId = subsession->id;
        snapshot.subsessionState = subsession->state;
        if (subsession->type == MULTIPLAYER_SUBSESSION_TRADE)
            snapshot.interactionState = MULTIPLAYER_INTERACTION_TRADE;
        else
            snapshot.interactionState = MULTIPLAYER_INTERACTION_BATTLE;
    }
    else if (BarrierIncludesPlayer(&sSession.interactionBarrier, sSession.localPlayerId))
    {
        snapshot.flags |= NET_PLAYER_FLAG_BUSY;
        if (sSession.interactionBarrier.type == MULTIPLAYER_BARRIER_SCRIPT)
            snapshot.interactionState = MULTIPLAYER_INTERACTION_SCRIPT;
        else if (sSession.interactionBarrier.type == MULTIPLAYER_BARRIER_WARP)
            snapshot.interactionState = MULTIPLAYER_INTERACTION_WARP;
    }
    if (NetTransport_WriteLocalSnapshot(&snapshot))
        sSession.players[sSession.localPlayerId] = snapshot;
}

static void PublishMoveIntent(u8 direction, u16 newKeys, u16 heldKeys)
{
    struct NetMoveIntent intent;
    const struct NetPlayerSnapshot *snapshot;

    if (!MultiplayerSession_IsOnline())
        return;
    if (sSession.localPlayerId >= MAX_NET_PLAYERS)
        return;
    snapshot = &sSession.players[sSession.localPlayerId];
    if (!snapshot->active)
        return;

    memset(&intent, 0, sizeof(intent));
    intent.header.clientFrame = sSession.localClientFrame;
    intent.header.serverTickSeen = sSession.bridgeTick;
    intent.header.actionSequence = sSession.localSnapshotSequence;
    intent.heldKeys = heldKeys;
    intent.newKeys = newKeys;
    intent.direction = direction;
    intent.movementActionId = snapshot->movementActionId;
    intent.mapGroup = snapshot->mapGroup;
    intent.mapNum = snapshot->mapNum;
    intent.x = snapshot->x;
    intent.y = snapshot->y;
    NetTransport_SendUnreliablePacket(NET_PACKET_MOVE_INTENT, &intent, sizeof(intent));
}

static void PublishInteractIntent(u8 type, u8 targetPlayerId, u8 mapGroup, u8 mapNum)
{
    struct NetInteractIntent intent;
    struct MultiplayerTransactionKey key;
    const struct NetPlayerSnapshot *snapshot;
    u32 actionSequence;

    if (!MultiplayerSession_IsOnline())
        return;
    if (sSession.localPlayerId >= MAX_NET_PLAYERS)
        return;
    snapshot = &sSession.players[sSession.localPlayerId];
    if (!snapshot->active)
        return;

    memset(&intent, 0, sizeof(intent));
    actionSequence = NextActionSequence();
    MultiplayerCommit_BuildKey(&key, sSession.sessionEpoch, sSession.localPlayerId, NET_PACKET_INTERACT_INTENT, NET_SUBSESSION_NONE, actionSequence);
    intent.header.clientFrame = sSession.localClientFrame;
    intent.header.serverTickSeen = sSession.bridgeTick;
    intent.header.actionSequence = actionSequence;
    intent.header.transactionId = MultiplayerCommit_GetTransactionId(&key);
    intent.targetPlayerId = targetPlayerId;
    intent.interactionType = type;
    intent.mapGroup = mapGroup;
    intent.mapNum = mapNum;
    intent.x = snapshot->x;
    intent.y = snapshot->y;
    NetTransport_SendPacket(NET_PACKET_INTERACT_INTENT, &intent, sizeof(intent));
}

static void SendProfileAck(u32 profileHash, u8 result, u16 detail);

static u16 GetCatalogChunkCount(void)
{
    if (gModCatalogEntryCount == 0)
        return 0;

    return (gModCatalogEntryCount + NET_CATALOG_CHUNK_ENTRY_COUNT - 1) / NET_CATALOG_CHUNK_ENTRY_COUNT;
}

static void SendModCatalog(void)
{
    struct NetClientCatalogBegin begin;
    struct NetClientCatalogChunk chunk;
    u16 chunkCount;
    u16 chunkIndex;
    u16 firstEntry;
    u16 remaining;
    u8 entryCount;
    u8 i;

    chunkCount = GetCatalogChunkCount();
    memset(&begin, 0, sizeof(begin));
    begin.catalogHash = gModCatalogHash;
    begin.entryCount = gModCatalogEntryCount;
    begin.chunkCount = chunkCount;
    begin.schemaHash = MOD_CATALOG_SCHEMA_HASH;
    if (!NetTransport_SendPacket(NET_PACKET_CLIENT_CATALOG_BEGIN, &begin, sizeof(begin)))
        return;

    for (chunkIndex = 0; chunkIndex < chunkCount; chunkIndex++)
    {
        firstEntry = chunkIndex * NET_CATALOG_CHUNK_ENTRY_COUNT;
        remaining = gModCatalogEntryCount - firstEntry;
        if (remaining > NET_CATALOG_CHUNK_ENTRY_COUNT)
            entryCount = NET_CATALOG_CHUNK_ENTRY_COUNT;
        else
            entryCount = remaining;

        memset(&chunk, 0, sizeof(chunk));
        chunk.catalogHash = gModCatalogHash;
        chunk.chunkIndex = chunkIndex;
        chunk.firstEntry = firstEntry;
        chunk.entryCount = entryCount;
        for (i = 0; i < entryCount; i++)
            chunk.entries[i] = gModCatalogEntries[firstEntry + i];
        if (!NetTransport_SendPacket(NET_PACKET_CLIENT_CATALOG_CHUNK, &chunk, sizeof(chunk)))
            return;
    }
}

static void ProcessInboundPackets(void)
{
    u8 i;
    u8 payload[NET_TRANSPORT_PACKET_PAYLOAD_SIZE];
    u16 payloadSize;
    struct NetPacketEnvelope envelope;
    const struct NetServerProfileBegin *profileBegin;
    const struct NetServerProfileChunk *profileChunk;
    const struct NetServerProfileCommit *profileCommit;
    u8 profileResult;

    for (i = 0; i < NET_RELIABLE_QUEUE_SIZE; i++)
    {
        payloadSize = 0;
        if (!NetTransport_ReceivePacket(&envelope, payload, sizeof(payload), &payloadSize))
            return;

        switch (envelope.packetType)
        {
        case NET_PACKET_COMMIT_RESULT:
            if (payloadSize == sizeof(struct NetCommitResult))
                MultiplayerCommit_ApplyServerResult((const struct NetCommitResult *)payload);
            break;
        case NET_PACKET_RESYNC_REQUEST:
            sSession.healthState = MULTIPLAYER_HEALTH_RESYNCING;
            break;
        case NET_PACKET_DISCONNECT_REASON:
            AbortLocalSubsessionsForDisconnect();
            sSession.healthState = MULTIPLAYER_HEALTH_DISCONNECTED;
            break;
        case NET_PACKET_SERVER_PROFILE_BEGIN:
            if (payloadSize == sizeof(struct NetServerProfileBegin))
            {
                profileBegin = (const struct NetServerProfileBegin *)payload;

                if (profileBegin->profileProtocolVersion != MOD_RUNTIME_PROFILE_PROTOCOL_VERSION)
                    SendProfileAck(profileBegin->profileHash, MOD_RUNTIME_PROFILE_RESULT_UNSUPPORTED_VERSION, profileBegin->profileProtocolVersion);
                else if ((profileBegin->capabilityFlags & ~MOD_RUNTIME_PROFILE_CAPABILITIES) != 0 || profileBegin->capabilityHash != MOD_RUNTIME_PROFILE_CAPABILITY_HASH)
                    SendProfileAck(profileBegin->profileHash, MOD_RUNTIME_PROFILE_RESULT_UNSUPPORTED_CAPABILITY, 0);
                else if (!ModRuntimeProfile_BeginReceive(profileBegin->profileHash, profileBegin->profileSize, profileBegin->chunkCount))
                    SendProfileAck(profileBegin->profileHash, MOD_RUNTIME_PROFILE_RESULT_OUT_OF_MEMORY, profileBegin->profileSize);
            }
            break;
        case NET_PACKET_SERVER_PROFILE_CHUNK:
            if (payloadSize == sizeof(struct NetServerProfileChunk))
            {
                profileChunk = (const struct NetServerProfileChunk *)payload;

                if (!ModRuntimeProfile_ReceiveChunk(profileChunk->profileHash, profileChunk->chunkIndex, profileChunk->offset, profileChunk->data, profileChunk->dataSize))
                    SendProfileAck(profileChunk->profileHash, MOD_RUNTIME_PROFILE_RESULT_BAD_SIZE, profileChunk->chunkIndex);
            }
            break;
        case NET_PACKET_SERVER_PROFILE_COMMIT:
            if (payloadSize == sizeof(struct NetServerProfileCommit))
            {
                profileCommit = (const struct NetServerProfileCommit *)payload;
                profileResult = ModRuntimeProfile_CommitReceive(profileCommit->profileHash);

                SendProfileAck(profileCommit->profileHash, profileResult, 0);
            }
            break;
        case NET_PACKET_SERVER_CATALOG_REQUEST:
            if (payloadSize == sizeof(struct NetServerCatalogRequest))
                SendModCatalog();
            break;
        default:
            break;
        }
    }
}

static void SendProfileAck(u32 profileHash, u8 result, u16 detail)
{
    struct NetServerProfileAck ack;

#if FEATURE_MULTIPLAYER_SMOKE_STATUS
    sLastSmokeProfileAckHash = profileHash;
    sLastSmokeProfileAckResult = result;
    UpdateSmokeStatus();
#endif

    memset(&ack, 0, sizeof(ack));
    ack.profileHash = profileHash;
    ack.result = result;
    ack.detail = detail;
    NetTransport_SendPacket(NET_PACKET_SERVER_PROFILE_ACK, &ack, sizeof(ack));
}

static void HandleTransportLoss(void)
{
    if (sSession.state == MULTIPLAYER_SESSION_OFFLINE || sSession.state == MULTIPLAYER_SESSION_CONNECTING)
    {
        ResetSession();
        if (sConnectRequested)
            sSession.state = MULTIPLAYER_SESSION_CONNECTING;
        return;
    }

    if (sTransportLossFrames < NET_PLAYER_DISCONNECT_FRAMES)
        sTransportLossFrames++;

    MultiplayerOverworld_Reset();
    if (sTransportLossFrames >= NET_PLAYER_DISCONNECT_FRAMES)
    {
        AbortLocalSubsessionsForDisconnect();
        ResetSession();
        if (sConnectRequested)
            sSession.state = MULTIPLAYER_SESSION_CONNECTING;
    }
    else if (sTransportLossFrames >= NET_PLAYER_STALE_FRAMES)
    {
        sSession.healthState = MULTIPLAYER_HEALTH_STALE;
    }
    else
    {
        sSession.healthState = MULTIPLAYER_HEALTH_DEGRADED;
    }
}

#endif

void MultiplayerSession_Init(void)
{
#if FEATURE_MULTIPLAYER
    ResetSession();
    NetTransport_Init();
    MultiplayerCommit_Init();
    MultiplayerOverworld_Init();
    MultiplayerCompanionSaveBeacon_Init();
#if FEATURE_MULTIPLAYER_AUTOCONNECT
    EngineRuntimeState_SetMultiplayerMode(OPTIONS_MULTIPLAYER_MODE_ONLINE);
    MultiplayerSession_RequestConnect();
#endif
#endif
}

void MultiplayerSession_Tick(void)
{
#if FEATURE_MULTIPLAYER
    struct NetTransportSessionView view;
    struct NetTransportSessionView sanitizedView;

    MultiplayerSession_RefreshRuntimeMode();
    if (!RuntimeAllowsOnline())
    {
#if FEATURE_MULTIPLAYER_SMOKE_STATUS
        UpdateSmokeStatus();
#endif
        return;
    }

    MultiplayerCompanionSaveBeacon_Tick(sSession.tick, sSession.state, sSession.healthState, sSession.localPlayerId, sSession.playerCount);

    if (!sConnectRequested && sSession.state == MULTIPLAYER_SESSION_OFFLINE)
    {
#if FEATURE_MULTIPLAYER_SMOKE_STATUS
        UpdateSmokeStatus();
#endif
        return;
    }

    NetTransport_Tick();
    if (!NetTransport_ReadSessionView(&view))
    {
        HandleTransportLoss();
#if FEATURE_MULTIPLAYER_SMOKE_STATUS
        UpdateSmokeStatus();
#endif
        return;
    }

    if (!SanitizeSessionView(&view, &sanitizedView))
    {
        MultiplayerOverworld_Reset();
        sSession.state = MULTIPLAYER_SESSION_ERROR;
#if FEATURE_MULTIPLAYER_SMOKE_STATUS
        UpdateSmokeStatus();
#endif
        return;
    }

    CopyViewIntoSession(&sanitizedView);
    ProcessInboundPackets();
    if (!PublishClientHello())
        PublishHeartbeat();
    PublishLocalSnapshot();
    MultiplayerOverworld_Tick(&sSession);
#if FEATURE_MULTIPLAYER_SMOKE_STATUS
    UpdateSmokeStatus();
#endif
#endif
}

void MultiplayerSession_OnMapLoad(void)
{
#if FEATURE_MULTIPLAYER
    if (RuntimeAllowsOnline())
        MultiplayerOverworld_OnMapLoad();
#endif
}

void MultiplayerSession_OnPlayerStep(u8 direction, u16 newKeys, u16 heldKeys)
{
#if FEATURE_MULTIPLAYER
    if (!RuntimeAllowsOnline())
        return;
    MultiplayerOverworld_OnPlayerStep(direction, newKeys, heldKeys);
    PublishMoveIntent(direction, newKeys, heldKeys);
#endif
}

void MultiplayerSession_OnBattleStart(u32 battleTypeFlags)
{
#if FEATURE_MULTIPLAYER
    if (RuntimeAllowsOnline())
        MultiplayerBattle_OnBattleStart(battleTypeFlags);
#endif
}

void MultiplayerSession_OnBattleEnd(u32 battleOutcome)
{
#if FEATURE_MULTIPLAYER
    if (RuntimeAllowsOnline())
        MultiplayerBattle_OnBattleEnd(battleOutcome);
#endif
}

void MultiplayerSession_RequestConnect(void)
{
#if FEATURE_MULTIPLAYER
    if (!RuntimeAllowsOnline())
        return;
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
    AbortLocalSubsessionsForDisconnect();
    MultiplayerOverworld_Reset();
    ResetSession();
#endif
}

void MultiplayerSession_RefreshRuntimeMode(void)
{
#if FEATURE_MULTIPLAYER
    if (RuntimeAllowsOnline())
    {
        if (!sConnectRequested)
            MultiplayerSession_RequestConnect();
    }
    else if (sConnectRequested
          || sSession.state != MULTIPLAYER_SESSION_OFFLINE
          || MultiplayerOverworld_GetActiveRemoteAvatarCount() != 0)
    {
        MultiplayerSession_RequestDisconnect();
    }
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
    if (!RuntimeAllowsOnline())
        return FALSE;
    if (sSession.healthState == MULTIPLAYER_HEALTH_DISCONNECTED
     || sSession.healthState == MULTIPLAYER_HEALTH_RESYNCING)
        return FALSE;

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

u32 MultiplayerSession_GetSessionEpoch(void)
{
#if FEATURE_MULTIPLAYER
    return sSession.sessionEpoch;
#else
    return 0;
#endif
}

u32 MultiplayerSession_GetServerClockSeconds(void)
{
#if FEATURE_MULTIPLAYER
    return sSession.serverClockSeconds;
#else
    return 0;
#endif
}

u32 MultiplayerSession_NextActionSequence(void)
{
#if FEATURE_MULTIPLAYER
    return NextActionSequence();
#else
    return 0;
#endif
}

bool8 MultiplayerSession_BuildTransactionKey(struct MultiplayerTransactionKey *key, u8 packetType, u8 subsessionId, u32 actionSequence)
{
#if FEATURE_MULTIPLAYER
    if (key == NULL || sSession.localPlayerId >= MAX_NET_PLAYERS || sSession.sessionEpoch == 0 || actionSequence == 0)
        return FALSE;

    MultiplayerCommit_BuildKey(key, sSession.sessionEpoch, sSession.localPlayerId, packetType, subsessionId, actionSequence);
    return TRUE;
#else
    (void)key;
    (void)packetType;
    (void)subsessionId;
    (void)actionSequence;
    return FALSE;
#endif
}

bool8 MultiplayerSession_IsPlayerActive(u8 playerId)
{
#if FEATURE_MULTIPLAYER
    if (!IsValidPlayerId(playerId))
        return FALSE;

    return sSession.players[playerId].active
        && sSession.players[playerId].playerId == playerId
        && !SnapshotIsStale(&sSession.players[playerId])
        && SnapshotTickIsFresh(&sSession.players[playerId], sSession.tick);
#else
    return FALSE;
#endif
}

bool8 MultiplayerSession_IsPlayerBusy(u8 playerId)
{
#if FEATURE_MULTIPLAYER
    if (!MultiplayerSession_IsPlayerActive(playerId))
        return FALSE;

    return SnapshotInteractionBlocks(&sSession.players[playerId])
        || MultiplayerSession_IsPlayerInSubsession(playerId);
#else
    return FALSE;
#endif
}

bool8 MultiplayerSession_IsPlayerInteractionBlocked(u8 playerId)
{
#if FEATURE_MULTIPLAYER
    if (!IsValidPlayerId(playerId))
        return FALSE;
    if (sSession.players[playerId].active && SnapshotIsStale(&sSession.players[playerId]))
        return TRUE;
    if (!MultiplayerSession_IsPlayerActive(playerId))
        return FALSE;

    return MultiplayerSession_IsPlayerBusy(playerId)
        || BarrierIncludesPlayer(&sSession.interactionBarrier, playerId);
#else
    return FALSE;
#endif
}

bool8 MultiplayerSession_IsPlayerInSubsession(u8 playerId)
{
#if FEATURE_MULTIPLAYER
    u8 i;

    for (i = 0; i < MAX_NET_SUBSESSIONS; i++)
    {
        if (BlockingSubsessionIncludesPlayer(&sSession.subsessions[i], playerId))
            return TRUE;
    }
#endif
    return FALSE;
}

bool8 MultiplayerSession_ArePlayersOnSameMap(u8 playerCount, const u8 *players)
{
#if FEATURE_MULTIPLAYER
    u8 i;
    u8 mapGroup;
    u8 mapNum;

    if (playerCount == 0 || playerCount > MAX_NET_PLAYERS || players == NULL)
        return FALSE;
    if (!MultiplayerSession_IsPlayerActive(players[0]))
        return FALSE;

    mapGroup = sSession.players[players[0]].mapGroup;
    mapNum = sSession.players[players[0]].mapNum;
    for (i = 1; i < playerCount; i++)
    {
        if (!MultiplayerSession_IsPlayerActive(players[i]))
            return FALSE;
        if (sSession.players[players[i]].mapGroup != mapGroup || sSession.players[players[i]].mapNum != mapNum)
            return FALSE;
    }

    return TRUE;
#else
    return FALSE;
#endif
}

bool8 MultiplayerSession_ArePlayersWithinRange(u8 playerCount, const u8 *players, u16 maxDistance)
{
#if FEATURE_MULTIPLAYER
    u8 i;
    s32 originX;
    s32 originY;

    if (!MultiplayerSession_ArePlayersOnSameMap(playerCount, players))
        return FALSE;

    originX = sSession.players[players[0]].x;
    originY = sSession.players[players[0]].y;
    for (i = 1; i < playerCount; i++)
    {
        s32 dx = (s32)sSession.players[players[i]].x - originX;
        s32 dy = (s32)sSession.players[players[i]].y - originY;

        if (dx < 0)
            dx = -dx;
        if (dy < 0)
            dy = -dy;
        if (dx + dy > maxDistance)
            return FALSE;
    }

    return TRUE;
#else
    return FALSE;
#endif
}

bool8 MultiplayerSession_StartInteractionBarrier(u8 type, u8 playerCount, const u8 *players)
{
#if FEATURE_MULTIPLAYER
    u8 i;
    u8 j;
    u8 targetPlayerId = NET_PLAYER_NONE;
    u8 mapGroup;
    u8 mapNum;

    if (!BarrierTypeIsValid(type))
        return FALSE;
    if (playerCount == 0 || playerCount > MAX_NET_BATTLE_PLAYERS || players == NULL)
        return FALSE;
    if (sSession.localPlayerId == NET_PLAYER_NONE || !MultiplayerSession_IsOnline())
        return FALSE;
    if (sSession.healthState != MULTIPLAYER_HEALTH_HEALTHY)
        return FALSE;
    if (sSession.interactionBarrier.active)
        return FALSE;
    if (!MultiplayerSession_ArePlayersOnSameMap(playerCount, players))
        return FALSE;

    mapGroup = sSession.players[players[0]].mapGroup;
    mapNum = sSession.players[players[0]].mapNum;
    for (i = 0; i < playerCount; i++)
    {
        if (MultiplayerSession_IsPlayerInteractionBlocked(players[i]))
            return FALSE;
        if (players[i] != sSession.localPlayerId && targetPlayerId == NET_PLAYER_NONE)
            targetPlayerId = players[i];

        for (j = i + 1; j < playerCount; j++)
        {
            if (players[i] == players[j])
                return FALSE;
        }
    }

    memset(&sSession.interactionBarrier, 0, sizeof(sSession.interactionBarrier));
    sSession.interactionBarrier.active = TRUE;
    sSession.interactionBarrier.type = type;
    sSession.interactionBarrier.ownerPlayerId = sSession.localPlayerId;
    sSession.interactionBarrier.playerCount = playerCount;
    sSession.interactionBarrier.mapGroup = mapGroup;
    sSession.interactionBarrier.mapNum = mapNum;
    sSession.interactionBarrier.timeoutFrames = NET_INTERACTION_BARRIER_TIMEOUT_FRAMES;
    memcpy(sSession.interactionBarrier.players, players, playerCount);
    if (type == MULTIPLAYER_BARRIER_SCRIPT || type == MULTIPLAYER_BARRIER_WARP)
        PublishInteractIntent(type, targetPlayerId, mapGroup, mapNum);
    return TRUE;
#else
    return FALSE;
#endif
}

void MultiplayerSession_ClearInteractionBarrier(u8 type)
{
#if FEATURE_MULTIPLAYER
    if (!sSession.interactionBarrier.active)
        return;
    if (type != MULTIPLAYER_BARRIER_NONE && sSession.interactionBarrier.type != type)
        return;

    ResetInteractionBarrier();
#endif
}

bool8 MultiplayerSession_StartSubsession(u8 type, u8 playerCount, const u8 *players)
{
#if FEATURE_MULTIPLAYER
    u8 i;
    u8 j;
    u8 maxPlayers;
    u8 barrierType;

    maxPlayers = GetMaxPlayersForSubsession(type);
    if (maxPlayers == 0 || playerCount == 0 || playerCount > maxPlayers || players == NULL)
        return FALSE;
    if (!MultiplayerSession_IsHost())
        return FALSE;
    if (!MultiplayerSession_ArePlayersOnSameMap(playerCount, players))
        return FALSE;

    for (i = 0; i < playerCount; i++)
    {
        if (MultiplayerSession_IsPlayerInteractionBlocked(players[i]))
            return FALSE;

        for (j = i + 1; j < playerCount; j++)
        {
            if (players[i] == players[j])
                return FALSE;
        }
    }

    barrierType = GetBarrierTypeForSubsession(type);
    if (!MultiplayerSession_StartInteractionBarrier(barrierType, playerCount, players))
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
            if (!NetTransport_SendPacket(NET_PACKET_SUBSESSION_INVITE, &sSession.subsessions[i], sizeof(sSession.subsessions[i])))
            {
                memset(&sSession.subsessions[i], 0, sizeof(sSession.subsessions[i]));
                MultiplayerSession_ClearInteractionBarrier(barrierType);
                return FALSE;
            }
            return TRUE;
        }
    }

    MultiplayerSession_ClearInteractionBarrier(barrierType);
#endif
    return FALSE;
}

void MultiplayerSession_EndSubsession(u8 subsessionId, u8 state)
{
#if FEATURE_MULTIPLAYER
    u8 i;
    u8 packetType;

    if (subsessionId == NET_SUBSESSION_NONE)
        return;
    if (!SubsessionStateIsTerminal(state))
        state = MULTIPLAYER_SUBSESSION_STATE_ERROR;

    for (i = 0; i < MAX_NET_SUBSESSIONS; i++)
    {
        if (sSession.subsessions[i].active && sSession.subsessions[i].id == subsessionId)
        {
            if (sSession.subsessions[i].type == MULTIPLAYER_SUBSESSION_TRADE
             && state == MULTIPLAYER_SUBSESSION_STATE_ENDING)
                state = MULTIPLAYER_SUBSESSION_STATE_ERROR;
            packetType = (state == MULTIPLAYER_SUBSESSION_STATE_ENDING) ? NET_PACKET_SUBSESSION_COMMIT : NET_PACKET_SUBSESSION_ABORT;
            sSession.subsessions[i].state = state;
            NetTransport_SendPacket(packetType, &sSession.subsessions[i], sizeof(sSession.subsessions[i]));
            memset(&sSession.subsessions[i], 0, sizeof(sSession.subsessions[i]));
            MultiplayerSession_ClearInteractionBarrier(MULTIPLAYER_BARRIER_NONE);
            return;
        }
    }
#endif
}
