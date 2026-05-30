#ifndef GUARD_MOD_LEVEL_CAP_H
#define GUARD_MOD_LEVEL_CAP_H

#include "mod/base.h"

#define MOD_LEVEL_CAP_BADGE_SLOTS 9

enum ModLevelCapMode
{
    MOD_LEVEL_CAP_MODE_NONE,
    MOD_LEVEL_CAP_MODE_SOFT,
    MOD_LEVEL_CAP_MODE_HARD,
};

enum ModLevelCapRareCandyPolicy
{
    MOD_LEVEL_CAP_RARE_CANDY_ALLOW,
    MOD_LEVEL_CAP_RARE_CANDY_BLOCK_AT_CAP,
};

struct Pokemon;

struct ModLevelCapDefinition
{
    const char *key;
    u8 mode;
    u8 soft_exp_percent;
    u8 rare_candy_policy;
    u8 reserved;
    s16 priority;
    u16 flags;
    u8 caps[MOD_LEVEL_CAP_BADGE_SLOTS];
};

bool8 LevelCapApi_IsDefinitionValid(const struct ModLevelCapDefinition *definition, bool8 allow_empty_default);
u8 LevelCapApi_GetActiveCap(void);
s16 LevelCapApi_ModifyBattleExp(struct Pokemon *mon, s16 gained_exp);
bool8 LevelCapApi_CanUseRareCandy(struct Pokemon *mon);

#endif // GUARD_MOD_LEVEL_CAP_H
