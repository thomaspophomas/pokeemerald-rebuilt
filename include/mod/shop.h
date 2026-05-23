#ifndef GUARD_MOD_SHOP_H
#define GUARD_MOD_SHOP_H

#include "global.h"

#define MOD_SHOP_MAX_ITEMS 32

#define MOD_SHOP_TYPE_NORMAL 0
#define MOD_SHOP_TYPE_DECOR  1
#define MOD_SHOP_TYPE_DECOR2 2

struct ModShopDefinition
{
    const char *key;
    u8 mapGroup;
    u8 mapNum;
    u8 martType;
    u8 itemCount;
    s16 priority;
    u16 flags;
    u16 items[MOD_SHOP_MAX_ITEMS];
};

bool8 ShopApi_IsDefinitionValid(const struct ModShopDefinition *definition, bool8 allowEmptyDefault);
const u16 *ShopApi_GetItemsForSale(u8 martType, const u16 *vanillaItems);

#endif // GUARD_MOD_SHOP_H
