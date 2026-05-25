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

static bool8 BadgeIdInRange(u8 badge_id)
{
    return badge_id < MOD_BADGE_COUNT && badge_id < MOD_SAVE_BADGE_LEVEL_COUNT;
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

static bool8 BadgeEffectTargetIsValid(u8 effect_kind, u8 target)
{
    switch (effect_kind)
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

bool8 BadgeApi_IsEffectDefinitionValid(const struct ModBadgeEffectDefinition *definition, bool8 allow_empty_none)
{
    if (definition == NULL)
        return FALSE;

    if (definition->effectKind == MOD_BADGE_EFFECT_TYPE_NONE)
        return allow_empty_none && BadgeEffectDefinitionIsEmptyNone(definition);

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

static u8 GetStoredBadgeLevel(u8 badge_id)
{
    struct ModSaveState *state;

    if (!BadgeIdInRange(badge_id))
        return 0;

    state = ModState_Get();
    return ClampBadgeLevel(state->badgeLevels[badge_id]);
}

static bool8 EffectKeyInRuntimeProfile(const char *key, const struct ModBadgeEffectDefinition *effects, u16 count)
{
    u16 badge_effect_index;

    if (key == NULL || effects == NULL)
        return FALSE;

    for (badge_effect_index = 0; badge_effect_index < count; badge_effect_index++)
    {
        if (effects[badge_effect_index].key != NULL && strcmp(effects[badge_effect_index].key, key) == 0)
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

static s32 SumBadgeEffects(const struct ModBadgeEffectDefinition *effects, u16 count, u8 effect_kind, u8 target)
{
    u16 badge_effect_index;
    s32 total = 0;
    u8 level;

    if (effects == NULL)
        return 0;

    for (badge_effect_index = 0; badge_effect_index < count; badge_effect_index++)
    {
        if (!BadgeApi_IsEffectDefinitionValid(&effects[badge_effect_index], FALSE))
            continue;
        if (effects[badge_effect_index].effectKind != effect_kind || effects[badge_effect_index].target != target)
            continue;

        level = BadgeApi_GetLevel(effects[badge_effect_index].badgeId);
        if (level == 0)
            continue;
        if (level > effects[badge_effect_index].maxLevel)
            level = effects[badge_effect_index].maxLevel;

        total += effects[badge_effect_index].percentPerLevel * level;
    }

    return total;
}

static s32 SumGeneratedBadgeEffectsExceptRuntimeKeys(
    const struct ModBadgeEffectDefinition *runtimeEffects,
    u16 runtimeCount,
    u8 effect_kind,
    u8 target)
{
    u16 badge_effect_index;
    s32 total = 0;
    u8 level;

    for (badge_effect_index = 0; badge_effect_index < gModBadgeEffectCount; badge_effect_index++)
    {
        if (!BadgeApi_IsEffectDefinitionValid(&gModBadgeEffects[badge_effect_index], TRUE))
            continue;
        if (EffectKeyInRuntimeProfile(gModBadgeEffects[badge_effect_index].key, runtimeEffects, runtimeCount))
            continue;
        if (gModBadgeEffects[badge_effect_index].effectKind != effect_kind || gModBadgeEffects[badge_effect_index].target != target)
            continue;

        level = BadgeApi_GetLevel(gModBadgeEffects[badge_effect_index].badgeId);
        if (level == 0)
            continue;
        if (level > gModBadgeEffects[badge_effect_index].maxLevel)
            level = gModBadgeEffects[badge_effect_index].maxLevel;

        total += gModBadgeEffects[badge_effect_index].percentPerLevel * level;
    }

    return total;
}

void BadgeApi_Init(void)
{
    struct ModSaveState *state = ModState_Get();
    u8 badge_level_index;
    bool8 changed = FALSE;

    for (badge_level_index = 0; badge_level_index < MOD_SAVE_BADGE_LEVEL_COUNT; badge_level_index++)
    {
        if (state->badgeLevels[badge_level_index] > MOD_BADGE_LEVEL_MAX)
        {
            state->badgeLevels[badge_level_index] = MOD_BADGE_LEVEL_MAX;
            changed = TRUE;
        }
    }

    if (changed)
        ModState_BumpRevision();
}

u8 BadgeApi_GetLevel(u8 badge_id)
{
    u8 stored;

    if (!BadgeIdInRange(badge_id))
        return 0;

    stored = GetStoredBadgeLevel(badge_id);
    if (stored != 0)
        return stored;
    if (FlagGet(sBadgeFlags[badge_id]))
        return 1;
    return 0;
}

bool8 BadgeApi_SetLevel(u8 badge_id, u8 level)
{
    struct ModSaveState *state;

    if (!BadgeIdInRange(badge_id))
        return FALSE;

    level = ClampBadgeLevel(level);
    state = ModState_Get();
    if (state->badgeLevels[badge_id] == level)
        return TRUE;

    state->badgeLevels[badge_id] = level;
    if (level != 0)
        FlagSet(sBadgeFlags[badge_id]);
    ModState_BumpRevision();
    return TRUE;
}

bool8 BadgeApi_IncrementLevel(u8 badge_id, u8 amount)
{
    u8 level;

    if (!BadgeIdInRange(badge_id))
        return FALSE;

    level = BadgeApi_GetLevel(badge_id);
    if (amount > MOD_BADGE_LEVEL_MAX - level)
        level = MOD_BADGE_LEVEL_MAX;
    else
        level += amount;

    return BadgeApi_SetLevel(badge_id, level);
}

s16 BadgeApi_GetEffectPercent(u8 effect_kind, u8 target, u8 battler_id)
{
    const struct ModBadgeEffectDefinition *runtimeEffects;
    u16 runtimeCount = 0;
    s32 total;

    if (battler_id >= MAX_BATTLERS_COUNT || battler_id >= gBattlersCount || GetBattlerSide(battler_id) != B_SIDE_PLAYER)
        return 0;
    if (!BadgeEffectTargetIsValid(effect_kind, target))
        return 0;

    runtimeEffects = ModRuntimeProfile_GetBadgeEffects(&runtimeCount);
    total = SumBadgeEffects(runtimeEffects, runtimeCount, effect_kind, target);
    total += SumGeneratedBadgeEffectsExceptRuntimeKeys(runtimeEffects, runtimeCount, effect_kind, target);
    return ClampEffectPercent(total);
}
