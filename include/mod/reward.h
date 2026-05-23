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
    u8 mapGroup;
    u8 mapNum;
    u16 vanillaItemId;
    u16 itemId;
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
    const char *hookKey;
    u8 source;
    u8 minLevel;
    u8 maxLevel;
    s16 priority;
    u16 flags;
    u16 itemId;
    u16 quantity;
    ModRewardHook hook;
};

bool8 RewardApi_IsDefinitionValid(const struct ModRewardDefinition *definition, bool8 allowEmptyDefault);
bool8 RewardApi_AdjustItemReward(u8 source, u8 level, u16 vanillaItemId, u16 *itemId, u16 *quantity);
ModRewardHook RewardApi_FindCompiledHook(const char *sourceKey, const char *hookKey);

#endif // GUARD_MOD_REWARD_H
