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
    u16 i;

    if (key == NULL || pokemon == NULL)
        return FALSE;

    for (i = 0; i < count; i++)
    {
        if (pokemon[i].key != NULL && strcmp(pokemon[i].key, key) == 0)
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
    const struct ModPokemonDataDefinition *best = NULL;
    u16 i;

    if (pokemon == NULL)
        return NULL;

    for (i = 0; i < count; i++)
    {
        const struct ModPokemonDataDefinition *definition = &pokemon[i];

        if (shadowingPokemon != NULL && RuntimePokemonKeyExists(definition->key, shadowingPokemon, shadowingCount))
            continue;
        if (!PokemonDataApi_IsDefinitionValid(definition, TRUE) || DefinitionIsEmptyDefault(definition))
            continue;
        if (definition->species != species)
            continue;
        if (best == NULL || definition->priority < best->priority)
            best = definition;
    }

    return best;
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

static void ApplyInfoOverride(struct SpeciesInfo *dest, const struct ModSpeciesInfoOverride *src, u16 flags)
{
    if (flags & MOD_POKEMON_OVERRIDE_BASE_STATS)
    {
        dest->baseHP = src->baseHP;
        dest->baseAttack = src->baseAttack;
        dest->baseDefense = src->baseDefense;
        dest->baseSpeed = src->baseSpeed;
        dest->baseSpAttack = src->baseSpAttack;
        dest->baseSpDefense = src->baseSpDefense;
    }
    if (flags & MOD_POKEMON_OVERRIDE_TYPES)
    {
        dest->types[0] = src->types[0];
        dest->types[1] = src->types[1];
    }
    if (flags & MOD_POKEMON_OVERRIDE_CATCH_RATE)
        dest->catchRate = src->catchRate;
    if (flags & MOD_POKEMON_OVERRIDE_EXP_YIELD)
        dest->expYield = src->expYield;
    if (flags & MOD_POKEMON_OVERRIDE_EV_YIELD)
    {
        dest->evYield_HP = src->evYields[0];
        dest->evYield_Attack = src->evYields[1];
        dest->evYield_Defense = src->evYields[2];
        dest->evYield_Speed = src->evYields[3];
        dest->evYield_SpAttack = src->evYields[4];
        dest->evYield_SpDefense = src->evYields[5];
    }
    if (flags & MOD_POKEMON_OVERRIDE_ITEMS)
    {
        dest->itemCommon = src->itemCommon;
        dest->itemRare = src->itemRare;
    }
    if (flags & MOD_POKEMON_OVERRIDE_GENDER)
        dest->genderRatio = src->genderRatio;
    if (flags & MOD_POKEMON_OVERRIDE_EGG_CYCLES)
        dest->eggCycles = src->eggCycles;
    if (flags & MOD_POKEMON_OVERRIDE_FRIENDSHIP)
        dest->friendship = src->friendship;
    if (flags & MOD_POKEMON_OVERRIDE_GROWTH_RATE)
        dest->growthRate = src->growthRate;
    if (flags & MOD_POKEMON_OVERRIDE_EGG_GROUPS)
    {
        dest->eggGroups[0] = src->eggGroups[0];
        dest->eggGroups[1] = src->eggGroups[1];
    }
    if (flags & MOD_POKEMON_OVERRIDE_ABILITIES)
    {
        dest->abilities[0] = src->abilities[0];
        dest->abilities[1] = src->abilities[1];
    }
    if (flags & MOD_POKEMON_OVERRIDE_SAFARI_FLEE)
        dest->safariZoneFleeRate = src->safariZoneFleeRate;
    if (flags & MOD_POKEMON_OVERRIDE_BODY_COLOR)
    {
        dest->bodyColor = src->bodyColor;
        dest->noFlip = src->noFlip;
    }
}

bool8 PokemonDataApi_IsDefinitionValid(const struct ModPokemonDataDefinition *definition, bool8 allowEmptyDefault)
{
    u8 i;

    if (definition == NULL)
        return FALSE;
    if (allowEmptyDefault && DefinitionIsEmptyDefault(definition))
        return TRUE;
    if (definition->key == NULL || definition->key[0] == '\0')
        return FALSE;
    if (definition->species == SPECIES_NONE || definition->species >= NUM_SPECIES)
        return FALSE;
    if (definition->levelUpMoveCount > MOD_POKEMON_MAX_LEVEL_UP_MOVES)
        return FALSE;
    if (definition->evolutionCount > MOD_POKEMON_MAX_EVOLUTIONS)
        return FALSE;
    for (i = 0; i < definition->levelUpMoveCount; i++)
    {
        if (definition->levelUpMoves[i].level == 0 || definition->levelUpMoves[i].level > MAX_LEVEL)
            return FALSE;
        if (definition->levelUpMoves[i].move == MOVE_NONE || definition->levelUpMoves[i].move >= MOVES_COUNT)
            return FALSE;
    }
    for (i = 0; i < definition->evolutionCount; i++)
    {
        if (definition->evolutions[i].targetSpecies == SPECIES_NONE || definition->evolutions[i].targetSpecies >= NUM_SPECIES)
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
    u8 i;

    if (species >= NUM_SPECIES)
        species = SPECIES_NONE;
    if (definition == NULL || !(definition->flags & MOD_POKEMON_OVERRIDE_LEVEL_UP))
        return gLevelUpLearnsets[species];

    for (i = 0; i < definition->levelUpMoveCount; i++)
        sEffectiveLevelUpLearnset[i] = (definition->levelUpMoves[i].level << 9) | definition->levelUpMoves[i].move;
    sEffectiveLevelUpLearnset[definition->levelUpMoveCount] = LEVEL_UP_END;
    return sEffectiveLevelUpLearnset;
}

const struct Evolution *PokemonDataApi_GetEvolutions(u16 species)
{
    const struct ModPokemonDataDefinition *definition = FindDefinition(species);
    u8 i;

    if (species >= NUM_SPECIES)
        species = SPECIES_NONE;
    if (definition == NULL || !(definition->flags & MOD_POKEMON_OVERRIDE_EVOLUTIONS))
        return gEvolutionTable[species];

    memset(sEffectiveEvolutions, 0, sizeof(sEffectiveEvolutions));
    for (i = 0; i < definition->evolutionCount; i++)
        sEffectiveEvolutions[i] = definition->evolutions[i];
    return sEffectiveEvolutions;
}

u8 PokemonDataApi_GetAbilityBySpecies(u16 species, u8 abilityNum)
{
    const struct SpeciesInfo *info = PokemonDataApi_GetSpeciesInfo(species);

    if (abilityNum != 0 && info->abilities[1] != 0)
        return info->abilities[1];
    return info->abilities[0];
}
