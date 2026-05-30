#include "mod_sdk.h"
#include "constants/battle.h"
#include "constants/pokemon.h"
#include "constants/region_map_sections.h"
#include "constants/weather.h"
#include "field_weather.h"
#include "global.fieldmap.h"
#include "main.h"
#include "overworld.h"
#include "random.h"
#include "task.h"
#include "battle_weather_stack.h"

#define BATTLE_WEATHER_STACK_MOD_ID "battle_weather_stack"
#define BATTLE_WEATHER_STACK_RULESET_ID "battle_weather_stack:stacked_weather"
#define BATTLE_WEATHER_STACK_STATE_VERSION 1
#define BATTLE_WEATHER_STACK_STATE_MAGIC 0x42575331

#define BWS_OVERWORLD_TASK_PRIORITY 0xFE
#define BWS_OVERWORLD_NO_WEATHER BWS_OVERWORLD_VISUAL_AUTO
#define BWS_OVERWORLD_RANDOM_SLOT_COUNT 2
#define BWS_OVERWORLD_SLOT_LIGHTING 2
#define BWS_MAPSEC_ANY 0xFFFF
#define BWS_MINUTES_PER_DAY (24 * 60)

struct BattleWeatherStackCombo
{
    u16 layers;
    const char *nameKey;
    const char *messageKey;
};

struct BattleWeatherStackMessageText
{
    const char *messageKey;
    const u8 *text;
};

struct BattleWeatherStackLayerInfo
{
    u16 layer;
    u32 battleWeather;
    u8 weatherBallType;
    u8 defaultOverworldWeather;
    bool8 hasTint;
    u16 tintR;
    u16 tintG;
    u16 tintB;
    const char *spriteKey;
};

struct BattleWeatherStackLayerPair
{
    u16 firstLayer;
    u16 secondLayer;
};

struct BattleWeatherStackVisualCombo
{
    u16 layers;
    u8 vanillaWeather;
};

struct BattleWeatherStackMovePowerEffect
{
    u16 layer;
    u8 moveType;
    u16 percent;
};

struct BattleWeatherStackStatEffect
{
    u16 layer;
    u32 pokemonTypeMask;
    u32 statMask;
    u16 percent;
};

struct BattleWeatherStackDamageTakenEffect
{
    u16 layer;
    u32 pokemonTypeMask;
    u16 percent;
};

struct BattleWeatherStackResidualEffect
{
    u16 layer;
    u32 includeTypeMask;
    u32 excludeTypeMask;
    u16 flags;
};

struct BattleWeatherStackOverworldRule
{
    u16 mapSection;
    u16 layer;
    u16 chancePerMinute;
    u8 minDurationMinutes;
    u8 maxDurationMinutes;
    u8 blockNewWeatherMinutes;
    u8 vanillaWeather;
};

struct BattleWeatherStackLightingRule
{
    u16 mapSection;
    u16 lightStartMinute;
    u16 lightEndMinute;
    u16 darkStartMinute;
    u16 darkEndMinute;
};

struct BattleWeatherStackOverworldSlot
{
    u16 layer;
    u32 endMinute;
    u8 vanillaWeather;
};

struct BattleWeatherStackPersistedSlot
{
    u16 layer;
    u16 vanillaWeather;
    u32 endMinute;
};

struct BattleWeatherStackStateBlock
{
    u32 magic;
    u16 mapSection;
    u16 currentLayers;
    u32 lastRollMinute;
    u32 blockNewWeatherUntilMinute;
    struct BattleWeatherStackPersistedSlot slots[BWS_OVERWORLD_RANDOM_SLOT_COUNT];
};

STATIC_ASSERT(sizeof(struct BattleWeatherStackStateBlock) <= 40, BattleWeatherStackStateBlockFitsManifest);

#include "../weather/generated_config.inc"

static EWRAM_DATA struct BattleWeatherStackOverworldSlot sOverworldSlots[BWS_OVERWORLD_RANDOM_SLOT_COUNT] = {0};
static EWRAM_DATA u16 sOverworldMapSection = BWS_MAPSEC_ANY;
static EWRAM_DATA u32 sLastRollMinute = 0;
static EWRAM_DATA u32 sBlockNewWeatherUntilMinute = 0;
static EWRAM_DATA u16 sCurrentOverworldLayers = BWS_LAYER_NONE;
static EWRAM_DATA u8 sCurrentOverworldVanillaWeather = WEATHER_NONE;
static EWRAM_DATA u8 sAppliedFieldWeather = BWS_OVERWORLD_NO_WEATHER;

u16 BattleWeatherStack_GetOverworldLayers(void);

static struct BattleWeatherStackStateBlock *GetStateBlock(void)
{
    return ModState_GetBlock(
        BATTLE_WEATHER_STACK_MOD_ID,
        sizeof(struct BattleWeatherStackStateBlock),
        BATTLE_WEATHER_STACK_STATE_VERSION);
}

