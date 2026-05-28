#ifndef GUARD_MOD_ENGINE_H
#define GUARD_MOD_ENGINE_H

#include "mod/base.h"

#define ENGINE_RULESET_FLAG_SAVE_COMPATIBLE (1 << 0)
#define ENGINE_RULESET_DEFAULT_ID "engine:gen3"

struct EngineRuleset;

typedef u8 (*EngineRulesetCaptureFunc)(const struct EngineRuleset *ruleset, u16 ball_item_id);
typedef u32 (*EngineRulesetBattleWeatherFunc)(const struct EngineRuleset *ruleset, u16 weather_layers);

struct EngineRuleset
{
    const char *ruleset_id;
    const char *name;
    u16 version;
    u32 flags;
    EngineRulesetCaptureFunc get_capture_modifier;
    EngineRulesetBattleWeatherFunc get_battle_weather_mask;
};

void EngineApi_Init(void);
const struct EngineRuleset *EngineApi_GetActiveRuleset(void);
bool8 EngineApi_SetActiveRuleset(const char *ruleset_id);
const struct EngineRuleset *EngineApi_FindRuleset(const char *ruleset_id);
u16 EngineApi_GetRulesetCount(void);

#endif // GUARD_MOD_ENGINE_H
