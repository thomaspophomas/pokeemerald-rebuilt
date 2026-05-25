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
    u8 base_hp;
    u8 base_attack;
    u8 base_defense;
    u8 base_speed;
    u8 base_sp_attack;
    u8 base_sp_defense;
    u8 types[2];
    u8 catch_rate;
    u8 exp_yield;
    u8 ev_yields[6];
    u16 item_common;
    u16 item_rare;
    u8 gender_ratio;
    u8 egg_cycles;
    u8 friendship;
    u8 growth_rate;
    u8 egg_groups[2];
    u8 abilities[2];
    u8 safari_zone_flee_rate;
    u8 body_color;
    bool8 no_flip;
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
    u8 level_up_move_count;
    struct ModLevelUpMoveDefinition level_up_moves[MOD_POKEMON_MAX_LEVEL_UP_MOVES];
    u8 evolution_count;
    struct Evolution evolutions[MOD_POKEMON_MAX_EVOLUTIONS];
};

bool8 PokemonDataApi_IsDefinitionValid(const struct ModPokemonDataDefinition *definition, bool8 allow_empty_default);
const struct SpeciesInfo *PokemonDataApi_GetSpeciesInfo(u16 species);
const u16 *PokemonDataApi_GetLevelUpLearnset(u16 species);
const struct Evolution *PokemonDataApi_GetEvolutions(u16 species);
u8 PokemonDataApi_GetAbilityBySpecies(u16 species, u8 ability_number);

#endif // GUARD_MOD_POKEMON_DATA_H
