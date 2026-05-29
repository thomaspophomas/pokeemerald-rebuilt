#include "mod_sdk.h"

static bool8 FishingMemory_ShouldStart(
    const struct FishingActionDefinition *definition,
    const struct FishingContext *context,
    const struct FishingActionRequest *request)
{
    return definition != NULL
        && context != NULL
        && request != NULL
        && context->phase == FISHING_PHASE_INPUT_WINDOW;
}

u8 FishingMemory_Start(const struct FishingActionDefinition *definition, struct FishingContext *context, struct FishingActionRequest *request)
{
    if (!FishingMemory_ShouldStart(definition, context, request))
        return FISHING_ACTION_CONTINUE;

    FishingApi_InitRequestFromDefinition(definition, request);
    return FISHING_ACTION_MEMORY_GAME;
}