static void StoreOverworldStateBlock(void)
{
    struct BattleWeatherStackStateBlock *state = GetStateBlock();
    u8 i;

    if (state == NULL)
        return;

    state->magic = BATTLE_WEATHER_STACK_STATE_MAGIC;
    state->mapSection = sOverworldMapSection;
    state->currentLayers = sCurrentOverworldLayers;
    state->lastRollMinute = sLastRollMinute;
    state->blockNewWeatherUntilMinute = sBlockNewWeatherUntilMinute;

    for (i = 0; i < ARRAY_COUNT(sOverworldSlots); i++)
    {
        state->slots[i].layer = sOverworldSlots[i].layer;
        state->slots[i].vanillaWeather = sOverworldSlots[i].vanillaWeather;
        state->slots[i].endMinute = sOverworldSlots[i].endMinute;
    }
}

static bool8 LoadOverworldStateBlock(void)
{
    struct BattleWeatherStackStateBlock *state = GetStateBlock();
    u8 i;

    if (state == NULL || state->magic != BATTLE_WEATHER_STACK_STATE_MAGIC)
        return FALSE;

    sOverworldMapSection = state->mapSection;
    sLastRollMinute = state->lastRollMinute;
    sBlockNewWeatherUntilMinute = state->blockNewWeatherUntilMinute;
    sCurrentOverworldLayers = state->currentLayers & BWS_LAYER_MASK;
    sCurrentOverworldVanillaWeather = WEATHER_NONE;
    sAppliedFieldWeather = BWS_OVERWORLD_NO_WEATHER;

    for (i = 0; i < ARRAY_COUNT(sOverworldSlots); i++)
    {
        sOverworldSlots[i].layer = state->slots[i].layer & BWS_LAYER_MASK;
        sOverworldSlots[i].vanillaWeather = state->slots[i].vanillaWeather;
        sOverworldSlots[i].endMinute = state->slots[i].endMinute;
    }

    return TRUE;
}

static u32 TypeMask(u8 type)
{
    if (type == TYPE_NONE)
        return 0;
    return 1u << type;
}

static bool8 HasAnyTypeMask(u8 type1, u8 type2, u32 typeMask)
{
    return ((TypeMask(type1) | TypeMask(type2)) & typeMask) != 0;
}

static bool8 HasNoTypeMask(u8 type1, u8 type2, u32 typeMask)
{
    return !HasAnyTypeMask(type1, type2, typeMask);
}

static const struct BattleWeatherStackLayerInfo *FindLayerInfo(u16 layer)
{
    u16 i;

    for (i = 0; i < ARRAY_COUNT(sLayerInfo); i++)
    {
        if (sLayerInfo[i].layer == layer)
            return &sLayerInfo[i];
    }

    return NULL;
}

static u16 ApplyPercent(u16 percent, u16 multiplier)
{
    return (u16)(((u32)percent * multiplier + 50) / 100);
}

static u16 ResolveConflict(u16 layers, u16 preferredLayers, u16 firstLayer, u16 secondLayer)
{
    if ((layers & firstLayer) && (layers & secondLayer))
    {
        if ((preferredLayers & secondLayer) && !(preferredLayers & firstLayer))
            layers &= ~firstLayer;
        else
            layers &= ~secondLayer;
    }

    return layers;
}

u8 BattleWeatherStack_CountLayers(u16 layers)
{
    u8 count = 0;
    u8 i;

    layers &= BWS_LAYER_MASK;
    for (i = 0; i < ARRAY_COUNT(sLayerTrimOrder); i++)
    {
        if (layers & sLayerTrimOrder[i])
            count++;
    }

    return count;
}

u16 BattleWeatherStack_NormalizeLayers(u16 layers, u16 preferredLayers)
{
    u16 normalizedLayers = 0;
    u16 i;

    layers &= BWS_LAYER_MASK;
    preferredLayers &= BWS_LAYER_MASK;

    for (i = 0; i < ARRAY_COUNT(sConflicts); i++)
        layers = ResolveConflict(layers, preferredLayers, sConflicts[i].firstLayer, sConflicts[i].secondLayer);

    for (i = 0; i < ARRAY_COUNT(sLayerTrimOrder) && BattleWeatherStack_CountLayers(normalizedLayers) < BWS_MAX_LAYERS; i++)
    {
        if (preferredLayers & layers & sLayerTrimOrder[i])
            normalizedLayers |= sLayerTrimOrder[i];
    }

    for (i = 0; i < ARRAY_COUNT(sLayerTrimOrder) && BattleWeatherStack_CountLayers(normalizedLayers) < BWS_MAX_LAYERS; i++)
    {
        if (layers & sLayerTrimOrder[i])
            normalizedLayers |= sLayerTrimOrder[i];
    }

    return normalizedLayers;
}

u16 BattleWeatherStack_CombineLayers(u16 baseLayers, u16 addedLayers)
{
    return BattleWeatherStack_NormalizeLayers(baseLayers | addedLayers, addedLayers);
}

bool8 BattleWeatherStack_LayersAreCompatible(u16 existingLayers, u16 candidateLayers)
{
    u16 combined = existingLayers | candidateLayers;
    u16 i;

    for (i = 0; i < ARRAY_COUNT(sConflicts); i++)
    {
        if ((combined & sConflicts[i].firstLayer) && (combined & sConflicts[i].secondLayer))
            return FALSE;
    }

    return BattleWeatherStack_CountLayers(combined) <= BWS_MAX_LAYERS;
}

