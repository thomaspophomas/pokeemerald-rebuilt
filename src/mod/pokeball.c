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

static const struct ModPokeBallDefinition *FindGeneratedBall(u16 itemId)
{
    u16 i;

    for (i = 0; i < gModPokeBallDefinitionCount; i++)
    {
        if (gModPokeBallDefinitions[i].itemId == itemId)
            return &gModPokeBallDefinitions[i];
    }

    return NULL;
}

static const struct ModPokeBallDefinition *FindVanillaBall(u16 itemId)
{
    u8 i;

    for (i = 0; i < ARRAY_COUNT(sVanillaPokeBallDefinitions); i++)
    {
        if (sVanillaPokeBallDefinitions[i].itemId == itemId)
            return &sVanillaPokeBallDefinitions[i];
    }

    return NULL;
}

bool8 PokeBallApi_IsBall(u16 itemId)
{
    return PokeBallApi_GetDefinition(itemId) != NULL;
}

const struct ModPokeBallDefinition *PokeBallApi_GetDefinition(u16 itemId)
{
    const struct ModPokeBallDefinition *definition = FindGeneratedBall(itemId);

    if (definition != NULL)
        return definition;

    return FindVanillaBall(itemId);
}

u8 PokeBallApi_GetCatchModifier(const struct PokeBallCatchContext *context)
{
    const struct ModPokeBallDefinition *definition;

    if (context == NULL)
        return POKEBALL_CATCH_MODIFIER_DEFAULT;

    definition = PokeBallApi_GetDefinition(context->itemId);
    if (definition != NULL && definition->catchModifier != NULL)
        return definition->catchModifier(context);

    switch (context->itemId)
    {
    case ITEM_MASTER_BALL:
        return POKEBALL_CATCH_MODIFIER_MASTER;
    case ITEM_NET_BALL:
        if (context->targetType1 == TYPE_WATER || context->targetType2 == TYPE_WATER
         || context->targetType1 == TYPE_BUG || context->targetType2 == TYPE_BUG)
            return 30;
        return 10;
    case ITEM_DIVE_BALL:
        if (context->mapType == MAP_TYPE_UNDERWATER)
            return 35;
        return 10;
    case ITEM_NEST_BALL:
        if (context->targetLevel < 40)
        {
            u8 modifier = 40 - context->targetLevel;
            if (modifier <= 9)
                modifier = 10;
            return modifier;
        }
        return 10;
    case ITEM_REPEAT_BALL:
        return context->alreadyCaught ? 30 : 10;
    case ITEM_TIMER_BALL:
        if (context->battleTurnCounter + 10 > 40)
            return 40;
        return context->battleTurnCounter + 10;
    }

    if (definition != NULL)
        return definition->baseCatchModifier;

    return POKEBALL_CATCH_MODIFIER_DEFAULT;
}

bool8 PokeBallApi_CalculateThrowResult(const struct PokeBallCatchContext *context, struct PokeBallThrowResult *result)
{
    u32 odds;
    u8 shakes;

    if (context == NULL || result == NULL)
        return FALSE;

    memset(result, 0, sizeof(*result));
    result->catchRate = context->catchRate;
    result->ballMultiplier = PokeBallApi_GetCatchModifier(context);
    result->usedMasterBall = context->itemId == ITEM_MASTER_BALL;
    result->recordCatchAttempt = context->itemId != ITEM_MASTER_BALL && context->itemId != ITEM_SAFARI_BALL;

    if (result->usedMasterBall)
    {
        result->caught = TRUE;
        result->shakes = BALL_3_SHAKES_SUCCESS;
        result->odds = 255;
        return TRUE;
    }

    if (context->targetMaxHp == 0 || result->catchRate == 0 || result->ballMultiplier == 0)
    {
        result->caught = FALSE;
        result->shakes = BALL_NO_SHAKES;
        result->odds = 0;
        return TRUE;
    }

    odds = (result->catchRate * result->ballMultiplier / 10)
        * (context->targetMaxHp * 3 - context->targetHp * 2)
        / (3 * context->targetMaxHp);

    if (context->targetStatus1 & (STATUS1_SLEEP | STATUS1_FREEZE))
        odds *= 2;
    if (context->targetStatus1 & (STATUS1_POISON | STATUS1_BURN | STATUS1_PARALYSIS | STATUS1_TOXIC_POISON))
        odds = (odds * 15) / 10;

    result->odds = odds;
    if (odds > 254)
    {
        result->caught = TRUE;
        result->shakes = BALL_3_SHAKES_SUCCESS;
        return TRUE;
    }

    if (odds == 0)
    {
        result->caught = FALSE;
        result->shakes = BALL_NO_SHAKES;
        return TRUE;
    }

    odds = Sqrt(Sqrt(16711680 / odds));
    odds = 1048560 / odds;

    for (shakes = 0; shakes < BALL_3_SHAKES_SUCCESS && Random() < odds; shakes++);

    result->caught = shakes == BALL_3_SHAKES_SUCCESS;
    result->shakes = shakes;
    return TRUE;
}

