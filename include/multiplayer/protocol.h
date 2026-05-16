#ifndef GUARD_MULTIPLAYER_PROTOCOL_H
#define GUARD_MULTIPLAYER_PROTOCOL_H

#include "global.h"
#include "multiplayer/constants.h"

enum NetPacketType
{
    NET_PACKET_NONE,
    NET_PACKET_PLAYER_SNAPSHOT,
    NET_PACKET_CLIENT_HELLO,
    NET_PACKET_SERVER_HELLO_ACK,
    NET_PACKET_HEARTBEAT,
    NET_PACKET_MOVE_INTENT,
    NET_PACKET_INTERACT_INTENT,
    NET_PACKET_BATTLE_ACTION,
    NET_PACKET_TRADE_ACTION,
    NET_PACKET_RESYNC_ACK,
    NET_PACKET_SERVER_SESSION_VIEW,
    NET_PACKET_SERVER_CLOCK,
    NET_PACKET_RESYNC_REQUEST,
    NET_PACKET_BARRIER_UPDATE,
    NET_PACKET_INTERACTION_LOCK_RESULT,
    NET_PACKET_SUBSESSION_UPDATE,
    NET_PACKET_COMMIT_RESULT,
    NET_PACKET_DISCONNECT_REASON,
    NET_PACKET_SUBSESSION_INVITE,
    NET_PACKET_SUBSESSION_ACCEPT,
    NET_PACKET_SUBSESSION_DECLINE,
    NET_PACKET_SUBSESSION_COMMIT,
    NET_PACKET_SUBSESSION_ABORT,
    NET_PACKET_BATTLE_INPUT,
    NET_PACKET_TRADE_INPUT,
    NET_PACKET_COUNT,
};

struct NetPacketEnvelope
{
    u32 magic;
    u16 protocolVersion;
    u16 headerSize;
    u32 sessionId;
    u32 sessionEpoch;
    u32 tick;
    u8 packetType;
    u8 playerId;
    u8 flags;
    u8 reserved;
    u32 sequence;
    u32 ack;
    u16 payloadSize;
    u16 checksum;
} __attribute__((packed));

struct NetClientHello
{
    u16 protocolVersion;
    u16 bridgeVersion;
    u32 buildId;
    u32 romHash;
    u32 rulesetHash;
    u32 featureFlags;
    u32 trainerId;
    u16 nameHash;
    u8 gender;
    u8 displayNameLength;
    u8 transportMode;
    bool8 profileReady;
    bool8 controlReady;
    u8 reserved;
    u8 displayName[PLAYER_NAME_LENGTH + 1];
} __attribute__((packed));

struct NetServerHelloAck
{
    bool8 accepted;
    u8 reason;
    u8 assignedPlayerId;
    u8 hostPlayerId;
    u32 sessionId;
    u32 sessionEpoch;
    u32 playerToken;
    u32 joinNonce;
    u32 serverTick;
    u32 serverClockSeconds;
} __attribute__((packed));

struct NetHeartbeat
{
    u32 clientFrame;
    u32 serverTickSeen;
    u32 sessionEpoch;
    u32 playerToken;
    u32 joinNonce;
} __attribute__((packed));

struct NetIntentHeader
{
    u32 clientFrame;
    u32 serverTickSeen;
    u32 actionSequence;
    u32 transactionId;
} __attribute__((packed));

struct NetMoveIntent
{
    struct NetIntentHeader header;
    u16 heldKeys;
    u16 newKeys;
    u8 direction;
    u8 movementActionId;
    u8 mapGroup;
    u8 mapNum;
    s16 x;
    s16 y;
} __attribute__((packed));

struct NetInteractIntent
{
    struct NetIntentHeader header;
    u8 targetPlayerId;
    u8 interactionType;
    u8 mapGroup;
    u8 mapNum;
    u8 targetKind;
    u8 targetLocalId;
    u8 targetElevation;
    u8 interactionPolicy;
    s16 x;
    s16 y;
    u16 scriptHash;
    u16 barrierId;
    u8 actionType;
    u8 resourceCount;
    u16 resourceChecksum;
} __attribute__((packed));

struct NetInteractionLockResult
{
    u32 transactionId;
    u32 sessionEpoch;
    u32 actionSequence;
    u32 serverRevision;
    u8 playerId;
    u8 result;
    u8 targetKind;
    u8 targetLocalId;
    u8 mapGroup;
    u8 mapNum;
    u8 targetElevation;
    u8 interactionType;
    s16 x;
    s16 y;
    u16 scriptHash;
    u16 barrierId;
    u8 actionType;
    u8 resourceCount;
    u16 resourceChecksum;
    u16 detail;
} __attribute__((packed));

struct NetBattleAction
{
    struct NetIntentHeader header;
    u8 subsessionId;
    u8 battlerSlot;
    u8 action;
    u8 target;
    u16 parameter;
    u16 reserved;
} __attribute__((packed));

struct NetTradeAction
{
    struct NetIntentHeader header;
    u8 subsessionId;
    u8 action;
    u16 partySlot;
    u32 tradeChecksum;
} __attribute__((packed));

struct NetServerClock
{
    u32 serverEpochSeconds;
    u32 serverTick;
    s16 timezoneOffsetMinutes;
    u16 flags;
} __attribute__((packed));

struct NetCommitResult
{
    u32 transactionId;
    u32 sessionEpoch;
    u32 actionSequence;
    u32 serverRevision;
    u16 payloadChecksum;
    u8 playerId;
    u8 packetType;
    u8 subsessionId;
    u8 reserved;
    u8 commitType;
    u8 result;
    u16 detail;
} __attribute__((packed));

struct NetDisconnectReason
{
    u16 reason;
    u16 detail;
} __attribute__((packed));

u16 NetProtocol_CalcChecksum(const void *data, u16 size);
u32 NetProtocol_MakeTransactionId(u32 sessionEpoch, u8 playerId, u8 packetType, u8 subsessionId, u32 actionSequence);
void NetProtocol_InitEnvelope(struct NetPacketEnvelope *envelope, u8 packetType, u8 playerId, u32 sessionId, u32 tick, u16 payloadSize);
void NetProtocol_InitEnvelopeWithEpoch(struct NetPacketEnvelope *envelope, u8 packetType, u8 playerId, u32 sessionId, u32 sessionEpoch, u32 tick, u16 payloadSize);
bool8 NetProtocol_ValidateEnvelope(const struct NetPacketEnvelope *envelope, u8 packetType, u16 payloadSize);

#endif // GUARD_MULTIPLAYER_PROTOCOL_H
