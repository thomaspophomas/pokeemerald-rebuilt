#include "global.h"
#include "battle.h"
#include "battle_controllers.h"
#include "battle_scripts.h"
#include "constants/items.h"
#include "constants/map_types.h"
#include "constants/maps.h"
#include "data.h"
#include "generated/mod_registry.h"
#include "mod/pokeball.h"
#include "overworld.h"
#include "pokedex.h"
#include "pokeball.h"
#include "pokemon.h"
#include "random.h"

static const struct ModPokeBallDefinition sVanillaPokeBallDefinitions[] =
{
    { "vanilla:master_ball",  ITEM_MASTER_BALL,  BALL_MASTER,  POKEBALL_CATCH_MODIFIER_MASTER, 0, NULL, BattleScript_BallThrow,       NULL },
    { "vanilla:ultra_ball",   ITEM_ULTRA_BALL,   BALL_ULTRA,   20,                             0, NULL, BattleScript_BallThrow,       NULL },
    { "vanilla:great_ball",   ITEM_GREAT_BALL,   BALL_GREAT,   15,                             0, NULL, BattleScript_BallThrow,       NULL },
    { "vanilla:poke_ball",    ITEM_POKE_BALL,    BALL_POKE,    10,                             0, NULL, BattleScript_BallThrow,       NULL },
    { "vanilla:safari_ball",  ITEM_SAFARI_BALL,  BALL_SAFARI,  15,                             0, NULL, BattleScript_SafariBallThrow, NULL },
    { "vanilla:net_ball",     ITEM_NET_BALL,     BALL_NET,     10,                             0, NULL, BattleScript_BallThrow,       NULL },
    { "vanilla:dive_ball",    ITEM_DIVE_BALL,    BALL_DIVE,    10,                             0, NULL, BattleScript_BallThrow,       NULL },
    { "vanilla:nest_ball",    ITEM_NEST_BALL,    BALL_NEST,    10,                             0, NULL, BattleScript_BallThrow,       NULL },
    { "vanilla:repeat_ball",  ITEM_REPEAT_BALL,  BALL_REPEAT,  10,                             0, NULL, BattleScript_BallThrow,       NULL },
    { "vanilla:timer_ball",   ITEM_TIMER_BALL,   BALL_TIMER,   10,                             0, NULL, BattleScript_BallThrow,       NULL },
    { "vanilla:luxury_ball",  ITEM_LUXURY_BALL,  BALL_LUXURY,  10,                             0, NULL, BattleScript_BallThrow,       NULL },
    { "vanilla:premier_ball", ITEM_PREMIER_BALL, BALL_PREMIER, 10,                             0, NULL, BattleScript_BallThrow,       NULL },
};

static const struct ModPokeBallDefinition *FindGeneratedBall(u16 item_id)
{
    u16 generated_ball_index;

    for (generated_ball_index = 0; generated_ball_index < gModPokeBallDefinitionCount; generated_ball_index++)
    {
        if (gModPokeBallDefinitions[generated_ball_index].item_id == item_id)
            return &gModPokeBallDefinitions[generated_ball_index];
    }

    return NULL;
}

static const struct ModPokeBallDefinition *FindVanillaBall(u16 item_id)
{
    u8 vanilla_ball_index;

    for (vanilla_ball_index = 0; vanilla_ball_index < ARRAY_COUNT(sVanillaPokeBallDefinitions); vanilla_ball_index++)
    {
        if (sVanillaPokeBallDefinitions[vanilla_ball_index].item_id == item_id)
            return &sVanillaPokeBallDefinitions[vanilla_ball_index];
    }

    return NULL;
}

bool8 PokeBallApi_IsBall(u16 item_id)
{
    return PokeBallApi_GetDefinition(item_id) != NULL;
}

const struct ModPokeBallDefinition *PokeBallApi_GetDefinition(u16 item_id)
{
    const struct ModPokeBallDefinition *definition = FindGeneratedBall(item_id);

    if (definition != NULL)
        return definition;

    return FindVanillaBall(item_id);
}

u8 PokeBallApi_GetCatchModifier(const struct PokeBallCatchContext *context)
{
    const struct ModPokeBallDefinition *definition;

    if (context == NULL)
        return POKEBALL_CATCH_MODIFIER_DEFAULT;

    definition = PokeBallApi_GetDefinition(context->item_id);
    if (definition != NULL && definition->catch_modifier != NULL)
        return definition->catch_modifier(context);

    switch (context->item_id)
    {
    case ITEM_MASTER_BALL:
        return POKEBALL_CATCH_MODIFIER_MASTER;
    case ITEM_NET_BALL:
        if (context->target_type_1 == TYPE_WATER || context->target_type_2 == TYPE_WATER
         || context->target_type_1 == TYPE_BUG || context->target_type_2 == TYPE_BUG)
            return 30;
        return 10;
    case ITEM_DIVE_BALL:
        if (context->map_type == MAP_TYPE_UNDERWATER)
            return 35;
        return 10;
    case ITEM_NEST_BALL:
        if (context->target_level < 40)
        {
            u8 modifier = 40 - context->target_level;
            if (modifier <= 9)
                modifier = 10;
            return modifier;
        }
        return 10;
    case ITEM_REPEAT_BALL:
        return context->already_caught ? 30 : 10;
    case ITEM_TIMER_BALL:
        if (context->battle_turn_counter + 10 > 40)
            return 40;
        return context->battle_turn_counter + 10;
    }

    if (definition != NULL)
        return definition->base_catch_modifier;

    return POKEBALL_CATCH_MODIFIER_DEFAULT;
}

