#include "global.h"
#include "mod/badge.h"
#include "mod/core.h"
#include "mod/engine.h"
#include "mod/event.h"
#include "mod/follower.h"
#include "mod/flags.h"
#include "mod/language.h"
#include "mod/runtime_profile.h"
#include "mod/state.h"
#include "mod/time.h"
#include "mod/weather.h"

void ModApi_Init(void)
{
    ModState_Init();
    ModState_LoadFromSave();
    LanguageApi_Init();
    ModEvent_Init();
    ModFlag_Init();
    BadgeApi_Init();
    ModTime_Init();
    ModWeather_Init();
    EngineApi_Init();
    ModRuntimeProfile_Init();
    ModFollower_Init();
    ModEvent_Emit(MOD_EVENT_GAME_INIT, NULL, 0);
}

void ModApi_RunFrame(void)
{
    ModEvent_BeginFrame();
    ModTime_RunFrame();
    ModWeather_RunFrame();
    ModFollower_RunFrame();
}

void ModApi_OnMapLoad(void)
{
    ModRuntimeProfile_OnMapLoad();
    ModWeather_OnMapLoad();
    ModFollower_OnMapLoad();
    ModEvent_Emit(MOD_EVENT_MAP_LOAD, NULL, 0);
}

void ModApi_OnPlayerStep(u8 direction, u16 newKeys, u16 heldKeys)
{
    struct ModEventPlayerStep payload;

    payload.direction = direction;
    payload.newKeys = newKeys;
    payload.heldKeys = heldKeys;
    ModEvent_Emit(MOD_EVENT_PLAYER_STEP, &payload, sizeof(payload));
    ModFollower_OnPlayerStep(direction, newKeys, heldKeys);
}

void ModApi_OnBattleStart(u32 battleTypeFlags)
{
    ModFollower_OnBattleStart();
    ModEvent_Emit(MOD_EVENT_BATTLE_STARTED, &battleTypeFlags, sizeof(battleTypeFlags));
}

void ModApi_OnBattleEnd(u32 battleOutcome)
{
    ModEvent_Emit(MOD_EVENT_BATTLE_ENDED, &battleOutcome, sizeof(battleOutcome));
    ModFollower_OnBattleEnd();
}
