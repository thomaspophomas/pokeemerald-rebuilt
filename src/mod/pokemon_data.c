#include "global.h"
#include "constants/moves.h"
#include "constants/species.h"
#include "generated/mod_registry.h"
#include "mod/pokemon_data.h"
#include "mod/runtime_profile.h"
#include "pokemon.h"

extern struct Evolution gEvolutionTable[][EVOS_PER_MON];

static EWRAM_DATA struct SpeciesInfo sEffectiveSpeciesInfo;
static EWRAM_DATA u16 sEffectiveLevelUpLearnset[MOD_POKEMON_MAX_LEVEL_UP_MOVES + 1];
static EWRAM_DATA struct Evolution sEffectiveEvolutions[MOD_POKEMON_MAX_EVOLUTIONS];

static bool8 DefinitionIsEmptyDefault(const struct ModPokemonDataDefinition *definition)
{
    return definition->key == NULL
        && definition->species == SPECIES_NONE
        && definition->priority == 0
        && definition->flags == 0
        && definition->levelUpMoveCount == 0
        && definition->evolutionCount == 0;
}

static bool8 RuntimePokemonKeyExists(const char *key, const struct ModPokemonDataDefinition *pokemon, u16 count)
{
    u16 pokemon_definition_index;

    if (key == NULL || pokemon == NULL)
        return FALSE;

    for (pokemon_definition_index = 0; pokemon_definition_index < count; pokemon_definition_index++)
    {
        if (pokemon[pokemon_definition_index].key != NULL && strcmp(pokemon[pokemon_definition_index].key, key) == 0)
            return TRUE;
    }

    return FALSE;
}

static const struct ModPokemonDataDefinition *FindBestDefinition(
    const struct ModPokemonDataDefinition *pokemon,
    u16 count,
    u16 species,
    const struct ModPokemonDataDefinition *shadowingPokemon,
    u16 shadowingCount)
{
    const struct ModPokemonDataDefinition *best_definition = NULL;
    u16 pokemon_definition_index;

    if (pokemon == NULL)
        return NULL;

    for (pokemon_definition_index = 0; pokemon_definition_index < count; pokemon_definition_index++)
    {
        const struct ModPokemonDataDefinition *definition = &pokemon[pokemon_definition_index];

        if (shadowingPokemon != NULL && RuntimePokemonKeyExists(definition->key, shadowingPokemon, shadowingCount))
            continue;
        if (!PokemonDataApi_IsDefinitionValid(definition, TRUE) || DefinitionIsEmptyDefault(definition))
            continue;
        if (definition->species != species)
            continue;
        if (best_definition == NULL || definition->priority < best_definition->priority)
            best_definition = definition;
    }

    return best_definition;
}

static const struct ModPokemonDataDefinition *FindDefinition(u16 species)
{
    const struct ModPokemonDataDefinition *runtimePokemon;
    const struct ModPokemonDataDefinition *best;
    u16 runtimeCount;

    if (species == SPECIES_NONE || species >= NUM_SPECIES)
        return NULL;

    runtimePokemon = ModRuntimeProfile_GetPokemonData(&runtimeCount);
    best = FindBestDefinition(runtimePokemon, runtimeCount, species, NULL, 0);
    if (best != NULL)
        return best;

    return FindBestDefinition(gModPokemonDataDefinitions, gModPokemonDataDefinitionCount, species, runtimePokemon, runtimeCount);
}

static void ApplyInfoOverride(struct SpeciesInfo *species_info_to_update, const struct ModSpeciesInfoOverride *species_info_override, u16 override_flags)
{
    if (override_flags & MOD_POKEMON_OVERRIDE_BASE_STATS)
    {
        species_info_to_update->baseHP = species_info_override->baseHP;
        species_info_to_update->baseAttack = species_info_override->baseAttack;
        species_info_to_update->baseDefense = species_info_override->baseDefense;
        species_info_to_update->baseSpeed = species_info_override->baseSpeed;
        species_info_to_update->baseSpAttack = species_info_override->baseSpAttack;
        species_info_to_update->baseSpDefense = species_info_override->baseSpDefense;
    }
    if (override_flags & MOD_POKEMON_OVERRIDE_TYPES)
    {
        species_info_to_update->types[0] = species_info_override->types[0];
        species_info_to_update->types[1] = species_info_override->types[1];
    }
    if (override_flags & MOD_POKEMON_OVERRIDE_CATCH_RATE)
        species_info_to_update->catchRate = species_info_override->catchRate;
    if (override_flags & MOD_POKEMON_OVERRIDE_EXP_YIELD)
        species_info_to_update->expYield = species_info_override->expYield;
    if (override_flags & MOD_POKEMON_OVERRIDE_EV_YIELD)
    {
        species_info_to_update->evYield_HP = species_info_override->evYields[0];
        species_info_to_update->evYield_Attack = species_info_override->evYields[1];
        species_info_to_update->evYield_Defense = species_info_override->evYields[2];
        species_info_to_update->evYield_Speed = species_info_override->evYields[3];
        species_info_to_update->evYield_SpAttack = species_info_override->evYields[4];
        species_info_to_update->evYield_SpDefense = species_info_override->evYields[5];
    }
    if (override_flags & MOD_POKEMON_OVERRIDE_ITEMS)
    {
        species_info_to_update->itemCommon = species_info_override->itemCommon;
        species_info_to_update->itemRare = species_info_override->itemRare;
    }
    if (override_flags & MOD_POKEMON_OVERRIDE_GENDER)
        species_info_to_update->genderRatio = species_info_override->genderRatio;
    if (override_flags & MOD_POKEMON_OVERRIDE_EGG_CYCLES)
        species_info_to_update->eggCycles = species_info_override->eggCycles;
    if (override_flags & MOD_POKEMON_OVERRIDE_FRIENDSHIP)
        species_info_to_update->friendship = species_info_override->friendship;
    if (override_flags & MOD_POKEMON_OVERRIDE_GROWTH_RATE)
        species_info_to_update->growthRate = species_info_override->growthRate;
    if (override_flags & MOD_POKEMON_OVERRIDE_EGG_GROUPS)
    {
        species_info_to_update->eggGroups[0] = species_info_override->eggGroups[0];
        species_info_to_update->eggGroups[1] = species_info_override->eggGroups[1];
    }
    if (override_flags & MOD_POKEMON_OVERRIDE_ABILITIES)
    {
        species_info_to_update->abilities[0] = species_info_override->abilities[0];
        species_info_to_update->abilities[1] = species_info_override->abilities[1];
    }
    if (override_flags & MOD_POKEMON_OVERRIDE_SAFARI_FLEE)
        species_info_to_update->safariZoneFleeRate = species_info_override->safariZoneFleeRate;
    if (override_flags & MOD_POKEMON_OVERRIDE_BODY_COLOR)
    {
        species_info_to_update->bodyColor = species_info_override->bodyColor;
        species_info_to_update->noFlip = species_info_override->noFlip;
    }
}

