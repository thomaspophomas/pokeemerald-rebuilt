#include "global.h"
#include "constants/battle.h"
#include "constants/weather.h"
#include "field_weather.h"
#include "generated/mod_registry.h"
#include "global.fieldmap.h"
#include "mod/engine.h"
#include "mod/event.h"
#include "mod/runtime_profile.h"
#include "mod/weather.h"

static EWRAM_DATA bool8 sOverrideActive = FALSE;
static EWRAM_DATA struct ModWeatherDisplay sOverride = {0};
static EWRAM_DATA u16 sOverrideTimer = 0;
static EWRAM_DATA struct ModWeatherDisplay sLastDisplay = {0};

static u16 PickPrimaryLayer(u16 layers, u16 preferredLayers)
{
    u16 primaryLayers = layers & MOD_WEATHER_LAYER_PRIMARY_MASK;
    u16 preferredPrimaryLayers = preferredLayers & primaryLayers & MOD_WEATHER_LAYER_PRIMARY_MASK;

    if (preferredPrimaryLayers != 0)
        primaryLayers = preferredPrimaryLayers;

    if (primaryLayers & MOD_WEATHER_LAYER_RAIN)
        return MOD_WEATHER_LAYER_RAIN;
    if (primaryLayers & MOD_WEATHER_LAYER_SAND)
        return MOD_WEATHER_LAYER_SAND;
    if (primaryLayers & MOD_WEATHER_LAYER_SNOW)
        return MOD_WEATHER_LAYER_SNOW;
    if (primaryLayers & MOD_WEATHER_LAYER_SUN)
        return MOD_WEATHER_LAYER_SUN;

    return MOD_WEATHER_LAYER_NONE;
}

u16 ModWeather_GetDefaultLayerMask(u8 weather)
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
    case WEATHER_DARKNESS:
        return MOD_WEATHER_LAYER_DARKNESS;
    case WEATHER_WIND:
        return MOD_WEATHER_LAYER_WIND;
    case WEATHER_EXTREME_SILENCE:
        return MOD_WEATHER_LAYER_EXTREME_SILENCE;
    default:
        return MOD_WEATHER_LAYER_NONE;
    }
}

u8 ModWeather_GetVanillaWeatherForLayers(u16 layers, u8 fallbackWeather)
{
    layers = ModWeather_NormalizeLayers(layers, 0);

    if (layers & MOD_WEATHER_LAYER_RAIN)
    {
        if (layers & MOD_WEATHER_LAYER_THUNDER)
            return WEATHER_RAIN_THUNDERSTORM;
        return WEATHER_RAIN;
    }
    if (layers & MOD_WEATHER_LAYER_SAND)
        return WEATHER_SANDSTORM;
    if (layers & MOD_WEATHER_LAYER_SNOW)
        return WEATHER_SNOW;
    if (layers & MOD_WEATHER_LAYER_SUN)
        return WEATHER_SUNNY;
    if (layers & MOD_WEATHER_LAYER_DARKNESS)
        return WEATHER_DARKNESS;
    if (layers & MOD_WEATHER_LAYER_WIND)
        return WEATHER_WIND;
    if (layers & MOD_WEATHER_LAYER_EXTREME_SILENCE)
        return WEATHER_EXTREME_SILENCE;

    return fallbackWeather;
}

bool8 ModWeather_LayersAreCompatible(u16 existingLayers, u16 candidateLayers)
{
    u16 existingPrimary = existingLayers & MOD_WEATHER_LAYER_PRIMARY_MASK;
    u16 candidatePrimary = candidateLayers & MOD_WEATHER_LAYER_PRIMARY_MASK;

    if (existingPrimary != 0 && candidatePrimary != 0 && existingPrimary != candidatePrimary)
        return FALSE;
    if ((existingLayers & MOD_WEATHER_LAYER_EXTREME_SILENCE) && (candidateLayers & (MOD_WEATHER_LAYER_WIND | MOD_WEATHER_LAYER_THUNDER)))
        return FALSE;
    if ((candidateLayers & MOD_WEATHER_LAYER_EXTREME_SILENCE) && (existingLayers & (MOD_WEATHER_LAYER_WIND | MOD_WEATHER_LAYER_THUNDER)))
        return FALSE;

    return TRUE;
}

u16 ModWeather_NormalizeLayers(u16 layers, u16 preferredLayers)
{
    u16 primaryLayer;

    layers &= MOD_WEATHER_LAYER_ANY_MASK;
    primaryLayer = PickPrimaryLayer(layers, preferredLayers);
    layers &= ~MOD_WEATHER_LAYER_PRIMARY_MASK;
    layers |= primaryLayer;

    if (layers & MOD_WEATHER_LAYER_EXTREME_SILENCE)
        layers &= ~(MOD_WEATHER_LAYER_WIND | MOD_WEATHER_LAYER_THUNDER);

    return layers;
}

