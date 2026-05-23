#ifndef GUARD_MOD_BATTLE_DATA_H
#define GUARD_MOD_BATTLE_DATA_H

#include "global.h"
#include "pokemon.h"

#define MOD_BATTLE_MOVE_OVERRIDE_EFFECT      (1 << 0)
#define MOD_BATTLE_MOVE_OVERRIDE_POWER       (1 << 1)
#define MOD_BATTLE_MOVE_OVERRIDE_TYPE        (1 << 2)
#define MOD_BATTLE_MOVE_OVERRIDE_ACCURACY    (1 << 3)
#define MOD_BATTLE_MOVE_OVERRIDE_PP          (1 << 4)
#define MOD_BATTLE_MOVE_OVERRIDE_SECONDARY   (1 << 5)
#define MOD_BATTLE_MOVE_OVERRIDE_TARGET      (1 << 6)
#define MOD_BATTLE_MOVE_OVERRIDE_PRIORITY    (1 << 7)
#define MOD_BATTLE_MOVE_OVERRIDE_FLAGS       (1 << 8)

struct ModBattleMoveDefinition
{
    const char *key;
    u16 move;
    s16 priority;
    u16 overrideFlags;
    struct BattleMove data;
};

bool8 BattleDataApi_IsMoveDefinitionValid(const struct ModBattleMoveDefinition *definition, bool8 allowEmptyDefault);
const struct BattleMove *BattleDataApi_GetMove(u16 move);
u8 BattleDataApi_GetMovePP(u16 move);

#endif // GUARD_MOD_BATTLE_DATA_H
