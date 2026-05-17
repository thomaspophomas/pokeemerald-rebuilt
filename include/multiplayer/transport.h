#ifndef GUARD_MULTIPLAYER_TRANSPORT_H
#define GUARD_MULTIPLAYER_TRANSPORT_H

#include "global.h"
#include "multiplayer/protocol.h"
#include "multiplayer/types.h"

#define NET_TRANSPORT_PACKET_PAYLOAD_SIZE 128

struct NetTransportPacketSlot
{
    struct NetPacketEnvelope envelope;
    u8 payload[NET_TRANSPORT_PACKET_PAYLOAD_SIZE];
} __attribute__((packed));

STATIC_ASSERT(sizeof(struct NetClientHello) <= NET_TRANSPORT_PACKET_PAYLOAD_SIZE, NetClientHelloPayloadTooLarge);
STATIC_ASSERT(sizeof(struct NetHeartbeat) <= NET_TRANSPORT_PACKET_PAYLOAD_SIZE, NetHeartbeatPayloadTooLarge);
STATIC_ASSERT(sizeof(struct NetMoveIntent) <= NET_TRANSPORT_PACKET_PAYLOAD_SIZE, NetMoveIntentPayloadTooLarge);
STATIC_ASSERT(sizeof(struct NetInteractIntent) <= NET_TRANSPORT_PACKET_PAYLOAD_SIZE, NetInteractIntentPayloadTooLarge);
STATIC_ASSERT(sizeof(struct NetBattleAction) <= NET_TRANSPORT_PACKET_PAYLOAD_SIZE, NetBattleActionPayloadTooLarge);
STATIC_ASSERT(sizeof(struct NetTradeAction) <= NET_TRANSPORT_PACKET_PAYLOAD_SIZE, NetTradeActionPayloadTooLarge);
STATIC_ASSERT(sizeof(struct NetCommitResult) <= NET_TRANSPORT_PACKET_PAYLOAD_SIZE, NetCommitResultPayloadTooLarge);
STATIC_ASSERT(sizeof(struct MultiplayerSubsession) <= NET_TRANSPORT_PACKET_PAYLOAD_SIZE, MultiplayerSubsessionPayloadTooLarge);

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
bool8 NetTransport_SendUnreliablePacket(u8 packetType, const void *payload, u16 payloadSize);
bool8 NetTransport_SendPacket(u8 packetType, const void *payload, u16 payloadSize);
bool8 NetTransport_ReceivePacket(struct NetPacketEnvelope *envelope, void *payload, u16 capacity, u16 *payloadSize);

#endif // GUARD_MULTIPLAYER_TRANSPORT_H
