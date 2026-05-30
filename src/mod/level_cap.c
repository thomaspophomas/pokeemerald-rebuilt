#include "global.h"
#include "constants/pokemon.h"
#include "generated/mod_registry.h"
#include "mod/flags.h"
#include "mod/level_cap.h"
#include "pokemon.h"

static bool8 DefinitionIsEmptyDefault(const struct ModLevelCapDefinition *definition)
{
    return definition->key == NULL
        && definition->mode == 0
        && definition->stage_count == 0
        && definition->rare_candy_policy == 0
        && definition->priority == 0
        && definition->flags == 0;
}

static bool8 StagesAreValid(const struct ModLevelCapDefinition *definition)
{
    u8 stage_index;

    if (definition->stage_count == 0 || definition->stage_count > MOD_LEVEL_CAP_MAX_STAGES)
        return FALSE;

    for (stage_index = 0; stage_index < definition->stage_count; stage_index++)
    {
        if (definition->stages[stage_index].level == 0 || definition->stages[stage_index].level > MAX_LEVEL)
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
    if (definition->rare_candy_policy > MOD_LEVEL_CAP_RARE_CANDY_BLOCK_AT_CAP)
        return FALSE;
    return StagesAreValid(definition);
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

static bool8 StageIsUnlocked(const struct ModLevelCapStage *stage)
{
    if (stage->unlock_flag == MOD_LEVEL_CAP_FLAG_ALWAYS)
        return TRUE;
    return ModFlag_Get(stage->unlock_flag);
}

static bool8 TryGetDefinitionActiveCap(const struct ModLevelCapDefinition *definition, u8 *cap)
{
    u8 stage_index;
    u8 active_cap = 0;

    for (stage_index = 0; stage_index < definition->stage_count; stage_index++)
    {
        if (!StageIsUnlocked(&definition->stages[stage_index]))
            continue;
        if (definition->stages[stage_index].level > active_cap)
            active_cap = definition->stages[stage_index].level;
    }

    if (active_cap == 0)
        return FALSE;

    *cap = active_cap;
    return TRUE;
}

u8 LevelCapApi_GetActiveCap(void)
{
    const struct ModLevelCapDefinition *definition = GetActiveLevelCapDefinition();
    u8 cap;

    if (definition == NULL)
        return MAX_LEVEL;

    return TryGetDefinitionActiveCap(definition, &cap) ? cap : MAX_LEVEL;
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

static u8 GetDynamicSoftExpPercent(u8 level, u8 cap)
{
    s16 distance = (s16)cap - level;

    if (distance > 3)
        return 100;
    if (distance == 3)
        return 90;
    if (distance == 2)
        return 60;
    if (distance == 1)
        return 30;
    if (distance == 0)
        return 15;
    if (distance <= -3)
        return 0;
    return 15 + (distance * 5);
}

s16 LevelCapApi_ModifyBattleExp(struct Pokemon *mon, s16 gained_exp)
{
    const struct ModLevelCapDefinition *definition = GetActiveLevelCapDefinition();
    u8 level;
    u8 cap;

    if (definition == NULL || mon == NULL || gained_exp <= 0)
        return gained_exp;

    level = GetMonData(mon, MON_DATA_LEVEL);
    if (!TryGetDefinitionActiveCap(definition, &cap))
        return gained_exp;

    if (definition->mode == MOD_LEVEL_CAP_MODE_HARD)
    {
        if (level >= cap)
            return 0;
        return gained_exp;
    }

    return ApplyPercentToExp(gained_exp, GetDynamicSoftExpPercent(level, cap));
}

bool8 LevelCapApi_CanUseRareCandy(struct Pokemon *mon)
{
    const struct ModLevelCapDefinition *definition = GetActiveLevelCapDefinition();
    u8 cap;

    if (definition == NULL || mon == NULL)
        return TRUE;
    if (definition->rare_candy_policy != MOD_LEVEL_CAP_RARE_CANDY_BLOCK_AT_CAP)
        return TRUE;
    if (!TryGetDefinitionActiveCap(definition, &cap))
        return TRUE;
    return GetMonData(mon, MON_DATA_LEVEL) < cap;
}
