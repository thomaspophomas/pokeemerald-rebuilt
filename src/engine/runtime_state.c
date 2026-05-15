#include "global.h"
#include "engine/runtime_state.h"

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
