#include "global.h"
#include "constants/items.h"
#include "generated/mod_registry.h"
#include "mod/runtime_profile.h"
#include "mod/shop.h"
#include "string_util.h"

static EWRAM_DATA u16 sRuntimeShopItems[MOD_SHOP_MAX_ITEMS + 1];

static bool8 DefinitionIsEmptyDefault(const struct ModShopDefinition *definition)
{
    u8 i;

    if (definition == NULL)
        return FALSE;
    if (definition->key != NULL)
        return FALSE;
    if (definition->mapGroup != 0 || definition->mapNum != 0 || definition->martType != 0 || definition->itemCount != 0)
        return FALSE;
    if (definition->priority != 0 || definition->flags != 0)
        return FALSE;
    for (i = 0; i < MOD_SHOP_MAX_ITEMS; i++)
    {
        if (definition->items[i] != ITEM_NONE)
            return FALSE;
    }
    return TRUE;
}

static bool8 RuntimeShopKeyExists(const char *key, const struct ModShopDefinition *shops, u16 count)
{
    u16 i;

    if (key == NULL)
        return FALSE;

    for (i = 0; i < count; i++)
    {
        if (shops[i].key != NULL && strcmp(shops[i].key, key) == 0)
            return TRUE;
    }

    return FALSE;
}

static const struct ModShopDefinition *FindBestShop(
    const struct ModShopDefinition *shops,
    u16 count,
    u8 mapGroup,
    u8 mapNum,
    u8 martType,
    const struct ModShopDefinition *shadowingShops,
    u16 shadowingCount)
{
    const struct ModShopDefinition *best = NULL;
    u16 i;

    if (shops == NULL)
        return NULL;

    for (i = 0; i < count; i++)
    {
        const struct ModShopDefinition *definition = &shops[i];

        if (shadowingShops != NULL && RuntimeShopKeyExists(definition->key, shadowingShops, shadowingCount))
            continue;
        if (!ShopApi_IsDefinitionValid(definition, TRUE) || DefinitionIsEmptyDefault(definition))
            continue;
        if (definition->mapGroup != mapGroup || definition->mapNum != mapNum || definition->martType != martType)
            continue;
        if (best == NULL || definition->priority < best->priority)
            best = definition;
    }

    return best;
}

bool8 ShopApi_IsDefinitionValid(const struct ModShopDefinition *definition, bool8 allowEmptyDefault)
{
    u8 i;

    if (definition == NULL)
        return FALSE;
    if (allowEmptyDefault && DefinitionIsEmptyDefault(definition))
        return TRUE;
    if (definition->key == NULL || definition->key[0] == '\0')
        return FALSE;
    if (definition->martType > MOD_SHOP_TYPE_DECOR2)
        return FALSE;
    if (definition->itemCount == 0 || definition->itemCount > MOD_SHOP_MAX_ITEMS)
        return FALSE;

    for (i = 0; i < definition->itemCount; i++)
    {
        if (definition->items[i] == ITEM_NONE || definition->items[i] >= ITEMS_COUNT)
            return FALSE;
    }

    return TRUE;
}

const u16 *ShopApi_GetItemsForSale(u8 martType, const u16 *vanillaItems)
{
    const struct ModShopDefinition *runtimeShops;
    const struct ModShopDefinition *definition;
    u16 runtimeCount;
    u8 mapGroup;
    u8 mapNum;
    u8 i;

    if (gSaveBlock1Ptr == NULL)
        return vanillaItems;

    mapGroup = gSaveBlock1Ptr->location.mapGroup;
    mapNum = gSaveBlock1Ptr->location.mapNum;
    runtimeShops = ModRuntimeProfile_GetShops(&runtimeCount);
    definition = FindBestShop(runtimeShops, runtimeCount, mapGroup, mapNum, martType, NULL, 0);
    if (definition == NULL)
        definition = FindBestShop(gModShopDefinitions, gModShopDefinitionCount, mapGroup, mapNum, martType, runtimeShops, runtimeCount);
    if (definition == NULL)
        return vanillaItems;

    for (i = 0; i < definition->itemCount; i++)
        sRuntimeShopItems[i] = definition->items[i];
    sRuntimeShopItems[definition->itemCount] = ITEM_NONE;
    return sRuntimeShopItems;
}
