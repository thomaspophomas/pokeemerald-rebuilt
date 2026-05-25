#include "global.h"
#include "constants/weather.h"
#include "field_weather.h"
#include "generated/mod_registry.h"
#include "global.fieldmap.h"
#include "mod/event.h"
#include "mod/runtime_profile.h"
#include "mod/weather.h"

static EWRAM_DATA bool8 sOverrideActive = FALSE;
static EWRAM_DATA struct ModWeatherDisplay sOverride = {0};
static EWRAM_DATA u16 sOverrideTimer = 0;
static EWRAM_DATA struct ModWeatherDisplay sLastDisplay = {0};

static u16 GetDefaultLayerMask(u8 weather)
{
    switch (weather)
    {
    case WEATHER_RAIN:
    case WEATHER_DOWNPOUR:
        return MOD_WEATHER_LAYER_RAIN;
    case WEATHER_RAIN_THUNDERSTORM:
        return MOD_WEATHER_LAYER_RAIN | MOD_WEATHER_LAYER_THUNDER;
    case WEATHER_SNOW:
        return MOD_WEATHER_LAYER_SNOW;
    case WEATHER_FOG_HORIZONTAL:
    case WEATHER_FOG_DIAGONAL:
        return MOD_WEATHER_LAYER_FOG;
    case WEATHER_SANDSTORM:
        return MOD_WEATHER_LAYER_SAND;
    case WEATHER_VOLCANIC_ASH:
        return MOD_WEATHER_LAYER_ASH;
    case WEATHER_SUNNY:
    case WEATHER_DROUGHT:
    case WEATHER_SUNNY_CLOUDS:
        return MOD_WEATHER_LAYER_SUN;
    case WEATHER_SHADE:
        return MOD_WEATHER_LAYER_DARKNESS;
    default:
        return MOD_WEATHER_LAYER_NONE;
    }
}

static void InitDisplayFromMap(struct ModWeatherDisplay *display)
{
    memset(display, 0, sizeof(*display));
    display->vanilla_weather = gMapHeader.weather;
    if (display->vanilla_weather == WEATHER_NONE)
        display->vanilla_weather = GetCurrentWeather();
    display->layers = GetDefaultLayerMask(display->vanilla_weather);
    display->source = MOD_WEATHER_SOURCE_MAP;
}

static void EmitWeatherChanged(const struct ModWeatherDisplay *oldDisplay, const struct ModWeatherDisplay *newDisplay)
{
    struct ModEventWeatherChanged payload;

    payload.old_weather = oldDisplay->vanilla_weather;
    payload.new_weather = newDisplay->vanilla_weather;
    payload.old_layers = oldDisplay->layers;
    payload.new_layers = newDisplay->layers;
    ModEvent_Emit(MOD_EVENT_WEATHER_CHANGED, &payload, sizeof(payload));
}

void ModWeather_Init(void)
{
    sOverrideActive = FALSE;
    sOverrideTimer = 0;
    InitDisplayFromMap(&sLastDisplay);
}

void ModWeather_RunFrame(void)
{
    struct ModWeatherDisplay display;

    if (sOverrideActive && sOverrideTimer != 0)
    {
        sOverrideTimer--;
        if (sOverrideTimer == 0)
            sOverrideActive = FALSE;
    }

    ModWeather_GetDisplayedWeather(&display);
    if (display.vanilla_weather != sLastDisplay.vanilla_weather || display.layers != sLastDisplay.layers)
        EmitWeatherChanged(&sLastDisplay, &display);

    sLastDisplay = display;
}

void ModWeather_OnMapLoad(void)
{
    struct ModWeatherDisplay display;

    ModWeather_GetDisplayedWeather(&display);
    sLastDisplay = display;
}

void ModWeather_GetDisplayedWeather(struct ModWeatherDisplay *display)
{
    u16 weather_provider_index;
    struct ModWeatherDisplay runtimeWeather;

    InitDisplayFromMap(display);

    for (weather_provider_index = 0; weather_provider_index < gModWeatherProviderCount; weather_provider_index++)
    {
        if (gModWeatherProviders[weather_provider_index].resolve != NULL)
            gModWeatherProviders[weather_provider_index].resolve(display);
    }

    if (sOverrideActive && sOverride.priority >= display->priority)
        *display = sOverride;
    if (ModRuntimeProfile_GetWeather(&runtimeWeather) && runtimeWeather.priority >= display->priority)
        *display = runtimeWeather;
}

u16 ModWeather_GetBattleWeatherMask(void)
{
    struct ModWeatherDisplay display;

    ModWeather_GetDisplayedWeather(&display);
    return display.battle_weather_mask;
}

void ModWeather_SetOverride(u8 source, u8 vanilla_weather, u16 layers, u16 duration_frames)
{
    sOverrideActive = TRUE;
    sOverride.vanilla_weather = vanilla_weather;
    sOverride.layers = layers;
    sOverride.source = source;
    sOverride.priority = 0xFF;
    sOverride.battle_weather_mask = 0;
    sOverrideTimer = duration_frames;
}

void ModWeather_ClearOverride(u8 source)
{
    if (sOverrideActive && sOverride.source == source)
    {
        sOverrideActive = FALSE;
        sOverrideTimer = 0;
    }
}
