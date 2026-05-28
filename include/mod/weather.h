#ifndef GUARD_MOD_WEATHER_H
#define GUARD_MOD_WEATHER_H

#include "mod/base.h"
#include "constants/weather.h"

enum ModWeatherLayer
{
    MOD_WEATHER_LAYER_NONE      = 0,
    MOD_WEATHER_LAYER_RAIN      = 1 << 0,
    MOD_WEATHER_LAYER_SNOW      = 1 << 1,
    MOD_WEATHER_LAYER_FOG       = 1 << 2,
    MOD_WEATHER_LAYER_SAND      = 1 << 3,
    MOD_WEATHER_LAYER_ASH       = 1 << 4,
    MOD_WEATHER_LAYER_SUN       = 1 << 5,
    MOD_WEATHER_LAYER_DARKNESS  = 1 << 6,
    MOD_WEATHER_LAYER_THUNDER   = 1 << 7,
};

enum ModWeatherSource
{
    MOD_WEATHER_SOURCE_DEFAULT,
    MOD_WEATHER_SOURCE_MAP,
    MOD_WEATHER_SOURCE_COORD,
    MOD_WEATHER_SOURCE_TIME,
    MOD_WEATHER_SOURCE_FLAG,
    MOD_WEATHER_SOURCE_EVENT,
    MOD_WEATHER_SOURCE_SCRIPT,
    MOD_WEATHER_SOURCE_SERVER,
};

struct ModWeatherDisplay
{
    u8 vanilla_weather;
    u16 layers;
    u8 source;
    u8 priority;
    u16 battle_weather_mask;
};

typedef bool8 (*ModWeatherProviderFunc)(struct ModWeatherDisplay *display);

struct ModWeatherProvider
{
    const char *weather_provider_id;
    s16 priority;
    ModWeatherProviderFunc resolve;
};

void ModWeather_Init(void);
void ModWeather_RunFrame(void);
void ModWeather_OnMapLoad(void);
void ModWeather_GetDisplayedWeather(struct ModWeatherDisplay *display);
u16 ModWeather_GetBattleWeatherMask(void);
void ModWeather_SetOverride(u8 source, u8 vanilla_weather, u16 layers, u16 duration_frames);
void ModWeather_ClearOverride(u8 source);

#endif // GUARD_MOD_WEATHER_H