static const struct BattleWeatherStackCombo *FindTextEntry(
    const struct BattleWeatherStackCombo *entries,
    u16 entryCount,
    u16 layers)
{
    u16 i;

    for (i = 0; i < entryCount; i++)
    {
        if (entries[i].layers == layers)
            return &entries[i];
    }

    return NULL;
}

static const u8 *FindMessageText(const char *messageKey)
{
    u16 i;

    for (i = 0; i < ARRAY_COUNT(sMessageText); i++)
    {
        if (strcmp(sMessageText[i].messageKey, messageKey) == 0)
            return sMessageText[i].text;
    }

    return sBwsMessage_composite;
}

const char *BattleWeatherStack_GetNameKey(u16 layers)
{
    const struct BattleWeatherStackCombo *entry;

    layers = BattleWeatherStack_NormalizeLayers(layers, layers);
    if (layers == BWS_LAYER_NONE)
        return "battle_weather_stack:name.clear";

    entry = FindTextEntry(sBaseLayerText, ARRAY_COUNT(sBaseLayerText), layers);
    if (entry != NULL)
        return entry->nameKey;

    entry = FindTextEntry(sComboText, ARRAY_COUNT(sComboText), layers);
    if (entry != NULL)
        return entry->nameKey;

    return "battle_weather_stack:name.composite";
}

const char *BattleWeatherStack_GetMessageKey(u16 layers)
{
    const struct BattleWeatherStackCombo *entry;

    layers = BattleWeatherStack_NormalizeLayers(layers, layers);
    if (layers == BWS_LAYER_NONE)
        return "battle_weather_stack:message.clear";

    entry = FindTextEntry(sBaseLayerText, ARRAY_COUNT(sBaseLayerText), layers);
    if (entry != NULL)
        return entry->messageKey;

    entry = FindTextEntry(sComboText, ARRAY_COUNT(sComboText), layers);
    if (entry != NULL)
        return entry->messageKey;

    return "battle_weather_stack:message.composite";
}

const u8 *BattleWeatherStack_GetBattleMessage(u16 layers)
{
    return FindMessageText(BattleWeatherStack_GetMessageKey(layers));
}

u32 BattleWeatherStack_BattleWeather(const struct EngineRuleset *ruleset, u16 weatherLayers)
{
    u32 battleWeather = 0;
    u16 layers;
    u16 i;

    (void)ruleset;
    layers = BattleWeatherStack_NormalizeLayers(weatherLayers, weatherLayers);

    for (i = 0; i < ARRAY_COUNT(sLayerInfo); i++)
    {
        if (layers & sLayerInfo[i].layer)
            battleWeather |= sLayerInfo[i].battleWeather;
    }

    return battleWeather;
}

u16 BattleWeatherStack_GetMovePowerPercent(u16 layers, u8 moveType)
{
    u16 percent = 100;
    u16 i;

    layers = BattleWeatherStack_NormalizeLayers(layers, layers);

    for (i = 0; i < ARRAY_COUNT(sMovePowerEffects); i++)
    {
        if ((layers & sMovePowerEffects[i].layer) && moveType == sMovePowerEffects[i].moveType)
            percent = ApplyPercent(percent, sMovePowerEffects[i].percent);
    }

    return percent;
}

u16 BattleWeatherStack_GetStatPercent(u16 layers, u8 statId, u8 type1, u8 type2)
{
    u16 percent = 100;
    u16 i;

    layers = BattleWeatherStack_NormalizeLayers(layers, layers);

    for (i = 0; i < ARRAY_COUNT(sStatEffects); i++)
    {
        if ((layers & sStatEffects[i].layer)
         && (sStatEffects[i].statMask & (1u << statId))
         && (sStatEffects[i].pokemonTypeMask == 0 || HasAnyTypeMask(type1, type2, sStatEffects[i].pokemonTypeMask)))
            percent = ApplyPercent(percent, sStatEffects[i].percent);
    }

    return percent;
}

u16 BattleWeatherStack_GetDamageTakenPercent(u16 layers, u8 type1, u8 type2)
{
    u16 percent = 100;
    u16 i;

    layers = BattleWeatherStack_NormalizeLayers(layers, layers);

    for (i = 0; i < ARRAY_COUNT(sDamageTakenEffects); i++)
    {
        if ((layers & sDamageTakenEffects[i].layer)
         && (sDamageTakenEffects[i].pokemonTypeMask == 0 || HasAnyTypeMask(type1, type2, sDamageTakenEffects[i].pokemonTypeMask)))
            percent = ApplyPercent(percent, sDamageTakenEffects[i].percent);
    }

    return percent;
}

