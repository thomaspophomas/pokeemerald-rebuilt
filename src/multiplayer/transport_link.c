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

static bool8 QueueFrame(u8 frame_type, const void *frame_payload, u16 frame_payload_size)
{
    struct NetLinkGatewayFrame *frame;

    if (frame_payload_size > NET_LINK_GATEWAY_FRAME_PAYLOAD_SIZE)
        return FALSE;
    if (frame_payload_size != 0 && frame_payload == NULL)
        return FALSE;
    if (QueueIsFull(sTxHead, sTxTail))
        return FALSE;

    frame = &sTxFrames[sTxHead];
    memset(frame, 0, sizeof(*frame));
    frame->magic = NET_LINK_GATEWAY_FRAME_MAGIC;
    frame->version = NET_LINK_GATEWAY_FRAME_VERSION;
    frame->frameType = frame_type;
    frame->payloadSize = frame_payload_size;
    sNextFrameSequence++;
    if (sNextFrameSequence == 0)
        sNextFrameSequence++;
    frame->sequence = sNextFrameSequence;
    if (frame_payload_size != 0)
        memcpy(frame->payload, frame_payload, frame_payload_size);
    sTxHead = NextQueueIndex(sTxHead);
    return TRUE;
}

static bool8 QueueInboundPacketSlot(const struct NetTransportPacketSlot *packet_slot_from_gateway)
{
    if (QueueIsFull(sInboundHead, sInboundTail))
        return FALSE;

    sInboundSlots[sInboundHead] = *packet_slot_from_gateway;
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
    if (envelope->payloadSize > payload_capacity || envelope->payloadSize > NET_TRANSPORT_PACKET_PAYLOAD_SIZE)
        return FALSE;

    return TRUE;
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
    NetProtocol_InitEnvelopeWithEpoch(envelope, packet_type, sView.localPlayerId, sView.sessionId, sView.sessionEpoch, sView.bridgeTick, packet_payload_size);
    envelope->sequence = NextOutboundSequence();
}

static void FinishOutboundEnvelope(struct NetPacketEnvelope *envelope, const void *packet_payload)
{
    envelope->checksum = NetProtocol_CalcChecksum(packet_payload, envelope->payloadSize);
}

static void BuildOutboundPacketSlot(struct NetTransportPacketSlot *packet_slot_for_client, const struct NetPacketEnvelope *envelope, const void *packet_payload)
{
    memset(packet_slot_for_client, 0, sizeof(*packet_slot_for_client));
    if (envelope->payloadSize != 0)
        memcpy(packet_slot_for_client->payload, packet_payload, envelope->payloadSize);
    NetTransport_CopyEnvelopeToPacketHeader(&packet_slot_for_client->header, envelope);
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
    struct NetTransportPacketSlot packet_slot_from_gateway;

    if (frame->payloadSize != sizeof(packet_slot_from_gateway))
        return;
    memcpy(&packet_slot_from_gateway, frame->payload, sizeof(packet_slot_from_gateway));
    QueueInboundPacketSlot(&packet_slot_from_gateway);
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
    u8 remote_link_player_id;
    u8 status;
    u8 localId;

    if (!sCableReady)
        return;

    localId = GetMultiplayerId();
    status = GetBlockReceivedStatus();
    for (remote_link_player_id = 0; remote_link_player_id < MAX_LINK_PLAYERS; remote_link_player_id++)
    {
        if (remote_link_player_id == localId)
            continue;
        if (((status >> remote_link_player_id) & 1) == 0)
            continue;

        ProcessFrame((const struct NetLinkGatewayFrame *)gBlockRecvBuffer[remote_link_player_id]);
        ResetBlockReceivedFlag(remote_link_player_id);
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
    if (snapshot->sessionEpoch != sView.sessionEpoch)
        return FALSE;

    if (QueueFrame(NET_LINK_FRAME_CLIENT_PLAYER_SNAPSHOT, snapshot, sizeof(*snapshot)))
    {
        sView.players[snapshot->playerId] = *snapshot;
        return TRUE;
    }

    return FALSE;
}

bool8 NetTransport_SendPacket(u8 packet_type, const void *packet_payload, u16 packet_payload_size)
{
    struct NetPacketEnvelope envelope;
    struct NetTransportPacketSlot packet_slot_for_client;

    if (!OutboundPacketCanBeSent(packet_type, packet_payload, packet_payload_size))
        return FALSE;

    SyncTransportSessionIdentity(sView.sessionId, sView.sessionEpoch);
    InitOutboundEnvelope(&envelope, packet_type, packet_payload_size);
    FinishOutboundEnvelope(&envelope, packet_payload);
    BuildOutboundPacketSlot(&packet_slot_for_client, &envelope, packet_payload);

    return QueueFrame(NET_LINK_FRAME_CLIENT_PACKET_SLOT, &packet_slot_for_client, sizeof(packet_slot_for_client));
}

bool8 NetTransport_SendUnreliablePacket(u8 packet_type, const void *packet_payload, u16 packet_payload_size)
{
    return NetTransport_SendPacket(packet_type, packet_payload, packet_payload_size);
}

bool8 NetTransport_ReceivePacket(struct NetPacketEnvelope *envelope, void *packet_payload, u16 payload_capacity, u16 *received_payload_size)
{
    struct NetTransportPacketSlot packet_slot_from_gateway;

    if (envelope == NULL || received_payload_size == NULL)
        return FALSE;
    if (!NetTransport_IsConnected())
        return FALSE;
    if (sInboundTail == sInboundHead)
        return FALSE;

    packet_slot_from_gateway = sInboundSlots[sInboundTail];
    sInboundTail = NextQueueIndex(sInboundTail);
    NetTransport_CopyPacketHeaderToEnvelope(envelope, &packet_slot_from_gateway.header);
    if (!PacketEnvelopeIsValid(envelope, payload_capacity))
        return FALSE;
    if (envelope->payloadSize != 0 && packet_payload == NULL)
        return FALSE;
    if (envelope->sequence <= sLastInboundSequence)
        return FALSE;
    if (envelope->payloadSize != 0)
        memcpy(packet_payload, packet_slot_from_gateway.payload, envelope->payloadSize);
    if (envelope->checksum != NetProtocol_CalcChecksum(packet_payload, envelope->payloadSize))
        return FALSE;

    sLastInboundSequence = envelope->sequence;
    *received_payload_size = envelope->payloadSize;
    return TRUE;
}

#endif // FEATURE_MULTIPLAYER && FEATURE_MULTIPLAYER_LINK_TRANSPORT
