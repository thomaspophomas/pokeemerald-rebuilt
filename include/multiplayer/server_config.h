#ifndef GUARD_MULTIPLAYER_SERVER_CONFIG_H
#define GUARD_MULTIPLAYER_SERVER_CONFIG_H

#include "global.h"
#include "multiplayer/constants.h"

#define NET_SERVER_CONFIG_MAGIC 0x4E534346 // "NSCF"
#define NET_SERVER_CONFIG_VERSION 1

enum NetConnectionStatus
{
    NET_CONNECTION_STATUS_NONE,
    NET_CONNECTION_STATUS_BRIDGE_MISSING,
    NET_CONNECTION_STATUS_CONNECTING,
    NET_CONNECTION_STATUS_CONNECTED,
    NET_CONNECTION_STATUS_SERVER_REFUSED,
    NET_CONNECTION_STATUS_BUILD_MISMATCH,
    NET_CONNECTION_STATUS_STALE,
    NET_CONNECTION_STATUS_RESYNCING,
    NET_CONNECTION_STATUS_DISCONNECTED,
    NET_CONNECTION_STATUS_BACKPRESSURE,
    NET_CONNECTION_STATUS_BAD_SERVER_CONFIG,
};

struct NetServerProfile
{
    bool8 active;
    u8 flags;
    u16 port;
    u8 ipv4[4];
    char label[NET_SERVER_LABEL_LENGTH + 1];
    u8 reserved;
};

struct NetServerConfig
{
    u32 magic;
    u16 version;
    u16 size;
    u8 selectedSlot;
    u8 lastConnectionStatus;
    u16 revision;
    struct NetServerProfile profiles[NET_SERVER_PROFILE_COUNT];
};

void NetServerConfig_InitDefaults(struct NetServerConfig *config);
bool8 NetServerConfig_IsValid(const struct NetServerConfig *config);
const struct NetServerProfile *NetServerConfig_GetSelectedProfile(const struct NetServerConfig *config);

#endif // GUARD_MULTIPLAYER_SERVER_CONFIG_H
