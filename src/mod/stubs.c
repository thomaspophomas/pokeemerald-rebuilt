#include "global.h"

#if !FEATURE_MODS

#include "battle.h"
#include "battle_controllers.h"
#include "battle_scripts.h"
#include "constants/items.h"
#include "constants/map_types.h"
#include "constants/moves.h"
#include "constants/species.h"
#include "constants/weather.h"
#include "data.h"
#include "field_weather.h"
#include "global.fieldmap.h"
#include "mod/badge.h"
#include "mod/battle_data.h"
#include "mod/battle_sprite.h"
#include "mod/core.h"
#include "mod/encounter.h"
#include "mod/engine.h"
#include "mod/event.h"
#include "mod/fishing.h"
#include "mod/flags.h"
#include "mod/item.h"
#include "mod/language.h"
#include "mod/level_cap.h"
#include "mod/map.h"
#include "mod/npc.h"
#include "mod/overworld_sprite.h"
#include "mod/pokeball.h"
#include "mod/pokemon_data.h"
#include "mod/reward.h"
#include "mod/shop.h"
#include "mod/sprite_asset.h"
#include "mod/state.h"
#include "mod/time.h"
#include "mod/trainer.h"
#include "mod/weather.h"
#include "overworld.h"
#include "pokedex.h"
#include "pokeball.h"
#include "pokemon.h"
#include "random.h"
#include "rtc.h"

extern struct Evolution gEvolutionTable[][EVOS_PER_MON];

void ModApi_Init(void)
{
}

void ModApi_RunFrame(void)
{
}

void ModApi_OnMapLoad(void)
{
}

void ModApi_OnPlayerStep(u8 direction, u16 new_keys, u16 held_keys)
{
}

void ModApi_OnBattleStart(u32 battle_type_flags)
{
}

void ModApi_OnBattleEnd(u32 battle_outcome)
{
}

u16 ModApi_Random(void)
{
    return Random();
}

void ModState_Init(void)
{
}

struct ModSaveState *ModState_Get(void)
{
    return NULL;
}

void ModState_LoadFromSave(void)
{
}

void ModState_SaveToSave(void)
{
}

u32 ModState_GetRevision(void)
{
    return 0;
}

void ModState_BumpRevision(void)
{
}

void *ModState_GetBlock(const char *mod_id, u16 size, u16 version)
{
    return NULL;
}

bool8 ModState_IsBlockFresh(const char *mod_id)
{
    return FALSE;
}

bool8 ModState_NeedsMigration(const char *mod_id)
{
    return FALSE;
}

bool8 ModState_MarkMigrated(const char *mod_id)
{
    return FALSE;
}

void ModFlag_Init(void)
{
}

u16 ModFlag_GetByName(const char *key)
{
    return MOD_FLAG_INVALID;
}

bool8 ModFlag_Get(u16 mod_flag_id)
{
    return FALSE;
}

bool8 ModFlag_Set(u16 mod_flag_id)
{
    return FALSE;
}

bool8 ModFlag_Clear(u16 mod_flag_id)
{
    return FALSE;
}

bool8 ModFlag_SetLogged(u16 mod_flag_id, const char *reason)
{
    return ModFlag_Set(mod_flag_id);
}

bool8 ModFlag_ClearLogged(u16 mod_flag_id, const char *reason)
{
    return ModFlag_Clear(mod_flag_id);
}

void ModEvent_Init(void)
{
}

void ModEvent_BeginFrame(void)
{
}

s8 ModEvent_Emit(u16 type, const void *payload, u16 size)
{
    return MOD_EVENT_RESULT_CONTINUE;
}

void BadgeApi_Init(void)
{
}

u8 BadgeApi_GetLevel(u8 badge_id)
{
    return badge_id < MOD_BADGE_COUNT ? 0 : 0;
}

bool8 BadgeApi_SetLevel(u8 badge_id, u8 level)
{
    return FALSE;
}

bool8 BadgeApi_IncrementLevel(u8 badge_id, u8 amount)
{
    return FALSE;
}

s16 BadgeApi_GetEffectPercent(u8 effect_kind, u8 target, u8 battler_id)
{
    return 0;
}

