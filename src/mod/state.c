#include "global.h"
#include "generated/mod_registry.h"
#include "mod/state.h"

static EWRAM_DATA struct ModSaveState sModState = {0};

STATIC_ASSERT(sizeof(struct ModSaveState) <= sizeof(((struct SaveBlock1 *)0)->unused_3598), ModSaveStateFitsUnusedSaveBlock1Space);

static void InitState(struct ModSaveState *state)
{
    memset(state, 0, sizeof(*state));
    state->magic = MOD_SAVE_STATE_MAGIC;
    state->version = MOD_SAVE_STATE_VERSION;
    state->size = sizeof(*state);
}

static const struct ModManifest *FindManifest(const char *mod_id, u16 *manifest_index)
{
    u16 index;

    if (mod_id == NULL)
        return NULL;

    for (index = 0; index < gModManifestCount; index++)
    {
        if (gModManifests[index].mod_id == NULL)
            continue;
        if (strcmp(gModManifests[index].mod_id, mod_id) == 0)
        {
            if (manifest_index != NULL)
                *manifest_index = index;
            return &gModManifests[index];
        }
    }

    return NULL;
}

static bool8 IsStateBlockInRange(const struct ModManifest *manifest, u16 size)
{
    if (manifest == NULL || manifest->state_bytes == 0)
        return FALSE;
    if (size > manifest->state_bytes)
        return FALSE;
    if (manifest->state_offset > MOD_SAVE_RESERVED_BYTES)
        return FALSE;
    if (manifest->state_bytes > MOD_SAVE_RESERVED_BYTES - manifest->state_offset)
        return FALSE;
    return TRUE;
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

void *ModState_GetBlock(const char *mod_id, u16 size, u16 version)
{
    const struct ModManifest *manifest;

    (void)version;
    ModState_Init();

    manifest = FindManifest(mod_id, NULL);
    if (!IsStateBlockInRange(manifest, size))
        return NULL;

    return &sModState.reserved[manifest->state_offset];
}

bool8 ModState_IsBlockFresh(const char *mod_id)
{
    const struct ModManifest *manifest;
    u16 manifest_index = 0;

    ModState_Init();
    manifest = FindManifest(mod_id, &manifest_index);
    if (manifest == NULL || manifest_index >= MOD_SAVE_MAX_STATE_BLOCKS)
        return FALSE;

    return sModState.block_versions[manifest_index] == 0;
}

bool8 ModState_NeedsMigration(const char *mod_id)
{
    const struct ModManifest *manifest;
    u16 manifest_index = 0;

    ModState_Init();
    manifest = FindManifest(mod_id, &manifest_index);
    if (manifest == NULL || manifest_index >= MOD_SAVE_MAX_STATE_BLOCKS)
        return FALSE;
    if (manifest->state_bytes == 0)
        return FALSE;

    return sModState.block_versions[manifest_index] != manifest->state_version;
}

bool8 ModState_MarkMigrated(const char *mod_id)
{
    const struct ModManifest *manifest;
    u16 manifest_index = 0;

    ModState_Init();
    manifest = FindManifest(mod_id, &manifest_index);
    if (manifest == NULL || manifest_index >= MOD_SAVE_MAX_STATE_BLOCKS)
        return FALSE;

    sModState.block_versions[manifest_index] = manifest->state_version;
    ModState_BumpRevision();
    return TRUE;
}
