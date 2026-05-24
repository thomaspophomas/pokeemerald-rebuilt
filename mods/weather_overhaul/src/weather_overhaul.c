#include "global.h"
#include "mod/weather.h"

bool8 WeatherOverhaul_ResolveWeather(struct ModWeatherDisplay *display)
{
    if (display == NULL)
        return FALSE;

    ModWeather_NormalizeDisplay(display);
    return TRUE;
}
