#ifndef GUARD_MULTIPLAYER_TYPES_H
#define GUARD_MULTIPLAYER_TYPES_H

#include "global.h"
#include "multiplayer/constants.h"

enum MultiplayerSessionState
{
    MULTIPLAYER_SESSION_OFFLINE,
    MULTIPLAYER_SESSION_CONNECTING,
    MULTIPLAYER_SESSION_LOBBY,
    MULTIPLAYER_SESSION_OVERWORLD_SYNC,
    MULTIPLAYER_SESSION_SUBSESSION,
    MULTIPLAYER_SESSION_DISCONNECTING,
    MULTIPLAYER_SESSION_ERROR,
};

enum MultiplayerSubsessionType
{
    MULTIPLAYER_SUBSESSION_NONE,
    MULTIPLAYER_SUBSESSION_PVE_BATTLE,
    MULTIPLAYER_SUBSESSION_PVP_BATTLE,
    MULTIPLAYER_SUBSESSION_TRADE,
};

enum MultiplayerSubsessionState
{
    MULTIPLAYER_SUBSESSION_STATE_NONE,
    MULTIPLAYER_SUBSESSION_STATE_INVITING,
    MULTIPLAYER_SUBSESSION_STATE_READY,
    MULTIPLAYER_SUBSESSION_STATE_ACTIVE,
    MULTIPLAYER_SUBSESSION_STATE_ENDING,
    MULTIPLAYER_SUBSESSION_STATE_ERROR,
};

enum MultiplayerInteractionState
{
    MULTIPLAYER_INTERACTION_NONE,
    MULTIPLAYER_INTERACTION_SCRIPT,
    MULTIPLAYER_INTERACTION_WARP,
    MULTIPLAYER_INTERACTION_BATTLE,
    MULTIPLAYER_INTERACTION_TRADE,
};

enum MultiplayerInteractionBarrierType
{
    MULTIPLAYER_BARRIER_NONE,
    MULTIPLAYER_BARRIER_SCRIPT,
    MULTIPLAYER_BARRIER_WARP,
    MULTIPLAYER_BARRIER_BATTLE_INVITE,
    MULTIPLAYER_BARRIER_TRADE_INVITE,
};

enum MultiplayerInteractionTargetKind
{
    MULTIPLAYER_INTERACTION_TARGET_NONE,
    MULTIPLAYER_INTERACTION_TARGET_NPC,
    MULTIPLAYER_INTERACTION_TARGET_BG_EVENT,
    MULTIPLAYER_INTERACTION_TARGET_METATILE,
    MULTIPLAYER_INTERACTION_TARGET_WATER,
};

enum MultiplayerInteractionPreflightResult
{
    MULTIPLAYER_INTERACTION_PREFLIGHT_START,
    MULTIPLAYER_INTERACTION_PREFLIGHT_WAIT,
    MULTIPLAYER_INTERACTION_PREFLIGHT_BUSY,
    MULTIPLAYER_INTERACTION_PREFLIGHT_DENIED,
};

enum MultiplayerInteractionLockResult
{
    MULTIPLAYER_INTERACTION_LOCK_GRANTED,
    MULTIPLAYER_INTERACTION_LOCK_BUSY,
    MULTIPLAYER_INTERACTION_LOCK_DENIED,
    MULTIPLAYER_INTERACTION_LOCK_TIMEOUT,
};

enum MultiplayerSessionHealth
{
    MULTIPLAYER_HEALTH_DISCONNECTED,
    MULTIPLAYER_HEALTH_HEALTHY,
    MULTIPLAYER_HEALTH_DEGRADED,
    MULTIPLAYER_HEALTH_STALE,
    MULTIPLAYER_HEALTH_RESYNCING,
};

enum MultiplayerCommitState
{
    MULTIPLAYER_COMMIT_STATE_NONE,
    MULTIPLAYER_COMMIT_STATE_PREPARED,
    MULTIPLAYER_COMMIT_STATE_COMMITTED,
    MULTIPLAYER_COMMIT_STATE_ROLLED_BACK,
};

