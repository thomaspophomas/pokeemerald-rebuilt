#include "global.h"
#include "constants/items.h"
#include "generated/mod_registry.h"
#include "mod/item.h"
#include "mod/language.h"
#include "mod/runtime_profile.h"

static bool8 DefinitionIsEmptyDefault(const struct ModItemDefinition *definition)
{
    return definition->key == NULL
        && definition->nameKey == NULL
        && definition->descriptionKey == NULL
        && definition->fieldUseHookKey == NULL
        && definition->battleUseHookKey == NULL
        && definition->itemId == ITEM_NONE
        && definition->flags == 0
        && definition->priority == 0
        && definition->fieldUseFunc == NULL
        && definition->battleUseFunc == NULL;
}

static bool8 RuntimeItemKeyExists(const char *key, const struct ModItemDefinition *items, u16 count)
{
    u16 item_index;

    if (key == NULL || items == NULL)
        return FALSE;

    for (item_index = 0; item_index < count; item_index++)
    {
        if (items[item_index].key != NULL && strcmp(items[item_index].key, key) == 0)
            return TRUE;
    }

    return FALSE;
}

static const struct ModItemDefinition *FindBestItem(
    const struct ModItemDefinition *items,
    u16 count,
    u16 item_id,
    const struct ModItemDefinition *shadowingItems,
    u16 shadowingCount)
{
    const struct ModItemDefinition *best_definition = NULL;
    u16 item_index;

    if (items == NULL)
        return NULL;

    for (item_index = 0; item_index < count; item_index++)
    {
        const struct ModItemDefinition *definition = &items[item_index];

        if (shadowingItems != NULL && RuntimeItemKeyExists(definition->key, shadowingItems, shadowingCount))
            continue;
        if (!ItemApi_IsDefinitionValid(definition, TRUE) || DefinitionIsEmptyDefault(definition))
            continue;
        if (definition->itemId != item_id)
            continue;
        if (best_definition == NULL || definition->priority < best_definition->priority)
            best_definition = definition;
    }

    return best_definition;
}

static const struct ModItemDefinition *FindItem(u16 item_id)
{
    const struct ModItemDefinition *runtimeItems;
    const struct ModItemDefinition *best;
    u16 runtimeCount;

    if (item_id == ITEM_NONE || item_id >= ITEMS_COUNT)
        return NULL;

    runtimeItems = ModRuntimeProfile_GetItems(&runtimeCount);
    best = FindBestItem(runtimeItems, runtimeCount, item_id, NULL, 0);
    if (best != NULL)
        return best;

    return FindBestItem(gModItemDefinitions, gModItemDefinitionCount, item_id, runtimeItems, runtimeCount);
}

bool8 ItemApi_IsDefinitionValid(const struct ModItemDefinition *definition, bool8 allow_empty_default)
{
    if (definition == NULL)
        return FALSE;
    if (allow_empty_default && DefinitionIsEmptyDefault(definition))
        return TRUE;
    if (definition->key == NULL || definition->key[0] == '\0')
        return FALSE;
    if (definition->itemId == ITEM_NONE || definition->itemId >= ITEMS_COUNT)
        return FALSE;
    if ((definition->flags & MOD_ITEM_OVERRIDE_NAME) && (definition->nameKey == NULL || definition->nameKey[0] == '\0'))
        return FALSE;
    if ((definition->flags & MOD_ITEM_OVERRIDE_DESCRIPTION) && (definition->descriptionKey == NULL || definition->descriptionKey[0] == '\0'))
        return FALSE;
    if ((definition->flags & MOD_ITEM_OVERRIDE_FIELD_USE) && definition->fieldUseFunc == NULL)
        return FALSE;
    if ((definition->flags & MOD_ITEM_OVERRIDE_BATTLE_USE) && definition->battleUseFunc == NULL)
        return FALSE;
    return TRUE;
}

const u8 *ItemApi_GetName(u16 item_id, const u8 *vanilla_name)
{
    const struct ModItemDefinition *definition = FindItem(item_id);

    if (definition != NULL && (definition->flags & MOD_ITEM_OVERRIDE_NAME))
        return LanguageApi_GetText(definition->nameKey);
    return vanilla_name;
}

u16 ItemApi_GetPrice(u16 item_id, u16 vanilla_price)
{
    const struct ModItemDefinition *definition = FindItem(item_id);

    if (definition != NULL && (definition->flags & MOD_ITEM_OVERRIDE_PRICE))
        return definition->price;
    return vanilla_price;
}

u8 ItemApi_GetHoldEffect(u16 item_id, u8 vanilla_hold_effect)
{
    const struct ModItemDefinition *definition = FindItem(item_id);

    if (definition != NULL && (definition->flags & MOD_ITEM_OVERRIDE_HOLD_EFFECT))
        return definition->holdEffect;
    return vanilla_hold_effect;
}

