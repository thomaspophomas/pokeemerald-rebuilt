#include "global.h"
#include "multiplayer/protocol.h"

u16 NetProtocol_CalcChecksum(const void *data, u16 size)
{
    const u8 *bytes = data;
    u16 checksum = 0;
    u16 i;

    for (i = 0; i < size; i++)
        checksum = (checksum << 1) ^ bytes[i];

    return checksum;
}

void NetProtocol_InitEnvelope(struct NetPacketEnvelope *envelope, u8 packetType, u8 playerId, u32 sessionId, u32 tick, u16 payloadSize)
{
    envelope->magic = NET_PROTOCOL_MAGIC;
    envelope->protocolVersion = NET_PROTOCOL_VERSION;
    envelope->headerSize = sizeof(*envelope);
    envelope->sessionId = sessionId;
    envelope->tick = tick;
    envelope->packetType = packetType;
    envelope->playerId = playerId;
    envelope->flags = 0;
    envelope->reserved = 0;
    envelope->sequence = 0;
    envelope->ack = 0;
    envelope->payloadSize = payloadSize;
    envelope->checksum = 0;
}

bool8 NetProtocol_ValidateEnvelope(const struct NetPacketEnvelope *envelope, u8 packetType, u16 payloadSize)
{
    if (envelope->magic != NET_PROTOCOL_MAGIC)
        return FALSE;
    if (envelope->protocolVersion != NET_PROTOCOL_VERSION)
        return FALSE;
    if (envelope->headerSize != sizeof(*envelope))
        return FALSE;
    if (envelope->packetType != packetType)
        return FALSE;
    if (envelope->payloadSize != payloadSize)
        return FALSE;

    return TRUE;
}