bool8 BadgeApi_IsEffectDefinitionValid(const struct ModBadgeEffectDefinition *definition, bool8 allow_empty_none)
{
    return allow_empty_none && definition != NULL && definition->key == NULL;
}

bool8 LevelCapApi_IsDefinitionValid(const struct ModLevelCapDefinition *definition, bool8 allow_empty_default)
{
    return allow_empty_default && definition != NULL && definition->key == NULL;
}

u8 LevelCapApi_GetActiveCap(void)
{
    return MAX_LEVEL;
}

s16 LevelCapApi_ModifyBattleExp(struct Pokemon *mon, s16 gained_exp)
{
    return gained_exp;
}

bool8 LevelCapApi_CanUseRareCandy(struct Pokemon *mon)
{
    return TRUE;
}

void ModTime_Init(void)
{
}

void ModTime_RunFrame(void)
{
}

void ModTime_Now(struct ModTimeSnapshot *snapshot)
{
    memset(snapshot, 0, sizeof(*snapshot));
    RtcCalcLocalTime();
    snapshot->day_count = RtcGetLocalDayCount();
    snapshot->minute_of_day = gLocalTime.hours * 60 + gLocalTime.minutes;
    if (snapshot->minute_of_day < 6 * 60)
        snapshot->segment = MOD_TIME_NIGHT;
    else if (snapshot->minute_of_day < 12 * 60)
        snapshot->segment = MOD_TIME_MORNING;
    else if (snapshot->minute_of_day < 18 * 60)
        snapshot->segment = MOD_TIME_DAY;
    else if (snapshot->minute_of_day < 21 * 60)
        snapshot->segment = MOD_TIME_EVENING;
    else
        snapshot->segment = MOD_TIME_NIGHT;
}

u8 ModTime_GetSegment(void)
{
    struct ModTimeSnapshot snapshot;

    ModTime_Now(&snapshot);
    return snapshot.segment;
}

bool8 ModTime_IsNight(void)
{
    return ModTime_GetSegment() == MOD_TIME_NIGHT;
}

u16 ModTime_GetDayCount(void)
{
    struct ModTimeSnapshot snapshot;

    ModTime_Now(&snapshot);
    return snapshot.day_count;
}

u16 ModTime_GetMinuteOfDay(void)
{
    struct ModTimeSnapshot snapshot;

    ModTime_Now(&snapshot);
    return snapshot.minute_of_day;
}

void ModWeather_Init(void)
{
}

void ModWeather_RunFrame(void)
{
}

void ModWeather_OnMapLoad(void)
{
}

void ModWeather_GetDisplayedWeather(struct ModWeatherDisplay *display)
{
    memset(display, 0, sizeof(*display));
    display->vanilla_weather = GetCurrentWeather();
    display->source = MOD_WEATHER_SOURCE_MAP;
}

u16 ModWeather_GetBattleWeatherMask(void)
{
    return 0;
}

void ModWeather_SetOverride(u8 source, u8 vanilla_weather, u16 layers, u16 duration_frames)
{
}

void ModWeather_ClearOverride(u8 source)
{
}

void EngineApi_Init(void)
{
}

const struct EngineRuleset *EngineApi_GetActiveRuleset(void)
{
    return NULL;
}

bool8 EngineApi_SetActiveRuleset(const char *ruleset_id)
{
    return FALSE;
}

const struct EngineRuleset *EngineApi_FindRuleset(const char *ruleset_id)
{
    return NULL;
}

u16 EngineApi_GetRulesetCount(void)
{
    return 0;
}

void LanguageApi_Init(void)
{
}

const char *LanguageApi_GetActiveLanguage(void)
{
    return LANGUAGE_API_DEFAULT_LANGUAGE;
}

bool8 LanguageApi_SetActiveLanguage(const char *language_id)
{
    return FALSE;
}

const u8 *LanguageApi_GetText(const char *key)
{
    return NULL;
}

const u8 *LanguageApi_Format(const char *key, const struct LanguageFormatArgs *args)
{
    return LanguageApi_GetText(key);
}

