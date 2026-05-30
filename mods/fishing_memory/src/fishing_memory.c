#include "mod_sdk.h"
#include "constants/characters.h"
#include "constants/flags.h"

#define FISHING_MEMORY_MODE_SHOW 0
#define FISHING_MEMORY_MODE_INPUT 1
#define FISHING_MEMORY_ARROW_UP 0
#define FISHING_MEMORY_ARROW_DOWN 1
#define FISHING_MEMORY_DEFAULT_SHOW_FRAMES 30
#define FISHING_MEMORY_DEFAULT_INPUT_FRAMES 90
#define FISHING_MEMORY_DEFAULT_WARNING_FRAMES 0
#define FISHING_MEMORY_TEXT_LENGTH ((MAX_LEVEL * 2) + 1)
#define FISHING_MEMORY_SCORE_TEXT_LENGTH 6

struct FishingMemoryCapStage
{
    u16 flag;
    u8 level;
};

static u8 sFishingMemorySequence[MAX_LEVEL];
static u8 sFishingMemoryMaxLevel;
static u8 sFishingMemoryScore;
static u8 sFishingMemoryLength;
static u8 sFishingMemoryInputIndex;
static u8 sFishingMemoryMode;
static u16 sFishingMemoryTimer;
static u16 sFishingMemoryShowFrames;
static u16 sFishingMemoryInputFrames;
static u16 sFishingMemoryWarningFrames;
static bool8 sFishingMemoryWarned;
static u8 sFishingMemoryText[FISHING_MEMORY_TEXT_LENGTH];
static u8 sFishingMemoryScoreText[FISHING_MEMORY_SCORE_TEXT_LENGTH];

#include "generated_cap_stages.inc"

static u8 FishingMemory_RunFrame(struct FishingActionFrameContext *context);

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

static u16 FishingMemory_ClampFrames(s16 value, u16 defaultValue)
{
    if (value <= 0)
        return defaultValue;
    if (value > 600)
        return 600;
    return value;
}

static u16 FishingMemory_SubtractFramePenalty(u16 value, u16 penalty, u16 minimum)
{
    if (penalty >= value)
        return minimum;
    value -= penalty;
    if (value < minimum)
        return minimum;
    return value;
}

static void FishingMemory_ApplyTargetLevelDifficulty(const struct FishingContext *context)
{
    u16 steps;

    if (!FISHING_MEMORY_TARGET_LEVEL_DIFFICULTY_ENABLED || context == NULL)
        return;
    if (FISHING_MEMORY_TARGET_LEVEL_DIFFICULTY_STEP == 0)
        return;

    steps = context->level / FISHING_MEMORY_TARGET_LEVEL_DIFFICULTY_STEP;
    sFishingMemoryShowFrames = FishingMemory_SubtractFramePenalty(
        sFishingMemoryShowFrames,
        steps * FISHING_MEMORY_TARGET_LEVEL_SHOW_FRAME_PENALTY,
        FISHING_MEMORY_TARGET_LEVEL_MIN_SHOW_FRAMES);
    sFishingMemoryInputFrames = FishingMemory_SubtractFramePenalty(
        sFishingMemoryInputFrames,
        steps * FISHING_MEMORY_TARGET_LEVEL_INPUT_FRAME_PENALTY,
        FISHING_MEMORY_TARGET_LEVEL_MIN_INPUT_FRAMES);
}

static u8 FishingMemory_GetConfiguredLevelCap(void)
{
    u8 i;
    u8 cap = MIN_LEVEL;

    for (i = 0; i < ARRAY_COUNT(sFishingMemoryCapStages); i++)
    {
        if (sFishingMemoryCapStages[i].flag == MOD_FLAG_INVALID
            || ModFlag_Get(sFishingMemoryCapStages[i].flag))
            cap = sFishingMemoryCapStages[i].level;
    }

    return cap;
}

