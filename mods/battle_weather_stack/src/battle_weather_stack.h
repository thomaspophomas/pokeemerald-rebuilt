#ifndef GUARD_BATTLE_WEATHER_STACK_H
#define GUARD_BATTLE_WEATHER_STACK_H

#include "mod_sdk.h"

#ifndef MOD_WEATHER_LAYER_WIND
#define MOD_WEATHER_LAYER_WIND (1 << 8)
#endif

#ifndef MOD_WEATHER_LAYER_EXTREME_SILENCE
#define MOD_WEATHER_LAYER_EXTREME_SILENCE (1 << 9)
#endif

#define BWS_LAYER_NONE      MOD_WEATHER_LAYER_NONE
#define BWS_LAYER_RAIN      MOD_WEATHER_LAYER_RAIN
#define BWS_LAYER_SNOW      MOD_WEATHER_LAYER_SNOW
#define BWS_LAYER_SAND      MOD_WEATHER_LAYER_SAND
#define BWS_LAYER_HEAT      MOD_WEATHER_LAYER_SUN
#define BWS_LAYER_DARKNESS  MOD_WEATHER_LAYER_DARKNESS
#define BWS_LAYER_WIND      MOD_WEATHER_LAYER_WIND
#define BWS_LAYER_SILENCE   MOD_WEATHER_LAYER_EXTREME_SILENCE
#define BWS_LAYER_LIGHT     (1 << 10)
#define BWS_LAYER_ASH_RAIN  (1 << 11)
#define BWS_LAYER_POLLEN    (1 << 12)

#define BWS_LAYER_MASK      (BWS_LAYER_RAIN     \
                           | BWS_LAYER_SNOW     \
                           | BWS_LAYER_SAND     \
                           | BWS_LAYER_HEAT     \
                           | BWS_LAYER_DARKNESS \
                           | BWS_LAYER_WIND     \
                           | BWS_LAYER_SILENCE  \
                           | BWS_LAYER_LIGHT    \
                           | BWS_LAYER_ASH_RAIN \
                           | BWS_LAYER_POLLEN)

#define BWS_MAX_LAYERS 3

#define BWS_RESIDUAL_NONE         0
#define BWS_RESIDUAL_SAND_CHIP    (1 << 0)
#define BWS_RESIDUAL_POISON       (1 << 1)
#define BWS_RESIDUAL_TOXIC_POISON (1 << 2)
#define BWS_RESIDUAL_LEECH        (1 << 3)
#define BWS_RESIDUAL_HEAL_1_16    (1 << 4)

#define BWS_OVERWORLD_VISUAL_AUTO 0xFF

u8 BattleWeatherStack_CountLayers(u16 layers);
u16 BattleWeatherStack_NormalizeLayers(u16 layers, u16 preferredLayers);
u16 BattleWeatherStack_CombineLayers(u16 baseLayers, u16 addedLayers);
bool8 BattleWeatherStack_LayersAreCompatible(u16 existingLayers, u16 candidateLayers);

const char *BattleWeatherStack_GetNameKey(u16 layers);
const char *BattleWeatherStack_GetMessageKey(u16 layers);
const u8 *BattleWeatherStack_GetBattleMessage(u16 layers);
u32 BattleWeatherStack_BattleWeather(const struct EngineRuleset *ruleset, u16 weatherLayers);

u16 BattleWeatherStack_GetMovePowerPercent(u16 layers, u8 moveType);
u16 BattleWeatherStack_GetStatPercent(u16 layers, u8 statId, u8 type1, u8 type2);
u16 BattleWeatherStack_GetDamageTakenPercent(u16 layers, u8 type1, u8 type2);
u16 BattleWeatherStack_GetResidualFlags(u16 layers, u8 type1, u8 type2);
u8 BattleWeatherStack_GetWeatherBallType(u16 layers);

u16 BattleWeatherStack_GetEffectiveBattleLayers(u16 layers, bool8 weatherEffectsSuppressed);
u16 BattleWeatherStack_GetMovePowerPercentEx(u16 layers, u8 moveType, bool8 weatherEffectsSuppressed);
u16 BattleWeatherStack_GetStatPercentEx(u16 layers, u8 statId, u8 type1, u8 type2, bool8 weatherEffectsSuppressed);
u16 BattleWeatherStack_GetDamageTakenPercentEx(u16 layers, u8 type1, u8 type2, bool8 weatherEffectsSuppressed);
u16 BattleWeatherStack_GetResidualFlagsEx(u16 layers, u8 type1, u8 type2, bool8 weatherEffectsSuppressed);
u8 BattleWeatherStack_GetWeatherBallTypeEx(u16 layers, bool8 weatherEffectsSuppressed);

u16 BattleWeatherStack_GetOverworldLayers(void);
u8 BattleWeatherStack_GetOverworldVanillaWeather(void);
bool8 BattleWeatherStack_GetOverworldTint(u16 *r, u16 *g, u16 *b);
bool8 BattleWeatherStack_ResolveOverworldWeather(struct ModWeatherDisplay *display);

u8 BattleWeatherStack_GetOverworldDebugSlotCount(void);
u16 BattleWeatherStack_GetOverworldDebugMapSection(void);
u32 BattleWeatherStack_GetOverworldDebugLastRollMinute(void);
u32 BattleWeatherStack_GetOverworldDebugBlockUntilMinute(void);
u16 BattleWeatherStack_GetOverworldDebugSlotLayer(u8 slotId);
u32 BattleWeatherStack_GetOverworldDebugSlotEndMinute(u8 slotId);
u8 BattleWeatherStack_GetOverworldDebugSlotVanillaWeather(u8 slotId);
void BattleWeatherStack_DebugClearOverworldWeather(void);
bool8 BattleWeatherStack_DebugForceOverworldSlot(u8 slotId, u16 layer, u8 vanillaWeather, u8 durationMinutes, u8 blockNewWeatherMinutes);
bool8 BattleWeatherStack_DebugForceOverworldWeather(u16 layer, u8 vanillaWeather, u8 durationMinutes, u8 blockNewWeatherMinutes);
bool8 BattleWeatherStack_DebugForceOverworldLayers(u16 layers, u8 durationMinutes, u8 blockNewWeatherMinutes);

#endif // GUARD_BATTLE_WEATHER_STACK_H
