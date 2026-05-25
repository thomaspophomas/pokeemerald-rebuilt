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
    u8 localPlayerId;
    u8 hostPlayerId;
    u8 playerCount;
    u8 transportMode;
    u8 reserved;
    u32 sessionId;
    u32 sessionEpoch;
    u32 playerToken;
    u32 joinNonce;
    u32 bridgeTick;
    u32 serverClockSeconds;
    u32 viewSequence;
    u32 localSnapshotSequence;
    u32 reliableOutboundHead;
    u32 reliableOutboundTail;
    u32 reliableInboundHead;
    u32 reliableInboundTail;
    u32 latestUnreliableSequence;
    struct NetPlayerSnapshot serverPlayers[MAX_NET_PLAYERS];
    struct MultiplayerSubsession serverSubsessions[MAX_NET_SUBSESSIONS];
    struct NetPlayerSnapshot localSnapshot;
    struct NetTransportPacketSlot latestUnreliable;
    struct NetTransportPacketSlot reliableOutbound[NET_RELIABLE_QUEUE_SIZE];
    struct NetTransportPacketSlot reliableInbound[NET_RELIABLE_QUEUE_SIZE];
} __attribute__((packed));

STATIC_ASSERT(sizeof(struct NetEmulatorBridgeBuffer) <= NET_EMULATOR_MAILBOX_MAX_SIZE, NetEmulatorBridgeMailboxTooLarge);

extern volatile struct NetEmulatorBridgeBuffer gNetEmulatorBridgeMailbox;

#endif // GUARD_MULTIPLAYER_BRIDGE_MAILBOX_H
