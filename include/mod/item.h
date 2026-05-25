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
    const char *name_key;
    const char *description_key;
    const char *field_use_hook_key;
    const char *battle_use_hook_key;
    u16 item_id;
    u16 price;
    u16 flags;
    u8 hold_effect;
    u8 hold_effect_param;
    u8 importance;
    u8 pocket;
    u8 type;
    u8 battle_usage;
    u8 secondary_id;
    s16 priority;
    ItemUseFunc field_use_func;
    ItemUseFunc battle_use_func;
};

bool8 ItemApi_IsDefinitionValid(const struct ModItemDefinition *definition, bool8 allow_empty_default);
const u8 *ItemApi_GetName(u16 item_id, const u8 *vanilla_name);
u16 ItemApi_GetPrice(u16 item_id, u16 vanilla_price);
u8 ItemApi_GetHoldEffect(u16 item_id, u8 vanilla_hold_effect);
u8 ItemApi_GetHoldEffectParam(u16 item_id, u8 vanilla_hold_effect_param);
const u8 *ItemApi_GetDescription(u16 item_id, const u8 *vanilla_description);
u8 ItemApi_GetImportance(u16 item_id, u8 vanilla_importance);
u8 ItemApi_GetPocket(u16 item_id, u8 vanilla_pocket);
u8 ItemApi_GetType(u16 item_id, u8 vanilla_type);
ItemUseFunc ItemApi_GetFieldFunc(u16 item_id, ItemUseFunc vanilla_field_func);
u8 ItemApi_GetBattleUsage(u16 item_id, u8 vanilla_battle_usage);
ItemUseFunc ItemApi_GetBattleFunc(u16 item_id, ItemUseFunc vanilla_battle_func);
u8 ItemApi_GetSecondaryId(u16 item_id, u8 vanilla_secondary_id);
ItemUseFunc ItemApi_FindCompiledFieldUseHook(const char *source_key, const char *hook_key);
ItemUseFunc ItemApi_FindCompiledBattleUseHook(const char *source_key, const char *hook_key);

#endif // GUARD_MOD_ITEM_H
