#ifndef GUARD_MULTIPLAYER_PROTOCOL_H
#define GUARD_MULTIPLAYER_PROTOCOL_H

#include "global.h"
#include "multiplayer/constants.h"
#include "mod/catalog.h"

enum NetPacketType
{
    NET_PACKET_NONE,
    NET_PACKET_PLAYER_SNAPSHOT,
    NET_PACKET_CLIENT_HELLO,
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
    NET_PACKET_SERVER_PROFILE_BEGIN,
    NET_PACKET_SERVER_PROFILE_CHUNK,
    NET_PACKET_SERVER_PROFILE_COMMIT,
    NET_PACKET_SERVER_PROFILE_ACK,
    NET_PACKET_SERVER_CATALOG_REQUEST,
    NET_PACKET_CLIENT_CATALOG_BEGIN,
    NET_PACKET_CLIENT_CATALOG_CHUNK,
    NET_PACKET_PLAYER_BATTLE_PROFILE,
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
    u16 profileProtocolVersion;
    u32 profileCapabilityFlags;
    u32 profileCapabilityHash;
    u32 modCatalogHash;
    u16 modCatalogCount;
    u8 transportMode;
    u8 reserved;
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
    s16 x;
    s16 y;
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
    u32 moneyAmount;
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
    u32 serverRevision;
    u16 payloadChecksum;
    u8 commitType;
    u8 result_code;
    u16 detail;
} __attribute__((packed));

struct NetDisconnectReason
{
    u16 reason;
    u16 detail;
} __attribute__((packed));

struct NetServerProfileBegin
{
    u32 profileHash;
    u16 profileSize;
    u16 chunkCount;
    u16 profileProtocolVersion;
    u16 reserved;
    u32 capabilityFlags;
    u32 capabilityHash;
} __attribute__((packed));

struct NetServerProfileChunk
{
    u32 profileHash;
    u16 chunkIndex;
    u16 offset;
    u8 dataSize;
    u8 reserved;
    u8 data[NET_PROFILE_CHUNK_DATA_SIZE];
} __attribute__((packed));

struct NetServerProfileCommit
{
    u32 profileHash;
} __attribute__((packed));

struct NetServerProfileAck
{
    u32 profileHash;
    u8 result_code;
    u8 reserved;
    u16 detail;
} __attribute__((packed));

struct NetServerCatalogRequest
{
    u32 catalogHash;
    u16 knownEntryCount;
    u16 reserved;
} __attribute__((packed));

struct NetClientCatalogBegin
{
    u32 catalogHash;
    u16 entryCount;
    u16 chunkCount;
    u32 schemaHash;
} __attribute__((packed));

struct NetClientCatalogChunk
{
    u32 catalogHash;
    u16 chunkIndex;
    u16 firstEntry;
    u8 entryCount;
    u8 reserved[3];
    struct ModCatalogEntry entries[NET_CATALOG_CHUNK_ENTRY_COUNT];
} __attribute__((packed));

u16 NetProtocol_CalcChecksum(const void *checksum_data, u16 checksum_data_size);
u32 NetProtocol_MakeTransactionId(u32 session_epoch, u8 player_id, u8 packet_type, u8 subsession_id, u32 action_sequence);
void NetProtocol_InitEnvelope(struct NetPacketEnvelope *envelope, u8 packet_type, u8 player_id, u32 session_id, u32 tick, u16 payload_size);
void NetProtocol_InitEnvelopeWithEpoch(struct NetPacketEnvelope *envelope, u8 packet_type, u8 player_id, u32 session_id, u32 session_epoch, u32 tick, u16 payload_size);
bool8 NetProtocol_ValidateEnvelope(const struct NetPacketEnvelope *envelope, u8 packet_type, u16 payload_size);

#endif // GUARD_MULTIPLAYER_PROTOCOL_H
