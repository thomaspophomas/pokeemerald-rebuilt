#ifndef GUARD_ENGINE_MODULE_REGISTRY_H
#define GUARD_ENGINE_MODULE_REGISTRY_H

#include "global.h"

#define ENGINE_MODULE_FLAG_RUNTIME_SETTINGS (1 << 0)
#define ENGINE_MODULE_FLAG_MULTIPLAYER      (1 << 1)

typedef void (*EngineModuleInitFunc)(void);
typedef void (*EngineModuleFrameFunc)(void);
typedef void (*EngineModuleMapLoadFunc)(void);
typedef void (*EngineModulePlayerStepFunc)(u8 direction, u16 newKeys, u16 heldKeys);
typedef void (*EngineModuleBattleFunc)(u32 value);

struct EngineModule
{
    const char *id;
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
void EngineModules_OnPlayerStep(u8 direction, u16 newKeys, u16 heldKeys);
void EngineModules_OnBattleStart(u32 battleTypeFlags);
void EngineModules_OnBattleEnd(u32 battleOutcome);

#endif // GUARD_ENGINE_MODULE_REGISTRY_H
