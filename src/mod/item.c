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
    u16 i;

    if (key == NULL || items == NULL)
        return FALSE;

    for (i = 0; i < count; i++)
    {
        if (items[i].key != NULL && strcmp(items[i].key, key) == 0)
            return TRUE;
    }

    return FALSE;
}

static const struct ModItemDefinition *FindBestItem(
    const struct ModItemDefinition *items,
    u16 count,
    u16 itemId,
    const struct ModItemDefinition *shadowingItems,
    u16 shadowingCount)
{
    const struct ModItemDefinition *best = NULL;
    u16 i;

    if (items == NULL)
        return NULL;

    for (i = 0; i < count; i++)
    {
        const struct ModItemDefinition *definition = &items[i];

        if (shadowingItems != NULL && RuntimeItemKeyExists(definition->key, shadowingItems, shadowingCount))
            continue;
        if (!ItemApi_IsDefinitionValid(definition, TRUE) || DefinitionIsEmptyDefault(definition))
            continue;
        if (definition->itemId != itemId)
            continue;
        if (best == NULL || definition->priority < best->priority)
            best = definition;
    }

    return best;
}

static const struct ModItemDefinition *FindItem(u16 itemId)
{
    const struct ModItemDefinition *runtimeItems;
    const struct ModItemDefinition *best;
    u16 runtimeCount;

    if (itemId == ITEM_NONE || itemId >= ITEMS_COUNT)
        return NULL;

    runtimeItems = ModRuntimeProfile_GetItems(&runtimeCount);
    best = FindBestItem(runtimeItems, runtimeCount, itemId, NULL, 0);
    if (best != NULL)
        return best;

    return FindBestItem(gModItemDefinitions, gModItemDefinitionCount, itemId, runtimeItems, runtimeCount);
}

bool8 ItemApi_IsDefinitionValid(const struct ModItemDefinition *definition, bool8 allowEmptyDefault)
{
    if (definition == NULL)
        return FALSE;
    if (allowEmptyDefault && DefinitionIsEmptyDefault(definition))
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

const u8 *ItemApi_GetName(u16 itemId, const u8 *vanilla)
{
    const struct ModItemDefinition *definition = FindItem(itemId);

    if (definition != NULL && (definition->flags & MOD_ITEM_OVERRIDE_NAME))
        return LanguageApi_GetText(definition->nameKey);
    return vanilla;
}

u16 ItemApi_GetPrice(u16 itemId, u16 vanilla)
{
    const struct ModItemDefinition *definition = FindItem(itemId);

    if (definition != NULL && (definition->flags & MOD_ITEM_OVERRIDE_PRICE))
        return definition->price;
    return vanilla;
}

u8 ItemApi_GetHoldEffect(u16 itemId, u8 vanilla)
{
    const struct ModItemDefinition *definition = FindItem(itemId);

    if (definition != NULL && (definition->flags & MOD_ITEM_OVERRIDE_HOLD_EFFECT))
        return definition->holdEffect;
    return vanilla;
}

u8 ItemApi_GetHoldEffectParam(u16 itemId, u8 vanilla)
{
    const struct ModItemDefinition *definition = FindItem(itemId);

    if (definition != NULL && (definition->flags & MOD_ITEM_OVERRIDE_HOLD_EFFECT_PARAM))
        return definition->holdEffectParam;
    return vanilla;
}

const u8 *ItemApi_GetDescription(u16 itemId, const u8 *vanilla)
{
    const struct ModItemDefinition *definition = FindItem(itemId);

    if (definition != NULL && (definition->flags & MOD_ITEM_OVERRIDE_DESCRIPTION))
        return LanguageApi_GetText(definition->descriptionKey);
    return vanilla;
}

u8 ItemApi_GetImportance(u16 itemId, u8 vanilla)
{
    const struct ModItemDefinition *definition = FindItem(itemId);

    if (definition != NULL && (definition->flags & MOD_ITEM_OVERRIDE_IMPORTANCE))
        return definition->importance;
    return vanilla;
}

u8 ItemApi_GetPocket(u16 itemId, u8 vanilla)
{
    const struct ModItemDefinition *definition = FindItem(itemId);

    if (definition != NULL && (definition->flags & MOD_ITEM_OVERRIDE_POCKET))
        return definition->pocket;
    return vanilla;
}

u8 ItemApi_GetType(u16 itemId, u8 vanilla)
{
    const struct ModItemDefinition *definition = FindItem(itemId);

    if (definition != NULL && (definition->flags & MOD_ITEM_OVERRIDE_TYPE))
        return definition->type;
    return vanilla;
}

ItemUseFunc ItemApi_GetFieldFunc(u16 itemId, ItemUseFunc vanilla)
{
    const struct ModItemDefinition *definition = FindItem(itemId);

    if (definition != NULL && (definition->flags & MOD_ITEM_OVERRIDE_FIELD_USE))
        return definition->fieldUseFunc;
    return vanilla;
}

u8 ItemApi_GetBattleUsage(u16 itemId, u8 vanilla)
{
    const struct ModItemDefinition *definition = FindItem(itemId);

    if (definition != NULL && (definition->flags & MOD_ITEM_OVERRIDE_BATTLE_USAGE))
        return definition->battleUsage;
    return vanilla;
}

ItemUseFunc ItemApi_GetBattleFunc(u16 itemId, ItemUseFunc vanilla)
{
    const struct ModItemDefinition *definition = FindItem(itemId);

    if (definition != NULL && (definition->flags & MOD_ITEM_OVERRIDE_BATTLE_USE))
        return definition->battleUseFunc;
    return vanilla;
}

u8 ItemApi_GetSecondaryId(u16 itemId, u8 vanilla)
{
    const struct ModItemDefinition *definition = FindItem(itemId);

    if (definition != NULL && (definition->flags & MOD_ITEM_OVERRIDE_SECONDARY_ID))
        return definition->secondaryId;
    return vanilla;
}

ItemUseFunc ItemApi_FindCompiledFieldUseHook(const char *sourceKey, const char *hookKey)
{
    u16 i;

    if (sourceKey == NULL || hookKey == NULL || sourceKey[0] == '\0' || hookKey[0] == '\0')
        return NULL;

    for (i = 0; i < gModItemDefinitionCount; i++)
    {
        if (gModItemDefinitions[i].key == NULL || gModItemDefinitions[i].fieldUseHookKey == NULL)
            continue;
        if (strcmp(gModItemDefinitions[i].key, sourceKey) == 0
         && strcmp(gModItemDefinitions[i].fieldUseHookKey, hookKey) == 0)
            return gModItemDefinitions[i].fieldUseFunc;
    }

    return NULL;
}

ItemUseFunc ItemApi_FindCompiledBattleUseHook(const char *sourceKey, const char *hookKey)
{
    u16 i;

    if (sourceKey == NULL || hookKey == NULL || sourceKey[0] == '\0' || hookKey[0] == '\0')
        return NULL;

    for (i = 0; i < gModItemDefinitionCount; i++)
    {
        if (gModItemDefinitions[i].key == NULL || gModItemDefinitions[i].battleUseHookKey == NULL)
            continue;
        if (strcmp(gModItemDefinitions[i].key, sourceKey) == 0
         && strcmp(gModItemDefinitions[i].battleUseHookKey, hookKey) == 0)
            return gModItemDefinitions[i].battleUseFunc;
    }

    return NULL;
}
