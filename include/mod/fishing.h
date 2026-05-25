#ifndef GUARD_MOD_FISHING_H
#define GUARD_MOD_FISHING_H

#include "global.h"
#include "constants/items.h"

#define FISHING_ACTION_PARAM_COUNT 4

#define FISHING_ACTION_ROD_OLD   (1 << OLD_ROD)
#define FISHING_ACTION_ROD_GOOD  (1 << GOOD_ROD)
#define FISHING_ACTION_ROD_SUPER (1 << SUPER_ROD)
#define FISHING_ACTION_ROD_ALL   (FISHING_ACTION_ROD_OLD | FISHING_ACTION_ROD_GOOD | FISHING_ACTION_ROD_SUPER)

enum FishingPhase
{
    FISHING_PHASE_START,
    FISHING_PHASE_ROUND_START,
    FISHING_PHASE_DOT_CONFIG,
    FISHING_PHASE_BITE_CHECK,
    FISHING_PHASE_INPUT_WINDOW,
    FISHING_PHASE_MORE_DOTS_CHECK,
    FISHING_PHASE_BEFORE_ENCOUNTER,
    FISHING_PHASE_END,
    FISHING_PHASE_COUNT,
};

#define FISHING_PHASE_MASK(phase) (1 << (phase))
#define FISHING_PHASE_MASK_ALL ((1 << FISHING_PHASE_COUNT) - 1)

enum FishingOutcome
{
    FISHING_OUTCOME_CONTINUE,
    FISHING_OUTCOME_NO_BITE,
    FISHING_OUTCOME_GOT_AWAY,
    FISHING_OUTCOME_START_ROUND,
    FISHING_OUTCOME_ON_HOOK,
    FISHING_OUTCOME_START_ENCOUNTER,
    FISHING_OUTCOME_CANCEL,
};

enum FishingActionHookResult
{
    FISHING_ACTION_CONTINUE,
    FISHING_ACTION_OVERRIDE,
    FISHING_ACTION_REQUEST_ACTION,
    FISHING_ACTION_CANCEL,
};

struct FishingContext
{
    u8 rod;
    u8 phase;
    u8 round;
    u8 dotsRequired;
    u8 dotsShown;
    u8 minRoundsRequired;
    u16 frame;
    bool8 bite;
    u8 currentOutcome;
    u8 mapGroup;
    u8 mapNum;
    s16 playerX;
    s16 playerY;
    u16 species;
    u8 level;
};

struct FishingActionRequest
{
    const char *promptKey;
    u16 requiredButtons;
    u16 timeoutFrames;
    u8 successOutcome;
    u8 failureOutcome;
};

struct FishingActionDefinition;
typedef u8 (*FishingActionHook)(const struct FishingActionDefinition *definition, struct FishingContext *context, struct FishingActionRequest *request);

struct FishingActionDefinition
{
    const char *key;
    const char *hookKey;
    u8 rodMask;
    u16 phaseMask;
    s16 priority;
    u16 flags;
    FishingActionHook hook;
    const char *promptKey;
    u16 buttonMask;
    u16 timeoutFrames;
    u8 successOutcome;
    u8 failureOutcome;
    s16 params[FISHING_ACTION_PARAM_COUNT];
};

void FishingApi_BeginAttempt(u8 rod);
void FishingApi_EndAttempt(u8 outcome);
bool8 FishingApi_HasActions(void);
bool8 FishingApi_IsDefinitionValid(const struct FishingActionDefinition *definition, bool8 allow_empty_default);
void FishingApi_InitRequestFromDefinition(const struct FishingActionDefinition *definition, struct FishingActionRequest *request);
u8 FishingApi_RunPhase(struct FishingContext *context, struct FishingActionRequest *request);
FishingActionHook FishingApi_FindCompiledHook(const char *source_key, const char *hook_key);
u8 FishingApi_RequestConfiguredAction(const struct FishingActionDefinition *definition, struct FishingContext *context, struct FishingActionRequest *request);

#endif // GUARD_MOD_FISHING_H
