#ifndef GUARD_MULTIPLAYER_BRIDGE_MAILBOX_H
#define GUARD_MULTIPLAYER_BRIDGE_MAILBOX_H

#include "global.h"
#include "multiplayer/transport.h"

#define NET_EMULATOR_MAILBOX_SYMBOL_NAME "gNetEmulatorBridgeMailbox"
#define NET_EMULATOR_MAILBOX_MEMORY_DOMAIN "EWRAM"
#define NET_EMULATOR_MAILBOX_MIN_ADDRESS EWRAM_START
#define NET_EMULATOR_MAILBOX_MAX_ADDRESS (EWRAM_END - 1)
#define NET_EMULATOR_MAILBOX_MAX_SIZE 4096

struct NetEmulatorBridgeBuffer
{
    u32 magic;
    u16 version;
    u8 connected;
    u8 local_player_id;
    u8 host_player_id;
    u8 player_count;
    u8 transport_mode;
    u8 reserved;
    u32 session_id;
    u32 session_epoch;
    u32 player_token;
    u32 join_nonce;
    u32 bridge_tick;
    u32 server_clock_seconds;
    u32 view_sequence;
    u32 local_snapshot_sequence;
    u32 reliable_outbound_head;
    u32 reliable_outbound_tail;
    u32 reliable_inbound_head;
    u32 reliable_inbound_tail;
    u32 latest_unreliable_sequence;
    struct NetPlayerSnapshot server_players[MAX_NET_PLAYERS];
    struct MultiplayerSubsession server_subsessions[MAX_NET_SUBSESSIONS];
    struct NetPlayerSnapshot local_snapshot;
    struct NetTransportPacketSlot latest_unreliable;
    struct NetTransportPacketSlot reliable_outbound[NET_RELIABLE_QUEUE_SIZE];
    struct NetTransportPacketSlot reliable_inbound[NET_RELIABLE_QUEUE_SIZE];
} __attribute__((packed));

STATIC_ASSERT(sizeof(struct NetEmulatorBridgeBuffer) <= NET_EMULATOR_MAILBOX_MAX_SIZE, NetEmulatorBridgeMailboxTooLarge);

extern volatile struct NetEmulatorBridgeBuffer gNetEmulatorBridgeMailbox;

#endif // GUARD_MULTIPLAYER_BRIDGE_MAILBOX_H