const struct MapHeader *MapApi_GetHeader(u16 map_id)
{
    return NULL;
}

const struct MapHeader *MapApi_FindByName(const char *key)
{
    return NULL;
}

const struct MapConnection *MapApi_GetConnection(u16 map_id, u8 direction)
{
    return NULL;
}

const struct WarpEvent *MapApi_GetWarpTarget(u16 map_id, u8 warp_id)
{
    return NULL;
}

bool8 MapApi_IsPositionValid(u16 map_id, s16 x, s16 y)
{
    return FALSE;
}

const struct ModMapDefinition *MapApi_FindDefinition(const char *key)
{
    return NULL;
}

const char *MapApi_GetScriptPath(u16 map_id)
{
    return NULL;
}

u8 NpcApi_Spawn(u16 npc_definition_id, u8 map_group, u8 map_number, s16 x, s16 y)
{
    return MOD_NPC_INSTANCE_NONE;
}

void NpcApi_Despawn(u8 npc_instance_id)
{
}

bool8 NpcApi_SetBehavior(u8 npc_instance_id, u8 movement_type)
{
    return FALSE;
}

bool8 NpcApi_SetVisible(u8 npc_instance_id, bool8 visible)
{
    return FALSE;
}

bool8 NpcApi_FindByLocalId(u8 map_group, u8 map_number, u8 object_event_local_id, u8 *npc_instance_id)
{
    return FALSE;
}

const struct ModNpcDefinition *NpcApi_FindDefinition(const char *key)
{
    return NULL;
}

u8 OverworldSpriteApi_GetGraphicsId(const char *key)
{
    return OVERWORLD_SPRITE_API_INVALID_GFX;
}

bool8 OverworldSpriteApi_SetObjectGraphics(u8 object_event_id, const char *key)
{
    return FALSE;
}

bool8 OverworldSpriteApi_SetPlayerOutfit(const char *key)
{
    return FALSE;
}

u8 OverworldSpriteApi_GetFollowerSprite(u16 species, u8 form, bool8 shiny)
{
    return OVERWORLD_SPRITE_API_INVALID_GFX;
}

u8 OverworldSpriteApi_CreateOrUpdateVirtualAvatar(const char *owner_key, const char *sprite_key, s16 x, s16 y)
{
    return SPRITE_NONE;
}

void OverworldSpriteApi_DestroyVirtualAvatar(const char *owner_key)
{
}

const struct ModSpriteAssetDefinition *SpriteAssetApi_Find(const char *key)
{
    return NULL;
}

bool8 SpriteAssetApi_LoadSheet(const char *key)
{
    return FALSE;
}

bool8 SpriteAssetApi_LoadPalette(const char *key)
{
    return FALSE;
}

void SpriteAssetApi_Release(const char *key)
{
}

const struct ModBattleSpriteDefinition *BattleSpriteApi_GetMonSprite(u16 species, u8 form, u8 side, u16 flags)
{
    return NULL;
}

const struct ModBattleSpriteDefinition *BattleSpriteApi_GetTrainerSprite(u16 trainer_id, u8 side)
{
    return NULL;
}

u8 BattleSpriteApi_GetBallSprite(u16 ball_item_id)
{
    return PokeBallApi_GetBallSprite(ball_item_id);
}

const struct ModBattleSpriteDefinition *BattleSpriteApi_GetMoveAnimSprite(const char *key)
{
    return NULL;
}

bool8 BattleSpriteApi_RefreshBattlerPalette(u8 battler_id)
{
    return FALSE;
}

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

const struct ModPokeBallDefinition *PokeBallApi_GetDefinition(u16 item_id)
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