u16 BattleWeatherStack_GetResidualFlags(u16 layers, u8 type1, u8 type2)
{
    u16 flags = BWS_RESIDUAL_NONE;
    u16 i;

    layers = BattleWeatherStack_NormalizeLayers(layers, layers);

    for (i = 0; i < ARRAY_COUNT(sResidualEffects); i++)
    {
        if ((layers & sResidualEffects[i].layer)
         && (sResidualEffects[i].includeTypeMask == 0 || HasAnyTypeMask(type1, type2, sResidualEffects[i].includeTypeMask))
         && (sResidualEffects[i].excludeTypeMask == 0 || HasNoTypeMask(type1, type2, sResidualEffects[i].excludeTypeMask)))
            flags |= sResidualEffects[i].flags;
    }

    return flags;
}

u8 BattleWeatherStack_GetWeatherBallType(u16 layers)
{
    const struct BattleWeatherStackLayerInfo *layerInfo;
    u16 i;

    layers = BattleWeatherStack_NormalizeLayers(layers, layers);

    for (i = 0; i < ARRAY_COUNT(sWeatherBallOrder); i++)
    {
        if (!(layers & sWeatherBallOrder[i]))
            continue;

        layerInfo = FindLayerInfo(sWeatherBallOrder[i]);
        if (layerInfo != NULL && layerInfo->weatherBallType != TYPE_NORMAL)
            return layerInfo->weatherBallType;
    }

    return TYPE_NORMAL;
}

u16 BattleWeatherStack_GetEffectiveBattleLayers(u16 layers, bool8 weatherEffectsSuppressed)
{
    if (weatherEffectsSuppressed)
        return BWS_LAYER_NONE;
    return BattleWeatherStack_NormalizeLayers(layers, layers);
}

u16 BattleWeatherStack_GetMovePowerPercentEx(u16 layers, u8 moveType, bool8 weatherEffectsSuppressed)
{
    return BattleWeatherStack_GetMovePowerPercent(
        BattleWeatherStack_GetEffectiveBattleLayers(layers, weatherEffectsSuppressed),
        moveType);
}

u16 BattleWeatherStack_GetStatPercentEx(u16 layers, u8 statId, u8 type1, u8 type2, bool8 weatherEffectsSuppressed)
{
    return BattleWeatherStack_GetStatPercent(
        BattleWeatherStack_GetEffectiveBattleLayers(layers, weatherEffectsSuppressed),
        statId,
        type1,
        type2);
}

u16 BattleWeatherStack_GetDamageTakenPercentEx(u16 layers, u8 type1, u8 type2, bool8 weatherEffectsSuppressed)
{
    return BattleWeatherStack_GetDamageTakenPercent(
        BattleWeatherStack_GetEffectiveBattleLayers(layers, weatherEffectsSuppressed),
        type1,
        type2);
}

u16 BattleWeatherStack_GetResidualFlagsEx(u16 layers, u8 type1, u8 type2, bool8 weatherEffectsSuppressed)
{
    return BattleWeatherStack_GetResidualFlags(
        BattleWeatherStack_GetEffectiveBattleLayers(layers, weatherEffectsSuppressed),
        type1,
        type2);
}

u8 BattleWeatherStack_GetWeatherBallTypeEx(u16 layers, bool8 weatherEffectsSuppressed)
{
    return BattleWeatherStack_GetWeatherBallType(
        BattleWeatherStack_GetEffectiveBattleLayers(layers, weatherEffectsSuppressed));
}

static bool8 IsOverworldWeatherContext(void)
{
    return gMain.callback2 == CB2_Overworld || gMain.callback2 == CB2_OverworldBasic;
}

static bool8 IsOutdoorWeatherMap(void)
{
    return IsMapTypeOutdoors(gMapHeader.mapType);
}

static u32 GetEpochMinute(const struct ModTimeSnapshot *snapshot)
{
    return ((u32)snapshot->day_count * BWS_MINUTES_PER_DAY) + snapshot->minute_of_day;
}

static bool8 MinuteInWindow(u16 minuteOfDay, u16 startMinute, u16 endMinute)
{
    if (startMinute == endMinute)
        return FALSE;
    if (startMinute < endMinute)
        return minuteOfDay >= startMinute && minuteOfDay < endMinute;
    return minuteOfDay >= startMinute || minuteOfDay < endMinute;
}

static bool8 HasOverworldRulesForMapSection(u16 mapSection)
{
    u16 i;

    for (i = 0; i < ARRAY_COUNT(sOverworldRules); i++)
    {
        if (sOverworldRules[i].mapSection == mapSection)
            return TRUE;
    }

    return FALSE;
}

static void ClearOverworldSlots(void)
{
    u8 i;

    for (i = 0; i < ARRAY_COUNT(sOverworldSlots); i++)
    {
        sOverworldSlots[i].layer = BWS_LAYER_NONE;
        sOverworldSlots[i].endMinute = 0;
        sOverworldSlots[i].vanillaWeather = WEATHER_NONE;
    }
}

static void ResetOverworldState(u16 mapSection, u32 currentMinute)
{
    ClearOverworldSlots();

    sOverworldMapSection = mapSection;
    sLastRollMinute = currentMinute;
    sBlockNewWeatherUntilMinute = currentMinute;
    sCurrentOverworldLayers = BWS_LAYER_NONE;
    sCurrentOverworldVanillaWeather = WEATHER_NONE;
    sAppliedFieldWeather = BWS_OVERWORLD_NO_WEATHER;
    StoreOverworldStateBlock();
}

