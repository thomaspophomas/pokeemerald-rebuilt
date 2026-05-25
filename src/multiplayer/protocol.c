#include "global.h"
#include "multiplayer/protocol.h"

u16 NetProtocol_CalcChecksum(const void *checksum_data, u16 checksum_data_size)
{
    const u8 *checksum_bytes = checksum_data;
    u16 checksum = 0;
    u16 byte_index;

    if (checksum_data == NULL && checksum_data_size != 0)
        return 0;

    for (byte_index = 0; byte_index < checksum_data_size; byte_index++)
        checksum = (checksum << 1) ^ checksum_bytes[byte_index];

    return checksum;
}

u32 NetProtocol_MakeTransactionId(u32 session_epoch, u8 player_id, u8 packet_type, u8 subsession_id, u32 action_sequence)
{
    u32 transaction_id = session_epoch ^ (action_sequence * 0x45D9F3BU);

    transaction_id ^= (u32)player_id << 24;
    transaction_id ^= (u32)packet_type << 16;
    transaction_id ^= (u32)subsession_id << 8;
    if (transaction_id == 0)
        transaction_id = 1;

    return transaction_id;
}

void NetProtocol_InitEnvelope(struct NetPacketEnvelope *envelope, u8 packet_type, u8 player_id, u32 session_id, u32 tick, u16 payload_size)
{
    NetProtocol_InitEnvelopeWithEpoch(envelope, packet_type, player_id, session_id, 0, tick, payload_size);
}

void NetProtocol_InitEnvelopeWithEpoch(struct NetPacketEnvelope *envelope, u8 packet_type, u8 player_id, u32 session_id, u32 session_epoch, u32 tick, u16 payload_size)
{
    envelope->magic = NET_PROTOCOL_MAGIC;
    envelope->protocolVersion = NET_PROTOCOL_VERSION;
    envelope->headerSize = sizeof(*envelope);
    envelope->sessionId = session_id;
    envelope->sessionEpoch = session_epoch;
    envelope->tick = tick;
    envelope->packetType = packet_type;
    envelope->playerId = player_id;
    envelope->flags = 0;
    envelope->reserved = 0;
    envelope->sequence = 0;
    envelope->ack = 0;
    envelope->payloadSize = payload_size;
    envelope->checksum = 0;
}

bool8 NetProtocol_ValidateEnvelope(const struct NetPacketEnvelope *envelope, u8 packet_type, u16 payload_size)
{
    if (envelope == NULL)
        return FALSE;
    if (envelope->magic != NET_PROTOCOL_MAGIC)
        return FALSE;
    if (envelope->protocolVersion != NET_PROTOCOL_VERSION)
        return FALSE;
    if (envelope->headerSize != sizeof(*envelope))
        return FALSE;
    if (packet_type == NET_PACKET_NONE || packet_type >= NET_PACKET_COUNT)
        return FALSE;
    if (envelope->packetType != packet_type)
        return FALSE;
    if (envelope->sequence == 0)
        return FALSE;
    if (envelope->payloadSize != payload_size)
        return FALSE;

    return TRUE;
}
