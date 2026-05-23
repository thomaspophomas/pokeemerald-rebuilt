#include "global.h"
#include "constants/moves.h"
#include "generated/mod_registry.h"
#include "mod/battle_data.h"
#include "mod/runtime_profile.h"

static EWRAM_DATA struct BattleMove sEffectiveBattleMove;

static bool8 DefinitionIsEmptyDefault(const struct ModBattleMoveDefinition *definition)
{
    return definition->key == NULL
        && definition->move == MOVE_NONE
        && definition->priority == 0
        && definition->overrideFlags == 0;
}

static bool8 RuntimeMoveKeyExists(const char *key, const struct ModBattleMoveDefinition *moves, u16 count)
{
    u16 i;

    if (key == NULL || moves == NULL)
        return FALSE;

    for (i = 0; i < count; i++)
    {
        if (moves[i].key != NULL && strcmp(moves[i].key, key) == 0)
            return TRUE;
    }

    return FALSE;
}

static const struct ModBattleMoveDefinition *FindBestMove(
    const struct ModBattleMoveDefinition *moves,
    u16 count,
    u16 move,
    const struct ModBattleMoveDefinition *shadowingMoves,
    u16 shadowingCount)
{
    const struct ModBattleMoveDefinition *best = NULL;
    u16 i;

    if (moves == NULL)
        return NULL;

    for (i = 0; i < count; i++)
    {
        const struct ModBattleMoveDefinition *definition = &moves[i];

        if (shadowingMoves != NULL && RuntimeMoveKeyExists(definition->key, shadowingMoves, shadowingCount))
            continue;
        if (!BattleDataApi_IsMoveDefinitionValid(definition, TRUE) || DefinitionIsEmptyDefault(definition))
            continue;
        if (definition->move != move)
            continue;
        if (best == NULL || definition->priority < best->priority)
            best = definition;
    }

    return best;
}

static const struct ModBattleMoveDefinition *FindMove(u16 move)
{
    const struct ModBattleMoveDefinition *runtimeMoves;
    const struct ModBattleMoveDefinition *best;
    u16 runtimeCount;

    if (move == MOVE_NONE || move >= MOVES_COUNT)
        return NULL;

    runtimeMoves = ModRuntimeProfile_GetBattleMoves(&runtimeCount);
    best = FindBestMove(runtimeMoves, runtimeCount, move, NULL, 0);
    if (best != NULL)
        return best;

    return FindBestMove(gModBattleMoveDefinitions, gModBattleMoveDefinitionCount, move, runtimeMoves, runtimeCount);
}

bool8 BattleDataApi_IsMoveDefinitionValid(const struct ModBattleMoveDefinition *definition, bool8 allowEmptyDefault)
{
    if (definition == NULL)
        return FALSE;
    if (allowEmptyDefault && DefinitionIsEmptyDefault(definition))
        return TRUE;
    if (definition->key == NULL || definition->key[0] == '\0')
        return FALSE;
    if (definition->move == MOVE_NONE || definition->move >= MOVES_COUNT)
        return FALSE;
    if (definition->overrideFlags == 0)
        return FALSE;
    return TRUE;
}

const struct BattleMove *BattleDataApi_GetMove(u16 move)
{
    const struct ModBattleMoveDefinition *definition;

    if (move >= MOVES_COUNT)
        move = MOVE_NONE;

    definition = FindMove(move);
    if (definition == NULL)
        return &gBattleMoves[move];

    sEffectiveBattleMove = gBattleMoves[move];
    if (definition->overrideFlags & MOD_BATTLE_MOVE_OVERRIDE_EFFECT)
        sEffectiveBattleMove.effect = definition->data.effect;
    if (definition->overrideFlags & MOD_BATTLE_MOVE_OVERRIDE_POWER)
        sEffectiveBattleMove.power = definition->data.power;
    if (definition->overrideFlags & MOD_BATTLE_MOVE_OVERRIDE_TYPE)
        sEffectiveBattleMove.type = definition->data.type;
    if (definition->overrideFlags & MOD_BATTLE_MOVE_OVERRIDE_ACCURACY)
        sEffectiveBattleMove.accuracy = definition->data.accuracy;
    if (definition->overrideFlags & MOD_BATTLE_MOVE_OVERRIDE_PP)
        sEffectiveBattleMove.pp = definition->data.pp;
    if (definition->overrideFlags & MOD_BATTLE_MOVE_OVERRIDE_SECONDARY)
        sEffectiveBattleMove.secondaryEffectChance = definition->data.secondaryEffectChance;
    if (definition->overrideFlags & MOD_BATTLE_MOVE_OVERRIDE_TARGET)
        sEffectiveBattleMove.target = definition->data.target;
    if (definition->overrideFlags & MOD_BATTLE_MOVE_OVERRIDE_PRIORITY)
        sEffectiveBattleMove.priority = definition->data.priority;
    if (definition->overrideFlags & MOD_BATTLE_MOVE_OVERRIDE_FLAGS)
        sEffectiveBattleMove.flags = definition->data.flags;
    return &sEffectiveBattleMove;
}

u8 BattleDataApi_GetMovePP(u16 move)
{
    return BattleDataApi_GetMove(move)->pp;
}