u16 ModWeather_CombineLayers(u16 baseLayers, u16 addedLayers)
{
    return ModWeather_NormalizeLayers(baseLayers | addedLayers, addedLayers);
}

u16 ModWeather_LayersToBattleWeatherMask(u16 layers)
{
    layers = ModWeather_NormalizeLayers(layers, 0);

    if (layers & MOD_WEATHER_LAYER_RAIN)
        return B_WEATHER_RAIN_PERMANENT;
    if (layers & MOD_WEATHER_LAYER_SAND)
        return B_WEATHER_SANDSTORM_PERMANENT;
    if (layers & MOD_WEATHER_LAYER_SUN)
        return B_WEATHER_SUN_PERMANENT;
    if (layers & MOD_WEATHER_LAYER_SNOW)
        return B_WEATHER_HAIL_TEMPORARY;

    return 0;
}

void ModWeather_NormalizeDisplay(struct ModWeatherDisplay *display)
{
    u16 fallbackLayers;

    if (display == NULL)
        return;

    fallbackLayers = ModWeather_GetDefaultLayerMask(display->vanillaWeather);
    display->layers = ModWeather_NormalizeLayers(display->layers, fallbackLayers);
    if (display->layers == MOD_WEATHER_LAYER_NONE)
        display->layers = fallbackLayers;
    display->vanillaWeather = ModWeather_GetVanillaWeatherForLayers(display->layers, display->vanillaWeather);
}

static void InitDisplayFromMap(struct ModWeatherDisplay *display)
{
    memset(display, 0, sizeof(*display));
    display->vanillaWeather = gMapHeader.weather;
    if (display->vanillaWeather == WEATHER_NONE)
        display->vanillaWeather = GetCurrentWeather();
    display->layers = ModWeather_GetDefaultLayerMask(display->vanillaWeather);
    display->source = MOD_WEATHER_SOURCE_MAP;
    ModWeather_NormalizeDisplay(display);
}

static void EmitWeatherChanged(const struct ModWeatherDisplay *oldDisplay, const struct ModWeatherDisplay *newDisplay)
{
    struct ModEventWeatherChanged payload;

    payload.oldWeather = oldDisplay->vanillaWeather;
    payload.newWeather = newDisplay->vanillaWeather;
    payload.oldLayers = oldDisplay->layers;
    payload.newLayers = newDisplay->layers;
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
    if (display.vanillaWeather != sLastDisplay.vanillaWeather || display.layers != sLastDisplay.layers)
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
    u16 i;
    struct ModWeatherDisplay runtimeWeather;

    InitDisplayFromMap(display);

    for (i = 0; i < gModWeatherProviderCount; i++)
    {
        if (gModWeatherProviders[i].resolve != NULL)
            gModWeatherProviders[i].resolve(display);
    }

    if (sOverrideActive && sOverride.priority >= display->priority)
        *display = sOverride;
    if (ModRuntimeProfile_GetWeather(&runtimeWeather) && runtimeWeather.priority >= display->priority)
        *display = runtimeWeather;

    ModWeather_NormalizeDisplay(display);
}

u16 ModWeather_GetBattleWeatherMask(void)
{
    struct ModWeatherDisplay display;
    const struct EngineRuleset *ruleset;
    u32 battleWeather;

    ModWeather_GetDisplayedWeather(&display);
    if (display.battleWeatherMask != 0)
        return display.battleWeatherMask;

    ruleset = EngineApi_GetActiveRuleset();
    if (ruleset != NULL && ruleset->getBattleWeatherMask != NULL)
    {
        battleWeather = ruleset->getBattleWeatherMask(ruleset, display.layers);
        if (battleWeather != 0)
            return (u16)battleWeather;
    }

    return ModWeather_LayersToBattleWeatherMask(display.layers);
}

void ModWeather_SetOverride(u8 source, u8 vanillaWeather, u16 layers, u16 durationFrames)
{
    sOverrideActive = TRUE;
    sOverride.vanillaWeather = vanillaWeather;
    sOverride.layers = ModWeather_NormalizeLayers(layers, ModWeather_GetDefaultLayerMask(vanillaWeather));
    sOverride.source = source;
    sOverride.priority = 0xFF;
    sOverride.battleWeatherMask = 0;
    ModWeather_NormalizeDisplay(&sOverride);
    sOverrideTimer = durationFrames;
}

void ModWeather_ClearOverride(u8 source)
{
    if (sOverrideActive && sOverride.source == source)
    {
        sOverrideActive = FALSE;
        sOverrideTimer = 0;
    }
}
