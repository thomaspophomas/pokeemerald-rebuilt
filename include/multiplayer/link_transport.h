#ifndef GUARD_MULTIPLAYER_LINK_TRANSPORT_H
#define GUARD_MULTIPLAYER_LINK_TRANSPORT_H

#include "global.h"
#include "multiplayer/transport.h"

#define NET_LINK_GATEWAY_LINK_TYPE 0x4E47 // "NG"
#define NET_LINK_GATEWAY_FRAME_MAGIC 0x31474C4E // "NLG1"
#define NET_LINK_GATEWAY_FRAME_VERSION 1
#define NET_LINK_GATEWAY_FRAME_SIZE 256
#define NET_LINK_GATEWAY_FRAME_HEADER_SIZE 12
#define NET_LINK_GATEWAY_FRAME_PAYLOAD_SIZE (NET_LINK_GATEWAY_FRAME_SIZE - NET_LINK_GATEWAY_FRAME_HEADER_SIZE)
#define NET_LINK_GATEWAY_QUEUE_SIZE 16

enum NetLinkGatewayFrameType
{
    NET_LINK_FRAME_NONE,
    NET_LINK_FRAME_SESSION_HEADER,
    NET_LINK_FRAME_SERVER_PLAYER_SNAPSHOT,
    NET_LINK_FRAME_SERVER_SUBSESSION,
    NET_LINK_FRAME_SERVER_PACKET_SLOT,
    NET_LINK_FRAME_CLIENT_PLAYER_SNAPSHOT,
    NET_LINK_FRAME_CLIENT_PACKET_SLOT,
};

struct NetLinkGatewayFrame
{
    u32 magic;
    u8 version;
    u8 frameType;
    u16 payloadSize;
    u32 sequence;
    u8 payload[NET_LINK_GATEWAY_FRAME_PAYLOAD_SIZE];
} __attribute__((packed));

struct NetLinkGatewaySessionHeader
{
    u8 connected;
    u8 localPlayerId;
    u8 hostPlayerId;
    u8 playerCount;
    u8 transportMode;
    u8 reserved[3];
    u32 sessionId;
    u32 sessionEpoch;
    u32 playerToken;
    u32 joinNonce;
    u32 bridgeTick;
    u32 serverClockSeconds;
} __attribute__((packed));

struct NetLinkGatewaySubsessionFrame
{
    u8 index;
    u8 reserved[3];
    struct MultiplayerSubsession subsession;
} __attribute__((packed));

STATIC_ASSERT(sizeof(struct NetLinkGatewayFrame) == NET_LINK_GATEWAY_FRAME_SIZE, NetLinkGatewayFrameSize);
STATIC_ASSERT(sizeof(struct NetTransportPacketSlot) <= NET_LINK_GATEWAY_FRAME_PAYLOAD_SIZE, NetLinkPacketSlotFitsFrame);
STATIC_ASSERT(sizeof(struct NetPlayerSnapshot) <= NET_LINK_GATEWAY_FRAME_PAYLOAD_SIZE, NetLinkSnapshotFitsFrame);
STATIC_ASSERT(sizeof(struct NetLinkGatewaySessionHeader) <= NET_LINK_GATEWAY_FRAME_PAYLOAD_SIZE, NetLinkSessionHeaderFitsFrame);
STATIC_ASSERT(sizeof(struct NetLinkGatewaySubsessionFrame) <= NET_LINK_GATEWAY_FRAME_PAYLOAD_SIZE, NetLinkSubsessionFitsFrame);

#endif // GUARD_MULTIPLAYER_LINK_TRANSPORT_H
