#ifndef GUARD_MOD_POKEBALL_H
#define GUARD_MOD_POKEBALL_H

#include "mod/base.h"

#define POKEBALL_CATCH_MODIFIER_DEFAULT 10
#define POKEBALL_CATCH_MODIFIER_MASTER  255

struct PokeBallCatchContext
{
    u16 item_id;
    u16 target_species;
    u8 target_level;
    u8 target_type_1;
    u8 target_type_2;
    u8 catch_rate;
    u16 target_max_hp;
    u16 target_hp;
    u32 target_status_1;
    u16 battle_turn_counter;
    u8 safari_catch_factor;
    bool8 already_caught;
    u8 map_type;
    u32 battle_type_flags;
};

struct PokeBallThrowResult
{
    bool8 caught;
    u8 shakes;
    u8 catch_rate;
    u8 ball_multiplier;
    u32 odds;
    bool8 used_master_ball;
    bool8 record_catch_attempt;
};

typedef u8 (*PokeBallCatchModifierFunc)(const struct PokeBallCatchContext *context);
typedef void (*PokeBallCatchCommitFunc)(const struct PokeBallCatchContext *context, const struct PokeBallThrowResult *throw_result);

struct ModPokeBallDefinition
{
    const char *key;
    u16 item_id;
    u8 ball_id;
    u8 base_catch_modifier;
    u16 flags;
    PokeBallCatchModifierFunc catch_modifier;
    const u8 *battle_script;
    PokeBallCatchCommitFunc commit_hook;
};

bool8 PokeBallApi_IsBall(u16 item_id);
const struct ModPokeBallDefinition *PokeBallApi_GetDefinition(u16 item_id);
u8 PokeBallApi_GetCatchModifier(const struct PokeBallCatchContext *context);
bool8 PokeBallApi_CalculateThrowResult(const struct PokeBallCatchContext *context, struct PokeBallThrowResult *throw_result);
const u8 *PokeBallApi_GetBattleScript(u16 item_id);
u8 PokeBallApi_GetBallSprite(u16 item_id);
void PokeBallApi_OnCatchCommit(const struct PokeBallCatchContext *context, const struct PokeBallThrowResult *throw_result);
void PokeBallApi_BuildContextFromBattle(struct PokeBallCatchContext *context, u16 item_id, u8 target_battler);

#endif // GUARD_MOD_POKEBALL_H
