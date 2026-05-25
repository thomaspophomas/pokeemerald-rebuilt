#include "global.h"
#include "mod/badge.h"
#include "mod/core.h"
#include "mod/engine.h"
#include "mod/event.h"
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
    ModEvent_Emit(MOD_EVENT_GAME_INIT, NULL, 0);
}

void ModApi_RunFrame(void)
{
    ModEvent_BeginFrame();
    ModTime_RunFrame();
    ModWeather_RunFrame();
}

void ModApi_OnMapLoad(void)
{
    ModRuntimeProfile_OnMapLoad();
    ModWeather_OnMapLoad();
    ModEvent_Emit(MOD_EVENT_MAP_LOAD, NULL, 0);
}

void ModApi_OnPlayerStep(u8 direction, u16 new_keys, u16 held_keys)
{
    struct ModEventPlayerStep payload;

    payload.direction = direction;
    payload.new_keys = new_keys;
    payload.held_keys = held_keys;
    ModEvent_Emit(MOD_EVENT_PLAYER_STEP, &payload, sizeof(payload));
}

void ModApi_OnBattleStart(u32 battle_type_flags)
{
    ModEvent_Emit(MOD_EVENT_BATTLE_STARTED, &battle_type_flags, sizeof(battle_type_flags));
}

void ModApi_OnBattleEnd(u32 battle_outcome)
{
    ModEvent_Emit(MOD_EVENT_BATTLE_ENDED, &battle_outcome, sizeof(battle_outcome));
}
