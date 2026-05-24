#ifndef GUARD_MOD_POKEBALL_H
#define GUARD_MOD_POKEBALL_H

#include "global.h"

#define POKEBALL_CATCH_MODIFIER_DEFAULT 10
#define POKEBALL_CATCH_MODIFIER_MASTER  255

struct PokeBallCatchContext
{
    u16 itemId;
    u16 targetSpecies;
    u8 targetLevel;
    u8 targetType1;
    u8 targetType2;
    u8 catchRate;
    u16 targetMaxHp;
    u16 targetHp;
    u32 targetStatus1;
    u16 battleTurnCounter;
    u8 safariCatchFactor;
    bool8 alreadyCaught;
    u8 mapType;
    u32 battleTypeFlags;
};

struct PokeBallThrowResult
{
    bool8 caught;
    u8 shakes;
    u8 catchRate;
    u8 ballMultiplier;
    u32 odds;
    bool8 usedMasterBall;
    bool8 recordCatchAttempt;
};

typedef u8 (*PokeBallCatchModifierFunc)(const struct PokeBallCatchContext *context);
typedef void (*PokeBallCatchCommitFunc)(const struct PokeBallCatchContext *context, const struct PokeBallThrowResult *result);

struct ModPokeBallDefinition
{
    const char *key;
    u16 itemId;
    u8 ballId;
    u8 baseCatchModifier;
    u16 flags;
    PokeBallCatchModifierFunc catchModifier;
    const u8 *battleScript;
    PokeBallCatchCommitFunc commitHook;
};

bool8 PokeBallApi_IsBall(u16 itemId);
const struct ModPokeBallDefinition *PokeBallApi_GetDefinition(u16 itemId);
u8 PokeBallApi_GetCatchModifier(const struct PokeBallCatchContext *context);
bool8 PokeBallApi_CalculateThrowResult(const struct PokeBallCatchContext *context, struct PokeBallThrowResult *result);
const u8 *PokeBallApi_GetBattleScript(u16 itemId);
u8 PokeBallApi_GetBallSprite(u16 itemId);
u8 PokeBallApi_GetFailedCatchAttackStages(const struct PokeBallCatchContext *context);
void PokeBallApi_OnCatchCommit(const struct PokeBallCatchContext *context, const struct PokeBallThrowResult *result);
void PokeBallApi_BuildContextFromBattle(struct PokeBallCatchContext *context, u16 itemId, u8 targetBattler);

#endif // GUARD_MOD_POKEBALL_H
