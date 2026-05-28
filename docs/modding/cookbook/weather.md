# Weather Mod

`mods/my_weather/weather/providers.json`:

```json
{
  "providers": [
    {
      "id": "route_rain",
      "handler": "MyWeather_Resolve",
      "priority": 10,
      "mode": "modify"
    }
  ]
}
```

`mods/my_weather/src/my_weather.c`:

```c
#include "mod_sdk.h"

bool8 MyWeather_Resolve(struct ModWeatherDisplay *display)
{
    display->vanilla_weather = WEATHER_RAIN;
    display->layers = MOD_WEATHER_LAYER_RAIN;
    display->source = MOD_WEATHER_SOURCE_FLAG;
    display->priority = 10;
    display->battle_weather_mask = 0;
    return TRUE;
}
```
