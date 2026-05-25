#include "global.h"
#include "battle.h"
#include "battle_session.h"
#include "field_weather.h"
#include "mod/weather.h"
#include "constants/battle.h"
#include "constants/weather.h"

static u16 GetVanillaFieldBattleWeather(u8 weather)
{
    switch (weather)
    {
    case WEATHER_RAIN:
    case WEATHER_RAIN_THUNDERSTORM:
    case WEATHER_DOWNPOUR:
        return B_WEATHER_RAIN_PERMANENT;
    case WEATHER_SANDSTORM:
    case WEATHER_VOLCANIC_ASH:
        return B_WEATHER_SANDSTORM_PERMANENT;
    case WEATHER_SUNNY:
    case WEATHER_DROUGHT:
        return B_WEATHER_SUN_PERMANENT;
    case WEATHER_SNOW:
        return B_WEATHER_HAIL_TEMPORARY;
    default:
        return 0;
    }
}

bool8 BattleSession_IsSecretBaseBattle(void)
{
    return (gBattleTypeFlags & BATTLE_TYPE_SECRET_BASE) != 0;
}

u16 BattleSession_GetCurrentFieldBattleWeather(void)
{
    struct ModWeatherDisplay display;
    u16 battleWeather = ModWeather_GetBattleWeatherMask();

    if (battleWeather != 0)
        return battleWeather;

    ModWeather_GetDisplayedWeather(&display);
    battleWeather = GetVanillaFieldBattleWeather(display.vanilla_weather);
    if (battleWeather != 0)
        return battleWeather;

    return GetVanillaFieldBattleWeather(GetCurrentWeather());
}

void BattleSession_SetWeather(u16 battleWeather)
{
    gBattleWeather = battleWeather;
}
