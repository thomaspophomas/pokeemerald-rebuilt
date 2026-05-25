#include "global.h"
#include "generated/mod_registry.h"
#include "mod/engine.h"
#include "mod/runtime_profile.h"

static EWRAM_DATA u16 sActiveRuleset = 0;

static bool8 StringEquals(const char *left, const char *right)
{
    if (left == NULL || right == NULL)
        return FALSE;
    return strcmp(left, right) == 0;
}

void EngineApi_Init(void)
{
    const struct EngineRuleset *ruleset;

    sActiveRuleset = 0;
    ruleset = EngineApi_FindRuleset(ENGINE_RULESET_DEFAULT_ID);
    if (ruleset != NULL)
        sActiveRuleset = ruleset - gModEngineRulesets;
}

const struct EngineRuleset *EngineApi_GetActiveRuleset(void)
{
    const char *runtimeRulesetId;
    const struct EngineRuleset *runtimeRuleset;

    runtimeRulesetId = ModRuntimeProfile_GetEngineRulesetId();
    runtimeRuleset = EngineApi_FindRuleset(runtimeRulesetId);
    if (runtimeRuleset != NULL)
        return runtimeRuleset;

    if (sActiveRuleset >= gModEngineRulesetCount)
        sActiveRuleset = 0;

    return &gModEngineRulesets[sActiveRuleset];
}

bool8 EngineApi_SetActiveRuleset(const char *ruleset_id)
{
    const struct EngineRuleset *ruleset;

    ruleset = EngineApi_FindRuleset(ruleset_id);
    if (ruleset == NULL)
        return FALSE;
    if ((ruleset->flags & ENGINE_RULESET_FLAG_SAVE_COMPATIBLE) == 0)
        return FALSE;

    sActiveRuleset = ruleset - gModEngineRulesets;
    return TRUE;
}

const struct EngineRuleset *EngineApi_FindRuleset(const char *ruleset_id)
{
    u16 ruleset_index;

    for (ruleset_index = 0; ruleset_index < gModEngineRulesetCount; ruleset_index++)
    {
        if (StringEquals(gModEngineRulesets[ruleset_index].ruleset_id, ruleset_id))
            return &gModEngineRulesets[ruleset_index];
    }

    return NULL;
}

u16 EngineApi_GetRulesetCount(void)
{
    return gModEngineRulesetCount;
}
