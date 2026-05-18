#ifndef GUARD_MOD_BADGE_H
#define GUARD_MOD_BADGE_H

#include "global.h"

#define MOD_BADGE_LEVEL_MAX 10

enum ModBadgeId
{
    MOD_BADGE_STONE,
    MOD_BADGE_KNUCKLE,
    MOD_BADGE_DYNAMO,
    MOD_BADGE_HEAT,
    MOD_BADGE_BALANCE,
    MOD_BADGE_FEATHER,
    MOD_BADGE_MIND,
    MOD_BADGE_RAIN,
    MOD_BADGE_COUNT,
};

enum ModBadgeEffectType
{
    MOD_BADGE_EFFECT_TYPE_NONE,
    MOD_BADGE_EFFECT_TYPE_RESISTANCE_PERCENT,
    MOD_BADGE_EFFECT_TYPE_DAMAGE_PERCENT,
    MOD_BADGE_EFFECT_TYPE_STAT_PERCENT,
};

struct ModBadgeEffectDefinition
{
    const char *key;
    u8 badgeId;
    u8 effectKind;
    u8 target;
    s8 percentPerLevel;
    u8 maxLevel;
    u16 flags;
};

void BadgeApi_Init(void);
u8 BadgeApi_GetLevel(u8 badgeId);
// Setting a level to 0 clears only the modded stored level. A vanilla badge
// flag still counts as minimum level 1.
bool8 BadgeApi_SetLevel(u8 badgeId, u8 level);
bool8 BadgeApi_IncrementLevel(u8 badgeId, u8 amount);
s16 BadgeApi_GetEffectPercent(u8 effectKind, u8 target, u8 battlerId);
bool8 BadgeApi_IsEffectDefinitionValid(const struct ModBadgeEffectDefinition *definition, bool8 allowEmptyNone);

#endif // GUARD_MOD_BADGE_H
