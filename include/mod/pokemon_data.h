#ifndef GUARD_MOD_POKEMON_DATA_H
#define GUARD_MOD_POKEMON_DATA_H

#include "global.h"
#include "constants/pokemon.h"
#include "constants/species.h"
#include "pokemon.h"

#define MOD_POKEMON_MAX_LEVEL_UP_MOVES 32
#define MOD_POKEMON_MAX_EVOLUTIONS EVOS_PER_MON

#define MOD_POKEMON_OVERRIDE_BASE_STATS   (1 << 0)
#define MOD_POKEMON_OVERRIDE_TYPES        (1 << 1)
#define MOD_POKEMON_OVERRIDE_CATCH_RATE   (1 << 2)
#define MOD_POKEMON_OVERRIDE_EXP_YIELD    (1 << 3)
#define MOD_POKEMON_OVERRIDE_EV_YIELD     (1 << 4)
#define MOD_POKEMON_OVERRIDE_ITEMS        (1 << 5)
#define MOD_POKEMON_OVERRIDE_GENDER       (1 << 6)
#define MOD_POKEMON_OVERRIDE_EGG_CYCLES   (1 << 7)
#define MOD_POKEMON_OVERRIDE_FRIENDSHIP   (1 << 8)
#define MOD_POKEMON_OVERRIDE_GROWTH_RATE  (1 << 9)
#define MOD_POKEMON_OVERRIDE_EGG_GROUPS   (1 << 10)
#define MOD_POKEMON_OVERRIDE_ABILITIES    (1 << 11)
#define MOD_POKEMON_OVERRIDE_SAFARI_FLEE  (1 << 12)
#define MOD_POKEMON_OVERRIDE_BODY_COLOR   (1 << 13)
#define MOD_POKEMON_OVERRIDE_LEVEL_UP     (1 << 14)
#define MOD_POKEMON_OVERRIDE_EVOLUTIONS   (1 << 15)

struct ModSpeciesInfoOverride
{
    u8 baseHP;
    u8 baseAttack;
    u8 baseDefense;
    u8 baseSpeed;
    u8 baseSpAttack;
    u8 baseSpDefense;
    u8 types[2];
    u8 catchRate;
    u8 expYield;
    u8 evYields[6];
    u16 itemCommon;
    u16 itemRare;
    u8 genderRatio;
    u8 eggCycles;
    u8 friendship;
    u8 growthRate;
    u8 eggGroups[2];
    u8 abilities[2];
    u8 safariZoneFleeRate;
    u8 bodyColor;
    bool8 noFlip;
};

struct ModLevelUpMoveDefinition
{
    u8 level;
    u16 move;
} __attribute__((packed));

struct ModPokemonDataDefinition
{
    const char *key;
    u16 species;
    s16 priority;
    u16 flags;
    struct ModSpeciesInfoOverride info;
    u8 levelUpMoveCount;
    struct ModLevelUpMoveDefinition levelUpMoves[MOD_POKEMON_MAX_LEVEL_UP_MOVES];
    u8 evolutionCount;
    struct Evolution evolutions[MOD_POKEMON_MAX_EVOLUTIONS];
};

bool8 PokemonDataApi_IsDefinitionValid(const struct ModPokemonDataDefinition *definition, bool8 allowEmptyDefault);
const struct SpeciesInfo *PokemonDataApi_GetSpeciesInfo(u16 species);
const u16 *PokemonDataApi_GetLevelUpLearnset(u16 species);
const struct Evolution *PokemonDataApi_GetEvolutions(u16 species);
u8 PokemonDataApi_GetAbilityBySpecies(u16 species, u8 abilityNum);

#endif // GUARD_MOD_POKEMON_DATA_H
