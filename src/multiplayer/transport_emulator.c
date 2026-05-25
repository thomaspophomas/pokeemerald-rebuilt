#include "global.h"
#include "multiplayer/bridge_mailbox.h"
#include "multiplayer/transport.h"

#if !FEATURE_MULTIPLAYER_LINK_TRANSPORT

#if FEATURE_MULTIPLAYER && FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT

EWRAM_DATA volatile struct NetEmulatorBridgeBuffer gNetEmulatorBridgeMailbox = {0};

static volatile struct NetEmulatorBridgeBuffer *const sBridge = &gNetEmulatorBridgeMailbox;
static EWRAM_DATA u32 sOutboundSequence = 0;
static EWRAM_DATA u32 sLastInboundSequence = 0;
static EWRAM_DATA u32 sCurrentSessionId = 0;
static EWRAM_DATA u32 sCurrentSessionEpoch = 0;

#define NET_TRANSPORT_STABLE_READ_TRIES 3

static u32 NextQueueIndex(u32 index)
{
    return (index + 1) % NET_RELIABLE_QUEUE_SIZE;
}

static bool8 BridgeHeaderIsValid(void)
{
    if (sBridge->magic != NET_EMULATOR_BRIDGE_MAGIC)
        return FALSE;
    if (sBridge->version != NET_EMULATOR_BRIDGE_VERSION)
        return FALSE;
    if (sBridge->local_player_id >= MAX_NET_PLAYERS)
        return FALSE;
    if (sBridge->host_player_id >= MAX_NET_PLAYERS)
        return FALSE;
    if (sBridge->player_count > MAX_NET_PLAYERS)
        return FALSE;
    if (sBridge->transport_mode != NET_TRANSPORT_MODE_SERVER_BRIDGE)
        return FALSE;
    if (sBridge->session_epoch == 0 || sBridge->player_token == 0 || sBridge->join_nonce == 0)
        return FALSE;

    return TRUE;
}

static bool8 PacketEnvelopeIsValid(const struct NetPacketEnvelope *envelope, u16 payload_capacity)
{
    if (envelope == NULL)
        return FALSE;
    if (envelope->magic != NET_PROTOCOL_MAGIC)
        return FALSE;
    if (envelope->protocol_version != NET_PROTOCOL_VERSION)
        return FALSE;
    if (envelope->header_size != sizeof(*envelope))
        return FALSE;
    if (envelope->session_id != sBridge->session_id)
        return FALSE;
    if (envelope->session_epoch != sBridge->session_epoch)
        return FALSE;
    if (envelope->packet_type == NET_PACKET_NONE || envelope->packet_type >= NET_PACKET_COUNT)
        return FALSE;
    if (envelope->player_id >= MAX_NET_PLAYERS)
        return FALSE;
    if (envelope->sequence == 0)
        return FALSE;
    if (envelope->payload_size > payload_capacity || envelope->payload_size > NET_TRANSPORT_PACKET_PAYLOAD_SIZE)
        return FALSE;

    return TRUE;
}

static void SyncTransportSessionIdentity(u32 session_id, u32 session_epoch)
{
    if (sCurrentSessionId == session_id && sCurrentSessionEpoch == session_epoch)
        return;

    sCurrentSessionId = session_id;
    sCurrentSessionEpoch = session_epoch;
    sOutboundSequence = 0;
    sLastInboundSequence = 0;
}

static bool8 OutboundPacketCanBeSent(u8 packet_type, const void *packet_payload, u16 packet_payload_size)
{
    if (!NetTransport_IsConnected())
        return FALSE;
    if (packet_type == NET_PACKET_NONE || packet_type >= NET_PACKET_COUNT)
        return FALSE;
    if (packet_payload_size > NET_TRANSPORT_PACKET_PAYLOAD_SIZE)
        return FALSE;
    if (packet_payload_size != 0 && packet_payload == NULL)
        return FALSE;

    return TRUE;
}

static u32 NextOutboundSequence(void)
{
    sOutboundSequence++;
    if (sOutboundSequence == 0)
        sOutboundSequence++;

    return sOutboundSequence;
}