u8 PokeBallApi_GetCatchModifier(const struct PokeBallCatchContext *context)
{
    const struct ModPokeBallDefinition *definition;

    if (context == NULL)
        return POKEBALL_CATCH_MODIFIER_DEFAULT;

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
        return context->map_type == MAP_TYPE_UNDERWATER ? 35 : 10;
    case ITEM_NEST_BALL:
        if (context->target_level < 40)
        {
            u8 modifier = 40 - context->target_level;
            return modifier <= 9 ? 10 : modifier;
        }
        return 10;
    case ITEM_REPEAT_BALL:
        return context->already_caught ? 30 : 10;
    case ITEM_TIMER_BALL:
        return context->battle_turn_counter + 10 > 40 ? 40 : context->battle_turn_counter + 10;
    }

    definition = PokeBallApi_GetDefinition(context->item_id);
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
    return item_id == ITEM_SAFARI_BALL ? BattleScript_SafariBallThrow : BattleScript_BallThrow;
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

bool8 BattleDataApi_IsMoveDefinitionValid(const struct ModBattleMoveDefinition *definition, bool8 allow_empty_default)
{
    return allow_empty_default && definition != NULL && definition->key == NULL;
}

const struct BattleMove *BattleDataApi_GetMove(u16 move)
{
    if (move >= MOVES_COUNT)
        move = MOVE_NONE;
    return &gBattleMoves[move];
}

u8 BattleDataApi_GetMovePP(u16 move)
{
    return BattleDataApi_GetMove(move)->pp;
}

bool8 PokemonDataApi_IsDefinitionValid(const struct ModPokemonDataDefinition *definition, bool8 allow_empty_default)
{
    return allow_empty_default && definition != NULL && definition->key == NULL;
}

const struct SpeciesInfo *PokemonDataApi_GetSpeciesInfo(u16 species)
{
    if (species >= NUM_SPECIES)
        species = SPECIES_NONE;
    return &gSpeciesInfo[species];
}

const u16 *PokemonDataApi_GetLevelUpLearnset(u16 species)
{
    if (species >= NUM_SPECIES)
        species = SPECIES_NONE;
    return gLevelUpLearnsets[species];
}

const struct Evolution *PokemonDataApi_GetEvolutions(u16 species)
{
    if (species >= NUM_SPECIES)
        species = SPECIES_NONE;
    return gEvolutionTable[species];
}

u8 PokemonDataApi_GetAbilityBySpecies(u16 species, u8 ability_number)
{
    const struct SpeciesInfo *info = PokemonDataApi_GetSpeciesInfo(species);

    if (ability_number != 0 && info->abilities[1] != 0)
        return info->abilities[1];
    return info->abilities[0];
}

bool8 EncounterApi_IsDefinitionValid(const struct ModEncounterDefinition *definition, bool8 allow_empty_default)
{
    return allow_empty_default && definition != NULL && definition->key == NULL;
}

bool8 EncounterApi_HasDefinition(u8 area, u8 rod)
{
    return FALSE;
}

u16 EncounterApi_GetEncounterRate(u8 area, u8 rod, u16 vanilla_encounter_rate)
{
    return vanilla_encounter_rate;
}

bool8 EncounterApi_TrySelectWildMon(u8 area, u8 rod, u8 flags, struct WildPokemon *wild_pokemon)
{
    return FALSE;
}

ModEncounterHook EncounterApi_FindCompiledHook(const char *source_key, const char *hook_key)
{
    return NULL;
}

bool8 FishingApi_HasActions(void)
{
    return FALSE;
}

void FishingApi_BeginAttempt(u8 rod)
{
}

void FishingApi_EndAttempt(u8 outcome)
{
}

bool8 FishingApi_IsDefinitionValid(const struct FishingActionDefinition *definition, bool8 allow_empty_default)
{
    return allow_empty_default && definition != NULL && definition->key == NULL;
}

void FishingApi_InitRequestFromDefinition(const struct FishingActionDefinition *definition, struct FishingActionRequest *request)
{
    memset(request, 0, sizeof(*request));
}

u8 FishingApi_RunPhase(struct FishingContext *context, struct FishingActionRequest *request)
{
    return FISHING_ACTION_CONTINUE;
}

FishingActionHook FishingApi_FindCompiledHook(const char *source_key, const char *hook_key)
{
    return NULL;
}

u8 FishingApi_RequestConfiguredAction(const struct FishingActionDefinition *definition, struct FishingContext *context, struct FishingActionRequest *request)
{
    return FISHING_ACTION_CONTINUE;
}

void FishingApi_SetNextEncounterLevel(u8 level)
{
}

