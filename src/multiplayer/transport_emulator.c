#include "global.h"
#include "multiplayer/transport.h"

#if FEATURE_MULTIPLAYER && FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT

struct EmulatorBridgeBuffer
{
    u32 magic;
    u16 version;
    u8 connected;
    u8 localPlayerId;
    u8 hostPlayerId;
    u8 playerCount;
    u8 reserved[3];
    u32 sessionId;
    u32 bridgeTick;
    struct NetPlayerSnapshot players[MAX_NET_PLAYERS];
    struct MultiplayerSubsession subsessions[MAX_NET_SUBSESSIONS];
    struct NetTransportPacketSlot outbound;
    struct NetTransportPacketSlot inbound;
} __attribute__((packed));

static volatile struct EmulatorBridgeBuffer *const sBridge = (volatile struct EmulatorBridgeBuffer *)NET_EMULATOR_SHARED_BASE;

static bool8 BridgeHeaderIsValid(void)
{
    if (sBridge->magic != NET_EMULATOR_BRIDGE_MAGIC)
        return FALSE;
    if (sBridge->version != NET_EMULATOR_BRIDGE_VERSION)
        return FALSE;
    if (sBridge->localPlayerId >= MAX_NET_PLAYERS)
        return FALSE;
    if (sBridge->playerCount > MAX_NET_PLAYERS)
        return FALSE;

    return TRUE;
}

#endif

void NetTransport_Init(void)
{
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
    if (!NetTransport_IsConnected())
        return FALSE;

    view->connected = sBridge->connected;
    view->localPlayerId = sBridge->localPlayerId;
    view->hostPlayerId = sBridge->hostPlayerId;
    view->playerCount = sBridge->playerCount;
    view->sessionId = sBridge->sessionId;
    view->bridgeTick = sBridge->bridgeTick;
    memcpy(view->players, (const void *)sBridge->players, sizeof(view->players));
    memcpy(view->subsessions, (const void *)sBridge->subsessions, sizeof(view->subsessions));
    return TRUE;
#else
    memset(view, 0, sizeof(*view));
    return FALSE;
#endif
}

bool8 NetTransport_WriteLocalSnapshot(const struct NetPlayerSnapshot *snapshot)
{
#if FEATURE_MULTIPLAYER && FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT
    if (!NetTransport_IsConnected())
        return FALSE;
    if (snapshot->playerId >= MAX_NET_PLAYERS)
        return FALSE;

    memcpy((void *)&sBridge->players[snapshot->playerId], snapshot, sizeof(*snapshot));
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
    if (payloadSize > NET_TRANSPORT_PACKET_PAYLOAD_SIZE)
        return FALSE;

    NetProtocol_InitEnvelope(&envelope, packetType, sBridge->localPlayerId, sBridge->sessionId, sBridge->bridgeTick, payloadSize);
    envelope.checksum = NetProtocol_CalcChecksum(payload, payloadSize);
    memcpy((void *)&sBridge->outbound.envelope, &envelope, sizeof(envelope));
    if (payloadSize != 0)
        memcpy((void *)sBridge->outbound.payload, payload, payloadSize);

    return TRUE;
#else
    return FALSE;
#endif
}

bool8 NetTransport_ReceivePacket(struct NetPacketEnvelope *envelope, void *payload, u16 capacity, u16 *payloadSize)
{
#if FEATURE_MULTIPLAYER && FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT
    if (!NetTransport_IsConnected())
        return FALSE;
    if (sBridge->inbound.envelope.magic != NET_PROTOCOL_MAGIC)
        return FALSE;
    if (sBridge->inbound.envelope.payloadSize > capacity)
        return FALSE;

    memcpy(envelope, (const void *)&sBridge->inbound.envelope, sizeof(*envelope));
    if (envelope->payloadSize != 0)
        memcpy(payload, (const void *)sBridge->inbound.payload, envelope->payloadSize);
    *payloadSize = envelope->payloadSize;

    return TRUE;
#else
    *payloadSize = 0;
    return FALSE;
#endif
}
