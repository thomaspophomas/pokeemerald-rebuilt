#include "mod_sdk.h"

s8 MinimalWeather_OnGameInit(const struct ModEvent *event)
{
    (void)event;
    return MOD_EVENT_RESULT_CONTINUE;
}

bool8 MinimalWeather_ResolveWeather(struct ModWeatherDisplay *display)
{
    display->vanilla_weather = WEATHER_RAIN;
    display->layers = MOD_WEATHER_LAYER_RAIN;
    display->source = MOD_WEATHER_SOURCE_FLAG;
    display->priority = 10;
    display->battle_weather_mask = 0;
    return TRUE;
}