const u8 *PokeBallApi_GetBattleScript(u16 itemId)
{
    const struct ModPokeBallDefinition *definition = PokeBallApi_GetDefinition(itemId);

    if (definition != NULL && definition->battleScript != NULL)
        return definition->battleScript;
    if (itemId == ITEM_SAFARI_BALL)
        return BattleScript_SafariBallThrow;

    return BattleScript_BallThrow;
}

u8 PokeBallApi_GetBallSprite(u16 itemId)
{
    const struct ModPokeBallDefinition *definition = PokeBallApi_GetDefinition(itemId);

    if (definition != NULL && definition->ballId < POKEBALL_COUNT)
        return definition->ballId;

    return BALL_POKE;
}

u8 PokeBallApi_GetFailedCatchAttackStages(const struct PokeBallCatchContext *context)
{
    u8 modifier;

    if (context == NULL || context->itemId == ITEM_MASTER_BALL || !PokeBallApi_IsBall(context->itemId))
        return 0;

    modifier = PokeBallApi_GetCatchModifier(context);
    return modifier >= 20 ? 2 : 1;
}

void PokeBallApi_OnCatchCommit(const struct PokeBallCatchContext *context, const struct PokeBallThrowResult *result)
{
    const struct ModPokeBallDefinition *definition;

    if (context == NULL || result == NULL || !result->caught)
        return;

    definition = PokeBallApi_GetDefinition(context->itemId);
    if (definition != NULL && definition->commitHook != NULL)
        definition->commitHook(context, result);
}

void PokeBallApi_BuildContextFromBattle(struct PokeBallCatchContext *context, u16 itemId, u8 targetBattler)
{
    u16 species;

    memset(context, 0, sizeof(*context));
    context->itemId = itemId;
    context->targetSpecies = gBattleMons[targetBattler].species;
    context->targetLevel = gBattleMons[targetBattler].level;
    context->targetType1 = gBattleMons[targetBattler].types[0];
    context->targetType2 = gBattleMons[targetBattler].types[1];
    context->targetMaxHp = gBattleMons[targetBattler].maxHP;
    context->targetHp = gBattleMons[targetBattler].hp;
    context->targetStatus1 = gBattleMons[targetBattler].status1;
    context->battleTurnCounter = gBattleResults.battleTurnCounter;
    context->safariCatchFactor = gBattleStruct->safariCatchFactor;
    context->mapType = GetCurrentMapType();
    context->battleTypeFlags = gBattleTypeFlags;

    if (itemId == ITEM_SAFARI_BALL)
        context->catchRate = gBattleStruct->safariCatchFactor * 1275 / 100;
    else
        context->catchRate = gSpeciesInfo[context->targetSpecies].catchRate;

    species = SpeciesToNationalPokedexNum(context->targetSpecies);
    context->alreadyCaught = GetSetPokedexFlag(species, FLAG_GET_CAUGHT);
}