static void ResetAllOverworldState(void)
{
    ResetOverworldState(BWS_MAPSEC_ANY, 0);
}

static void InitOverworldStateFromSave(void)
{
    if (ModState_NeedsMigration(BATTLE_WEATHER_STACK_MOD_ID) || !LoadOverworldStateBlock())
    {
        ResetAllOverworldState();
        ModState_MarkMigrated(BATTLE_WEATHER_STACK_MOD_ID);
        StoreOverworldStateBlock();
    }
}

static void ExpireOverworldSlots(u32 currentMinute)
{
    bool8 changed = FALSE;
    u8 i;

    for (i = 0; i < ARRAY_COUNT(sOverworldSlots); i++)
    {
        if (sOverworldSlots[i].layer != BWS_LAYER_NONE && currentMinute >= sOverworldSlots[i].endMinute)
        {
            sOverworldSlots[i].layer = BWS_LAYER_NONE;
            sOverworldSlots[i].endMinute = 0;
            sOverworldSlots[i].vanillaWeather = WEATHER_NONE;
            changed = TRUE;
        }
    }

    if (changed)
        StoreOverworldStateBlock();
}

static u16 GetRandomOverworldLayers(void)
{
    u16 layers = BWS_LAYER_NONE;
    u8 i;

    for (i = 0; i < ARRAY_COUNT(sOverworldSlots); i++)
        layers |= sOverworldSlots[i].layer;

    return BattleWeatherStack_NormalizeLayers(layers, layers);
}

static u8 FindFreeOverworldSlot(void)
{
    u8 i;

    for (i = 0; i < ARRAY_COUNT(sOverworldSlots); i++)
    {
        if (sOverworldSlots[i].layer == BWS_LAYER_NONE)
            return i;
    }

    return BWS_OVERWORLD_RANDOM_SLOT_COUNT;
}

static u8 GetOverworldDurationMinutes(const struct BattleWeatherStackOverworldRule *rule)
{
    u8 spread;

    if (rule->maxDurationMinutes <= rule->minDurationMinutes)
        return rule->minDurationMinutes;

    spread = rule->maxDurationMinutes - rule->minDurationMinutes + 1;
    return rule->minDurationMinutes + (Random() % spread);
}

static bool8 CanStartOverworldRule(const struct BattleWeatherStackOverworldRule *rule, u16 activeRandomLayers)
{
    if (rule->chancePerMinute == 0)
        return FALSE;
    if (activeRandomLayers & rule->layer)
        return FALSE;
    if (!BattleWeatherStack_LayersAreCompatible(activeRandomLayers, rule->layer))
        return FALSE;
    return TRUE;
}

static void StartOverworldRule(const struct BattleWeatherStackOverworldRule *rule, u32 currentMinute)
{
    u8 slotId;
    u8 durationMinutes;

    slotId = FindFreeOverworldSlot();
    if (slotId >= BWS_OVERWORLD_RANDOM_SLOT_COUNT)
        return;

    durationMinutes = GetOverworldDurationMinutes(rule);
    sOverworldSlots[slotId].layer = rule->layer;
    sOverworldSlots[slotId].endMinute = currentMinute + durationMinutes;
    sOverworldSlots[slotId].vanillaWeather = rule->vanillaWeather;
    sBlockNewWeatherUntilMinute = currentMinute + rule->blockNewWeatherMinutes;
    StoreOverworldStateBlock();
}

static void RollOverworldWeather(u16 mapSection, u32 currentMinute)
{
    u32 totalChance = 0;
    u32 roll;
    u32 pick;
    u16 activeRandomLayers;
    u16 i;

    if (currentMinute < sBlockNewWeatherUntilMinute)
        return;
    if (FindFreeOverworldSlot() >= BWS_OVERWORLD_RANDOM_SLOT_COUNT)
        return;

    activeRandomLayers = GetRandomOverworldLayers();
    for (i = 0; i < ARRAY_COUNT(sOverworldRules); i++)
    {
        if (sOverworldRules[i].mapSection == mapSection && CanStartOverworldRule(&sOverworldRules[i], activeRandomLayers))
            totalChance += sOverworldRules[i].chancePerMinute;
    }

    if (totalChance == 0)
        return;

    if (totalChance < BWS_OVERWORLD_CHANCE_SCALE)
    {
        roll = Random() % BWS_OVERWORLD_CHANCE_SCALE;
        if (roll >= totalChance)
            return;
    }

    pick = Random32() % totalChance;
    for (i = 0; i < ARRAY_COUNT(sOverworldRules); i++)
    {
        if (sOverworldRules[i].mapSection != mapSection || !CanStartOverworldRule(&sOverworldRules[i], activeRandomLayers))
            continue;

        if (pick < sOverworldRules[i].chancePerMinute)
        {
            StartOverworldRule(&sOverworldRules[i], currentMinute);
            return;
        }

        pick -= sOverworldRules[i].chancePerMinute;
    }
}

