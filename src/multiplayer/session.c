#include "global.h"
#include "engine/runtime_state.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "event_scripts.h"
#include "global.fieldmap.h"
#include "mod/npc.h"
#include "multiplayer/session.h"
#include "multiplayer/commit.h"
#include "multiplayer/transport.h"
#include "multiplayer/overworld.h"
#include "multiplayer/battle.h"
#include "script.h"

#if FEATURE_MULTIPLAYER

static EWRAM_DATA struct MultiplayerSession sSession = {0};
static EWRAM_DATA struct MultiplayerPendingTransaction sPendingTransactions[NET_PENDING_TX_COUNT] = {0};
static EWRAM_DATA bool8 sConnectRequested = FALSE;
static EWRAM_DATA bool8 sClientHelloSent = FALSE;
static EWRAM_DATA bool8 sClientHelloAcked = FALSE;
static EWRAM_DATA u16 sClientHelloRetryTimer = 0;
static EWRAM_DATA u8 sClientHelloRetryCount = 0;
static EWRAM_DATA u16 sHeartbeatTimer = 0;
static EWRAM_DATA u16 sTransportLossFrames = 0;
static EWRAM_DATA bool8 sPlayerControlReady = FALSE;
static EWRAM_DATA bool8 sPlayerProfileInitialized = FALSE;
static EWRAM_DATA bool8 sAutoconnectPending = FALSE;
static EWRAM_DATA struct
{
    bool8 active;
    u8 objectEventId;
    u8 facing;
    const u8 *script;
    struct MultiplayerInteractionTarget target;
    struct MultiplayerTransactionKey key;
} sPendingInteraction = {0};

static void ClearPendingInteraction(bool8 rollback);
static void ClearPendingTransactionForKey(const struct MultiplayerTransactionKey *key);

static bool8 RuntimeAllowsOnline(void)
{
    return EngineRuntimeState_IsMultiplayerOnlineEnabled();
}

static bool8 PlayerProfileFieldsAreComplete(void)
{
    u8 i;

    if (gSaveBlock2Ptr == NULL)
        return FALSE;
    if (gSaveBlock2Ptr->playerName[0] == EOS)
        return FALSE;
    if (gSaveBlock2Ptr->playerGender != MALE && gSaveBlock2Ptr->playerGender != FEMALE)
        return FALSE;
    for (i = 0; i < PLAYER_NAME_LENGTH; i++)
    {
        if (gSaveBlock2Ptr->playerName[i] == EOS)
            return TRUE;
    }

    return TRUE;
}

static bool8 PlayerProfileIsComplete(void)
{
    return sPlayerProfileInitialized && PlayerProfileFieldsAreComplete();
}

static u32 GetPlayerTrainerId32(void)
{
    if (gSaveBlock2Ptr == NULL)
        return 0;

    return gSaveBlock2Ptr->playerTrainerId[0]
        | (gSaveBlock2Ptr->playerTrainerId[1] << 8)
        | (gSaveBlock2Ptr->playerTrainerId[2] << 16)
        | (gSaveBlock2Ptr->playerTrainerId[3] << 24);
}

static u16 HashPlayerName(void)
{
    u8 i;
    u16 hash = 0x811C;

    if (gSaveBlock2Ptr == NULL)
        return 0;

    for (i = 0; i < PLAYER_NAME_LENGTH && gSaveBlock2Ptr->playerName[i] != EOS; i++)
        hash = (hash ^ gSaveBlock2Ptr->playerName[i]) * 167;

    if (hash == 0)
        hash = 1;
    return hash;
}

static u8 CopyPlayerDisplayName(u8 *dest)
{
    u8 i;

    if (dest == NULL)
        return 0;

    memset(dest, EOS, PLAYER_NAME_LENGTH + 1);
    if (gSaveBlock2Ptr == NULL)
        return 0;

    for (i = 0; i < PLAYER_NAME_LENGTH && gSaveBlock2Ptr->playerName[i] != EOS; i++)
        dest[i] = gSaveBlock2Ptr->playerName[i];
    dest[i] = EOS;
    return i;
}