static void InitOutboundEnvelope(struct NetPacketEnvelope *envelope, u8 packet_type, u16 packet_payload_size)
{
    NetProtocol_InitEnvelopeWithEpoch(envelope, packet_type, sBridge->local_player_id, sBridge->session_id, sBridge->session_epoch, sBridge->bridge_tick, packet_payload_size);
    envelope->sequence = NextOutboundSequence();
}

static void FinishOutboundEnvelope(struct NetPacketEnvelope *envelope, const void *packet_payload)
{
    envelope->checksum = NetProtocol_CalcChecksum(packet_payload, envelope->payload_size);
}

static bool8 QueueReliableOutboundPacket(const struct NetPacketEnvelope *envelope, const void *packet_payload)
{
    u32 head = sBridge->reliable_outbound_head % NET_RELIABLE_QUEUE_SIZE;
    u32 nextHead = NextQueueIndex(head);
    u32 tail = sBridge->reliable_outbound_tail % NET_RELIABLE_QUEUE_SIZE;

    if (nextHead == tail)
        return FALSE;

    if (envelope->payload_size != 0)
        memcpy((void *)sBridge->reliable_outbound[head].payload, packet_payload, envelope->payload_size);
    NetTransport_CopyEnvelopeToPacketHeader(&sBridge->reliable_outbound[head].header, envelope);
    sBridge->reliable_outbound_head = nextHead;
    return TRUE;
}

static void WriteLatestUnreliablePacket(const struct NetPacketEnvelope *envelope, const void *packet_payload)
{
    if (envelope->payload_size != 0)
        memcpy((void *)sBridge->latest_unreliable.payload, packet_payload, envelope->payload_size);
    NetTransport_CopyEnvelopeToPacketHeader(&sBridge->latest_unreliable.header, envelope);
    sBridge->latest_unreliable_sequence = envelope->sequence;
}

#endif

void NetTransport_Init(void)
{
#if FEATURE_MULTIPLAYER && FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT
    sOutboundSequence = 0;
    sLastInboundSequence = 0;
    sCurrentSessionId = 0;
    sCurrentSessionEpoch = 0;
#endif
}

void NetTransport_Tick(void)
{
}

bool8 NetTransport_IsConnected(void)
{
#if FEATURE_MULTIPLAYER && FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT
    return BridgeHeaderIsValid() && sBridge->connected;
#else
    return FALSE;
#endif
}

bool8 NetTransport_ReadSessionView(struct NetTransportSessionView *view)
{
#if FEATURE_MULTIPLAYER && FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT
    u8 attempt;

    if (view == NULL)
        return FALSE;
    if (!NetTransport_IsConnected())
        return FALSE;

    for (attempt = 0; attempt < NET_TRANSPORT_STABLE_READ_TRIES; attempt++)
    {
        u32 startSequence = sBridge->view_sequence;

        if (startSequence & 1)
            continue;

        view->connected = sBridge->connected;
        view->local_player_id = sBridge->local_player_id;
        view->host_player_id = sBridge->host_player_id;
        view->player_count = sBridge->player_count;
        view->transport_mode = sBridge->transport_mode;
        view->session_id = sBridge->session_id;
        view->session_epoch = sBridge->session_epoch;
        view->player_token = sBridge->player_token;
        view->join_nonce = sBridge->join_nonce;
        view->bridge_tick = sBridge->bridge_tick;
        view->server_clock_seconds = sBridge->server_clock_seconds;
        memcpy(view->players, (const void *)sBridge->server_players, sizeof(view->players));
        memcpy(view->subsessions, (const void *)sBridge->server_subsessions, sizeof(view->subsessions));

        if (startSequence == sBridge->view_sequence && !(startSequence & 1))
        {
            SyncTransportSessionIdentity(view->session_id, view->session_epoch);
            return TRUE;
        }
    }

    return FALSE;
#else
    if (view != NULL)
        memset(view, 0, sizeof(*view));
    return FALSE;
#endif
}