static const struct BattleWeatherStackLightingRule *GetLightingRule(u16 mapSection)
{
    const struct BattleWeatherStackLightingRule *fallback = NULL;
    u16 i;

    for (i = 0; i < ARRAY_COUNT(sLightingRules); i++)
    {
        if (sLightingRules[i].mapSection == mapSection)
            return &sLightingRules[i];
        if (sLightingRules[i].mapSection == BWS_MAPSEC_ANY)
            fallback = &sLightingRules[i];
    }

    return fallback;
}

static u16 GetScheduledLightingLayer(u16 mapSection, u16 minuteOfDay)
{
    const struct BattleWeatherStackLightingRule *rule = GetLightingRule(mapSection);

    if (rule == NULL)
        return BWS_LAYER_NONE;
    if (MinuteInWindow(minuteOfDay, rule->darkStartMinute, rule->darkEndMinute))
        return BWS_LAYER_DARKNESS;
    if (MinuteInWindow(minuteOfDay, rule->lightStartMinute, rule->lightEndMinute))
        return BWS_LAYER_LIGHT;
    return BWS_LAYER_NONE;
}

static u16 GetCurrentOverworldLayers(u16 mapSection, u16 minuteOfDay)
{
    u16 randomLayers = GetRandomOverworldLayers();
    u16 lightingLayer = GetScheduledLightingLayer(mapSection, minuteOfDay);

    if (lightingLayer != BWS_LAYER_NONE && BattleWeatherStack_LayersAreCompatible(randomLayers, lightingLayer))
        return BattleWeatherStack_CombineLayers(randomLayers, lightingLayer);

    return randomLayers;
}

static bool8 GetOverworldSlotVisualForLayer(u16 layer, u8 *vanillaWeather)
{
    u8 i;

    for (i = 0; i < ARRAY_COUNT(sOverworldSlots); i++)
    {
        if ((sOverworldSlots[i].layer & layer) && sOverworldSlots[i].vanillaWeather != WEATHER_NONE)
        {
            *vanillaWeather = sOverworldSlots[i].vanillaWeather;
            return TRUE;
        }
    }

    return FALSE;
}

static u8 GetOverworldLayerVisual(u16 layer, u8 fallbackWeather)
{
    u8 vanillaWeather;

    if (GetOverworldSlotVisualForLayer(layer, &vanillaWeather))
        return vanillaWeather;
    return fallbackWeather;
}

static u8 GetOverworldVanillaWeather(u16 layers)
{
    const struct BattleWeatherStackLayerInfo *layerInfo;
    u16 i;

    layers = BattleWeatherStack_NormalizeLayers(layers, layers);

    for (i = 0; i < ARRAY_COUNT(sOverworldVisualCombos); i++)
    {
        if ((layers & sOverworldVisualCombos[i].layers) == sOverworldVisualCombos[i].layers)
            return sOverworldVisualCombos[i].vanillaWeather;
    }

    for (i = 0; i < ARRAY_COUNT(sOverworldVisualPriority); i++)
    {
        if (!(layers & sOverworldVisualPriority[i]))
            continue;

        layerInfo = FindLayerInfo(sOverworldVisualPriority[i]);
        if (layerInfo != NULL)
            return GetOverworldLayerVisual(layerInfo->layer, layerInfo->defaultOverworldWeather);
    }

    return WEATHER_NONE;
}

bool8 BattleWeatherStack_GetOverworldTint(u16 *r, u16 *g, u16 *b)
{
    u16 layers = BattleWeatherStack_GetOverworldLayers();
    u16 i;

    if (r == NULL || g == NULL || b == NULL)
        return FALSE;

    *r = 256;
    *g = 256;
    *b = 256;

    for (i = 0; i < ARRAY_COUNT(sLayerInfo); i++)
    {
        if ((layers & sLayerInfo[i].layer) && sLayerInfo[i].hasTint)
        {
            *r = sLayerInfo[i].tintR;
            *g = sLayerInfo[i].tintG;
            *b = sLayerInfo[i].tintB;
            return TRUE;
        }
    }

    return FALSE;
}

static bool8 UpdateOverworldWeatherState(void)
{
    struct ModTimeSnapshot snapshot;
    u16 mapSection = gMapHeader.regionMapSectionId;
    u16 newLayers;
    u8 newVanillaWeather;
    u32 currentMinute;
    bool8 changed = FALSE;

    if (!IsOutdoorWeatherMap())
    {
        if (sOverworldMapSection != mapSection || sCurrentOverworldLayers != BWS_LAYER_NONE || sCurrentOverworldVanillaWeather != WEATHER_NONE)
        {
            ClearOverworldSlots();
            sOverworldMapSection = mapSection;
            sCurrentOverworldLayers = BWS_LAYER_NONE;
            sCurrentOverworldVanillaWeather = WEATHER_NONE;
            StoreOverworldStateBlock();
        }
        return FALSE;
    }

    ModTime_Now(&snapshot);
    currentMinute = GetEpochMinute(&snapshot);

    if (sOverworldMapSection != mapSection)
        ResetOverworldState(mapSection, currentMinute);

    ExpireOverworldSlots(currentMinute);

    if (currentMinute != sLastRollMinute)
    {
        sLastRollMinute = currentMinute;
        changed = TRUE;
        if (HasOverworldRulesForMapSection(mapSection))
            RollOverworldWeather(mapSection, currentMinute);
    }

    newLayers = GetCurrentOverworldLayers(mapSection, snapshot.minute_of_day);
    newVanillaWeather = GetOverworldVanillaWeather(newLayers);
    if (sCurrentOverworldLayers != newLayers || sCurrentOverworldVanillaWeather != newVanillaWeather)
        changed = TRUE;

    sCurrentOverworldLayers = newLayers;
    sCurrentOverworldVanillaWeather = newVanillaWeather;
    if (changed)
        StoreOverworldStateBlock();
    return TRUE;
}

