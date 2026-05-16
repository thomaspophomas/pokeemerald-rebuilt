#include "global.h"
#include "engine/runtime_state.h"
#include "mod/state.h"

static EWRAM_DATA struct EngineRuntimeState sEngineRuntimeState = {0};

void EngineRuntimeState_Init(void)
{
    if (sEngineRuntimeState.magic == ENGINE_RUNTIME_STATE_MAGIC
     && sEngineRuntimeState.version == ENGINE_RUNTIME_STATE_VERSION
     && sEngineRuntimeState.size == sizeof(sEngineRuntimeState))
        return;

    memset(&sEngineRuntimeState, 0, sizeof(sEngineRuntimeState));
    sEngineRuntimeState.magic = ENGINE_RUNTIME_STATE_MAGIC;
    sEngineRuntimeState.version = ENGINE_RUNTIME_STATE_VERSION;
    sEngineRuntimeState.size = sizeof(sEngineRuntimeState);
    sEngineRuntimeState.multiplayerMode = OPTIONS_MULTIPLAYER_MODE_SOLO;
}

const struct EngineRuntimeState *EngineRuntimeState_Get(void)
{
    return &sEngineRuntimeState;
}

bool8 EngineRuntimeState_HasFlag(u32 flag)
{
    return (sEngineRuntimeState.runtimeFlags & flag) != 0;
}

void EngineRuntimeState_SetFlag(u32 flag)
{
    sEngineRuntimeState.runtimeFlags |= flag;
}

void EngineRuntimeState_ClearFlag(u32 flag)
{
    sEngineRuntimeState.runtimeFlags &= ~flag;
}

u8 EngineRuntimeState_NormalizeMultiplayerMode(u8 mode)
{
    if (mode >= OPTIONS_MULTIPLAYER_MODE_COUNT)
        return OPTIONS_MULTIPLAYER_MODE_SOLO;

    return mode;
}

void EngineRuntimeState_LoadFromSave(void)
{
    u8 mode;

    EngineRuntimeState_Init();
    mode = EngineRuntimeState_NormalizeMultiplayerMode(gSaveBlock2Ptr->optionsMultiplayerMode);
    gSaveBlock2Ptr->optionsMultiplayerMode = mode;
    gSaveBlock2Ptr->optionsMultiplayerReserved = 0;
    EngineRuntimeState_SetMultiplayerMode(mode);
}

void EngineRuntimeState_SaveToSave(void)
{
    EngineRuntimeState_Init();
    gSaveBlock2Ptr->optionsMultiplayerMode = EngineRuntimeState_NormalizeMultiplayerMode(sEngineRuntimeState.multiplayerMode);
    gSaveBlock2Ptr->optionsMultiplayerReserved = 0;
}

u8 EngineRuntimeState_GetMultiplayerMode(void)
{
    EngineRuntimeState_Init();
    return EngineRuntimeState_NormalizeMultiplayerMode(sEngineRuntimeState.multiplayerMode);
}

void EngineRuntimeState_SetMultiplayerMode(u8 mode)
{
    EngineRuntimeState_Init();
    sEngineRuntimeState.multiplayerMode = EngineRuntimeState_NormalizeMultiplayerMode(mode);
    if (sEngineRuntimeState.multiplayerMode == OPTIONS_MULTIPLAYER_MODE_ONLINE)
        EngineRuntimeState_SetFlag(ENGINE_RUNTIME_FLAG_MULTIPLAYER_ENABLED);
    else
        EngineRuntimeState_ClearFlag(ENGINE_RUNTIME_FLAG_MULTIPLAYER_ENABLED);

    EngineRuntimeState_SaveToSave();
}

bool8 EngineRuntimeState_IsMultiplayerOnlineEnabled(void)
{
    return EngineRuntimeState_GetMultiplayerMode() == OPTIONS_MULTIPLAYER_MODE_ONLINE
        && EngineRuntimeState_HasFlag(ENGINE_RUNTIME_FLAG_MULTIPLAYER_ENABLED);
}

const struct NetServerConfig *EngineRuntimeState_GetServerConfig(void)
{
    struct ModSaveState *state = ModState_Get();

    if (!NetServerConfig_IsValid(&state->multiplayerServerConfig))
    {
        NetServerConfig_InitDefaults(&state->multiplayerServerConfig);
        ModState_SaveToSave();
    }

    return &state->multiplayerServerConfig;
}

const struct NetServerProfile *EngineRuntimeState_GetSelectedServerProfile(void)
{
    return NetServerConfig_GetSelectedProfile(EngineRuntimeState_GetServerConfig());
}

void EngineRuntimeState_SetSelectedServerProfile(u8 slot)
{
    struct ModSaveState *state = ModState_Get();

    if (slot >= NET_SERVER_PROFILE_COUNT)
        return;
    if (!NetServerConfig_IsValid(&state->multiplayerServerConfig))
        NetServerConfig_InitDefaults(&state->multiplayerServerConfig);
    state->multiplayerServerConfig.selectedSlot = slot;
    state->multiplayerServerConfig.revision++;
    ModState_SaveToSave();
}

void EngineRuntimeState_SetServerProfile(u8 slot, const struct NetServerProfile *profile)
{
    struct ModSaveState *state = ModState_Get();

    if (slot >= NET_SERVER_PROFILE_COUNT || profile == NULL)
        return;
    if (!NetServerConfig_IsValid(&state->multiplayerServerConfig))
        NetServerConfig_InitDefaults(&state->multiplayerServerConfig);
    state->multiplayerServerConfig.profiles[slot] = *profile;
    state->multiplayerServerConfig.revision++;
    ModState_SaveToSave();
}

u8 EngineRuntimeState_GetConnectionStatus(void)
{
    return EngineRuntimeState_GetServerConfig()->lastConnectionStatus;
}

void EngineRuntimeState_SetConnectionStatus(u8 status)
{
    struct ModSaveState *state = ModState_Get();

    if (!NetServerConfig_IsValid(&state->multiplayerServerConfig))
        NetServerConfig_InitDefaults(&state->multiplayerServerConfig);
    state->multiplayerServerConfig.lastConnectionStatus = status;
    ModState_SaveToSave();
}
