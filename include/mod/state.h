#ifndef GUARD_MOD_STATE_H
#define GUARD_MOD_STATE_H

#include "global.h"
#include "multiplayer/server_config.h"

#define MOD_SAVE_STATE_MAGIC 0x4D535456 // "MSTV"
#define MOD_SAVE_STATE_VERSION 2
#define MOD_SAVE_FLAG_BYTES 128
#define MOD_SAVE_RESERVED_BYTES 144

struct ModSaveState
{
    u32 magic;
    u16 version;
    u16 size;
    u32 revision;
    u8 flags[MOD_SAVE_FLAG_BYTES];
    struct NetServerConfig multiplayerServerConfig;
    u8 reserved[MOD_SAVE_RESERVED_BYTES];
};

void ModState_Init(void);
struct ModSaveState *ModState_Get(void);
void ModState_LoadFromSave(void);
void ModState_SaveToSave(void);
u32 ModState_GetRevision(void);
void ModState_BumpRevision(void);

#endif // GUARD_MOD_STATE_H
