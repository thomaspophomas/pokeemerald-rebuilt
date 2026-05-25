#ifndef GUARD_MULTIPLAYER_TRANSPORT_H
#define GUARD_MULTIPLAYER_TRANSPORT_H

#include "global.h"
#include "multiplayer/protocol.h"
#include "multiplayer/types.h"

#define NET_TRANSPORT_PACKET_PAYLOAD_SIZE 96
#define NET_TRANSPORT_PACKET_HEADER_SIZE 22
#define NET_TRANSPORT_PACKET_SLOT_SIZE (NET_TRANSPORT_PACKET_HEADER_SIZE + NET_TRANSPORT_PACKET_PAYLOAD_SIZE)

struct NetTransportPacketHeader
{
    u32 sessionId;
    u32 sessionEpoch;
    u32 tick;
    u32 sequence;
    u16 payloadSize;
    u16 checksum;
    u8 packetType;
    u8 playerId;
} __attribute__((packed));

struct NetTransportPacketSlot
{
    struct NetTransportPacketHeader header;
    u8 payload[NET_TRANSPORT_PACKET_PAYLOAD_SIZE];
} __attribute__((packed));

STATIC_ASSERT(sizeof(struct NetTransportPacketHeader) == NET_TRANSPORT_PACKET_HEADER_SIZE, NetTransportPacketHeaderSize);
STATIC_ASSERT(sizeof(struct NetTransportPacketSlot) == NET_TRANSPORT_PACKET_SLOT_SIZE, NetTransportPacketSlotSize);

STATIC_ASSERT(sizeof(struct NetClientHello) <= NET_TRANSPORT_PACKET_PAYLOAD_SIZE, NetClientHelloPayloadTooLarge);
STATIC_ASSERT(sizeof(struct NetHeartbeat) <= NET_TRANSPORT_PACKET_PAYLOAD_SIZE, NetHeartbeatPayloadTooLarge);
STATIC_ASSERT(sizeof(struct NetMoveIntent) <= NET_TRANSPORT_PACKET_PAYLOAD_SIZE, NetMoveIntentPayloadTooLarge);
STATIC_ASSERT(sizeof(struct NetInteractIntent) <= NET_TRANSPORT_PACKET_PAYLOAD_SIZE, NetInteractIntentPayloadTooLarge);
STATIC_ASSERT(sizeof(struct NetBattleAction) <= NET_TRANSPORT_PACKET_PAYLOAD_SIZE, NetBattleActionPayloadTooLarge);
STATIC_ASSERT(sizeof(struct NetTradeAction) <= NET_TRANSPORT_PACKET_PAYLOAD_SIZE, NetTradeActionPayloadTooLarge);
STATIC_ASSERT(sizeof(struct NetCommitResult) <= NET_TRANSPORT_PACKET_PAYLOAD_SIZE, NetCommitResultPayloadTooLarge);
STATIC_ASSERT(sizeof(struct NetServerProfileBegin) <= NET_TRANSPORT_PACKET_PAYLOAD_SIZE, NetServerProfileBeginPayloadTooLarge);
STATIC_ASSERT(sizeof(struct NetServerProfileChunk) <= NET_TRANSPORT_PACKET_PAYLOAD_SIZE, NetServerProfileChunkPayloadTooLarge);
STATIC_ASSERT(sizeof(struct NetServerProfileCommit) <= NET_TRANSPORT_PACKET_PAYLOAD_SIZE, NetServerProfileCommitPayloadTooLarge);
STATIC_ASSERT(sizeof(struct NetServerProfileAck) <= NET_TRANSPORT_PACKET_PAYLOAD_SIZE, NetServerProfileAckPayloadTooLarge);
STATIC_ASSERT(sizeof(struct NetServerCatalogRequest) <= NET_TRANSPORT_PACKET_PAYLOAD_SIZE, NetServerCatalogRequestPayloadTooLarge);
STATIC_ASSERT(sizeof(struct NetClientCatalogBegin) <= NET_TRANSPORT_PACKET_PAYLOAD_SIZE, NetClientCatalogBeginPayloadTooLarge);
STATIC_ASSERT(sizeof(struct NetClientCatalogChunk) <= NET_TRANSPORT_PACKET_PAYLOAD_SIZE, NetClientCatalogChunkPayloadTooLarge);
STATIC_ASSERT(sizeof(struct NetPlayerBattleProfile) <= NET_TRANSPORT_PACKET_PAYLOAD_SIZE, NetPlayerBattleProfilePayloadTooLarge);
STATIC_ASSERT(sizeof(struct MultiplayerSubsession) <= NET_TRANSPORT_PACKET_PAYLOAD_SIZE, MultiplayerSubsessionPayloadTooLarge);

static inline void NetTransport_CopyEnvelopeToPacketHeader(volatile struct NetTransportPacketHeader *header, const struct NetPacketEnvelope *envelope)
{
    header->sessionId = envelope->sessionId;
    header->sessionEpoch = envelope->sessionEpoch;
    header->tick = envelope->tick;
    header->sequence = envelope->sequence;
    header->payloadSize = envelope->payloadSize;
    header->checksum = envelope->checksum;
    header->packetType = envelope->packetType;
    header->playerId = envelope->playerId;
}

static inline void NetTransport_CopyPacketHeaderToEnvelope(struct NetPacketEnvelope *envelope, const volatile struct NetTransportPacketHeader *header)
{
    envelope->magic = NET_PROTOCOL_MAGIC;
    envelope->protocolVersion = NET_PROTOCOL_VERSION;
    envelope->headerSize = sizeof(*envelope);
    envelope->sessionId = header->sessionId;
    envelope->sessionEpoch = header->sessionEpoch;
    envelope->tick = header->tick;
    envelope->packetType = header->packetType;
    envelope->playerId = header->playerId;
    envelope->flags = 0;
    envelope->reserved = 0;
    envelope->sequence = header->sequence;
    envelope->ack = 0;
    envelope->payloadSize = header->payloadSize;
    envelope->checksum = header->checksum;
}

struct NetTransportSessionView
{
    bool8 connected;
    u8 localPlayerId;
    u8 hostPlayerId;
    u8 playerCount;
    u8 transportMode;
    u32 sessionId;
    u32 sessionEpoch;
    u32 playerToken;
    u32 joinNonce;
    u32 bridgeTick;
    u32 serverClockSeconds;
    struct NetPlayerSnapshot players[MAX_NET_PLAYERS];
    struct MultiplayerSubsession subsessions[MAX_NET_SUBSESSIONS];
};

void NetTransport_Init(void);
void NetTransport_Tick(void);
bool8 NetTransport_IsConnected(void);
bool8 NetTransport_ReadSessionView(struct NetTransportSessionView *view);
bool8 NetTransport_WriteLocalSnapshot(const struct NetPlayerSnapshot *snapshot);
bool8 NetTransport_SendUnreliablePacket(u8 packet_type, const void *packet_payload, u16 packet_payload_size);
bool8 NetTransport_SendPacket(u8 packet_type, const void *packet_payload, u16 packet_payload_size);
bool8 NetTransport_ReceivePacket(struct NetPacketEnvelope *envelope, void *packet_payload, u16 payload_capacity, u16 *received_payload_size);

#endif // GUARD_MULTIPLAYER_TRANSPORT_H
