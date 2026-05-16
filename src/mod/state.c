#include "global.h"
#include "mod/state.h"

static EWRAM_DATA struct ModSaveState sModState = {0};

STATIC_ASSERT(sizeof(struct ModSaveState) <= sizeof(((struct SaveBlock1 *)0)->unused_3598), ModSaveStateFitsUnusedSaveBlock1Space);

static void InitState(struct ModSaveState *state)
{
    memset(state, 0, sizeof(*state));
    state->magic = MOD_SAVE_STATE_MAGIC;
    state->version = MOD_SAVE_STATE_VERSION;
    state->size = sizeof(*state);
    NetServerConfig_InitDefaults(&state->multiplayerServerConfig);
}

void ModState_Init(void)
{
    if (sModState.magic == MOD_SAVE_STATE_MAGIC
     && sModState.version == MOD_SAVE_STATE_VERSION
     && sModState.size == sizeof(sModState))
        return;

    InitState(&sModState);
}

struct ModSaveState *ModState_Get(void)
{
    ModState_Init();
    return &sModState;
}

void ModState_LoadFromSave(void)
{
    struct ModSaveState *saveState;

    ModState_Init();
    if (gSaveBlock1Ptr == NULL)
        return;

    saveState = (struct ModSaveState *)gSaveBlock1Ptr->unused_3598;
    if (saveState->magic != MOD_SAVE_STATE_MAGIC
     || saveState->version != MOD_SAVE_STATE_VERSION
     || saveState->size != sizeof(*saveState))
    {
        InitState(&sModState);
        ModState_SaveToSave();
        return;
    }

    memcpy(&sModState, saveState, sizeof(sModState));
    if (!NetServerConfig_IsValid(&sModState.multiplayerServerConfig))
        NetServerConfig_InitDefaults(&sModState.multiplayerServerConfig);
}

void ModState_SaveToSave(void)
{
    ModState_Init();
    if (gSaveBlock1Ptr == NULL)
        return;

    memcpy(gSaveBlock1Ptr->unused_3598, &sModState, sizeof(sModState));
}

u32 ModState_GetRevision(void)
{
    ModState_Init();
    return sModState.revision;
}

void ModState_BumpRevision(void)
{
    ModState_Init();
    sModState.revision++;
    ModState_SaveToSave();
}
