#ifndef GUARD_MOD_STATE_H
#define GUARD_MOD_STATE_H

#include "mod/base.h"

#define MOD_SAVE_STATE_MAGIC 0x4D535456 // "MSTV"
#define MOD_SAVE_STATE_VERSION 1
#define MOD_SAVE_FLAG_BYTES 128
#define MOD_SAVE_BADGE_LEVEL_COUNT 8
#define MOD_SAVE_BADGE_LEVEL_BYTES MOD_SAVE_BADGE_LEVEL_COUNT
#define MOD_SAVE_MAX_STATE_BLOCKS 16
#define MOD_SAVE_STATE_VERSION_BYTES (MOD_SAVE_MAX_STATE_BLOCKS * 2)
#define MOD_SAVE_RESERVED_BYTES (224 - MOD_SAVE_BADGE_LEVEL_BYTES - MOD_SAVE_STATE_VERSION_BYTES)

struct ModSaveState
{
    u32 magic;
    u16 version;
    u16 size;
    u32 revision;
    u8 flags[MOD_SAVE_FLAG_BYTES];
    u8 badge_levels[MOD_SAVE_BADGE_LEVEL_BYTES];
    u16 block_versions[MOD_SAVE_MAX_STATE_BLOCKS];
    u8 reserved[MOD_SAVE_RESERVED_BYTES];
};

void ModState_Init(void);
struct ModSaveState *ModState_Get(void);
void ModState_LoadFromSave(void);
void ModState_SaveToSave(void);
u32 ModState_GetRevision(void);
void ModState_BumpRevision(void);
void *ModState_GetBlock(const char *mod_id, u16 size, u16 version);
bool8 ModState_IsBlockFresh(const char *mod_id);
bool8 ModState_NeedsMigration(const char *mod_id);
bool8 ModState_MarkMigrated(const char *mod_id);

#endif // GUARD_MOD_STATE_H
