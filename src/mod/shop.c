#include "global.h"
#include "constants/items.h"
#include "generated/mod_registry.h"
#include "mod/runtime_profile.h"
#include "mod/shop.h"
#include "string_util.h"

static EWRAM_DATA u16 sRuntimeShopItems[MOD_SHOP_MAX_ITEMS + 1];

static bool8 DefinitionIsEmptyDefault(const struct ModShopDefinition *definition)
{
    u8 shop_item_index;

    if (definition == NULL)
        return FALSE;
    if (definition->key != NULL)
        return FALSE;
    if (definition->map_group != 0 || definition->map_num != 0 || definition->mart_type != 0 || definition->item_count != 0)
        return FALSE;
    if (definition->priority != 0 || definition->flags != 0)
        return FALSE;
    for (shop_item_index = 0; shop_item_index < MOD_SHOP_MAX_ITEMS; shop_item_index++)
    {
        if (definition->items[shop_item_index] != ITEM_NONE)
            return FALSE;
    }
    return TRUE;
}

static bool8 RuntimeShopKeyExists(const char *key, const struct ModShopDefinition *shops, u16 count)
{
    u16 shop_index;

    if (key == NULL)
        return FALSE;

    for (shop_index = 0; shop_index < count; shop_index++)
    {
        if (shops[shop_index].key != NULL && strcmp(shops[shop_index].key, key) == 0)
            return TRUE;
    }

    return FALSE;
}

static const struct ModShopDefinition *FindBestShop(
    const struct ModShopDefinition *shops,
    u16 count,
    u8 map_group,
    u8 map_num,
    u8 mart_type,
    const struct ModShopDefinition *shadowingShops,
    u16 shadowingCount)
{
    const struct ModShopDefinition *best_definition = NULL;
    u16 shop_index;

    if (shops == NULL)
        return NULL;

    for (shop_index = 0; shop_index < count; shop_index++)
    {
        const struct ModShopDefinition *definition = &shops[shop_index];

        if (shadowingShops != NULL && RuntimeShopKeyExists(definition->key, shadowingShops, shadowingCount))
            continue;
        if (!ShopApi_IsDefinitionValid(definition, TRUE) || DefinitionIsEmptyDefault(definition))
            continue;
        if (definition->map_group != map_group || definition->map_num != map_num || definition->mart_type != mart_type)
            continue;
        if (best_definition == NULL || definition->priority < best_definition->priority)
            best_definition = definition;
    }

    return best_definition;
}

bool8 ShopApi_IsDefinitionValid(const struct ModShopDefinition *definition, bool8 allow_empty_default)
{
    u8 shop_item_index;

    if (definition == NULL)
        return FALSE;
    if (allow_empty_default && DefinitionIsEmptyDefault(definition))
        return TRUE;
    if (definition->key == NULL || definition->key[0] == '\0')
        return FALSE;
    if (definition->mart_type > MOD_SHOP_TYPE_DECOR2)
        return FALSE;
    if (definition->item_count == 0 || definition->item_count > MOD_SHOP_MAX_ITEMS)
        return FALSE;

    for (shop_item_index = 0; shop_item_index < definition->item_count; shop_item_index++)
    {
        if (definition->items[shop_item_index] == ITEM_NONE || definition->items[shop_item_index] >= ITEMS_COUNT)
            return FALSE;
    }

    return TRUE;
}

const u16 *ShopApi_GetItemsForSale(u8 mart_type, const u16 *vanilla_items)
{
    const struct ModShopDefinition *runtimeShops;
    const struct ModShopDefinition *definition;
    u16 runtimeCount;
    u8 map_group;
    u8 map_num;
    u8 shop_item_index;

    if (gSaveBlock1Ptr == NULL)
        return vanilla_items;

    map_group = gSaveBlock1Ptr->location.mapGroup;
    map_num = gSaveBlock1Ptr->location.mapNum;
    runtimeShops = ModRuntimeProfile_GetShops(&runtimeCount);
    definition = FindBestShop(runtimeShops, runtimeCount, map_group, map_num, mart_type, NULL, 0);
    if (definition == NULL)
        definition = FindBestShop(gModShopDefinitions, gModShopDefinitionCount, map_group, map_num, mart_type, runtimeShops, runtimeCount);
    if (definition == NULL)
        return vanilla_items;

    for (shop_item_index = 0; shop_item_index < definition->item_count; shop_item_index++)
        sRuntimeShopItems[shop_item_index] = definition->items[shop_item_index];
    sRuntimeShopItems[definition->item_count] = ITEM_NONE;
    return sRuntimeShopItems;
}
