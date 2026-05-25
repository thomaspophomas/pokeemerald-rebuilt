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
        && definition->override_flags == 0;
}

static bool8 RuntimeMoveKeyExists(const char *key, const struct ModBattleMoveDefinition *moves, u16 count)
{
    u16 move_index;

    if (key == NULL || moves == NULL)
        return FALSE;

    for (move_index = 0; move_index < count; move_index++)
    {
        if (moves[move_index].key != NULL && strcmp(moves[move_index].key, key) == 0)
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
    const struct ModBattleMoveDefinition *best_definition = NULL;
    u16 move_index;

    if (moves == NULL)
        return NULL;

    for (move_index = 0; move_index < count; move_index++)
    {
        const struct ModBattleMoveDefinition *definition = &moves[move_index];

        if (shadowingMoves != NULL && RuntimeMoveKeyExists(definition->key, shadowingMoves, shadowingCount))
            continue;
        if (!BattleDataApi_IsMoveDefinitionValid(definition, TRUE) || DefinitionIsEmptyDefault(definition))
            continue;
        if (definition->move != move)
            continue;
        if (best_definition == NULL || definition->priority < best_definition->priority)
            best_definition = definition;
    }

    return best_definition;
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

bool8 BattleDataApi_IsMoveDefinitionValid(const struct ModBattleMoveDefinition *definition, bool8 allow_empty_default)
{
    if (definition == NULL)
        return FALSE;
    if (allow_empty_default && DefinitionIsEmptyDefault(definition))
        return TRUE;
    if (definition->key == NULL || definition->key[0] == '\0')
        return FALSE;
    if (definition->move == MOVE_NONE || definition->move >= MOVES_COUNT)
        return FALSE;
    if (definition->override_flags == 0)
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
    if (definition->override_flags & MOD_BATTLE_MOVE_OVERRIDE_EFFECT)
        sEffectiveBattleMove.effect = definition->data.effect;
    if (definition->override_flags & MOD_BATTLE_MOVE_OVERRIDE_POWER)
        sEffectiveBattleMove.power = definition->data.power;
    if (definition->override_flags & MOD_BATTLE_MOVE_OVERRIDE_TYPE)
        sEffectiveBattleMove.type = definition->data.type;
    if (definition->override_flags & MOD_BATTLE_MOVE_OVERRIDE_ACCURACY)
        sEffectiveBattleMove.accuracy = definition->data.accuracy;
    if (definition->override_flags & MOD_BATTLE_MOVE_OVERRIDE_PP)
        sEffectiveBattleMove.pp = definition->data.pp;
    if (definition->override_flags & MOD_BATTLE_MOVE_OVERRIDE_SECONDARY)
        sEffectiveBattleMove.secondaryEffectChance = definition->data.secondaryEffectChance;
    if (definition->override_flags & MOD_BATTLE_MOVE_OVERRIDE_TARGET)
        sEffectiveBattleMove.target = definition->data.target;
    if (definition->override_flags & MOD_BATTLE_MOVE_OVERRIDE_PRIORITY)
        sEffectiveBattleMove.priority = definition->data.priority;
    if (definition->override_flags & MOD_BATTLE_MOVE_OVERRIDE_FLAGS)
        sEffectiveBattleMove.flags = definition->data.flags;
    return &sEffectiveBattleMove;
}

u8 BattleDataApi_GetMovePP(u16 move)
{
    return BattleDataApi_GetMove(move)->pp;
}