static void ResetSession(void)
{
    memset(&sSession, 0, sizeof(sSession));
    memset(sPendingTransactions, 0, sizeof(sPendingTransactions));
    memset(&sPendingInteraction, 0, sizeof(sPendingInteraction));
    sSession.state = MULTIPLAYER_SESSION_OFFLINE;
    sSession.localPlayerId = NET_PLAYER_NONE;
    sSession.hostPlayerId = NET_PLAYER_NONE;
    sSession.transportMode = NET_TRANSPORT_MODE_SERVER_BRIDGE;
    sSession.healthState = MULTIPLAYER_HEALTH_DISCONNECTED;
    sClientHelloSent = FALSE;
    sClientHelloAcked = FALSE;
    sClientHelloRetryTimer = 0;
    sClientHelloRetryCount = 0;
    sHeartbeatTimer = 0;
    sTransportLossFrames = 0;
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
    return view->transportMode == NET_TRANSPORT_MODE_SERVER_BRIDGE
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

static bool8 InteractionTargetIsValid(const struct MultiplayerInteractionTarget *target)
{
    if (target == NULL)
        return FALSE;
    if (target->targetKind == MULTIPLAYER_INTERACTION_TARGET_NONE)
        return FALSE;
    if (!MapLocationIsValid(target->mapGroup, target->mapNum))
        return FALSE;
    if (target->x < NET_PLAYER_COORD_MIN || target->x > NET_PLAYER_COORD_MAX)
        return FALSE;
    if (target->y < NET_PLAYER_COORD_MIN || target->y > NET_PLAYER_COORD_MAX)
        return FALSE;
    if (!ElevationIsValid(target->elevation))
        return FALSE;
    if (target->interactionPolicy > MOD_NPC_INTERACTION_DISABLED_ONLINE)
        return FALSE;

    return TRUE;
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
    ClearPendingInteraction(TRUE);
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
    if (sSession.interactionBarrier.type == MULTIPLAYER_BARRIER_SCRIPT && !sPendingInteraction.active)
    {
        if (ArePlayerFieldControlsLocked())
        {
            sSession.interactionBarrier.timeoutFrames = NET_INTERACTION_BARRIER_TIMEOUT_FRAMES;
            return;
        }

        ResetInteractionBarrier();
        return;
    }
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

static void SetConnectionStatus(u8 status)
{
    if (EngineRuntimeState_GetConnectionStatus() != status)
        EngineRuntimeState_SetConnectionStatus(status);
}

static void ClearPendingTransactions(void)
{
    memset(sPendingTransactions, 0, sizeof(sPendingTransactions));
}

static bool8 InteractionTargetsEqual(const struct MultiplayerInteractionTarget *left, const struct MultiplayerInteractionTarget *right)
{
    return left->targetKind == right->targetKind
        && left->mapGroup == right->mapGroup
        && left->mapNum == right->mapNum
        && left->localId == right->localId
        && left->elevation == right->elevation
        && left->x == right->x
        && left->y == right->y
        && left->scriptHash == right->scriptHash;
}

static bool8 PendingTransactionMatchesKey(const struct MultiplayerPendingTransaction *pending, const struct MultiplayerTransactionKey *key)
{
    return pending->active
        && pending->key.sessionEpoch == key->sessionEpoch
        && pending->key.actionSequence == key->actionSequence
        && pending->key.playerId == key->playerId
        && pending->key.packetType == key->packetType
        && pending->key.subsessionId == key->subsessionId;
}

static bool8 PendingTransactionMatchesResult(const struct MultiplayerPendingTransaction *pending, const struct NetCommitResult *result)
{
    return pending->active
        && pending->key.sessionEpoch == result->sessionEpoch
        && pending->key.actionSequence == result->actionSequence
        && pending->key.playerId == result->playerId
        && pending->key.packetType == result->packetType
        && pending->key.subsessionId == result->subsessionId;
}

static void ClearPendingTransactionForResult(const struct NetCommitResult *result)
{
    u8 i;

    if (result == NULL)
        return;

    for (i = 0; i < NET_PENDING_TX_COUNT; i++)
    {
        if (PendingTransactionMatchesResult(&sPendingTransactions[i], result))
        {
            memset(&sPendingTransactions[i], 0, sizeof(sPendingTransactions[i]));
            return;
        }
    }
}

static void ClearPendingTransactionForKey(const struct MultiplayerTransactionKey *key)
{
    u8 i;

    if (key == NULL)
        return;

    for (i = 0; i < NET_PENDING_TX_COUNT; i++)
    {
        if (PendingTransactionMatchesKey(&sPendingTransactions[i], key))
        {
            memset(&sPendingTransactions[i], 0, sizeof(sPendingTransactions[i]));
            return;
        }
    }
}

static void ClearPendingInteraction(bool8 rollback)
{
    if (!sPendingInteraction.active)
        return;

    ClearPendingTransactionForKey(&sPendingInteraction.key);
    if (rollback)
    {
        MultiplayerCommit_Rollback(
            &sPendingInteraction.key,
            MULTIPLAYER_COMMIT_NONE,
            &sPendingInteraction.target,
            sizeof(sPendingInteraction.target),
            NULL);
        UnlockPlayerFieldControls();
    }
    memset(&sPendingInteraction, 0, sizeof(sPendingInteraction));
}

static struct MultiplayerPendingTransaction *GetOrAllocPendingTransaction(const struct MultiplayerTransactionKey *key)
{
    u8 i;

    for (i = 0; i < NET_PENDING_TX_COUNT; i++)
    {
        if (PendingTransactionMatchesKey(&sPendingTransactions[i], key))
            return &sPendingTransactions[i];
    }

    for (i = 0; i < NET_PENDING_TX_COUNT; i++)
    {
        if (!sPendingTransactions[i].active)
            return &sPendingTransactions[i];
    }

    return NULL;
}

static bool8 SendPendingTransactionNow(struct MultiplayerPendingTransaction *pending)
{
    if (pending == NULL || !pending->active)
        return FALSE;
    if (NetTransport_SendPacket(pending->packetType, pending->payload, pending->payloadSize))
    {
        pending->retryTimer = NET_PENDING_TX_RETRY_FRAMES;
        return TRUE;
    }

    SetConnectionStatus(NET_CONNECTION_STATUS_BACKPRESSURE);
    return FALSE;
}

static void ReplayPendingTransactions(void)
{
    u8 i;

    for (i = 0; i < NET_PENDING_TX_COUNT; i++)
    {
        if (!sPendingTransactions[i].active)
            continue;
        if (sPendingTransactions[i].retryTimer != 0)
        {
            sPendingTransactions[i].retryTimer--;
            continue;
        }
        if (sPendingTransactions[i].retryCount >= NET_PENDING_TX_MAX_RETRIES)
        {
            MultiplayerCommit_Rollback(
                &sPendingTransactions[i].key,
                sPendingTransactions[i].commitType,
                sPendingTransactions[i].payload,
                sPendingTransactions[i].payloadSize,
                NULL);
            memset(&sPendingTransactions[i], 0, sizeof(sPendingTransactions[i]));
            SetConnectionStatus(NET_CONNECTION_STATUS_STALE);
            continue;
        }

        sPendingTransactions[i].retryCount++;
        SendPendingTransactionNow(&sPendingTransactions[i]);
    }
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
        sClientHelloAcked = FALSE;
        sClientHelloRetryTimer = 0;
        sClientHelloRetryCount = 0;
        sHeartbeatTimer = 0;
        sSession.localClientFrame = 0;
        sSession.localSnapshotSequence = 0;
        sSession.localActionSequence = 0;
        ClearPendingTransactions();
        MultiplayerCommit_Init();
    }
    sSession.healthState = MULTIPLAYER_HEALTH_HEALTHY;
    SetConnectionStatus(sClientHelloAcked ? NET_CONNECTION_STATUS_CONNECTED : NET_CONNECTION_STATUS_CONNECTING);
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

    if (sClientHelloAcked)
        return FALSE;
    if (sClientHelloRetryTimer != 0)
    {
        sClientHelloRetryTimer--;
        return TRUE;
    }
    if (sClientHelloRetryCount >= NET_CLIENT_HELLO_MAX_RETRIES)
    {
        SetConnectionStatus(NET_CONNECTION_STATUS_STALE);
        return TRUE;
    }
    if (sSession.sessionEpoch == 0 || sSession.playerToken == 0 || sSession.joinNonce == 0)
        return FALSE;

    memset(&hello, 0, sizeof(hello));
    hello.protocolVersion = NET_PROTOCOL_VERSION;
    hello.bridgeVersion = NET_EMULATOR_BRIDGE_VERSION;
    hello.buildId = NET_PROTOCOL_BUILD_ID;
    hello.romHash = NET_PROTOCOL_ROM_HASH;
    hello.rulesetHash = NET_RULESET_HASH;
    hello.featureFlags = BuildFeatureFlags();
    hello.trainerId = GetPlayerTrainerId32();
    hello.nameHash = HashPlayerName();
    hello.gender = gSaveBlock2Ptr != NULL ? gSaveBlock2Ptr->playerGender : 0xFF;
    hello.displayNameLength = CopyPlayerDisplayName(hello.displayName);
    hello.transportMode = NET_TRANSPORT_MODE_SERVER_BRIDGE;
    hello.profileReady = PlayerProfileIsComplete();
    hello.controlReady = sPlayerControlReady;

    if (NetTransport_SendPacket(NET_PACKET_CLIENT_HELLO, &hello, sizeof(hello)))
    {
        sClientHelloSent = TRUE;
        sClientHelloRetryCount++;
        sClientHelloRetryTimer = NET_CLIENT_HELLO_RETRY_FRAMES;
        sHeartbeatTimer = NET_HEARTBEAT_INTERVAL_FRAMES;
        SetConnectionStatus(NET_CONNECTION_STATUS_CONNECTING);
        return TRUE;
    }

    SetConnectionStatus(NET_CONNECTION_STATUS_BACKPRESSURE);
    return FALSE;
}

static bool8 ServerHelloAckIsValid(const struct NetServerHelloAck *ack)
{
    if (ack == NULL || !ack->accepted)
        return FALSE;
    if (ack->assignedPlayerId >= MAX_NET_PLAYERS || ack->hostPlayerId >= MAX_NET_PLAYERS)
        return FALSE;
    if (ack->sessionId == 0 || ack->sessionEpoch == 0 || ack->playerToken == 0 || ack->joinNonce == 0)
        return FALSE;
    if (ack->sessionId != sSession.sessionId || ack->sessionEpoch != sSession.sessionEpoch)
        return FALSE;
    if (ack->playerToken != sSession.playerToken || ack->joinNonce != sSession.joinNonce)
        return FALSE;
    if (sSession.bridgeTick != 0 && ack->serverTick != 0)
    {
        if (ack->serverTick < sSession.bridgeTick
         && sSession.bridgeTick - ack->serverTick > NET_PLAYER_SNAPSHOT_FUTURE_SKEW_FRAMES)
            return FALSE;
    }

    return TRUE;
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

    if (!sClientHelloAcked)
        return;
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

static bool8 SendInteractIntent(u8 type, u8 targetPlayerId, const struct MultiplayerInteractionTarget *target, const struct MultiplayerTransactionKey *key)
{
    struct NetInteractIntent intent;
    const struct NetPlayerSnapshot *snapshot;

    if (!MultiplayerSession_IsOnline())
        return FALSE;
    if (sSession.localPlayerId >= MAX_NET_PLAYERS)
        return FALSE;
    if (target == NULL || key == NULL)
        return FALSE;
    snapshot = &sSession.players[sSession.localPlayerId];
    if (!snapshot->active)
        return FALSE;

    memset(&intent, 0, sizeof(intent));
    intent.header.clientFrame = sSession.localClientFrame;
    intent.header.serverTickSeen = sSession.bridgeTick;
    intent.header.actionSequence = key->actionSequence;
    intent.header.transactionId = MultiplayerCommit_GetTransactionId(key);
    intent.targetPlayerId = targetPlayerId;
    intent.interactionType = type;
    intent.mapGroup = target->mapGroup;
    intent.mapNum = target->mapNum;
    intent.targetKind = target->targetKind;
    intent.targetLocalId = target->localId;
    intent.targetElevation = target->elevation;
    intent.interactionPolicy = target->interactionPolicy;
    intent.x = target->x;
    intent.y = target->y;
    intent.scriptHash = target->scriptHash;
    return MultiplayerSession_SendReliableAction(NET_PACKET_INTERACT_INTENT, MULTIPLAYER_COMMIT_NONE, key, &intent, sizeof(intent));
}

static void PublishInteractIntent(u8 type, u8 targetPlayerId, u8 mapGroup, u8 mapNum)
{
    struct MultiplayerInteractionTarget target;
    struct MultiplayerTransactionKey key;
    const struct NetPlayerSnapshot *snapshot;
    u32 actionSequence;

    if (!MultiplayerSession_IsOnline() || sSession.localPlayerId >= MAX_NET_PLAYERS)
        return;
    snapshot = &sSession.players[sSession.localPlayerId];
    if (!snapshot->active)
        return;

    memset(&target, 0, sizeof(target));
    target.targetKind = MULTIPLAYER_INTERACTION_TARGET_METATILE;
    target.interactionPolicy = MOD_NPC_INTERACTION_EXCLUSIVE;
    target.mapGroup = mapGroup;
    target.mapNum = mapNum;
    target.elevation = snapshot->elevation;
    target.x = snapshot->x;
    target.y = snapshot->y;
    actionSequence = NextActionSequence();
    if (!MultiplayerSession_BuildTransactionKey(&key, NET_PACKET_INTERACT_INTENT, NET_SUBSESSION_NONE, actionSequence))
        return;
    SendInteractIntent(type, targetPlayerId, &target, &key);
}

static bool8 InteractionLockResultMatchesPending(const struct NetInteractionLockResult *result)
{
    if (!sPendingInteraction.active || result == NULL)
        return FALSE;
    if (result->sessionEpoch != sPendingInteraction.key.sessionEpoch)
        return FALSE;
    if (result->actionSequence != sPendingInteraction.key.actionSequence)
        return FALSE;
    if (result->playerId != sPendingInteraction.key.playerId)
        return FALSE;
    if (result->targetKind != sPendingInteraction.target.targetKind)
        return FALSE;
    if (result->targetLocalId != sPendingInteraction.target.localId)
        return FALSE;
    if (result->mapGroup != sPendingInteraction.target.mapGroup || result->mapNum != sPendingInteraction.target.mapNum)
        return FALSE;
    if (result->targetElevation != sPendingInteraction.target.elevation)
        return FALSE;
    if (result->x != sPendingInteraction.target.x || result->y != sPendingInteraction.target.y)
        return FALSE;
    if (result->scriptHash != sPendingInteraction.target.scriptHash)
        return FALSE;

    return TRUE;
}

static bool8 StartGrantedInteractionScript(void)
{
    u8 objectEventId = sPendingInteraction.objectEventId;

    if (!sPendingInteraction.active || sPendingInteraction.script == NULL)
        return FALSE;

    if (sPendingInteraction.target.targetKind == MULTIPLAYER_INTERACTION_TARGET_NPC)
    {
        if (objectEventId >= OBJECT_EVENTS_COUNT
         || !gObjectEvents[objectEventId].active
         || gObjectEvents[objectEventId].localId != sPendingInteraction.target.localId)
        {
            if (!TryGetObjectEventIdByLocalIdAndMap(
                    sPendingInteraction.target.localId,
                    sPendingInteraction.target.mapNum,
                    sPendingInteraction.target.mapGroup,
                    &objectEventId))
                return FALSE;
        }

        gSelectedObjectEvent = objectEventId;
        gSpecialVar_LastTalked = sPendingInteraction.target.localId;
        gSpecialVar_Facing = sPendingInteraction.facing;
    }

    ClearPendingTransactionForKey(&sPendingInteraction.key);
    ScriptContext_SetupScript(sPendingInteraction.script);
    memset(&sPendingInteraction, 0, sizeof(sPendingInteraction));
    return TRUE;
}

static void HandleInteractionLockResult(const struct NetInteractionLockResult *result)
{
    struct MultiplayerTransactionKey key;
    struct MultiplayerInteractionTarget target;

    if (!InteractionLockResultMatchesPending(result))
        return;
    target = sPendingInteraction.target;

    MultiplayerCommit_BuildKey(
        &key,
        result->sessionEpoch,
        result->playerId,
        NET_PACKET_INTERACT_INTENT,
        NET_SUBSESSION_NONE,
        result->actionSequence);

    if (result->result == MULTIPLAYER_INTERACTION_LOCK_GRANTED)
    {
        if (StartGrantedInteractionScript())
            MultiplayerCommit_Commit(&key, MULTIPLAYER_COMMIT_NONE, &target, sizeof(target), NULL);
        else
        {
            MultiplayerCommit_Rollback(&key, MULTIPLAYER_COMMIT_NONE, &target, sizeof(target), NULL);
            ClearPendingInteraction(FALSE);
            memset(&sSession.interactionBarrier, 0, sizeof(sSession.interactionBarrier));
            UnlockPlayerFieldControls();
            ScriptContext_SetupScript(EventScript_MultiplayerNpcBusy);
        }
        return;
    }

    MultiplayerCommit_Rollback(&key, MULTIPLAYER_COMMIT_NONE, &target, sizeof(target), NULL);
    ClearPendingInteraction(FALSE);
    memset(&sSession.interactionBarrier, 0, sizeof(sSession.interactionBarrier));
    UnlockPlayerFieldControls();
    ScriptContext_SetupScript(EventScript_MultiplayerNpcBusy);
}

static void ProcessInboundPackets(void)
{
    u8 i;
    u8 payload[NET_TRANSPORT_PACKET_PAYLOAD_SIZE];
    u16 payloadSize;
    struct NetPacketEnvelope envelope;

    for (i = 0; i < NET_RELIABLE_QUEUE_SIZE; i++)
    {
        payloadSize = 0;
        if (!NetTransport_ReceivePacket(&envelope, payload, sizeof(payload), &payloadSize))
            return;

        switch (envelope.packetType)
        {
        case NET_PACKET_SERVER_HELLO_ACK:
            if (payloadSize == sizeof(struct NetServerHelloAck))
            {
                const struct NetServerHelloAck *ack = (const struct NetServerHelloAck *)payload;

                if (ServerHelloAckIsValid(ack))
                {
                    sClientHelloAcked = TRUE;
                    sClientHelloSent = TRUE;
                    sSession.localPlayerId = ack->assignedPlayerId;
                    sSession.hostPlayerId = ack->hostPlayerId;
                    sSession.sessionId = ack->sessionId;
                    sSession.sessionEpoch = ack->sessionEpoch;
                    sSession.playerToken = ack->playerToken;
                    sSession.joinNonce = ack->joinNonce;
                    sSession.bridgeTick = ack->serverTick;
                    sSession.serverClockSeconds = ack->serverClockSeconds;
                    SetConnectionStatus(NET_CONNECTION_STATUS_CONNECTED);
                }
                else if (!ack->accepted)
                {
                    sClientHelloAcked = FALSE;
                    sSession.state = MULTIPLAYER_SESSION_ERROR;
                    SetConnectionStatus(ack->reason != NET_CONNECTION_STATUS_NONE ? ack->reason : NET_CONNECTION_STATUS_SERVER_REFUSED);
                }
                else
                {
                    sClientHelloAcked = FALSE;
                    sSession.state = MULTIPLAYER_SESSION_ERROR;
                    SetConnectionStatus(NET_CONNECTION_STATUS_SERVER_REFUSED);
                }
            }
            break;
        case NET_PACKET_COMMIT_RESULT:
            if (payloadSize == sizeof(struct NetCommitResult))
            {
                if (MultiplayerCommit_ApplyServerResult((const struct NetCommitResult *)payload))
                    ClearPendingTransactionForResult((const struct NetCommitResult *)payload);
            }
            break;
        case NET_PACKET_INTERACTION_LOCK_RESULT:
            if (payloadSize == sizeof(struct NetInteractionLockResult))
                HandleInteractionLockResult((const struct NetInteractionLockResult *)payload);
            break;
        case NET_PACKET_RESYNC_REQUEST:
            sSession.healthState = MULTIPLAYER_HEALTH_RESYNCING;
            SetConnectionStatus(NET_CONNECTION_STATUS_RESYNCING);
            break;
        case NET_PACKET_DISCONNECT_REASON:
            AbortLocalSubsessionsForDisconnect();
            sSession.healthState = MULTIPLAYER_HEALTH_DISCONNECTED;
            SetConnectionStatus(NET_CONNECTION_STATUS_DISCONNECTED);
            break;
        default:
            break;
        }
    }
}

static void HandleTransportLoss(u8 status)
{
    if (status == NET_CONNECTION_STATUS_NONE)
        status = NET_CONNECTION_STATUS_BRIDGE_MISSING;
    SetConnectionStatus(status);
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
        SetConnectionStatus(NET_CONNECTION_STATUS_STALE);
    }
    else
    {
        sSession.healthState = MULTIPLAYER_HEALTH_DEGRADED;
        SetConnectionStatus(NET_CONNECTION_STATUS_CONNECTING);
    }
}

#endif

void MultiplayerSession_Init(void)
{
#if FEATURE_MULTIPLAYER
    sPlayerControlReady = FALSE;
    sPlayerProfileInitialized = FALSE;
    ResetSession();
    NetTransport_Init();
    MultiplayerCommit_Init();
    MultiplayerOverworld_Init();
#if FEATURE_MULTIPLAYER_AUTOCONNECT
    sAutoconnectPending = TRUE;
#else
    sAutoconnectPending = FALSE;
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
        return;
    if (!MultiplayerSession_CanRunOnlineLifecycle())
        return;
    if (!sConnectRequested && sSession.state == MULTIPLAYER_SESSION_OFFLINE)
        return;

    NetTransport_Tick();
    NetTransport_SetServerConfig(EngineRuntimeState_GetServerConfig());
    if (!NetTransport_ReadSessionView(&view))
    {
        HandleTransportLoss(NetTransport_GetConnectionStatus());
        return;
    }

    if (!SanitizeSessionView(&view, &sanitizedView))
    {
        MultiplayerOverworld_Reset();
        sSession.state = MULTIPLAYER_SESSION_ERROR;
        return;
    }

    CopyViewIntoSession(&sanitizedView);
    ProcessInboundPackets();
    if (!sClientHelloAcked)
    {
        PublishClientHello();
        return;
    }

    PublishHeartbeat();
    ReplayPendingTransactions();
    PublishLocalSnapshot();
    MultiplayerOverworld_Tick(&sSession);
#endif
}

void MultiplayerSession_OnMapLoad(void)
{
#if FEATURE_MULTIPLAYER
    if (MultiplayerSession_CanRunOnlineLifecycle())
        MultiplayerOverworld_OnMapLoad();
#endif
}

void MultiplayerSession_OnPlayerStep(u8 direction, u16 newKeys, u16 heldKeys)
{
#if FEATURE_MULTIPLAYER
    if (!MultiplayerSession_CanRunOnlineLifecycle())
        return;
    MultiplayerOverworld_OnPlayerStep(direction, newKeys, heldKeys);
    PublishMoveIntent(direction, newKeys, heldKeys);
#endif
}

void MultiplayerSession_OnBattleStart(u32 battleTypeFlags)
{
#if FEATURE_MULTIPLAYER
    if (MultiplayerSession_CanRunOnlineLifecycle())
        MultiplayerBattle_OnBattleStart(battleTypeFlags);
#endif
}

void MultiplayerSession_OnBattleEnd(u32 battleOutcome)
{
#if FEATURE_MULTIPLAYER
    if (MultiplayerSession_CanRunOnlineLifecycle())
        MultiplayerBattle_OnBattleEnd(battleOutcome);
#endif
}

void MultiplayerSession_RequestConnect(void)
{
#if FEATURE_MULTIPLAYER
    if (!MultiplayerSession_CanRunOnlineLifecycle())
        return;
    sConnectRequested = TRUE;
    SetConnectionStatus(NET_CONNECTION_STATUS_CONNECTING);
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
    ClearPendingTransactions();
    MultiplayerOverworld_Reset();
    ResetSession();
    SetConnectionStatus(NET_CONNECTION_STATUS_DISCONNECTED);
#endif
}

void MultiplayerSession_RefreshRuntimeMode(void)
{
#if FEATURE_MULTIPLAYER
#if FEATURE_MULTIPLAYER_AUTOCONNECT
    if (sAutoconnectPending && sPlayerControlReady && PlayerProfileIsComplete() && !RuntimeAllowsOnline())
    {
        sAutoconnectPending = FALSE;
        EngineRuntimeState_SetMultiplayerMode(OPTIONS_MULTIPLAYER_MODE_ONLINE);
    }
#endif

    if (RuntimeAllowsOnline())
    {
        if (!MultiplayerSession_CanRunOnlineLifecycle())
        {
            if (sConnectRequested
             || sSession.state != MULTIPLAYER_SESSION_OFFLINE
             || MultiplayerOverworld_GetActiveRemoteAvatarCount() != 0)
                MultiplayerSession_RequestDisconnect();
            return;
        }
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

void MultiplayerSession_SuspendForNewGame(void)
{
#if FEATURE_MULTIPLAYER
    sPlayerControlReady = FALSE;
    sPlayerProfileInitialized = FALSE;
#if FEATURE_MULTIPLAYER_AUTOCONNECT
    sAutoconnectPending = TRUE;
#endif
    sConnectRequested = FALSE;
    AbortLocalSubsessionsForDisconnect();
    ClearPendingTransactions();
    MultiplayerCommit_Init();
    MultiplayerOverworld_Reset();
    ResetSession();
    SetConnectionStatus(NET_CONNECTION_STATUS_DISCONNECTED);
#endif
}

void MultiplayerSession_MarkPlayerControlReady(void)
{
#if FEATURE_MULTIPLAYER
    if (sPlayerControlReady)
        return;
    if (!PlayerProfileFieldsAreComplete())
        return;
    if (ArePlayerFieldControlsLocked())
        return;
    if (!MultiplayerOverworld_CanTick())
        return;

    sPlayerProfileInitialized = TRUE;
    sPlayerControlReady = TRUE;
#endif
}

bool8 MultiplayerSession_CanRunOnlineLifecycle(void)
{
#if FEATURE_MULTIPLAYER
    return RuntimeAllowsOnline()
        && sPlayerControlReady
        && PlayerProfileIsComplete();
#else
    return FALSE;
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
    if (!RuntimeAllowsOnline() || !sClientHelloAcked || !MultiplayerSession_CanRunOnlineLifecycle())
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

bool8 MultiplayerSession_SendReliableAction(u8 packetType, u8 commitType, const struct MultiplayerTransactionKey *key, const void *payload, u16 payloadSize)
{
#if FEATURE_MULTIPLAYER
    struct MultiplayerPendingTransaction *pending;

    if (key == NULL || payload == NULL || payloadSize == 0 || payloadSize > NET_TRANSPORT_PACKET_PAYLOAD_SIZE)
        return FALSE;
    if (!MultiplayerSession_IsOnline())
        return FALSE;

    MultiplayerCommit_Prepare(key, commitType, payload, payloadSize, NULL);
    pending = GetOrAllocPendingTransaction(key);
    if (pending == NULL)
    {
        MultiplayerCommit_Rollback(key, commitType, payload, payloadSize, NULL);
        SetConnectionStatus(NET_CONNECTION_STATUS_BACKPRESSURE);
        return FALSE;
    }

    memset(pending, 0, sizeof(*pending));
    pending->active = TRUE;
    pending->packetType = packetType;
    pending->commitType = commitType;
    pending->key = *key;
    pending->payloadSize = payloadSize;
    memcpy(pending->payload, payload, payloadSize);
    SendPendingTransactionNow(pending);
    return TRUE;
#else
    (void)packetType;
    (void)commitType;
    (void)key;
    (void)payload;
    (void)payloadSize;
    return FALSE;
#endif
}

u8 MultiplayerSession_PreflightInteraction(const struct MultiplayerInteractionTarget *target, const u8 *script, u8 objectEventId, u8 facing)
{
#if FEATURE_MULTIPLAYER
    struct MultiplayerTransactionKey key;
    u32 actionSequence;

    if (!RuntimeAllowsOnline() || !MultiplayerSession_IsOnline())
        return MULTIPLAYER_INTERACTION_PREFLIGHT_START;
    if (script == NULL || !InteractionTargetIsValid(target))
        return MULTIPLAYER_INTERACTION_PREFLIGHT_DENIED;
    if (target->interactionPolicy == MOD_NPC_INTERACTION_SHARED_READONLY)
        return MULTIPLAYER_INTERACTION_PREFLIGHT_START;
    if (target->interactionPolicy == MOD_NPC_INTERACTION_DISABLED_ONLINE)
        return MULTIPLAYER_INTERACTION_PREFLIGHT_DENIED;
    if (sSession.localPlayerId >= MAX_NET_PLAYERS || sSession.healthState != MULTIPLAYER_HEALTH_HEALTHY)
        return MULTIPLAYER_INTERACTION_PREFLIGHT_BUSY;
    if (sSession.interactionBarrier.active)
    {
        if (sPendingInteraction.active && InteractionTargetsEqual(&sPendingInteraction.target, target))
            return MULTIPLAYER_INTERACTION_PREFLIGHT_WAIT;
        return MULTIPLAYER_INTERACTION_PREFLIGHT_BUSY;
    }
    if (MultiplayerSession_IsPlayerInteractionBlocked(sSession.localPlayerId))
        return MULTIPLAYER_INTERACTION_PREFLIGHT_BUSY;

    actionSequence = NextActionSequence();
    if (!MultiplayerSession_BuildTransactionKey(&key, NET_PACKET_INTERACT_INTENT, NET_SUBSESSION_NONE, actionSequence))
        return MULTIPLAYER_INTERACTION_PREFLIGHT_DENIED;

    memset(&sSession.interactionBarrier, 0, sizeof(sSession.interactionBarrier));
    sSession.interactionBarrier.active = TRUE;
    sSession.interactionBarrier.type = MULTIPLAYER_BARRIER_SCRIPT;
    sSession.interactionBarrier.ownerPlayerId = sSession.localPlayerId;
    sSession.interactionBarrier.playerCount = 1;
    sSession.interactionBarrier.players[0] = sSession.localPlayerId;
    sSession.interactionBarrier.mapGroup = target->mapGroup;
    sSession.interactionBarrier.mapNum = target->mapNum;
    sSession.interactionBarrier.timeoutFrames = NET_INTERACTION_BARRIER_TIMEOUT_FRAMES;

    memset(&sPendingInteraction, 0, sizeof(sPendingInteraction));
    sPendingInteraction.active = TRUE;
    sPendingInteraction.objectEventId = objectEventId;
    sPendingInteraction.facing = facing;
    sPendingInteraction.script = script;
    sPendingInteraction.target = *target;
    sPendingInteraction.key = key;
    LockPlayerFieldControls();

    if (!SendInteractIntent(MULTIPLAYER_BARRIER_SCRIPT, NET_PLAYER_NONE, target, &key))
    {
        ResetInteractionBarrier();
        return MULTIPLAYER_INTERACTION_PREFLIGHT_BUSY;
    }

    return MULTIPLAYER_INTERACTION_PREFLIGHT_WAIT;
#else
    (void)target;
    (void)script;
    (void)objectEventId;
    (void)facing;
    return MULTIPLAYER_INTERACTION_PREFLIGHT_START;
#endif
}

bool8 MultiplayerSession_TryStartWarpBarrier(u8 mapGroup, u8 mapNum, u8 warpId, s16 x, s16 y)
{
#if FEATURE_MULTIPLAYER
    struct MultiplayerInteractionTarget target;
    struct MultiplayerTransactionKey key;
    const struct NetPlayerSnapshot *snapshot;
    u32 actionSequence;

    if (!RuntimeAllowsOnline())
        return TRUE;
    if (!MultiplayerSession_IsOnline())
        return FALSE;
    if (sSession.localPlayerId >= MAX_NET_PLAYERS || sSession.healthState != MULTIPLAYER_HEALTH_HEALTHY)
        return FALSE;
    if (sSession.interactionBarrier.active || MultiplayerSession_IsPlayerInteractionBlocked(sSession.localPlayerId))
        return FALSE;

    snapshot = &sSession.players[sSession.localPlayerId];
    if (!snapshot->active)
        return FALSE;

    memset(&target, 0, sizeof(target));
    target.targetKind = MULTIPLAYER_INTERACTION_TARGET_METATILE;
    target.interactionPolicy = MOD_NPC_INTERACTION_EXCLUSIVE;
    target.mapGroup = snapshot->mapGroup;
    target.mapNum = snapshot->mapNum;
    target.localId = warpId;
    target.elevation = snapshot->elevation;
    target.x = snapshot->x;
    target.y = snapshot->y;
    target.scriptHash = ((u16)mapGroup << 8) ^ mapNum ^ ((u16)warpId << 4) ^ (u16)x ^ (u16)y;

    if (!InteractionTargetIsValid(&target))
        return FALSE;

    actionSequence = NextActionSequence();
    if (!MultiplayerSession_BuildTransactionKey(&key, NET_PACKET_INTERACT_INTENT, NET_SUBSESSION_NONE, actionSequence))
        return FALSE;

    memset(&sSession.interactionBarrier, 0, sizeof(sSession.interactionBarrier));
    sSession.interactionBarrier.active = TRUE;
    sSession.interactionBarrier.type = MULTIPLAYER_BARRIER_WARP;
    sSession.interactionBarrier.ownerPlayerId = sSession.localPlayerId;
    sSession.interactionBarrier.playerCount = 1;
    sSession.interactionBarrier.players[0] = sSession.localPlayerId;
    sSession.interactionBarrier.mapGroup = target.mapGroup;
    sSession.interactionBarrier.mapNum = target.mapNum;
    sSession.interactionBarrier.timeoutFrames = NET_INTERACTION_BARRIER_TIMEOUT_FRAMES;

    if (!SendInteractIntent(MULTIPLAYER_BARRIER_WARP, NET_PLAYER_NONE, &target, &key))
    {
        ResetInteractionBarrier();
        return FALSE;
    }

    return FALSE;
#else
    (void)mapGroup;
    (void)mapNum;
    (void)warpId;
    (void)x;
    (void)y;
    return TRUE;
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