enum MultiplayerCommitType
{
    MULTIPLAYER_COMMIT_NONE,
    MULTIPLAYER_COMMIT_TRADE,
    MULTIPLAYER_COMMIT_ITEM,
    MULTIPLAYER_COMMIT_BATTLE,
    MULTIPLAYER_COMMIT_STORY_FLAG,
    MULTIPLAYER_COMMIT_OUTFIT,
    MULTIPLAYER_COMMIT_WEATHER_REWARD,
    MULTIPLAYER_COMMIT_MONEY,
    MULTIPLAYER_COMMIT_POKEMON,
};

enum MultiplayerCommitResult
{
    MULTIPLAYER_COMMIT_RESULT_PENDING,
    MULTIPLAYER_COMMIT_RESULT_OK,
    MULTIPLAYER_COMMIT_RESULT_REJECTED,
    MULTIPLAYER_COMMIT_RESULT_ROLLED_BACK,
};

struct MultiplayerTransactionKey
{
    u32 sessionEpoch;
    u32 actionSequence;
    u8 playerId;
    u8 packetType;
    u8 subsessionId;
    u8 reserved;
};

struct MultiplayerCommitLogEntry
{
    bool8 active;
    u8 state;
    u8 result;
    u8 commitType;
    struct MultiplayerTransactionKey key;
    u32 serverRevision;
    u16 payloadChecksum;
    u16 detail;
};

struct MultiplayerPendingTransaction
{
    bool8 active;
    u8 packetType;
    u8 commitType;
    u8 retryCount;
    u16 retryTimer;
    u16 payloadSize;
    struct MultiplayerTransactionKey key;
    u8 payload[NET_TRANSPORT_PACKET_PAYLOAD_SIZE];
};

struct MultiplayerInteractionTarget
{
    u8 targetKind;
    u8 interactionPolicy;
    u8 mapGroup;
    u8 mapNum;
    u8 localId;
    u8 elevation;
    s16 x;
    s16 y;
    u16 scriptHash;
};

struct NetPlayerSnapshot
{
    bool8 active;
    u8 playerId;
    u8 mapGroup;
    u8 mapNum;
    s16 x;
    s16 y;
    u8 elevation;
    u8 facingDirection;
    u8 movementActionId;
    u8 avatarGraphicsId;
    u8 outfitId;
    u8 interactionState;
    u8 subsessionId;
    u8 subsessionState;
    u16 graphicsRevision;
    u16 flags;
    u32 tick;
    u32 clientFrame;
    u32 serverTickSeen;
    u32 sequence;
    u32 sessionEpoch;
    u32 playerToken;
    u32 joinNonce;
    u16 staleFrames;
    u8 anomalyScore;
    u8 trustFlags;
};

struct MultiplayerSubsession
{
    bool8 active;
    u8 id;
    u8 type;
    u8 state;
    u8 hostPlayerId;
    u8 playerCount;
    u8 players[MAX_NET_BATTLE_PLAYERS];
    u16 timeoutFrames;
};

struct MultiplayerInteractionBarrier
{
    bool8 active;
    u8 type;
    u8 ownerPlayerId;
    u8 playerCount;
    u8 players[MAX_NET_BATTLE_PLAYERS];
    u8 mapGroup;
    u8 mapNum;
    u16 timeoutFrames;
};

struct MultiplayerSession
{
    u8 state;
    u8 localPlayerId;
    u8 hostPlayerId;
    u8 playerCount;
    u16 lastError;
    u32 sessionId;
    u32 sessionEpoch;
    u32 playerToken;
    u32 joinNonce;
    u32 tick;
    u32 bridgeTick;
    u32 serverClockSeconds;
    u32 localClientFrame;
    u32 localSnapshotSequence;
    u32 localActionSequence;
    u8 transportMode;
    u8 anomalyScore;
    u8 healthState;
    u8 reserved;
    struct NetPlayerSnapshot players[MAX_NET_PLAYERS];
    struct MultiplayerSubsession subsessions[MAX_NET_SUBSESSIONS];
    struct MultiplayerInteractionBarrier interactionBarrier;
};

#endif // GUARD_MULTIPLAYER_TYPES_H
