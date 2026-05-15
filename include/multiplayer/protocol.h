#ifndef GUARD_MULTIPLAYER_PROTOCOL_H
#define GUARD_MULTIPLAYER_PROTOCOL_H

#include "global.h"
#include "multiplayer/constants.h"

enum NetPacketType
{
    NET_PACKET_NONE,
    NET_PACKET_PLAYER_SNAPSHOT,
    NET_PACKET_SUBSESSION_INVITE,
    NET_PACKET_SUBSESSION_ACCEPT,
    NET_PACKET_SUBSESSION_DECLINE,
    NET_PACKET_SUBSESSION_COMMIT,
    NET_PACKET_SUBSESSION_ABORT,
    NET_PACKET_BATTLE_INPUT,
    NET_PACKET_TRADE_INPUT,
};

struct NetPacketEnvelope
{
    u32 magic;
    u16 protocolVersion;
    u16 headerSize;
    u32 sessionId;
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

u16 NetProtocol_CalcChecksum(const void *data, u16 size);
void NetProtocol_InitEnvelope(struct NetPacketEnvelope *envelope, u8 packetType, u8 playerId, u32 sessionId, u32 tick, u16 payloadSize);
bool8 NetProtocol_ValidateEnvelope(const struct NetPacketEnvelope *envelope, u8 packetType, u16 payloadSize);

#endif // GUARD_MULTIPLAYER_PROTOCOL_H
