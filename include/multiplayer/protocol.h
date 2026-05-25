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
    u16 protocol_version;
    u16 header_size;
    u32 session_id;
    u32 session_epoch;
    u32 tick;
    u8 packet_type;
    u8 player_id;
    u8 flags;
    u8 reserved;
    u32 sequence;
    u32 ack;
    u16 payload_size;
    u16 checksum;
} __attribute__((packed));

struct NetClientHello
{
    u16 protocol_version;
    u16 bridge_version;
    u32 build_id;
    u32 rom_hash;
    u32 ruleset_hash;
    u32 feature_flags;
    u16 profile_protocol_version;
    u32 profile_capability_flags;
    u32 profile_capability_hash;
    u32 mod_catalog_hash;
    u16 mod_catalog_count;
    u8 transport_mode;
    u8 reserved;
} __attribute__((packed));

struct NetHeartbeat
{
    u32 client_frame;
    u32 server_tick_seen;
    u32 session_epoch;
    u32 player_token;
    u32 join_nonce;
} __attribute__((packed));

struct NetIntentHeader
{
    u32 client_frame;
    u32 server_tick_seen;
    u32 action_sequence;
    u32 transaction_id;
} __attribute__((packed));

struct NetMoveIntent
{
    struct NetIntentHeader header;
    u16 held_keys;
    u16 new_keys;
    u8 direction;
    u8 movement_action_id;
    u8 map_group;
    u8 map_num;
    s16 x;
    s16 y;
} __attribute__((packed));

struct NetInteractIntent
{
    struct NetIntentHeader header;
    u8 target_player_id;
    u8 interaction_type;
    u8 map_group;
    u8 map_num;
    s16 x;
    s16 y;
} __attribute__((packed));

struct NetBattleAction
{
    struct NetIntentHeader header;
    u8 subsession_id;
    u8 battler_slot;
    u8 action;
    u8 target;
    u16 parameter;
    u16 reserved;
} __attribute__((packed));

struct NetTradeAction
{
    struct NetIntentHeader header;
    u8 subsession_id;
    u8 action;
    u16 party_slot;
    u32 trade_checksum;
    u32 money_amount;
} __attribute__((packed));

struct NetServerClock
{
    u32 server_epoch_seconds;
    u32 server_tick;
    s16 timezone_offset_minutes;
    u16 flags;
} __attribute__((packed));

struct NetCommitResult
{
    u32 transaction_id;
    u32 server_revision;
    u16 payload_checksum;
    u8 commit_type;
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
    u32 profile_hash;
    u16 profile_size;
    u16 chunk_count;
    u16 profile_protocol_version;
    u16 reserved;
    u32 capability_flags;
    u32 capability_hash;
} __attribute__((packed));

struct NetServerProfileChunk
{
    u32 profile_hash;
    u16 chunk_index;
    u16 offset;
    u8 data_size;
    u8 reserved;
    u8 data[NET_PROFILE_CHUNK_DATA_SIZE];
} __attribute__((packed));

struct NetServerProfileCommit
{
    u32 profile_hash;
} __attribute__((packed));

struct NetServerProfileAck
{
    u32 profile_hash;
    u8 result_code;
    u8 reserved;
    u16 detail;
} __attribute__((packed));

struct NetServerCatalogRequest
{
    u32 catalog_hash;
    u16 known_entry_count;
    u16 reserved;
} __attribute__((packed));

struct NetClientCatalogBegin
{
    u32 catalog_hash;
    u16 entry_count;
    u16 chunk_count;
    u32 schema_hash;
} __attribute__((packed));

struct NetClientCatalogChunk
{
    u32 catalog_hash;
    u16 chunk_index;
    u16 first_entry;
    u8 entry_count;
    u8 reserved[3];
    struct ModCatalogEntry entries[NET_CATALOG_CHUNK_ENTRY_COUNT];
} __attribute__((packed));

u16 NetProtocol_CalcChecksum(const void *checksum_data, u16 checksum_data_size);
u32 NetProtocol_MakeTransactionId(u32 session_epoch, u8 player_id, u8 packet_type, u8 subsession_id, u32 action_sequence);
void NetProtocol_InitEnvelope(struct NetPacketEnvelope *envelope, u8 packet_type, u8 player_id, u32 session_id, u32 tick, u16 payload_size);
void NetProtocol_InitEnvelopeWithEpoch(struct NetPacketEnvelope *envelope, u8 packet_type, u8 player_id, u32 session_id, u32 session_epoch, u32 tick, u16 payload_size);
bool8 NetProtocol_ValidateEnvelope(const struct NetPacketEnvelope *envelope, u8 packet_type, u16 payload_size);

#endif // GUARD_MULTIPLAYER_PROTOCOL_H
