#include "global.h"
#include "engine/module_registry.h"
#include "engine/runtime_state.h"

#if FEATURE_MODS
#include "mod/core.h"
#endif

#if FEATURE_MULTIPLAYER
#include "multiplayer/session.h"
#endif

static const struct EngineModule sEngineModules[] =
{
#if FEATURE_MODS
    {
        "mods",
        ENGINE_MODULE_FLAG_RUNTIME_SETTINGS | ENGINE_MODULE_FLAG_MOD_API,
        ModApi_Init,
        ModApi_RunFrame,
        ModApi_OnMapLoad,
        ModApi_OnPlayerStep,
        ModApi_OnBattleStart,
        ModApi_OnBattleEnd,
    },
#endif
#if FEATURE_MULTIPLAYER
    {
        "multiplayer",
        ENGINE_MODULE_FLAG_RUNTIME_SETTINGS | ENGINE_MODULE_FLAG_MULTIPLAYER,
        MultiplayerSession_Init,
        MultiplayerSession_Tick,
        MultiplayerSession_OnMapLoad,
        MultiplayerSession_OnPlayerStep,
        MultiplayerSession_OnBattleStart,
        MultiplayerSession_OnBattleEnd,
    },
#endif
    {0}
};

static bool8 sEngineModulesInitialized;

static u8 GetEngineModuleCount(void)
{
    u8 module_count = 0;

    while (sEngineModules[module_count].module_id != NULL)
        module_count++;

    return module_count;
}

void EngineModules_Init(void)
{
    u8 module_index;
    u8 module_count;

    if (sEngineModulesInitialized)
        return;

    EngineRuntimeState_Init();

    module_count = GetEngineModuleCount();
    for (module_index = 0; module_index < module_count; module_index++)
    {
        if (sEngineModules[module_index].init != NULL)
            sEngineModules[module_index].init();
    }

    sEngineModulesInitialized = TRUE;
}

void EngineModules_RunFrame(void)
{
    u8 module_index;
    u8 module_count;

    if (!sEngineModulesInitialized)
        return;

    module_count = GetEngineModuleCount();
    for (module_index = 0; module_index < module_count; module_index++)
    {
        if (sEngineModules[module_index].frame != NULL)
            sEngineModules[module_index].frame();
    }
}

void EngineModules_OnMapLoad(void)
{
    u8 module_index;
    u8 module_count;

    if (!sEngineModulesInitialized)
        return;

    module_count = GetEngineModuleCount();
    for (module_index = 0; module_index < module_count; module_index++)
    {
        if (sEngineModules[module_index].mapLoad != NULL)
            sEngineModules[module_index].mapLoad();
    }
}

void EngineModules_OnPlayerStep(u8 direction, u16 new_keys, u16 held_keys)
{
    u8 module_index;
    u8 module_count;

    if (!sEngineModulesInitialized)
        return;

    module_count = GetEngineModuleCount();
    for (module_index = 0; module_index < module_count; module_index++)
    {
        if (sEngineModules[module_index].playerStep != NULL)
            sEngineModules[module_index].playerStep(direction, new_keys, held_keys);
    }
}

void EngineModules_OnBattleStart(u32 battle_type_flags)
{
    u8 module_index;
    u8 module_count;

    if (!sEngineModulesInitialized)
        return;

    module_count = GetEngineModuleCount();
    for (module_index = 0; module_index < module_count; module_index++)
    {
        if (sEngineModules[module_index].battleStart != NULL)
            sEngineModules[module_index].battleStart(battle_type_flags);
    }
}

void EngineModules_OnBattleEnd(u32 battle_outcome)
{
    u8 module_index;
    u8 module_count;

    if (!sEngineModulesInitialized)
        return;

    module_count = GetEngineModuleCount();
    for (module_index = 0; module_index < module_count; module_index++)
    {
        if (sEngineModules[module_index].battleEnd != NULL)
            sEngineModules[module_index].battleEnd(battle_outcome);
    }
}