bool8 NetTransport_WriteLocalSnapshot(const struct NetPlayerSnapshot *snapshot)
{
#if FEATURE_MULTIPLAYER && FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT
    if (snapshot == NULL)
        return FALSE;
    if (!NetTransport_IsConnected())
        return FALSE;
    if (snapshot->player_id >= MAX_NET_PLAYERS)
        return FALSE;
    if (snapshot->player_id != sBridge->local_player_id)
        return FALSE;
    if (snapshot->session_epoch != sBridge->session_epoch)
        return FALSE;

    sBridge->local_snapshot_sequence++;
    memcpy((void *)&sBridge->local_snapshot, snapshot, sizeof(*snapshot));
    sBridge->local_snapshot_sequence++;
    return TRUE;
#else
    return FALSE;
#endif
}

bool8 NetTransport_SendPacket(u8 packet_type, const void *packet_payload, u16 packet_payload_size)
{
#if FEATURE_MULTIPLAYER && FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT
    struct NetPacketEnvelope envelope;

    if (!OutboundPacketCanBeSent(packet_type, packet_payload, packet_payload_size))
        return FALSE;

    SyncTransportSessionIdentity(sBridge->session_id, sBridge->session_epoch);
    InitOutboundEnvelope(&envelope, packet_type, packet_payload_size);
    FinishOutboundEnvelope(&envelope, packet_payload);
    return QueueReliableOutboundPacket(&envelope, packet_payload);
#else
    return FALSE;
#endif
}

bool8 NetTransport_SendUnreliablePacket(u8 packet_type, const void *packet_payload, u16 packet_payload_size)
{
#if FEATURE_MULTIPLAYER && FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT
    struct NetPacketEnvelope envelope;

    if (!OutboundPacketCanBeSent(packet_type, packet_payload, packet_payload_size))
        return FALSE;

    SyncTransportSessionIdentity(sBridge->session_id, sBridge->session_epoch);
    InitOutboundEnvelope(&envelope, packet_type, packet_payload_size);
    FinishOutboundEnvelope(&envelope, packet_payload);
    WriteLatestUnreliablePacket(&envelope, packet_payload);

    return TRUE;
#else
    return FALSE;
#endif
}

bool8 NetTransport_ReceivePacket(struct NetPacketEnvelope *envelope, void *packet_payload, u16 payload_capacity, u16 *received_payload_size)
{
#if FEATURE_MULTIPLAYER && FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT
    u32 head;
    u32 tail;

    if (envelope == NULL || received_payload_size == NULL)
        return FALSE;
    if (!NetTransport_IsConnected())
        return FALSE;
    SyncTransportSessionIdentity(sBridge->session_id, sBridge->session_epoch);
    head = sBridge->reliable_inbound_head % NET_RELIABLE_QUEUE_SIZE;
    tail = sBridge->reliable_inbound_tail % NET_RELIABLE_QUEUE_SIZE;
    if (tail == head)
        return FALSE;

    NetTransport_CopyPacketHeaderToEnvelope(envelope, &sBridge->reliable_inbound[tail].header);
    if (!PacketEnvelopeIsValid(envelope, payload_capacity))
    {
        sBridge->reliable_inbound_tail = NextQueueIndex(tail);
        return FALSE;
    }
    if (envelope->payload_size != 0 && packet_payload == NULL)
    {
        sBridge->reliable_inbound_tail = NextQueueIndex(tail);
        return FALSE;
    }
    if (envelope->sequence <= sLastInboundSequence)
    {
        sBridge->reliable_inbound_tail = NextQueueIndex(tail);
        return FALSE;
    }

    if (envelope->payload_size != 0)
        memcpy(packet_payload, (const void *)sBridge->reliable_inbound[tail].payload, envelope->payload_size);
    if (envelope->checksum != NetProtocol_CalcChecksum(packet_payload, envelope->payload_size))
    {
        sBridge->reliable_inbound_tail = NextQueueIndex(tail);
        return FALSE;
    }

    sLastInboundSequence = envelope->sequence;
    *received_payload_size = envelope->payload_size;
    sBridge->reliable_inbound_tail = NextQueueIndex(tail);

    return TRUE;
#else
    if (received_payload_size != NULL)
        *received_payload_size = 0;
    return FALSE;
#endif
}

#endif // !FEATURE_MULTIPLAYER_LINK_TRANSPORT