static void FishingMemory_PrintSequence(void)
{
    u8 i;
    u8 *text = sFishingMemoryText;

    for (i = 0; i < sFishingMemoryLength && i < MAX_LEVEL; i++)
    {
        if (i != 0)
            *text++ = FISHING_TEXT_CHAR_SPACE;
        *text++ = sFishingMemorySequence[i] == FISHING_MEMORY_ARROW_DOWN
            ? FISHING_TEXT_CHAR_DOWN_ARROW
            : FISHING_TEXT_CHAR_UP_ARROW;
    }
    *text = FISHING_TEXT_EOS;

    FishingApi_PrintText(sFishingMemoryText);
}

static void FishingMemory_PrintInputPrompt(bool8 warning)
{
    static const u8 sPrompt[] = { FISHING_TEXT_CHAR_QUESTION_MARK, FISHING_TEXT_EOS };
    static const u8 sWarningPrompt[] = {
        FISHING_TEXT_CHAR_QUESTION_MARK,
        FISHING_TEXT_CHAR_SPACE,
        FISHING_TEXT_CHAR_QUESTION_MARK,
        FISHING_TEXT_EOS
    };

    FishingApi_PrintText(warning ? sWarningPrompt : sPrompt);
}

static void FishingMemory_PrintScore(void)
{
    u8 score = sFishingMemoryScore;

    sFishingMemoryScoreText[0] = CHAR_LV;
    sFishingMemoryScoreText[1] = FISHING_TEXT_CHAR_SPACE;
    if (score >= 100)
    {
        sFishingMemoryScoreText[2] = CHAR_1;
        sFishingMemoryScoreText[3] = CHAR_0;
        sFishingMemoryScoreText[4] = CHAR_0;
        sFishingMemoryScoreText[5] = FISHING_TEXT_EOS;
    }
    else
    {
        sFishingMemoryScoreText[2] = (score / 10) + CHAR_0;
        sFishingMemoryScoreText[3] = (score % 10) + CHAR_0;
        sFishingMemoryScoreText[4] = FISHING_TEXT_EOS;
    }

    FishingApi_PrintText(sFishingMemoryScoreText);
}

static void FishingMemory_StartNextRound(void)
{
    if (sFishingMemoryLength < sFishingMemoryMaxLevel)
    {
        sFishingMemorySequence[sFishingMemoryLength] = ModApi_Random() & 1;
        sFishingMemoryLength++;
    }

    sFishingMemoryInputIndex = 0;
    sFishingMemoryTimer = 0;
    sFishingMemoryWarned = FALSE;
    sFishingMemoryMode = FISHING_MEMORY_MODE_SHOW;
    FishingMemory_PrintSequence();
}

static void FishingMemory_StartGame(const struct FishingActionRequest *request, const struct FishingContext *context)
{
    s16 maxLevel;
    u8 configuredLevelCap;

    maxLevel = request->params[0];
    if (maxLevel <= 0 || maxLevel > MAX_LEVEL)
        sFishingMemoryMaxLevel = MAX_LEVEL;
    else if (maxLevel < MIN_LEVEL)
        sFishingMemoryMaxLevel = MIN_LEVEL;
    else
        sFishingMemoryMaxLevel = maxLevel;

    configuredLevelCap = FishingMemory_GetConfiguredLevelCap();
    if (sFishingMemoryMaxLevel > configuredLevelCap)
        sFishingMemoryMaxLevel = configuredLevelCap;

    sFishingMemoryShowFrames = FishingMemory_ClampFrames(request->params[1], FISHING_MEMORY_DEFAULT_SHOW_FRAMES);
    if (request->params[2] > 0)
        sFishingMemoryInputFrames = FishingMemory_ClampFrames(request->params[2], FISHING_MEMORY_DEFAULT_INPUT_FRAMES);
    else
        sFishingMemoryInputFrames = FishingMemory_ClampFrames(request->timeout_frames, FISHING_MEMORY_DEFAULT_INPUT_FRAMES);
    sFishingMemoryWarningFrames = FishingMemory_ClampFrames(request->params[3], FISHING_MEMORY_DEFAULT_WARNING_FRAMES);
    if (sFishingMemoryWarningFrames >= sFishingMemoryInputFrames)
        sFishingMemoryWarningFrames = 0;
    FishingMemory_ApplyTargetLevelDifficulty(context);

    sFishingMemoryScore = 0;
    sFishingMemoryLength = 0;
    sFishingMemoryInputIndex = 0;
    sFishingMemoryTimer = 0;
    sFishingMemoryWarned = FALSE;
    sFishingMemoryMode = FISHING_MEMORY_MODE_SHOW;
    FishingMemory_StartNextRound();
}

