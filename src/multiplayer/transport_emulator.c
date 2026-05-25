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
    if (sBridge->localPlayerId >= MAX_NET_PLAYERS)
        return FALSE;
    if (sBridge->hostPlayerId >= MAX_NET_PLAYERS)
        return FALSE;
    if (sBridge->playerCount > MAX_NET_PLAYERS)
        return FALSE;
    if (sBridge->transportMode != NET_TRANSPORT_MODE_SERVER_BRIDGE)
        return FALSE;
    if (sBridge->sessionEpoch == 0 || sBridge->playerToken == 0 || sBridge->joinNonce == 0)
        return FALSE;

    return TRUE;
}

static bool8 PacketEnvelopeIsValid(const struct NetPacketEnvelope *envelope, u16 payload_capacity)
{
    if (envelope == NULL)
        return FALSE;
    if (envelope->magic != NET_PROTOCOL_MAGIC)
        return FALSE;
    if (envelope->protocolVersion != NET_PROTOCOL_VERSION)
        return FALSE;
    if (envelope->headerSize != sizeof(*envelope))
        return FALSE;
    if (envelope->sessionId != sBridge->sessionId)
        return FALSE;
    if (envelope->sessionEpoch != sBridge->sessionEpoch)
        return FALSE;
    if (envelope->packetType == NET_PACKET_NONE || envelope->packetType >= NET_PACKET_COUNT)
        return FALSE;
    if (envelope->playerId >= MAX_NET_PLAYERS)
        return FALSE;
    if (envelope->sequence == 0)
        return FALSE;
    if (envelope->payloadSize > payload_capacity || envelope->payloadSize > NET_TRANSPORT_PACKET_PAYLOAD_SIZE)
        return FALSE;

    return TRUE;
}

static void SyncTransportSessionIdentity(u32 sessionId, u32 sessionEpoch)
{
    if (sCurrentSessionId == sessionId && sCurrentSessionEpoch == sessionEpoch)
        return;

    sCurrentSessionId = sessionId;
    sCurrentSessionEpoch = sessionEpoch;
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
    NetProtocol_InitEnvelopeWithEpoch(envelope, packet_type, sBridge->localPlayerId, sBridge->sessionId, sBridge->sessionEpoch, sBridge->bridgeTick, packet_payload_size);
    envelope->sequence = NextOutboundSequence();
}

static void FinishOutboundEnvelope(struct NetPacketEnvelope *envelope, const void *packet_payload)
{
    envelope->checksum = NetProtocol_CalcChecksum(packet_payload, envelope->payloadSize);
}

static bool8 QueueReliableOutboundPacket(const struct NetPacketEnvelope *envelope, const void *packet_payload)
{
    u32 head = sBridge->reliableOutboundHead % NET_RELIABLE_QUEUE_SIZE;
    u32 nextHead = NextQueueIndex(head);
    u32 tail = sBridge->reliableOutboundTail % NET_RELIABLE_QUEUE_SIZE;

    if (nextHead == tail)
        return FALSE;

    if (envelope->payloadSize != 0)
        memcpy((void *)sBridge->reliableOutbound[head].payload, packet_payload, envelope->payloadSize);
    NetTransport_CopyEnvelopeToPacketHeader(&sBridge->reliableOutbound[head].header, envelope);
    sBridge->reliableOutboundHead = nextHead;
    return TRUE;
}

static void WriteLatestUnreliablePacket(const struct NetPacketEnvelope *envelope, const void *packet_payload)
{
    if (envelope->payloadSize != 0)
        memcpy((void *)sBridge->latestUnreliable.payload, packet_payload, envelope->payloadSize);
    NetTransport_CopyEnvelopeToPacketHeader(&sBridge->latestUnreliable.header, envelope);
    sBridge->latestUnreliableSequence = envelope->sequence;
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
        u32 startSequence = sBridge->viewSequence;

        if (startSequence & 1)
            continue;

        view->connected = sBridge->connected;
        view->localPlayerId = sBridge->localPlayerId;
        view->hostPlayerId = sBridge->hostPlayerId;
        view->playerCount = sBridge->playerCount;
        view->transportMode = sBridge->transportMode;
        view->sessionId = sBridge->sessionId;
        view->sessionEpoch = sBridge->sessionEpoch;
        view->playerToken = sBridge->playerToken;
        view->joinNonce = sBridge->joinNonce;
        view->bridgeTick = sBridge->bridgeTick;
        view->serverClockSeconds = sBridge->serverClockSeconds;
        memcpy(view->players, (const void *)sBridge->serverPlayers, sizeof(view->players));
        memcpy(view->subsessions, (const void *)sBridge->serverSubsessions, sizeof(view->subsessions));

        if (startSequence == sBridge->viewSequence && !(startSequence & 1))
        {
            SyncTransportSessionIdentity(view->sessionId, view->sessionEpoch);
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
    if (snapshot->playerId >= MAX_NET_PLAYERS)
        return FALSE;
    if (snapshot->playerId != sBridge->localPlayerId)
        return FALSE;
    if (snapshot->sessionEpoch != sBridge->sessionEpoch)
        return FALSE;

    sBridge->localSnapshotSequence++;
    memcpy((void *)&sBridge->localSnapshot, snapshot, sizeof(*snapshot));
    sBridge->localSnapshotSequence++;
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

    SyncTransportSessionIdentity(sBridge->sessionId, sBridge->sessionEpoch);
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

    SyncTransportSessionIdentity(sBridge->sessionId, sBridge->sessionEpoch);
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
    SyncTransportSessionIdentity(sBridge->sessionId, sBridge->sessionEpoch);
    head = sBridge->reliableInboundHead % NET_RELIABLE_QUEUE_SIZE;
    tail = sBridge->reliableInboundTail % NET_RELIABLE_QUEUE_SIZE;
    if (tail == head)
        return FALSE;

    NetTransport_CopyPacketHeaderToEnvelope(envelope, &sBridge->reliableInbound[tail].header);
    if (!PacketEnvelopeIsValid(envelope, payload_capacity))
    {
        sBridge->reliableInboundTail = NextQueueIndex(tail);
        return FALSE;
    }
    if (envelope->payloadSize != 0 && packet_payload == NULL)
    {
        sBridge->reliableInboundTail = NextQueueIndex(tail);
        return FALSE;
    }
    if (envelope->sequence <= sLastInboundSequence)
    {
        sBridge->reliableInboundTail = NextQueueIndex(tail);
        return FALSE;
    }

    if (envelope->payloadSize != 0)
        memcpy(packet_payload, (const void *)sBridge->reliableInbound[tail].payload, envelope->payloadSize);
    if (envelope->checksum != NetProtocol_CalcChecksum(packet_payload, envelope->payloadSize))
    {
        sBridge->reliableInboundTail = NextQueueIndex(tail);
        return FALSE;
    }

    sLastInboundSequence = envelope->sequence;
    *received_payload_size = envelope->payloadSize;
    sBridge->reliableInboundTail = NextQueueIndex(tail);

    return TRUE;
#else
    if (received_payload_size != NULL)
        *received_payload_size = 0;
    return FALSE;
#endif
}

#endif // !FEATURE_MULTIPLAYER_LINK_TRANSPORT
