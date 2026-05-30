#include "global.h"
#include "constants/pokemon.h"
#include "generated/mod_registry.h"
#include "mod/badge.h"
#include "mod/level_cap.h"
#include "pokemon.h"

static bool8 DefinitionIsEmptyDefault(const struct ModLevelCapDefinition *definition)
{
    return definition->key == NULL
        && definition->mode == 0
        && definition->soft_exp_percent == 0
        && definition->rare_candy_policy == 0
        && definition->priority == 0
        && definition->flags == 0;
}

static bool8 CapTableIsValid(const struct ModLevelCapDefinition *definition)
{
    u8 badge_slot;

    for (badge_slot = 0; badge_slot < MOD_LEVEL_CAP_BADGE_SLOTS; badge_slot++)
    {
        if (definition->caps[badge_slot] == 0 || definition->caps[badge_slot] > MAX_LEVEL)
            return FALSE;
    }

    return TRUE;
}

bool8 LevelCapApi_IsDefinitionValid(const struct ModLevelCapDefinition *definition, bool8 allow_empty_default)
{
    if (definition == NULL)
        return FALSE;
    if (allow_empty_default && DefinitionIsEmptyDefault(definition))
        return TRUE;
    if (definition->key == NULL || definition->key[0] == '\0')
        return FALSE;
    if (definition->mode > MOD_LEVEL_CAP_MODE_HARD)
        return FALSE;
    if (definition->mode == MOD_LEVEL_CAP_MODE_NONE)
        return TRUE;
    if (definition->soft_exp_percent > 100)
        return FALSE;
    if (definition->rare_candy_policy > MOD_LEVEL_CAP_RARE_CANDY_BLOCK_AT_CAP)
        return FALSE;
    return CapTableIsValid(definition);
}

static const struct ModLevelCapDefinition *GetActiveLevelCapDefinition(void)
{
    const struct ModLevelCapDefinition *best_definition = NULL;
    u16 definition_index;

    for (definition_index = 0; definition_index < gModLevelCapCount; definition_index++)
    {
        if (!LevelCapApi_IsDefinitionValid(&gModLevelCaps[definition_index], TRUE))
            continue;
        if (gModLevelCaps[definition_index].mode == MOD_LEVEL_CAP_MODE_NONE)
            continue;
        if (best_definition == NULL || gModLevelCaps[definition_index].priority < best_definition->priority)
            best_definition = &gModLevelCaps[definition_index];
    }

    return best_definition;
}

static u8 CountEarnedBadges(void)
{
    u8 badge_index;
    u8 badge_count = 0;

    for (badge_index = 0; badge_index < MOD_BADGE_COUNT; badge_index++)
    {
        if (BadgeApi_GetLevel(badge_index) > 0)
            badge_count++;
    }

    return badge_count;
}

u8 LevelCapApi_GetActiveCap(void)
{
    const struct ModLevelCapDefinition *definition = GetActiveLevelCapDefinition();
    u8 badge_count;

    if (definition == NULL)
        return MAX_LEVEL;

    badge_count = CountEarnedBadges();
    if (badge_count >= MOD_LEVEL_CAP_BADGE_SLOTS)
        badge_count = MOD_LEVEL_CAP_BADGE_SLOTS - 1;

    return definition->caps[badge_count];
}

static s16 ApplyPercentToExp(s16 gained_exp, u8 percent)
{
    s32 scaled_exp;

    if (gained_exp <= 0 || percent == 0)
        return 0;
    if (percent >= 100)
        return gained_exp;

    scaled_exp = (gained_exp * percent) / 100;
    if (scaled_exp == 0)
        scaled_exp = 1;
    if (scaled_exp > 32767)
        return 32767;
    return scaled_exp;
}

s16 LevelCapApi_ModifyBattleExp(struct Pokemon *mon, s16 gained_exp)
{
    const struct ModLevelCapDefinition *definition = GetActiveLevelCapDefinition();
    u8 level;

    if (definition == NULL || mon == NULL || gained_exp <= 0)
        return gained_exp;

    level = GetMonData(mon, MON_DATA_LEVEL);
    if (level < LevelCapApi_GetActiveCap())
        return gained_exp;

    if (definition->mode == MOD_LEVEL_CAP_MODE_HARD)
        return 0;

    return ApplyPercentToExp(gained_exp, definition->soft_exp_percent);
}

bool8 LevelCapApi_CanUseRareCandy(struct Pokemon *mon)
{
    const struct ModLevelCapDefinition *definition = GetActiveLevelCapDefinition();

    if (definition == NULL || mon == NULL)
        return TRUE;
    if (definition->rare_candy_policy != MOD_LEVEL_CAP_RARE_CANDY_BLOCK_AT_CAP)
        return TRUE;
    return GetMonData(mon, MON_DATA_LEVEL) < LevelCapApi_GetActiveCap();
}
