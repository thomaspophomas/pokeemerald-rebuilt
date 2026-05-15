#include "global.h"
#include "engine/module_registry.h"
#include "engine/runtime_state.h"

#if FEATURE_MULTIPLAYER
#include "multiplayer/session.h"
#endif

static const struct EngineModule sEngineModules[] =
{
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
    u8 count = 0;

    while (sEngineModules[count].id != NULL)
        count++;

    return count;
}

void EngineModules_Init(void)
{
    u8 i;
    u8 count;

    if (sEngineModulesInitialized)
        return;

    EngineRuntimeState_Init();

    count = GetEngineModuleCount();
    for (i = 0; i < count; i++)
    {
        if (sEngineModules[i].init != NULL)
            sEngineModules[i].init();
    }

    sEngineModulesInitialized = TRUE;
}

void EngineModules_RunFrame(void)
{
    u8 i;
    u8 count;

    if (!sEngineModulesInitialized)
        return;

    count = GetEngineModuleCount();
    for (i = 0; i < count; i++)
    {
        if (sEngineModules[i].frame != NULL)
            sEngineModules[i].frame();
    }
}

void EngineModules_OnMapLoad(void)
{
    u8 i;
    u8 count;

    if (!sEngineModulesInitialized)
        return;

    count = GetEngineModuleCount();
    for (i = 0; i < count; i++)
    {
        if (sEngineModules[i].mapLoad != NULL)
            sEngineModules[i].mapLoad();
    }
}

void EngineModules_OnPlayerStep(u8 direction, u16 newKeys, u16 heldKeys)
{
    u8 i;
    u8 count;

    if (!sEngineModulesInitialized)
        return;

    count = GetEngineModuleCount();
    for (i = 0; i < count; i++)
    {
        if (sEngineModules[i].playerStep != NULL)
            sEngineModules[i].playerStep(direction, newKeys, heldKeys);
    }
}

void EngineModules_OnBattleStart(u32 battleTypeFlags)
{
    u8 i;
    u8 count;

    if (!sEngineModulesInitialized)
        return;

    count = GetEngineModuleCount();
    for (i = 0; i < count; i++)
    {
        if (sEngineModules[i].battleStart != NULL)
            sEngineModules[i].battleStart(battleTypeFlags);
    }
}

void EngineModules_OnBattleEnd(u32 battleOutcome)
{
    u8 i;
    u8 count;

    if (!sEngineModulesInitialized)
        return;

    count = GetEngineModuleCount();
    for (i = 0; i < count; i++)
    {
        if (sEngineModules[i].battleEnd != NULL)
            sEngineModules[i].battleEnd(battleOutcome);
    }
}
