#include "global.h"
#include "multiplayer/server_config.h"

static void CopyLabel(char *dest, const char *src)
{
    u8 i;

    for (i = 0; i < NET_SERVER_LABEL_LENGTH && src[i] != '\0'; i++)
        dest[i] = src[i];
    dest[i] = '\0';
}

static void InitProfile(struct NetServerProfile *profile, bool8 active, const char *label, u8 a, u8 b, u8 c, u8 d, u16 port)
{
    memset(profile, 0, sizeof(*profile));
    profile->active = active;
    profile->port = port;
    profile->ipv4[0] = a;
    profile->ipv4[1] = b;
    profile->ipv4[2] = c;
    profile->ipv4[3] = d;
    if (label != NULL)
        CopyLabel(profile->label, label);
}

void NetServerConfig_InitDefaults(struct NetServerConfig *config)
{
    if (config == NULL)
        return;

    memset(config, 0, sizeof(*config));
    config->magic = NET_SERVER_CONFIG_MAGIC;
    config->version = NET_SERVER_CONFIG_VERSION;
    config->size = sizeof(*config);
    config->selectedSlot = 0;
    config->lastConnectionStatus = NET_CONNECTION_STATUS_NONE;
    config->revision = 1;
    InitProfile(&config->profiles[0], TRUE, "TAILSCALE", 100, 64, 0, 1, NET_DEFAULT_SERVER_PORT);
    InitProfile(&config->profiles[1], FALSE, "SERVER 2", 0, 0, 0, 0, NET_DEFAULT_SERVER_PORT);
    InitProfile(&config->profiles[2], FALSE, "SERVER 3", 0, 0, 0, 0, NET_DEFAULT_SERVER_PORT);
}

bool8 NetServerConfig_IsValid(const struct NetServerConfig *config)
{
    if (config == NULL)
        return FALSE;
    if (config->magic != NET_SERVER_CONFIG_MAGIC)
        return FALSE;
    if (config->version != NET_SERVER_CONFIG_VERSION)
        return FALSE;
    if (config->size != sizeof(*config))
        return FALSE;
    if (config->selectedSlot >= NET_SERVER_PROFILE_COUNT)
        return FALSE;
    if (config->profiles[config->selectedSlot].port == 0)
        return FALSE;

    return TRUE;
}

const struct NetServerProfile *NetServerConfig_GetSelectedProfile(const struct NetServerConfig *config)
{
    if (!NetServerConfig_IsValid(config))
        return NULL;
    if (!config->profiles[config->selectedSlot].active)
        return NULL;

    return &config->profiles[config->selectedSlot];
}