static void Task_BattleWeatherStackOverworld(u8 taskId)
{
    struct ModWeatherDisplay display;

    (void)taskId;

    if (!IsOverworldWeatherContext())
        return;

    ModWeather_GetDisplayedWeather(&display);
    if (sAppliedFieldWeather != display.vanilla_weather)
    {
        SetNextWeather(display.vanilla_weather);
        sAppliedFieldWeather = display.vanilla_weather;
    }
}

static void EnsureOverworldWeatherTask(void)
{
    if (!FuncIsActiveTask(Task_BattleWeatherStackOverworld))
        CreateTask(Task_BattleWeatherStackOverworld, BWS_OVERWORLD_TASK_PRIORITY);
}

u16 BattleWeatherStack_GetOverworldLayers(void)
{
    UpdateOverworldWeatherState();
    return sCurrentOverworldLayers;
}

u8 BattleWeatherStack_GetOverworldVanillaWeather(void)
{
    UpdateOverworldWeatherState();
    return sCurrentOverworldVanillaWeather;
}

u8 BattleWeatherStack_GetOverworldDebugSlotCount(void)
{
    return BWS_OVERWORLD_RANDOM_SLOT_COUNT;
}

u16 BattleWeatherStack_GetOverworldDebugMapSection(void)
{
    UpdateOverworldWeatherState();
    return sOverworldMapSection;
}

u32 BattleWeatherStack_GetOverworldDebugLastRollMinute(void)
{
    UpdateOverworldWeatherState();
    return sLastRollMinute;
}

u32 BattleWeatherStack_GetOverworldDebugBlockUntilMinute(void)
{
    UpdateOverworldWeatherState();
    return sBlockNewWeatherUntilMinute;
}

u16 BattleWeatherStack_GetOverworldDebugSlotLayer(u8 slotId)
{
    UpdateOverworldWeatherState();
    if (slotId >= BWS_OVERWORLD_RANDOM_SLOT_COUNT)
        return BWS_LAYER_NONE;
    return sOverworldSlots[slotId].layer;
}

u32 BattleWeatherStack_GetOverworldDebugSlotEndMinute(u8 slotId)
{
    UpdateOverworldWeatherState();
    if (slotId >= BWS_OVERWORLD_RANDOM_SLOT_COUNT)
        return 0;
    return sOverworldSlots[slotId].endMinute;
}

u8 BattleWeatherStack_GetOverworldDebugSlotVanillaWeather(u8 slotId)
{
    UpdateOverworldWeatherState();
    if (slotId >= BWS_OVERWORLD_RANDOM_SLOT_COUNT)
        return WEATHER_NONE;
    return sOverworldSlots[slotId].vanillaWeather;
}

void BattleWeatherStack_DebugClearOverworldWeather(void)
{
    struct ModTimeSnapshot snapshot;

    ModTime_Now(&snapshot);
    ClearOverworldSlots();
    sOverworldMapSection = gMapHeader.regionMapSectionId;
    sLastRollMinute = GetEpochMinute(&snapshot);
    sBlockNewWeatherUntilMinute = sLastRollMinute;
    sCurrentOverworldLayers = GetScheduledLightingLayer(gMapHeader.regionMapSectionId, snapshot.minute_of_day);
    sCurrentOverworldVanillaWeather = GetOverworldVanillaWeather(sCurrentOverworldLayers);
    sAppliedFieldWeather = BWS_OVERWORLD_NO_WEATHER;
    StoreOverworldStateBlock();
}