u8 ItemApi_GetHoldEffectParam(u16 item_id, u8 vanilla_hold_effect_param)
{
    const struct ModItemDefinition *definition = FindItem(item_id);

    if (definition != NULL && (definition->flags & MOD_ITEM_OVERRIDE_HOLD_EFFECT_PARAM))
        return definition->holdEffectParam;
    return vanilla_hold_effect_param;
}

const u8 *ItemApi_GetDescription(u16 item_id, const u8 *vanilla_description)
{
    const struct ModItemDefinition *definition = FindItem(item_id);

    if (definition != NULL && (definition->flags & MOD_ITEM_OVERRIDE_DESCRIPTION))
        return LanguageApi_GetText(definition->descriptionKey);
    return vanilla_description;
}

u8 ItemApi_GetImportance(u16 item_id, u8 vanilla_importance)
{
    const struct ModItemDefinition *definition = FindItem(item_id);

    if (definition != NULL && (definition->flags & MOD_ITEM_OVERRIDE_IMPORTANCE))
        return definition->importance;
    return vanilla_importance;
}

u8 ItemApi_GetPocket(u16 item_id, u8 vanilla_pocket)
{
    const struct ModItemDefinition *definition = FindItem(item_id);

    if (definition != NULL && (definition->flags & MOD_ITEM_OVERRIDE_POCKET))
        return definition->pocket;
    return vanilla_pocket;
}

u8 ItemApi_GetType(u16 item_id, u8 vanilla_type)
{
    const struct ModItemDefinition *definition = FindItem(item_id);

    if (definition != NULL && (definition->flags & MOD_ITEM_OVERRIDE_TYPE))
        return definition->type;
    return vanilla_type;
}

ItemUseFunc ItemApi_GetFieldFunc(u16 item_id, ItemUseFunc vanilla_field_func)
{
    const struct ModItemDefinition *definition = FindItem(item_id);

    if (definition != NULL && (definition->flags & MOD_ITEM_OVERRIDE_FIELD_USE))
        return definition->fieldUseFunc;
    return vanilla_field_func;
}

u8 ItemApi_GetBattleUsage(u16 item_id, u8 vanilla_battle_usage)
{
    const struct ModItemDefinition *definition = FindItem(item_id);

    if (definition != NULL && (definition->flags & MOD_ITEM_OVERRIDE_BATTLE_USAGE))
        return definition->battleUsage;
    return vanilla_battle_usage;
}

ItemUseFunc ItemApi_GetBattleFunc(u16 item_id, ItemUseFunc vanilla_battle_func)
{
    const struct ModItemDefinition *definition = FindItem(item_id);

    if (definition != NULL && (definition->flags & MOD_ITEM_OVERRIDE_BATTLE_USE))
        return definition->battleUseFunc;
    return vanilla_battle_func;
}

u8 ItemApi_GetSecondaryId(u16 item_id, u8 vanilla_secondary_id)
{
    const struct ModItemDefinition *definition = FindItem(item_id);

    if (definition != NULL && (definition->flags & MOD_ITEM_OVERRIDE_SECONDARY_ID))
        return definition->secondaryId;
    return vanilla_secondary_id;
}

ItemUseFunc ItemApi_FindCompiledFieldUseHook(const char *source_key, const char *hook_key)
{
    u16 item_index;

    if (source_key == NULL || hook_key == NULL || source_key[0] == '\0' || hook_key[0] == '\0')
        return NULL;

    for (item_index = 0; item_index < gModItemDefinitionCount; item_index++)
    {
        if (gModItemDefinitions[item_index].key == NULL || gModItemDefinitions[item_index].fieldUseHookKey == NULL)
            continue;
        if (strcmp(gModItemDefinitions[item_index].key, source_key) == 0
         && strcmp(gModItemDefinitions[item_index].fieldUseHookKey, hook_key) == 0)
            return gModItemDefinitions[item_index].fieldUseFunc;
    }

    return NULL;
}

ItemUseFunc ItemApi_FindCompiledBattleUseHook(const char *source_key, const char *hook_key)
{
    u16 item_index;

    if (source_key == NULL || hook_key == NULL || source_key[0] == '\0' || hook_key[0] == '\0')
        return NULL;

    for (item_index = 0; item_index < gModItemDefinitionCount; item_index++)
    {
        if (gModItemDefinitions[item_index].key == NULL || gModItemDefinitions[item_index].battleUseHookKey == NULL)
            continue;
        if (strcmp(gModItemDefinitions[item_index].key, source_key) == 0
         && strcmp(gModItemDefinitions[item_index].battleUseHookKey, hook_key) == 0)
            return gModItemDefinitions[item_index].battleUseFunc;
    }

    return NULL;
}