bool8 PokeBallApi_CalculateThrowResult(const struct PokeBallCatchContext *context, struct PokeBallThrowResult *throw_result)
{
    u32 odds;
    u8 shakes;

    if (context == NULL || throw_result == NULL)
        return FALSE;

    memset(throw_result, 0, sizeof(*throw_result));
    throw_result->catch_rate = context->catch_rate;
    throw_result->ball_multiplier = PokeBallApi_GetCatchModifier(context);
    throw_result->used_master_ball = context->item_id == ITEM_MASTER_BALL;
    throw_result->record_catch_attempt = context->item_id != ITEM_MASTER_BALL && context->item_id != ITEM_SAFARI_BALL;

    if (throw_result->used_master_ball)
    {
        throw_result->caught = TRUE;
        throw_result->shakes = BALL_3_SHAKES_SUCCESS;
        throw_result->odds = 255;
        return TRUE;
    }

    if (context->target_max_hp == 0 || throw_result->catch_rate == 0 || throw_result->ball_multiplier == 0)
    {
        throw_result->caught = FALSE;
        throw_result->shakes = BALL_NO_SHAKES;
        throw_result->odds = 0;
        return TRUE;
    }

    odds = (throw_result->catch_rate * throw_result->ball_multiplier / 10)
        * (context->target_max_hp * 3 - context->target_hp * 2)
        / (3 * context->target_max_hp);

    if (context->target_status_1 & (STATUS1_SLEEP | STATUS1_FREEZE))
        odds *= 2;
    if (context->target_status_1 & (STATUS1_POISON | STATUS1_BURN | STATUS1_PARALYSIS | STATUS1_TOXIC_POISON))
        odds = (odds * 15) / 10;

    throw_result->odds = odds;
    if (odds > 254)
    {
        throw_result->caught = TRUE;
        throw_result->shakes = BALL_3_SHAKES_SUCCESS;
        return TRUE;
    }

    if (odds == 0)
    {
        throw_result->caught = FALSE;
        throw_result->shakes = BALL_NO_SHAKES;
        return TRUE;
    }

    odds = Sqrt(Sqrt(16711680 / odds));
    odds = 1048560 / odds;

    for (shakes = 0; shakes < BALL_3_SHAKES_SUCCESS && Random() < odds; shakes++);

    throw_result->caught = shakes == BALL_3_SHAKES_SUCCESS;
    throw_result->shakes = shakes;
    return TRUE;
}

const u8 *PokeBallApi_GetBattleScript(u16 item_id)
{
    const struct ModPokeBallDefinition *definition = PokeBallApi_GetDefinition(item_id);

    if (definition != NULL && definition->battle_script != NULL)
        return definition->battle_script;
    if (item_id == ITEM_SAFARI_BALL)
        return BattleScript_SafariBallThrow;

    return BattleScript_BallThrow;
}

u8 PokeBallApi_GetBallSprite(u16 item_id)
{
    const struct ModPokeBallDefinition *definition = PokeBallApi_GetDefinition(item_id);

    if (definition != NULL && definition->ball_id < POKEBALL_COUNT)
        return definition->ball_id;

    return BALL_POKE;
}

void PokeBallApi_OnCatchCommit(const struct PokeBallCatchContext *context, const struct PokeBallThrowResult *throw_result)
{
    const struct ModPokeBallDefinition *definition;

    if (context == NULL || throw_result == NULL || !throw_result->caught)
        return;

    definition = PokeBallApi_GetDefinition(context->item_id);
    if (definition != NULL && definition->commit_hook != NULL)
        definition->commit_hook(context, throw_result);
}

void PokeBallApi_BuildContextFromBattle(struct PokeBallCatchContext *context, u16 item_id, u8 target_battler)
{
    u16 species;

    memset(context, 0, sizeof(*context));
    context->item_id = item_id;
    context->target_species = gBattleMons[target_battler].species;
    context->target_level = gBattleMons[target_battler].level;
    context->target_type_1 = gBattleMons[target_battler].types[0];
    context->target_type_2 = gBattleMons[target_battler].types[1];
    context->target_max_hp = gBattleMons[target_battler].maxHP;
    context->target_hp = gBattleMons[target_battler].hp;
    context->target_status_1 = gBattleMons[target_battler].status1;
    context->battle_turn_counter = gBattleResults.battleTurnCounter;
    context->safari_catch_factor = gBattleStruct->safariCatchFactor;
    context->map_type = GetCurrentMapType();
    context->battle_type_flags = gBattleTypeFlags;

    if (item_id == ITEM_SAFARI_BALL)
        context->catch_rate = gBattleStruct->safariCatchFactor * 1275 / 100;
    else
        context->catch_rate = gSpeciesInfo[context->target_species].catchRate;

    species = SpeciesToNationalPokedexNum(context->target_species);
    context->already_caught = GetSetPokedexFlag(species, FLAG_GET_CAUGHT);
}
