#ifndef GUARD_MOD_REWARD_H
#define GUARD_MOD_REWARD_H

#include "global.h"

#define MOD_REWARD_SOURCE_PICKUP_COMMON 1
#define MOD_REWARD_SOURCE_PICKUP_RARE   2
#define MOD_REWARD_SOURCE_PYRAMID_PICKUP 3

#define MOD_REWARD_FLAG_KEEP_VANILLA_QUANTITY (1 << 0)

struct ModRewardContext
{
    u8 source;
    u8 level;
    u8 map_group;
    u8 map_num;
    u16 vanilla_item_id;
    u16 item_id;
    u16 quantity;
};

struct ModRewardDefinition;
typedef u8 (*ModRewardHook)(const struct ModRewardDefinition *definition, struct ModRewardContext *context);

enum ModRewardHookResult
{
    MOD_REWARD_CONTINUE,
    MOD_REWARD_OVERRIDE,
    MOD_REWARD_CANCEL,
};

struct ModRewardDefinition
{
    const char *key;
    const char *hook_key;
    u8 source;
    u8 min_level;
    u8 max_level;
    s16 priority;
    u16 flags;
    u16 item_id;
    u16 quantity;
    ModRewardHook hook;
};

bool8 RewardApi_IsDefinitionValid(const struct ModRewardDefinition *definition, bool8 allow_empty_default);
bool8 RewardApi_AdjustItemReward(u8 source, u8 level, u16 vanilla_item_id, u16 *item_id, u16 *quantity);
ModRewardHook RewardApi_FindCompiledHook(const char *source_key, const char *hook_key);

#endif // GUARD_MOD_REWARD_H
