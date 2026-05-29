#include "mod_sdk.h"

u8 FishingMemory_Start(const struct FishingActionDefinition *definition, struct FishingContext *context, struct FishingActionRequest *request)
{
    if (definition == NULL || context == NULL || request == NULL)
        return FISHING_ACTION_CONTINUE;
    if (context->phase != FISHING_PHASE_INPUT_WINDOW)
        return FISHING_ACTION_CONTINUE;

    FishingApi_InitRequestFromDefinition(definition, request);
    return FISHING_ACTION_MEMORY_GAME;
}
