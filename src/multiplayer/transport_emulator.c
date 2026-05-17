#include "global.h"
#include "multiplayer/bridge_mailbox.h"
#include "multiplayer/transport.h"

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

static bool8 PacketEnvelopeIsValid(const struct NetPacketEnvelope *envelope, u16 capacity)
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
    if (envelope->payloadSize > capacity || envelope->payloadSize > NET_TRANSPORT_PACKET_PAYLOAD_SIZE)
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
    if (snapshot->sessionEpoch != sBridge->sessionEpoch
        || snapshot->playerToken != sBridge->playerToken
        || snapshot->joinNonce != sBridge->joinNonce)
        return FALSE;

    sBridge->localSnapshotSequence++;
    memcpy((void *)&sBridge->localSnapshot, snapshot, sizeof(*snapshot));
    sBridge->localSnapshotSequence++;
    return TRUE;
#else
    return FALSE;
#endif
}

bool8 NetTransport_SendPacket(u8 packetType, const void *payload, u16 payloadSize)
{
#if FEATURE_MULTIPLAYER && FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT
    struct NetPacketEnvelope envelope;

    if (!NetTransport_IsConnected())
        return FALSE;
    SyncTransportSessionIdentity(sBridge->sessionId, sBridge->sessionEpoch);
    if (packetType == NET_PACKET_NONE || packetType >= NET_PACKET_COUNT)
        return FALSE;
    if (payloadSize > NET_TRANSPORT_PACKET_PAYLOAD_SIZE)
        return FALSE;
    if (payloadSize != 0 && payload == NULL)
        return FALSE;
    {
        u32 head = sBridge->reliableOutboundHead % NET_RELIABLE_QUEUE_SIZE;
        u32 nextHead = NextQueueIndex(head);
        u32 tail = sBridge->reliableOutboundTail % NET_RELIABLE_QUEUE_SIZE;

        if (nextHead == tail)
            return FALSE;

        sOutboundSequence++;
        if (sOutboundSequence == 0)
            sOutboundSequence++;
        NetProtocol_InitEnvelopeWithEpoch(&envelope, packetType, sBridge->localPlayerId, sBridge->sessionId, sBridge->sessionEpoch, sBridge->bridgeTick, payloadSize);
        envelope.sequence = sOutboundSequence;
        envelope.checksum = NetProtocol_CalcChecksum(payload, payloadSize);

        if (payloadSize != 0)
            memcpy((void *)sBridge->reliableOutbound[head].payload, payload, payloadSize);
        memcpy((void *)&sBridge->reliableOutbound[head].envelope, &envelope, sizeof(envelope));
        sBridge->reliableOutboundHead = nextHead;
    }

    return TRUE;
#else
    return FALSE;
#endif
}

bool8 NetTransport_SendUnreliablePacket(u8 packetType, const void *payload, u16 payloadSize)
{
#if FEATURE_MULTIPLAYER && FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT
    struct NetPacketEnvelope envelope;

    if (!NetTransport_IsConnected())
        return FALSE;
    SyncTransportSessionIdentity(sBridge->sessionId, sBridge->sessionEpoch);
    if (packetType == NET_PACKET_NONE || packetType >= NET_PACKET_COUNT)
        return FALSE;
    if (payloadSize > NET_TRANSPORT_PACKET_PAYLOAD_SIZE)
        return FALSE;
    if (payloadSize != 0 && payload == NULL)
        return FALSE;

    sOutboundSequence++;
    if (sOutboundSequence == 0)
        sOutboundSequence++;
    NetProtocol_InitEnvelopeWithEpoch(&envelope, packetType, sBridge->localPlayerId, sBridge->sessionId, sBridge->sessionEpoch, sBridge->bridgeTick, payloadSize);
    envelope.sequence = sOutboundSequence;
    envelope.checksum = NetProtocol_CalcChecksum(payload, payloadSize);

    if (payloadSize != 0)
        memcpy((void *)sBridge->latestUnreliable.payload, payload, payloadSize);
    memcpy((void *)&sBridge->latestUnreliable.envelope, &envelope, sizeof(envelope));
    sBridge->latestUnreliableSequence = sOutboundSequence;

    return TRUE;
#else
    return FALSE;
#endif
}

bool8 NetTransport_ReceivePacket(struct NetPacketEnvelope *envelope, void *payload, u16 capacity, u16 *payloadSize)
{
#if FEATURE_MULTIPLAYER && FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT
    u32 head;
    u32 tail;

    if (envelope == NULL || payloadSize == NULL)
        return FALSE;
    if (!NetTransport_IsConnected())
        return FALSE;
    SyncTransportSessionIdentity(sBridge->sessionId, sBridge->sessionEpoch);
    head = sBridge->reliableInboundHead % NET_RELIABLE_QUEUE_SIZE;
    tail = sBridge->reliableInboundTail % NET_RELIABLE_QUEUE_SIZE;
    if (tail == head)
        return FALSE;

    memcpy(envelope, (const void *)&sBridge->reliableInbound[tail].envelope, sizeof(*envelope));
    if (!PacketEnvelopeIsValid(envelope, capacity))
    {
        sBridge->reliableInboundTail = NextQueueIndex(tail);
        return FALSE;
    }
    if (envelope->payloadSize != 0 && payload == NULL)
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
        memcpy(payload, (const void *)sBridge->reliableInbound[tail].payload, envelope->payloadSize);
    if (envelope->checksum != NetProtocol_CalcChecksum(payload, envelope->payloadSize))
    {
        sBridge->reliableInboundTail = NextQueueIndex(tail);
        return FALSE;
    }

    sLastInboundSequence = envelope->sequence;
    *payloadSize = envelope->payloadSize;
    sBridge->reliableInboundTail = NextQueueIndex(tail);

    return TRUE;
#else
    if (payloadSize != NULL)
        *payloadSize = 0;
    return FALSE;
#endif
}
