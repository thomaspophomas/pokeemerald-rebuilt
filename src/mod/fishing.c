#include "global.h"
#include "generated/mod_registry.h"
#include "mod/fishing.h"
#include "mod/runtime_profile.h"

static EWRAM_DATA u8 sFishingAttemptRod = 0;
static EWRAM_DATA bool8 sFishingAttemptActive = FALSE;

static u8 RodToMask(u8 rod)
{
    if (rod > SUPER_ROD)
        return 0;
    return 1 << rod;
}

static bool8 DefinitionIsEmptyDefault(const struct FishingActionDefinition *definition)
{
    return definition->key == NULL
        && definition->hookKey == NULL
        && definition->rodMask == 0
        && definition->phaseMask == 0
        && definition->priority == 0
        && definition->flags == 0
        && definition->hook == NULL
        && definition->promptKey == NULL
        && definition->buttonMask == 0
        && definition->timeoutFrames == 0
        && definition->successOutcome == 0
        && definition->failureOutcome == 0;
}

static bool8 OutcomeIsValid(u8 outcome)
{
    return outcome <= FISHING_OUTCOME_CANCEL;
}

static bool8 PhaseMaskIsValid(u16 phaseMask)
{
    return phaseMask != 0 && (phaseMask & ~FISHING_PHASE_MASK_ALL) == 0;
}

static bool8 DefinitionMatchesContext(const struct FishingActionDefinition *definition, const struct FishingContext *context)
{
    if (definition == NULL || context == NULL)
        return FALSE;
    if ((definition->rodMask & RodToMask(context->rod)) == 0)
        return FALSE;
    if ((definition->phaseMask & FISHING_PHASE_MASK(context->phase)) == 0)
        return FALSE;
    return TRUE;
}

static bool8 RuntimeActionKeyExists(const char *key, const struct FishingActionDefinition *actions, u16 count)
{
    u16 i;

    if (key == NULL || actions == NULL)
        return FALSE;

    for (i = 0; i < count; i++)
    {
        if (actions[i].key != NULL && strcmp(actions[i].key, key) == 0)
            return TRUE;
    }

    return FALSE;
}

static u8 RunActions(
    const struct FishingActionDefinition *actions,
    u16 count,
    struct FishingContext *context,
    struct FishingActionRequest *request,
    const struct FishingActionDefinition *shadowingActions,
    u16 shadowingActionCount)
{
    u16 i;
    u8 result;

    if (actions == NULL)
        return FISHING_ACTION_CONTINUE;

    for (i = 0; i < count; i++)
    {
        if (!DefinitionMatchesContext(&actions[i], context))
            continue;
        if (actions[i].hook == NULL)
            continue;
        if (RuntimeActionKeyExists(actions[i].key, shadowingActions, shadowingActionCount))
            continue;

        result = actions[i].hook(&actions[i], context, request);
        switch (result)
        {
        case FISHING_ACTION_CONTINUE:
            break;
        case FISHING_ACTION_OVERRIDE:
        case FISHING_ACTION_REQUEST_ACTION:
        case FISHING_ACTION_CANCEL:
            return result;
        default:
            return FISHING_ACTION_CONTINUE;
        }
    }

    return FISHING_ACTION_CONTINUE;
}

void FishingApi_BeginAttempt(u8 rod)
{
    sFishingAttemptRod = rod;
    sFishingAttemptActive = TRUE;
}

void FishingApi_EndAttempt(u8 outcome)
{
    (void)outcome;
    sFishingAttemptActive = FALSE;
}

bool8 FishingApi_HasActions(void)
{
    u16 runtimeCount = 0;

    if (gModFishingActionCount != 0)
        return TRUE;
    return ModRuntimeProfile_GetFishingActions(&runtimeCount) != NULL && runtimeCount != 0;
}

bool8 FishingApi_IsDefinitionValid(const struct FishingActionDefinition *definition, bool8 allowEmptyDefault)
{
    if (definition == NULL)
        return FALSE;
    if (allowEmptyDefault && DefinitionIsEmptyDefault(definition))
        return TRUE;
    if (definition->key == NULL || definition->key[0] == '\0')
        return FALSE;
    if (definition->hookKey == NULL || definition->hookKey[0] == '\0')
        return FALSE;
    if (definition->hook == NULL)
        return FALSE;
    if (definition->rodMask == 0 || (definition->rodMask & ~FISHING_ACTION_ROD_ALL) != 0)
        return FALSE;
    if (!PhaseMaskIsValid(definition->phaseMask))
        return FALSE;
    if (!OutcomeIsValid(definition->successOutcome) || !OutcomeIsValid(definition->failureOutcome))
        return FALSE;
    return TRUE;
}

void FishingApi_InitRequestFromDefinition(const struct FishingActionDefinition *definition, struct FishingActionRequest *request)
{
    if (request == NULL)
        return;

    memset(request, 0, sizeof(*request));
    request->successOutcome = FISHING_OUTCOME_CONTINUE;
    request->failureOutcome = FISHING_OUTCOME_GOT_AWAY;

    if (definition == NULL)
        return;

    request->promptKey = definition->promptKey;
    request->requiredButtons = definition->buttonMask;
    request->timeoutFrames = definition->timeoutFrames;
    request->successOutcome = definition->successOutcome;
    request->failureOutcome = definition->failureOutcome;
}

u8 FishingApi_RunPhase(struct FishingContext *context, struct FishingActionRequest *request)
{
    const struct FishingActionDefinition *runtimeActions;
    u16 runtimeCount = 0;
    u8 result;

    if (context == NULL || request == NULL)
        return FISHING_ACTION_CONTINUE;
    if (!sFishingAttemptActive || context->rod != sFishingAttemptRod)
        return FISHING_ACTION_CONTINUE;
    if (context->phase >= FISHING_PHASE_COUNT)
        return FISHING_ACTION_CONTINUE;

    FishingApi_InitRequestFromDefinition(NULL, request);
    runtimeActions = ModRuntimeProfile_GetFishingActions(&runtimeCount);

    result = RunActions(gModFishingActions, gModFishingActionCount, context, request, runtimeActions, runtimeCount);
    if (result != FISHING_ACTION_CONTINUE)
        return result;

    return RunActions(runtimeActions, runtimeCount, context, request, NULL, 0);
}

FishingActionHook FishingApi_FindCompiledHook(const char *sourceKey, const char *hookKey)
{
    u16 i;

    if (sourceKey == NULL || hookKey == NULL || sourceKey[0] == '\0' || hookKey[0] == '\0')
        return NULL;

    for (i = 0; i < gModFishingActionCount; i++)
    {
        if (gModFishingActions[i].key == NULL || gModFishingActions[i].hookKey == NULL)
            continue;
        if (strcmp(gModFishingActions[i].key, sourceKey) == 0
         && strcmp(gModFishingActions[i].hookKey, hookKey) == 0)
            return gModFishingActions[i].hook;
    }

    return NULL;
}

u8 FishingApi_RequestConfiguredAction(const struct FishingActionDefinition *definition, struct FishingContext *context, struct FishingActionRequest *request)
{
    (void)context;

    if (definition == NULL || request == NULL || definition->buttonMask == 0)
        return FISHING_ACTION_CONTINUE;

    FishingApi_InitRequestFromDefinition(definition, request);
    return FISHING_ACTION_REQUEST_ACTION;
}
