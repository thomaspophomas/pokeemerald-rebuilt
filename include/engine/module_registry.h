#ifndef GUARD_ENGINE_MODULE_REGISTRY_H
#define GUARD_ENGINE_MODULE_REGISTRY_H

#include "global.h"

#define ENGINE_MODULE_FLAG_RUNTIME_SETTINGS (1 << 0)
#define ENGINE_MODULE_FLAG_MULTIPLAYER      (1 << 1)
#define ENGINE_MODULE_FLAG_MOD_API          (1 << 2)

typedef void (*EngineModuleInitFunc)(void);
typedef void (*EngineModuleFrameFunc)(void);
typedef void (*EngineModuleMapLoadFunc)(void);
typedef void (*EngineModulePlayerStepFunc)(u8 direction, u16 new_keys, u16 held_keys);
typedef void (*EngineModuleBattleFunc)(u32 battle_value);

struct EngineModule
{
    const char *module_id;
    u32 flags;
    EngineModuleInitFunc init;
    EngineModuleFrameFunc frame;
    EngineModuleMapLoadFunc mapLoad;
    EngineModulePlayerStepFunc playerStep;
    EngineModuleBattleFunc battleStart;
    EngineModuleBattleFunc battleEnd;
};

void EngineModules_Init(void);
void EngineModules_RunFrame(void);
void EngineModules_OnMapLoad(void);
void EngineModules_OnPlayerStep(u8 direction, u16 new_keys, u16 held_keys);
void EngineModules_OnBattleStart(u32 battle_type_flags);
void EngineModules_OnBattleEnd(u32 battle_outcome);

#endif // GUARD_ENGINE_MODULE_REGISTRY_H
