#ifndef GUARD_MOD_ITEM_H
#define GUARD_MOD_ITEM_H

#include "global.h"
#include "../item.h"

#define MOD_ITEM_OVERRIDE_NAME             (1 << 0)
#define MOD_ITEM_OVERRIDE_DESCRIPTION      (1 << 1)
#define MOD_ITEM_OVERRIDE_PRICE            (1 << 2)
#define MOD_ITEM_OVERRIDE_HOLD_EFFECT      (1 << 3)
#define MOD_ITEM_OVERRIDE_HOLD_EFFECT_PARAM (1 << 4)
#define MOD_ITEM_OVERRIDE_IMPORTANCE       (1 << 5)
#define MOD_ITEM_OVERRIDE_POCKET           (1 << 6)
#define MOD_ITEM_OVERRIDE_TYPE             (1 << 7)
#define MOD_ITEM_OVERRIDE_FIELD_USE        (1 << 8)
#define MOD_ITEM_OVERRIDE_BATTLE_USAGE     (1 << 9)
#define MOD_ITEM_OVERRIDE_BATTLE_USE       (1 << 10)
#define MOD_ITEM_OVERRIDE_SECONDARY_ID     (1 << 11)

struct ModItemDefinition
{
    const char *key;
    const char *nameKey;
    const char *descriptionKey;
    const char *fieldUseHookKey;
    const char *battleUseHookKey;
    u16 itemId;
    u16 price;
    u16 flags;
    u8 holdEffect;
    u8 holdEffectParam;
    u8 importance;
    u8 pocket;
    u8 type;
    u8 battleUsage;
    u8 secondaryId;
    s16 priority;
    ItemUseFunc fieldUseFunc;
    ItemUseFunc battleUseFunc;
};

bool8 ItemApi_IsDefinitionValid(const struct ModItemDefinition *definition, bool8 allowEmptyDefault);
const u8 *ItemApi_GetName(u16 itemId, const u8 *vanilla);
u16 ItemApi_GetPrice(u16 itemId, u16 vanilla);
u8 ItemApi_GetHoldEffect(u16 itemId, u8 vanilla);
u8 ItemApi_GetHoldEffectParam(u16 itemId, u8 vanilla);
const u8 *ItemApi_GetDescription(u16 itemId, const u8 *vanilla);
u8 ItemApi_GetImportance(u16 itemId, u8 vanilla);
u8 ItemApi_GetPocket(u16 itemId, u8 vanilla);
u8 ItemApi_GetType(u16 itemId, u8 vanilla);
ItemUseFunc ItemApi_GetFieldFunc(u16 itemId, ItemUseFunc vanilla);
u8 ItemApi_GetBattleUsage(u16 itemId, u8 vanilla);
ItemUseFunc ItemApi_GetBattleFunc(u16 itemId, ItemUseFunc vanilla);
u8 ItemApi_GetSecondaryId(u16 itemId, u8 vanilla);
ItemUseFunc ItemApi_FindCompiledFieldUseHook(const char *sourceKey, const char *hookKey);
ItemUseFunc ItemApi_FindCompiledBattleUseHook(const char *sourceKey, const char *hookKey);

#endif // GUARD_MOD_ITEM_H
