#ifndef GUARD_ENGINE_RUNTIME_STATE_H
#define GUARD_ENGINE_RUNTIME_STATE_H

#include "global.h"

#define ENGINE_RUNTIME_STATE_MAGIC 0x4D4F4453 // "MODS"
#define ENGINE_RUNTIME_STATE_VERSION 1

#define ENGINE_RUNTIME_FLAG_MULTIPLAYER_ENABLED (1 << 0)
#define ENGINE_RUNTIME_FLAG_FOLLOWERS_ENABLED   (1 << 1)
#define ENGINE_RUNTIME_FLAG_DAY_NIGHT_ENABLED   (1 << 2)
#define ENGINE_RUNTIME_FLAG_WEATHER_LAYERS      (1 << 3)

struct EngineRuntimeState
{
    u32 magic;
    u16 version;
    u16 size;
    u32 runtimeFlags;
    u8 multiplayerMode;
    u8 weatherMode;
    u8 followerMode;
    u8 storyMode;
    u8 reserved[16];
};

void EngineRuntimeState_Init(void);
const struct EngineRuntimeState *EngineRuntimeState_Get(void);
bool8 EngineRuntimeState_HasFlag(u32 flag);
void EngineRuntimeState_SetFlag(u32 flag);
void EngineRuntimeState_ClearFlag(u32 flag);
u8 EngineRuntimeState_NormalizeMultiplayerMode(u8 mode);
void EngineRuntimeState_LoadFromSave(void);
void EngineRuntimeState_SaveToSave(void);
u8 EngineRuntimeState_GetMultiplayerMode(void);
void EngineRuntimeState_SetMultiplayerMode(u8 mode);
bool8 EngineRuntimeState_IsMultiplayerOnlineEnabled(void);

#endif // GUARD_ENGINE_RUNTIME_STATE_H