void FishingApi_PrintText(const u8 *text)
{
}

bool8 ShopApi_IsDefinitionValid(const struct ModShopDefinition *definition, bool8 allow_empty_default)
{
    return allow_empty_default && definition != NULL && definition->key == NULL;
}

const u16 *ShopApi_GetItemsForSale(u8 mart_type, const u16 *vanilla_items)
{
    return vanilla_items;
}

bool8 ItemApi_IsDefinitionValid(const struct ModItemDefinition *definition, bool8 allow_empty_default)
{
    return allow_empty_default && definition != NULL && definition->key == NULL;
}

const u8 *ItemApi_GetName(u16 item_id, const u8 *vanilla_name)
{
    return vanilla_name;
}

u16 ItemApi_GetPrice(u16 item_id, u16 vanilla_price)
{
    return vanilla_price;
}

u8 ItemApi_GetHoldEffect(u16 item_id, u8 vanilla_hold_effect)
{
    return vanilla_hold_effect;
}

u8 ItemApi_GetHoldEffectParam(u16 item_id, u8 vanilla_hold_effect_param)
{
    return vanilla_hold_effect_param;
}

const u8 *ItemApi_GetDescription(u16 item_id, const u8 *vanilla_description)
{
    return vanilla_description;
}

u8 ItemApi_GetImportance(u16 item_id, u8 vanilla_importance)
{
    return vanilla_importance;
}

u8 ItemApi_GetPocket(u16 item_id, u8 vanilla_pocket)
{
    return vanilla_pocket;
}

u8 ItemApi_GetType(u16 item_id, u8 vanilla_type)
{
    return vanilla_type;
}

ItemUseFunc ItemApi_GetFieldFunc(u16 item_id, ItemUseFunc vanilla_field_func)
{
    return vanilla_field_func;
}

u8 ItemApi_GetBattleUsage(u16 item_id, u8 vanilla_battle_usage)
{
    return vanilla_battle_usage;
}

ItemUseFunc ItemApi_GetBattleFunc(u16 item_id, ItemUseFunc vanilla_battle_func)
{
    return vanilla_battle_func;
}

u8 ItemApi_GetSecondaryId(u16 item_id, u8 vanilla_secondary_id)
{
    return vanilla_secondary_id;
}

ItemUseFunc ItemApi_FindCompiledFieldUseHook(const char *source_key, const char *hook_key)
{
    return NULL;
}

ItemUseFunc ItemApi_FindCompiledBattleUseHook(const char *source_key, const char *hook_key)
{
    return NULL;
}

bool8 RewardApi_IsDefinitionValid(const struct ModRewardDefinition *definition, bool8 allow_empty_default)
{
    return allow_empty_default && definition != NULL && definition->key == NULL;
}

bool8 RewardApi_AdjustItemReward(u8 source, u8 level, u16 vanilla_item_id, u16 *item_id, u16 *quantity)
{
    return FALSE;
}

ModRewardHook RewardApi_FindCompiledHook(const char *source_key, const char *hook_key)
{
    return NULL;
}

bool8 TrainerApi_IsDefinitionValid(const struct ModTrainerDefinition *definition, bool8 allow_empty_default)
{
    return allow_empty_default && definition != NULL && definition->key == NULL;
}

const struct ModTrainerDefinition *TrainerApi_GetDefinition(u16 trainer_id)
{
    return NULL;
}

u8 TrainerApi_GetPartySize(u16 trainer_id, u8 vanilla)
{
    return vanilla;
}

u8 TrainerApi_GetPartyLevel(u16 trainer_id, u8 party_index, u8 vanilla)
{
    return vanilla;
}

bool8 TrainerApi_CreateParty(u16 trainer_id, struct Pokemon *party, u8 max_count, u8 *party_size)
{
    return FALSE;
}

bool8 TrainerApi_IsDoubleBattle(u16 trainer_id, bool8 vanilla)
{
    return vanilla;
}

u8 TrainerApi_GetTrainerClass(u16 trainer_id, u8 vanilla)
{
    return vanilla;
}

#endif // !FEATURE_MODS
