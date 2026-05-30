#ifndef GUARD_MOD_LEVEL_CAP_H
#define GUARD_MOD_LEVEL_CAP_H

#include "mod/flags.h"

#define MOD_LEVEL_CAP_MAX_STAGES 16
#define MOD_LEVEL_CAP_FLAG_ALWAYS MOD_FLAG_INVALID
#define MOD_LEVEL_CAP_EXP_DELTA_MIN (-99)
#define MOD_LEVEL_CAP_EXP_DELTA_MAX 99
#define MOD_LEVEL_CAP_EXP_DELTA_COUNT (MOD_LEVEL_CAP_EXP_DELTA_MAX - MOD_LEVEL_CAP_EXP_DELTA_MIN + 1)

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

struct ModLevelCapStage
{
    u16 unlock_flag;
    u8 level;
    u8 reserved;
};

struct Pokemon;

struct ModLevelCapDefinition
{
    const char *key;
    u8 mode;
    u8 stage_count;
    u8 rare_candy_policy;
    u8 reserved;
    s16 priority;
    u16 flags;
    struct ModLevelCapStage stages[MOD_LEVEL_CAP_MAX_STAGES];
    u8 soft_exp_curve[MOD_LEVEL_CAP_EXP_DELTA_COUNT];
};

bool8 LevelCapApi_IsDefinitionValid(const struct ModLevelCapDefinition *definition, bool8 allow_empty_default);
u8 LevelCapApi_GetActiveCap(void);
s16 LevelCapApi_ModifyBattleExp(struct Pokemon *mon, s16 gained_exp);
bool8 LevelCapApi_CanUseRareCandy(struct Pokemon *mon);

#endif // GUARD_MOD_LEVEL_CAP_H
