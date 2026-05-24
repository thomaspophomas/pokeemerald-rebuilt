#ifndef GUARD_MOD_WEATHER_H
#define GUARD_MOD_WEATHER_H

#include "global.h"

enum ModWeatherLayer
{
    MOD_WEATHER_LAYER_NONE      = 0,
    MOD_WEATHER_LAYER_RAIN      = 1 << 0,
    MOD_WEATHER_LAYER_SNOW      = 1 << 1,
    MOD_WEATHER_LAYER_HAIL      = MOD_WEATHER_LAYER_SNOW,
    MOD_WEATHER_LAYER_FOG       = 1 << 2,
    MOD_WEATHER_LAYER_SAND      = 1 << 3,
    MOD_WEATHER_LAYER_ASH       = 1 << 4,
    MOD_WEATHER_LAYER_SUN       = 1 << 5,
    MOD_WEATHER_LAYER_DARKNESS  = 1 << 6,
    MOD_WEATHER_LAYER_THUNDER   = 1 << 7,
    MOD_WEATHER_LAYER_WIND      = 1 << 8,
    MOD_WEATHER_LAYER_EXTREME_SILENCE = 1 << 9,
};

#define MOD_WEATHER_LAYER_PRIMARY_MASK (MOD_WEATHER_LAYER_RAIN | MOD_WEATHER_LAYER_SNOW | MOD_WEATHER_LAYER_SAND | MOD_WEATHER_LAYER_SUN)
#define MOD_WEATHER_LAYER_OVERLAY_MASK (MOD_WEATHER_LAYER_FOG | MOD_WEATHER_LAYER_ASH | MOD_WEATHER_LAYER_DARKNESS | MOD_WEATHER_LAYER_THUNDER | MOD_WEATHER_LAYER_WIND | MOD_WEATHER_LAYER_EXTREME_SILENCE)
#define MOD_WEATHER_LAYER_ANY_MASK     (MOD_WEATHER_LAYER_PRIMARY_MASK | MOD_WEATHER_LAYER_OVERLAY_MASK)

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
    u8 vanillaWeather;
    u16 layers;
    u8 source;
    u8 priority;
    u16 battleWeatherMask;
};

typedef bool8 (*ModWeatherProviderFunc)(struct ModWeatherDisplay *display);

struct ModWeatherProvider
{
    const char *id;
    s16 priority;
    ModWeatherProviderFunc resolve;
};

void ModWeather_Init(void);
void ModWeather_RunFrame(void);
void ModWeather_OnMapLoad(void);
void ModWeather_GetDisplayedWeather(struct ModWeatherDisplay *display);
u16 ModWeather_GetBattleWeatherMask(void);
u16 ModWeather_GetDefaultLayerMask(u8 weather);
u8 ModWeather_GetVanillaWeatherForLayers(u16 layers, u8 fallbackWeather);
bool8 ModWeather_LayersAreCompatible(u16 existingLayers, u16 candidateLayers);
u16 ModWeather_NormalizeLayers(u16 layers, u16 preferredLayers);
u16 ModWeather_CombineLayers(u16 baseLayers, u16 addedLayers);
u16 ModWeather_LayersToBattleWeatherMask(u16 layers);
void ModWeather_NormalizeDisplay(struct ModWeatherDisplay *display);
void ModWeather_SetOverride(u8 source, u8 vanillaWeather, u16 layers, u16 durationFrames);
void ModWeather_ClearOverride(u8 source);

#endif // GUARD_MOD_WEATHER_H
