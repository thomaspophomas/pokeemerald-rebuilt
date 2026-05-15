#ifndef GUARD_MULTIPLAYER_TRANSPORT_H
#define GUARD_MULTIPLAYER_TRANSPORT_H

#include "global.h"
#include "multiplayer/protocol.h"
#include "multiplayer/types.h"

#define NET_TRANSPORT_PACKET_PAYLOAD_SIZE 256

struct NetTransportPacketSlot
{
    struct NetPacketEnvelope envelope;
    u8 payload[NET_TRANSPORT_PACKET_PAYLOAD_SIZE];
} __attribute__((packed));

struct NetTransportSessionView
{
    bool8 connected;
    u8 localPlayerId;
    u8 hostPlayerId;
    u8 playerCount;
    u32 sessionId;
    u32 bridgeTick;
    struct NetPlayerSnapshot players[MAX_NET_PLAYERS];
    struct MultiplayerSubsession subsessions[MAX_NET_SUBSESSIONS];
};

void NetTransport_Init(void);
void NetTransport_Tick(void);
bool8 NetTransport_IsConnected(void);
bool8 NetTransport_ReadSessionView(struct NetTransportSessionView *view);
bool8 NetTransport_WriteLocalSnapshot(const struct NetPlayerSnapshot *snapshot);
bool8 NetTransport_SendPacket(u8 packetType, const void *payload, u16 payloadSize);
bool8 NetTransport_ReceivePacket(struct NetPacketEnvelope *envelope, void *payload, u16 capacity, u16 *payloadSize);

#endif // GUARD_MULTIPLAYER_TRANSPORT_H
