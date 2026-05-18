#include "global.h"
#include "battle.h"
#include "battle_anim.h"
#include "constants/battle.h"
#include "constants/flags.h"
#include "constants/pokemon.h"
#include "event_data.h"
#include "generated/mod_registry.h"
#include "mod/badge.h"
#include "mod/runtime_profile.h"
#include "mod/state.h"

static const u16 sBadgeFlags[MOD_BADGE_COUNT] =
{
    FLAG_BADGE01_GET,
    FLAG_BADGE02_GET,
    FLAG_BADGE03_GET,
    FLAG_BADGE04_GET,
    FLAG_BADGE05_GET,
    FLAG_BADGE06_GET,
    FLAG_BADGE07_GET,
    FLAG_BADGE08_GET,
};

static bool8 BadgeIdInRange(u8 badgeId)
{
    return badgeId < MOD_BADGE_COUNT && badgeId < MOD_SAVE_BADGE_LEVEL_COUNT;
}

static u8 ClampBadgeLevel(u8 level)
{
    return level > MOD_BADGE_LEVEL_MAX ? MOD_BADGE_LEVEL_MAX : level;
}

static bool8 BadgeEffectDefinitionIsEmptyNone(const struct ModBadgeEffectDefinition *definition)
{
    return definition->key == NULL
        && definition->badgeId == 0
        && definition->target == 0
        && definition->percentPerLevel == 0
        && definition->maxLevel == 0
        && definition->flags == 0;
}

static bool8 BadgeEffectTargetIsValid(u8 effectKind, u8 target)
{
    switch (effectKind)
    {
    case MOD_BADGE_EFFECT_TYPE_RESISTANCE_PERCENT:
    case MOD_BADGE_EFFECT_TYPE_DAMAGE_PERCENT:
        return target != TYPE_NONE && target < NUMBER_OF_MON_TYPES;
    case MOD_BADGE_EFFECT_TYPE_STAT_PERCENT:
        return target < NUM_BATTLE_STATS;
    default:
        return FALSE;
    }
}

bool8 BadgeApi_IsEffectDefinitionValid(const struct ModBadgeEffectDefinition *definition, bool8 allowEmptyNone)
{
    if (definition == NULL)
        return FALSE;

    if (definition->effectKind == MOD_BADGE_EFFECT_TYPE_NONE)
        return allowEmptyNone && BadgeEffectDefinitionIsEmptyNone(definition);

    if (definition->key == NULL || definition->key[0] == '\0')
        return FALSE;
    if (definition->badgeId >= MOD_BADGE_COUNT)
        return FALSE;
    if (definition->effectKind > MOD_BADGE_EFFECT_TYPE_STAT_PERCENT)
        return FALSE;
    if (!BadgeEffectTargetIsValid(definition->effectKind, definition->target))
        return FALSE;
    if (definition->percentPerLevel < -100 || definition->percentPerLevel > 100)
        return FALSE;
    if (definition->maxLevel == 0 || definition->maxLevel > MOD_BADGE_LEVEL_MAX)
        return FALSE;

    return TRUE;
}

static u8 GetStoredBadgeLevel(u8 badgeId)
{
    struct ModSaveState *state;

    if (!BadgeIdInRange(badgeId))
        return 0;

    state = ModState_Get();
    return ClampBadgeLevel(state->badgeLevels[badgeId]);
}

static bool8 EffectKeyInRuntimeProfile(const char *key, const struct ModBadgeEffectDefinition *effects, u16 count)
{
    u16 i;

    if (key == NULL || effects == NULL)
        return FALSE;

    for (i = 0; i < count; i++)
    {
        if (effects[i].key != NULL && strcmp(effects[i].key, key) == 0)
            return TRUE;
    }

    return FALSE;
}

static s16 ClampEffectPercent(s32 value)
{
    if (value > 32767)
        return 32767;
    if (value < -32768)
        return -32768;
    return value;
}

