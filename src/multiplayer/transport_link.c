#include "global.h"
#include "link.h"
#include "multiplayer/link_transport.h"

#if FEATURE_MULTIPLAYER && FEATURE_MULTIPLAYER_LINK_TRANSPORT

static EWRAM_DATA bool8 sLinkStarted = FALSE;
static EWRAM_DATA bool8 sCableReady = FALSE;
static EWRAM_DATA struct NetTransportSessionView sView = {0};
static EWRAM_DATA u32 sCurrentSessionId = 0;
static EWRAM_DATA u32 sCurrentSessionEpoch = 0;
static EWRAM_DATA u32 sOutboundSequence = 0;
static EWRAM_DATA u32 sLastInboundSequence = 0;
static EWRAM_DATA u32 sLastGatewaySequence = 0;
static EWRAM_DATA u32 sNextFrameSequence = 0;
static EWRAM_DATA u8 sTxHead = 0;
static EWRAM_DATA u8 sTxTail = 0;
static EWRAM_DATA u8 sInboundHead = 0;
static EWRAM_DATA u8 sInboundTail = 0;
static EWRAM_DATA struct NetLinkGatewayFrame sTxFrames[NET_LINK_GATEWAY_QUEUE_SIZE] = {0};
static EWRAM_DATA struct NetTransportPacketSlot sInboundSlots[NET_LINK_GATEWAY_QUEUE_SIZE] = {0};

static u8 NextQueueIndex(u8 index)
{
    return (index + 1) % NET_LINK_GATEWAY_QUEUE_SIZE;
}

static bool8 QueueIsFull(u8 head, u8 tail)
{
    return NextQueueIndex(head) == tail;
}

static bool8 QueueFrame(u8 frameType, const void *payload, u16 payloadSize)
{
    struct NetLinkGatewayFrame *frame;

    if (payloadSize > NET_LINK_GATEWAY_FRAME_PAYLOAD_SIZE)
        return FALSE;
    if (payloadSize != 0 && payload == NULL)
        return FALSE;
    if (QueueIsFull(sTxHead, sTxTail))
        return FALSE;

    frame = &sTxFrames[sTxHead];
    memset(frame, 0, sizeof(*frame));
    frame->magic = NET_LINK_GATEWAY_FRAME_MAGIC;
    frame->version = NET_LINK_GATEWAY_FRAME_VERSION;
    frame->frameType = frameType;
    frame->payloadSize = payloadSize;
    sNextFrameSequence++;
    if (sNextFrameSequence == 0)
        sNextFrameSequence++;
    frame->sequence = sNextFrameSequence;
    if (payloadSize != 0)
        memcpy(frame->payload, payload, payloadSize);
    sTxHead = NextQueueIndex(sTxHead);
    return TRUE;
}

static bool8 QueueInboundSlot(const struct NetTransportPacketSlot *slot)
{
    if (QueueIsFull(sInboundHead, sInboundTail))
        return FALSE;

    sInboundSlots[sInboundHead] = *slot;
    sInboundHead = NextQueueIndex(sInboundHead);
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
    sInboundHead = 0;
    sInboundTail = 0;
}