bool8 BattleWeatherStack_DebugForceOverworldSlot(u8 slotId, u16 layer, u8 vanillaWeather, u8 durationMinutes, u8 blockNewWeatherMinutes)
{
    struct ModTimeSnapshot snapshot;
    u16 otherLayers = BWS_LAYER_NONE;
    u32 currentMinute;
    u8 i;

    if (slotId >= BWS_OVERWORLD_RANDOM_SLOT_COUNT)
        return FALSE;
    if (durationMinutes == 0)
        return FALSE;

    layer = BattleWeatherStack_NormalizeLayers(layer, layer);
    if (BattleWeatherStack_CountLayers(layer) != 1)
        return FALSE;

    for (i = 0; i < ARRAY_COUNT(sOverworldSlots); i++)
    {
        if (i != slotId)
            otherLayers |= sOverworldSlots[i].layer;
    }

    if (!BattleWeatherStack_LayersAreCompatible(otherLayers, layer))
        return FALSE;

    ModTime_Now(&snapshot);
    currentMinute = GetEpochMinute(&snapshot);
    if (vanillaWeather == BWS_OVERWORLD_VISUAL_AUTO)
        vanillaWeather = GetOverworldVanillaWeather(layer);

    sOverworldMapSection = gMapHeader.regionMapSectionId;
    sOverworldSlots[slotId].layer = layer;
    sOverworldSlots[slotId].endMinute = currentMinute + durationMinutes;
    sOverworldSlots[slotId].vanillaWeather = vanillaWeather;
    sLastRollMinute = currentMinute;
    sBlockNewWeatherUntilMinute = currentMinute + blockNewWeatherMinutes;
    sCurrentOverworldLayers = GetCurrentOverworldLayers(sOverworldMapSection, snapshot.minute_of_day);
    sCurrentOverworldVanillaWeather = GetOverworldVanillaWeather(sCurrentOverworldLayers);
    sAppliedFieldWeather = BWS_OVERWORLD_NO_WEATHER;
    StoreOverworldStateBlock();
    return TRUE;
}

bool8 BattleWeatherStack_DebugForceOverworldWeather(u16 layer, u8 vanillaWeather, u8 durationMinutes, u8 blockNewWeatherMinutes)
{
    u8 slotId = FindFreeOverworldSlot();

    if (slotId >= BWS_OVERWORLD_RANDOM_SLOT_COUNT)
        return FALSE;
    return BattleWeatherStack_DebugForceOverworldSlot(slotId, layer, vanillaWeather, durationMinutes, blockNewWeatherMinutes);
}

bool8 BattleWeatherStack_DebugForceOverworldLayers(u16 layers, u8 durationMinutes, u8 blockNewWeatherMinutes)
{
    struct ModTimeSnapshot snapshot;
    u16 normalizedLayers;
    u32 currentMinute;
    u8 slotId = 0;
    u16 i;

    if (durationMinutes == 0)
        return FALSE;

    normalizedLayers = BattleWeatherStack_NormalizeLayers(layers, layers);
    if (normalizedLayers == BWS_LAYER_NONE || BattleWeatherStack_CountLayers(normalizedLayers) > BWS_OVERWORLD_RANDOM_SLOT_COUNT)
        return FALSE;

    ModTime_Now(&snapshot);
    currentMinute = GetEpochMinute(&snapshot);
    ClearOverworldSlots();

    for (i = 0; i < ARRAY_COUNT(sLayerTrimOrder); i++)
    {
        if (!(normalizedLayers & sLayerTrimOrder[i]))
            continue;
        sOverworldSlots[slotId].layer = sLayerTrimOrder[i];
        sOverworldSlots[slotId].endMinute = currentMinute + durationMinutes;
        sOverworldSlots[slotId].vanillaWeather = GetOverworldVanillaWeather(sLayerTrimOrder[i]);
        slotId++;
    }

    sOverworldMapSection = gMapHeader.regionMapSectionId;
    sLastRollMinute = currentMinute;
    sBlockNewWeatherUntilMinute = currentMinute + blockNewWeatherMinutes;
    sCurrentOverworldLayers = GetCurrentOverworldLayers(sOverworldMapSection, snapshot.minute_of_day);
    sCurrentOverworldVanillaWeather = GetOverworldVanillaWeather(sCurrentOverworldLayers);
    sAppliedFieldWeather = BWS_OVERWORLD_NO_WEATHER;
    StoreOverworldStateBlock();
    return TRUE;
}

bool8 BattleWeatherStack_ResolveOverworldWeather(struct ModWeatherDisplay *display)
{
    if (!UpdateOverworldWeatherState() || sCurrentOverworldLayers == BWS_LAYER_NONE)
        return FALSE;

    display->vanilla_weather = sCurrentOverworldVanillaWeather;
    display->layers = sCurrentOverworldLayers;
    display->source = MOD_WEATHER_SOURCE_TIME;
    display->priority = 200;
    display->battle_weather_mask = BattleWeatherStack_BattleWeather(NULL, sCurrentOverworldLayers);
    return TRUE;
}

s8 BattleWeatherStack_OnGameInit(const struct ModEvent *event)
{
    (void)event;
    InitOverworldStateFromSave();
    EngineApi_SetActiveRuleset(BATTLE_WEATHER_STACK_RULESET_ID);
    return MOD_EVENT_RESULT_CONTINUE;
}

s8 BattleWeatherStack_OnMapLoad(const struct ModEvent *event)
{
    (void)event;
    sAppliedFieldWeather = BWS_OVERWORLD_NO_WEATHER;
    EnsureOverworldWeatherTask();
    return MOD_EVENT_RESULT_CONTINUE;
}

s8 BattleWeatherStack_OnTimeSegmentChanged(const struct ModEvent *event)
{
    (void)event;
    sAppliedFieldWeather = BWS_OVERWORLD_NO_WEATHER;
    EnsureOverworldWeatherTask();
    return MOD_EVENT_RESULT_CONTINUE;
}