static u8 FishingMemory_FinishWithScore(void)
{
    u8 encounterLevel;

    if (sFishingMemoryScore == 0)
        return FISHING_OUTCOME_GOT_AWAY;

    encounterLevel = sFishingMemoryScore;
    if (encounterLevel < FISHING_MEMORY_SAFE_MIN_LEVEL)
        encounterLevel = FISHING_MEMORY_SAFE_MIN_LEVEL;
    FishingApi_SetNextEncounterLevel(encounterLevel);
    return FISHING_OUTCOME_ON_HOOK;
}

static u8 FishingMemory_RunFrame(struct FishingActionFrameContext *context)
{
    u16 pressed;
    u16 expected;

    if (context == NULL)
        return FISHING_OUTCOME_GOT_AWAY;

    if (sFishingMemoryMode == FISHING_MEMORY_MODE_SHOW)
    {
        sFishingMemoryTimer++;
        if (sFishingMemoryTimer >= sFishingMemoryShowFrames)
        {
            sFishingMemoryTimer = 0;
            sFishingMemoryInputIndex = 0;
            sFishingMemoryWarned = FALSE;
            sFishingMemoryMode = FISHING_MEMORY_MODE_INPUT;
            FishingMemory_PrintInputPrompt(FALSE);
        }
        return FISHING_OUTCOME_CONTINUE;
    }

    pressed = context->new_keys & (DPAD_UP | DPAD_DOWN);
    if (pressed != 0)
    {
        expected = sFishingMemorySequence[sFishingMemoryInputIndex] == FISHING_MEMORY_ARROW_DOWN ? DPAD_DOWN : DPAD_UP;
        if (pressed == expected)
        {
            sFishingMemoryScore++;
            if (sFishingMemoryScore >= sFishingMemoryMaxLevel)
                return FishingMemory_FinishWithScore();

            sFishingMemoryInputIndex++;
            sFishingMemoryTimer = 0;
            sFishingMemoryWarned = FALSE;
            if (sFishingMemoryInputIndex >= sFishingMemoryLength)
                FishingMemory_StartNextRound();
            else
                FishingMemory_PrintScore();
            return FISHING_OUTCOME_CONTINUE;
        }

        return FishingMemory_FinishWithScore();
    }

    if (sFishingMemoryInputFrames != 0)
    {
        sFishingMemoryTimer++;
        if (!sFishingMemoryWarned
            && sFishingMemoryWarningFrames != 0
            && sFishingMemoryTimer + sFishingMemoryWarningFrames >= sFishingMemoryInputFrames)
        {
            sFishingMemoryWarned = TRUE;
            FishingMemory_PrintInputPrompt(TRUE);
        }
        if (sFishingMemoryTimer >= sFishingMemoryInputFrames)
            return FishingMemory_FinishWithScore();
    }

    return FISHING_OUTCOME_CONTINUE;
}

u8 FishingMemory_Start(const struct FishingActionDefinition *definition, struct FishingContext *context, struct FishingActionRequest *request)
{
    if (!FishingMemory_ShouldStart(definition, context, request))
        return FISHING_ACTION_CONTINUE;

    FishingApi_InitRequestFromDefinition(definition, request);
    FishingMemory_StartGame(request, context);
    request->frame_hook = FishingMemory_RunFrame;
    return FISHING_ACTION_REQUEST_ACTION;
}