static bool8 SessionHeaderIsValid(const struct NetLinkGatewaySessionHeader *header)
{
    if (header->transportMode != NET_TRANSPORT_MODE_LINK_GATEWAY)
        return FALSE;
    if (header->localPlayerId >= MAX_NET_PLAYERS)
        return FALSE;
    if (header->hostPlayerId >= MAX_NET_PLAYERS)
        return FALSE;
    if (header->playerCount > MAX_NET_PLAYERS)
        return FALSE;
    if (header->sessionId == 0 || header->sessionEpoch == 0 || header->playerToken == 0 || header->joinNonce == 0)
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
    if (envelope->sessionId != sView.sessionId)
        return FALSE;
    if (envelope->sessionEpoch != sView.sessionEpoch)
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

static void EnsureLinkOpen(void)
{
    if (sLinkStarted)
        return;

    gLinkType = NET_LINK_GATEWAY_LINK_TYPE;
    OpenLinkTimed();
    sLinkStarted = TRUE;
    sCableReady = FALSE;
}

static void UpdateCableReady(void)
{
    u8 status;

    if (!sLinkStarted || sCableReady)
        return;

    status = GetLinkPlayerDataExchangeStatusTimed(2, 2);
    if (status == EXCHANGE_COMPLETE)
    {
        sCableReady = TRUE;
    }
    else if (status == EXCHANGE_TIMED_OUT || status == EXCHANGE_DIFF_SELECTIONS || status == EXCHANGE_WRONG_NUM_PLAYERS)
    {
        CloseLink();
        sLinkStarted = FALSE;
        sCableReady = FALSE;
    }
}

static bool8 FrameHeaderIsValid(const struct NetLinkGatewayFrame *frame)
{
    if (frame->magic != NET_LINK_GATEWAY_FRAME_MAGIC)
        return FALSE;
    if (frame->version != NET_LINK_GATEWAY_FRAME_VERSION)
        return FALSE;
    if (frame->payloadSize > NET_LINK_GATEWAY_FRAME_PAYLOAD_SIZE)
        return FALSE;
    if (frame->sequence != 0 && frame->sequence <= sLastGatewaySequence)
        return FALSE;

    return TRUE;
}

static void ApplySessionHeader(const struct NetLinkGatewayFrame *frame)
{
    struct NetLinkGatewaySessionHeader header;

    if (frame->payloadSize != sizeof(header))
        return;
    memcpy(&header, frame->payload, sizeof(header));
    if (!SessionHeaderIsValid(&header))
        return;

    sView.connected = header.connected;
    sView.localPlayerId = header.localPlayerId;
    sView.hostPlayerId = header.hostPlayerId;
    sView.playerCount = header.playerCount;
    sView.transportMode = header.transportMode;
    sView.sessionId = header.sessionId;
    sView.sessionEpoch = header.sessionEpoch;
    sView.playerToken = header.playerToken;
    sView.joinNonce = header.joinNonce;
    sView.bridgeTick = header.bridgeTick;
    sView.serverClockSeconds = header.serverClockSeconds;
    SyncTransportSessionIdentity(sView.sessionId, sView.sessionEpoch);
}

static void ApplyPlayerSnapshot(const struct NetLinkGatewayFrame *frame)
{
    struct NetPlayerSnapshot snapshot;

    if (frame->payloadSize != sizeof(snapshot))
        return;
    memcpy(&snapshot, frame->payload, sizeof(snapshot));
    if (snapshot.playerId >= MAX_NET_PLAYERS)
        return;

    sView.players[snapshot.playerId] = snapshot;
}

static void ApplySubsession(const struct NetLinkGatewayFrame *frame)
{
    struct NetLinkGatewaySubsessionFrame subsession;

    if (frame->payloadSize != sizeof(subsession))
        return;
    memcpy(&subsession, frame->payload, sizeof(subsession));
    if (subsession.index >= MAX_NET_SUBSESSIONS)
        return;

    sView.subsessions[subsession.index] = subsession.subsession;
}

static void ApplyPacketSlot(const struct NetLinkGatewayFrame *frame)
{
    struct NetTransportPacketSlot slot;

    if (frame->payloadSize != sizeof(slot))
        return;
    memcpy(&slot, frame->payload, sizeof(slot));
    QueueInboundSlot(&slot);
}

static void ProcessFrame(const struct NetLinkGatewayFrame *frame)
{
    if (!FrameHeaderIsValid(frame))
        return;

    switch (frame->frameType)
    {
    case NET_LINK_FRAME_SESSION_HEADER:
        ApplySessionHeader(frame);
        break;
    case NET_LINK_FRAME_SERVER_PLAYER_SNAPSHOT:
        ApplyPlayerSnapshot(frame);
        break;
    case NET_LINK_FRAME_SERVER_SUBSESSION:
        ApplySubsession(frame);
        break;
    case NET_LINK_FRAME_SERVER_PACKET_SLOT:
        ApplyPacketSlot(frame);
        break;
    }

    if (frame->sequence != 0)
        sLastGatewaySequence = frame->sequence;
}

static void ProcessReceivedFrames(void)
{
    u8 i;
    u8 status;
    u8 localId;

    if (!sCableReady)
        return;

    localId = GetMultiplayerId();
    status = GetBlockReceivedStatus();
    for (i = 0; i < MAX_LINK_PLAYERS; i++)
    {
        if (i == localId)
            continue;
        if (((status >> i) & 1) == 0)
            continue;

        ProcessFrame((const struct NetLinkGatewayFrame *)gBlockRecvBuffer[i]);
        ResetBlockReceivedFlag(i);
    }
}

static void FlushTxQueue(void)
{
    if (!sCableReady)
        return;
    if (sTxTail == sTxHead)
        return;
    if (!IsLinkTaskFinished())
        return;
    if (SendBlock(0, &sTxFrames[sTxTail], sizeof(sTxFrames[sTxTail])))
        sTxTail = NextQueueIndex(sTxTail);
}

void NetTransport_Init(void)
{
    sLinkStarted = FALSE;
    sCableReady = FALSE;
    memset(&sView, 0, sizeof(sView));
    sCurrentSessionId = 0;
    sCurrentSessionEpoch = 0;
    sOutboundSequence = 0;
    sLastInboundSequence = 0;
    sLastGatewaySequence = 0;
    sNextFrameSequence = 0;
    sTxHead = 0;
    sTxTail = 0;
    sInboundHead = 0;
    sInboundTail = 0;
}

void NetTransport_Tick(void)
{
    EnsureLinkOpen();
    UpdateCableReady();
    ProcessReceivedFrames();
    FlushTxQueue();
}

bool8 NetTransport_IsConnected(void)
{
    return sCableReady
        && sView.connected
        && sView.transportMode == NET_TRANSPORT_MODE_LINK_GATEWAY
        && sView.sessionEpoch != 0
        && sView.playerToken != 0
        && sView.joinNonce != 0;
}

bool8 NetTransport_ReadSessionView(struct NetTransportSessionView *view)
{
    if (view == NULL)
        return FALSE;
    if (!NetTransport_IsConnected())
        return FALSE;

    *view = sView;
    return TRUE;
}

bool8 NetTransport_WriteLocalSnapshot(const struct NetPlayerSnapshot *snapshot)
{
    if (snapshot == NULL)
        return FALSE;
    if (!NetTransport_IsConnected())
        return FALSE;
    if (snapshot->playerId != sView.localPlayerId)
        return FALSE;
    if (snapshot->sessionEpoch != sView.sessionEpoch
        || snapshot->playerToken != sView.playerToken
        || snapshot->joinNonce != sView.joinNonce)
        return FALSE;

    if (QueueFrame(NET_LINK_FRAME_CLIENT_PLAYER_SNAPSHOT, snapshot, sizeof(*snapshot)))
    {
        sView.players[snapshot->playerId] = *snapshot;
        return TRUE;
    }

    return FALSE;
}

bool8 NetTransport_SendPacket(u8 packetType, const void *payload, u16 payloadSize)
{
    struct NetTransportPacketSlot slot;

    if (!NetTransport_IsConnected())
        return FALSE;
    SyncTransportSessionIdentity(sView.sessionId, sView.sessionEpoch);
    if (packetType == NET_PACKET_NONE || packetType >= NET_PACKET_COUNT)
        return FALSE;
    if (payloadSize > NET_TRANSPORT_PACKET_PAYLOAD_SIZE)
        return FALSE;
    if (payloadSize != 0 && payload == NULL)
        return FALSE;

    memset(&slot, 0, sizeof(slot));
    sOutboundSequence++;
    if (sOutboundSequence == 0)
        sOutboundSequence++;
    NetProtocol_InitEnvelopeWithEpoch(&slot.envelope, packetType, sView.localPlayerId, sView.sessionId, sView.sessionEpoch, sView.bridgeTick, payloadSize);
    slot.envelope.sequence = sOutboundSequence;
    slot.envelope.checksum = NetProtocol_CalcChecksum(payload, payloadSize);
    if (payloadSize != 0)
        memcpy(slot.payload, payload, payloadSize);

    return QueueFrame(NET_LINK_FRAME_CLIENT_PACKET_SLOT, &slot, sizeof(slot));
}

bool8 NetTransport_SendUnreliablePacket(u8 packetType, const void *payload, u16 payloadSize)
{
    return NetTransport_SendPacket(packetType, payload, payloadSize);
}

bool8 NetTransport_ReceivePacket(struct NetPacketEnvelope *envelope, void *payload, u16 capacity, u16 *payloadSize)
{
    struct NetTransportPacketSlot slot;

    if (envelope == NULL || payloadSize == NULL)
        return FALSE;
    if (!NetTransport_IsConnected())
        return FALSE;
    if (sInboundTail == sInboundHead)
        return FALSE;

    slot = sInboundSlots[sInboundTail];
    sInboundTail = NextQueueIndex(sInboundTail);
    *envelope = slot.envelope;
    if (!PacketEnvelopeIsValid(envelope, capacity))
        return FALSE;
    if (envelope->payloadSize != 0 && payload == NULL)
        return FALSE;
    if (envelope->sequence <= sLastInboundSequence)
        return FALSE;
    if (envelope->payloadSize != 0)
        memcpy(payload, slot.payload, envelope->payloadSize);
    if (envelope->checksum != NetProtocol_CalcChecksum(payload, envelope->payloadSize))
        return FALSE;

    sLastInboundSequence = envelope->sequence;
    *payloadSize = envelope->payloadSize;
    return TRUE;
}

#endif // FEATURE_MULTIPLAYER && FEATURE_MULTIPLAYER_LINK_TRANSPORT