static s32 SumBadgeEffects(const struct ModBadgeEffectDefinition *effects, u16 count, u8 effectKind, u8 target)
{
    u16 i;
    s32 total = 0;
    u8 level;

    if (effects == NULL)
        return 0;

    for (i = 0; i < count; i++)
    {
        if (!BadgeApi_IsEffectDefinitionValid(&effects[i], FALSE))
            continue;
        if (effects[i].effectKind != effectKind || effects[i].target != target)
            continue;

        level = BadgeApi_GetLevel(effects[i].badgeId);
        if (level == 0)
            continue;
        if (level > effects[i].maxLevel)
            level = effects[i].maxLevel;

        total += effects[i].percentPerLevel * level;
    }

    return total;
}

static s32 SumGeneratedBadgeEffectsExceptRuntimeKeys(
    const struct ModBadgeEffectDefinition *runtimeEffects,
    u16 runtimeCount,
    u8 effectKind,
    u8 target)
{
    u16 i;
    s32 total = 0;
    u8 level;

    for (i = 0; i < gModBadgeEffectCount; i++)
    {
        if (!BadgeApi_IsEffectDefinitionValid(&gModBadgeEffects[i], TRUE))
            continue;
        if (EffectKeyInRuntimeProfile(gModBadgeEffects[i].key, runtimeEffects, runtimeCount))
            continue;
        if (gModBadgeEffects[i].effectKind != effectKind || gModBadgeEffects[i].target != target)
            continue;

        level = BadgeApi_GetLevel(gModBadgeEffects[i].badgeId);
        if (level == 0)
            continue;
        if (level > gModBadgeEffects[i].maxLevel)
            level = gModBadgeEffects[i].maxLevel;

        total += gModBadgeEffects[i].percentPerLevel * level;
    }

    return total;
}

void BadgeApi_Init(void)
{
    struct ModSaveState *state = ModState_Get();
    u8 i;
    bool8 changed = FALSE;

    for (i = 0; i < MOD_SAVE_BADGE_LEVEL_COUNT; i++)
    {
        if (state->badgeLevels[i] > MOD_BADGE_LEVEL_MAX)
        {
            state->badgeLevels[i] = MOD_BADGE_LEVEL_MAX;
            changed = TRUE;
        }
    }

    if (changed)
        ModState_BumpRevision();
}

u8 BadgeApi_GetLevel(u8 badgeId)
{
    u8 stored;

    if (!BadgeIdInRange(badgeId))
        return 0;

    stored = GetStoredBadgeLevel(badgeId);
    if (stored != 0)
        return stored;
    if (FlagGet(sBadgeFlags[badgeId]))
        return 1;
    return 0;
}

bool8 BadgeApi_SetLevel(u8 badgeId, u8 level)
{
    struct ModSaveState *state;

    if (!BadgeIdInRange(badgeId))
        return FALSE;

    level = ClampBadgeLevel(level);
    state = ModState_Get();
    if (state->badgeLevels[badgeId] == level)
        return TRUE;

    state->badgeLevels[badgeId] = level;
    if (level != 0)
        FlagSet(sBadgeFlags[badgeId]);
    ModState_BumpRevision();
    return TRUE;
}

bool8 BadgeApi_IncrementLevel(u8 badgeId, u8 amount)
{
    u8 level;

    if (!BadgeIdInRange(badgeId))
        return FALSE;

    level = BadgeApi_GetLevel(badgeId);
    if (amount > MOD_BADGE_LEVEL_MAX - level)
        level = MOD_BADGE_LEVEL_MAX;
    else
        level += amount;

    return BadgeApi_SetLevel(badgeId, level);
}

s16 BadgeApi_GetEffectPercent(u8 effectKind, u8 target, u8 battlerId)
{
    const struct ModBadgeEffectDefinition *runtimeEffects;
    u16 runtimeCount = 0;
    s32 total;

    if (battlerId >= MAX_BATTLERS_COUNT || battlerId >= gBattlersCount || GetBattlerSide(battlerId) != B_SIDE_PLAYER)
        return 0;
    if (!BadgeEffectTargetIsValid(effectKind, target))
        return 0;

    runtimeEffects = ModRuntimeProfile_GetBadgeEffects(&runtimeCount);
    total = SumBadgeEffects(runtimeEffects, runtimeCount, effectKind, target);
    total += SumGeneratedBadgeEffectsExceptRuntimeKeys(runtimeEffects, runtimeCount, effectKind, target);
    return ClampEffectPercent(total);
}