bool8 PokemonDataApi_IsDefinitionValid(const struct ModPokemonDataDefinition *definition, bool8 allow_empty_default)
{
    u8 level_up_move_index;
    u8 evolution_index;

    if (definition == NULL)
        return FALSE;
    if (allow_empty_default && DefinitionIsEmptyDefault(definition))
        return TRUE;
    if (definition->key == NULL || definition->key[0] == '\0')
        return FALSE;
    if (definition->species == SPECIES_NONE || definition->species >= NUM_SPECIES)
        return FALSE;
    if (definition->levelUpMoveCount > MOD_POKEMON_MAX_LEVEL_UP_MOVES)
        return FALSE;
    if (definition->evolutionCount > MOD_POKEMON_MAX_EVOLUTIONS)
        return FALSE;
    for (level_up_move_index = 0; level_up_move_index < definition->levelUpMoveCount; level_up_move_index++)
    {
        if (definition->levelUpMoves[level_up_move_index].level == 0 || definition->levelUpMoves[level_up_move_index].level > MAX_LEVEL)
            return FALSE;
        if (definition->levelUpMoves[level_up_move_index].move == MOVE_NONE || definition->levelUpMoves[level_up_move_index].move >= MOVES_COUNT)
            return FALSE;
    }
    for (evolution_index = 0; evolution_index < definition->evolutionCount; evolution_index++)
    {
        if (definition->evolutions[evolution_index].targetSpecies == SPECIES_NONE || definition->evolutions[evolution_index].targetSpecies >= NUM_SPECIES)
            return FALSE;
    }
    return TRUE;
}

const struct SpeciesInfo *PokemonDataApi_GetSpeciesInfo(u16 species)
{
    const struct ModPokemonDataDefinition *definition;

    if (species >= NUM_SPECIES)
        species = SPECIES_NONE;

    definition = FindDefinition(species);
    if (definition == NULL)
        return &gSpeciesInfo[species];

    sEffectiveSpeciesInfo = gSpeciesInfo[species];
    ApplyInfoOverride(&sEffectiveSpeciesInfo, &definition->info, definition->flags);
    return &sEffectiveSpeciesInfo;
}

const u16 *PokemonDataApi_GetLevelUpLearnset(u16 species)
{
    const struct ModPokemonDataDefinition *definition = FindDefinition(species);
    u8 level_up_move_index;

    if (species >= NUM_SPECIES)
        species = SPECIES_NONE;
    if (definition == NULL || !(definition->flags & MOD_POKEMON_OVERRIDE_LEVEL_UP))
        return gLevelUpLearnsets[species];

    for (level_up_move_index = 0; level_up_move_index < definition->levelUpMoveCount; level_up_move_index++)
        sEffectiveLevelUpLearnset[level_up_move_index] = (definition->levelUpMoves[level_up_move_index].level << 9) | definition->levelUpMoves[level_up_move_index].move;
    sEffectiveLevelUpLearnset[definition->levelUpMoveCount] = LEVEL_UP_END;
    return sEffectiveLevelUpLearnset;
}

const struct Evolution *PokemonDataApi_GetEvolutions(u16 species)
{
    const struct ModPokemonDataDefinition *definition = FindDefinition(species);
    u8 evolution_index;

    if (species >= NUM_SPECIES)
        species = SPECIES_NONE;
    if (definition == NULL || !(definition->flags & MOD_POKEMON_OVERRIDE_EVOLUTIONS))
        return gEvolutionTable[species];

    memset(sEffectiveEvolutions, 0, sizeof(sEffectiveEvolutions));
    for (evolution_index = 0; evolution_index < definition->evolutionCount; evolution_index++)
        sEffectiveEvolutions[evolution_index] = definition->evolutions[evolution_index];
    return sEffectiveEvolutions;
}

u8 PokemonDataApi_GetAbilityBySpecies(u16 species, u8 ability_number)
{
    const struct SpeciesInfo *info = PokemonDataApi_GetSpeciesInfo(species);

    if (ability_number != 0 && info->abilities[1] != 0)
        return info->abilities[1];
    return info->abilities[0];
}
