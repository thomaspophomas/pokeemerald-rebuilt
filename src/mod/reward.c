#include "global.h"
#include "constants/items.h"
#include "generated/mod_registry.h"
#include "mod/reward.h"
#include "mod/runtime_profile.h"

static bool8 DefinitionIsEmptyDefault(const struct ModRewardDefinition *definition)
{
    return definition->key == NULL
        && definition->hookKey == NULL
        && definition->source == 0
        && definition->minLevel == 0
        && definition->maxLevel == 0
        && definition->priority == 0
        && definition->flags == 0
        && definition->itemId == ITEM_NONE
        && definition->quantity == 0
        && definition->hook == NULL;
}

static bool8 RuntimeRewardKeyExists(const char *key, const struct ModRewardDefinition *rewards, u16 count)
{
    u16 reward_index;

    if (key == NULL || rewards == NULL)
        return FALSE;

    for (reward_index = 0; reward_index < count; reward_index++)
    {
        if (rewards[reward_index].key != NULL && strcmp(rewards[reward_index].key, key) == 0)
            return TRUE;
    }

    return FALSE;
}

static bool8 DefinitionMatches(const struct ModRewardDefinition *definition, u8 source, u8 level)
{
    if (!RewardApi_IsDefinitionValid(definition, TRUE) || DefinitionIsEmptyDefault(definition))
        return FALSE;
    if (definition->source != source)
        return FALSE;
    if (level < definition->minLevel || level > definition->maxLevel)
        return FALSE;
    return TRUE;
}

static const struct ModRewardDefinition *FindBestReward(
    const struct ModRewardDefinition *rewards,
    u16 count,
    u8 source,
    u8 level,
    const struct ModRewardDefinition *shadowingRewards,
    u16 shadowingCount)
{
    const struct ModRewardDefinition *best_definition = NULL;
    u16 reward_index;

    if (rewards == NULL)
        return NULL;

    for (reward_index = 0; reward_index < count; reward_index++)
    {
        const struct ModRewardDefinition *definition = &rewards[reward_index];

        if (shadowingRewards != NULL && RuntimeRewardKeyExists(definition->key, shadowingRewards, shadowingCount))
            continue;
        if (!DefinitionMatches(definition, source, level))
            continue;
        if (best_definition == NULL || definition->priority < best_definition->priority)
            best_definition = definition;
    }

    return best_definition;
}

static const struct ModRewardDefinition *FindReward(u8 source, u8 level)
{
    const struct ModRewardDefinition *runtimeRewards;
    const struct ModRewardDefinition *best;
    u16 runtimeCount;

    runtimeRewards = ModRuntimeProfile_GetRewards(&runtimeCount);
    best = FindBestReward(runtimeRewards, runtimeCount, source, level, NULL, 0);
    if (best != NULL)
        return best;

    return FindBestReward(gModRewardDefinitions, gModRewardDefinitionCount, source, level, runtimeRewards, runtimeCount);
}

bool8 RewardApi_IsDefinitionValid(const struct ModRewardDefinition *definition, bool8 allow_empty_default)
{
    if (definition == NULL)
        return FALSE;
    if (allow_empty_default && DefinitionIsEmptyDefault(definition))
        return TRUE;
    if (definition->key == NULL || definition->key[0] == '\0')
        return FALSE;
    if (definition->source == 0)
        return FALSE;
    if (definition->minLevel == 0 || definition->maxLevel == 0 || definition->minLevel > 100 || definition->maxLevel > 100)
        return FALSE;
    if (definition->minLevel > definition->maxLevel)
        return FALSE;
    if (definition->hook == NULL && (definition->itemId == ITEM_NONE || definition->itemId >= ITEMS_COUNT))
        return FALSE;
    if (definition->quantity == 0)
        return FALSE;
    return TRUE;
}

bool8 RewardApi_AdjustItemReward(u8 source, u8 level, u16 vanilla_item_id, u16 *item_id, u16 *quantity)
{
    const struct ModRewardDefinition *definition;
    struct ModRewardContext context;
    u8 reward_hook_result;

    if (item_id == NULL || quantity == NULL)
        return FALSE;

    definition = FindReward(source, level);
    if (definition == NULL)
        return FALSE;

    memset(&context, 0, sizeof(context));
    context.source = source;
    context.level = level;
    context.vanillaItemId = vanilla_item_id;
    context.itemId = definition->itemId;
    context.quantity = (definition->flags & MOD_REWARD_FLAG_KEEP_VANILLA_QUANTITY) ? *quantity : definition->quantity;
    if (gSaveBlock1Ptr != NULL)
    {
        context.mapGroup = gSaveBlock1Ptr->location.mapGroup;
        context.mapNum = gSaveBlock1Ptr->location.mapNum;
    }

    reward_hook_result = MOD_REWARD_OVERRIDE;
    if (definition->hook != NULL)
        reward_hook_result = definition->hook(definition, &context);
    if (reward_hook_result == MOD_REWARD_CANCEL)
        return TRUE;
    if (reward_hook_result != MOD_REWARD_OVERRIDE)
        return FALSE;
    if (context.itemId == ITEM_NONE || context.itemId >= ITEMS_COUNT || context.quantity == 0)
        return TRUE;

    *item_id = context.itemId;
    *quantity = context.quantity;
    return TRUE;
}

ModRewardHook RewardApi_FindCompiledHook(const char *source_key, const char *hook_key)
{
    u16 reward_index;

    if (source_key == NULL || hook_key == NULL || source_key[0] == '\0' || hook_key[0] == '\0')
        return NULL;

    for (reward_index = 0; reward_index < gModRewardDefinitionCount; reward_index++)
    {
        if (gModRewardDefinitions[reward_index].key == NULL || gModRewardDefinitions[reward_index].hookKey == NULL)
            continue;
        if (strcmp(gModRewardDefinitions[reward_index].key, source_key) == 0
         && strcmp(gModRewardDefinitions[reward_index].hookKey, hook_key) == 0)
            return gModRewardDefinitions[reward_index].hook;
    }

    return NULL;
}
