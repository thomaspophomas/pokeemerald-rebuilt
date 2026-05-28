#ifndef GUARD_MOD_ENCOUNTER_H
#define GUARD_MOD_ENCOUNTER_H

#include "mod/base.h"
#include "constants/items.h"
#include "wild_encounter.h"

#define MOD_ENCOUNTER_MAX_SLOTS LAND_WILD_COUNT

#define MOD_ENCOUNTER_AREA_LAND       0
#define MOD_ENCOUNTER_AREA_WATER      1
#define MOD_ENCOUNTER_AREA_ROCK_SMASH 2
#define MOD_ENCOUNTER_AREA_FISHING    3

#define MOD_ENCOUNTER_ROD_OLD   (1 << OLD_ROD)
#define MOD_ENCOUNTER_ROD_GOOD  (1 << GOOD_ROD)
#define MOD_ENCOUNTER_ROD_SUPER (1 << SUPER_ROD)
#define MOD_ENCOUNTER_ROD_ALL   (MOD_ENCOUNTER_ROD_OLD | MOD_ENCOUNTER_ROD_GOOD | MOD_ENCOUNTER_ROD_SUPER)

enum ModEncounterPhase
{
    MOD_ENCOUNTER_PHASE_RATE,
    MOD_ENCOUNTER_PHASE_SELECT,
};

enum ModEncounterHookResult
{
    MOD_ENCOUNTER_HOOK_CONTINUE,
    MOD_ENCOUNTER_HOOK_OVERRIDE,
    MOD_ENCOUNTER_HOOK_CANCEL,
};

struct ModEncounterSlot
{
    u8 min_level;
    u8 max_level;
    u16 species;
    u8 weight;
    u8 rod_mask;
    u16 flags;
} __attribute__((packed));

struct ModEncounterContext
{
    u8 map_group;
    u8 map_num;
    u8 area;
    u8 rod;
    u8 phase;
    u8 flags;
    u8 slot_index;
    u8 level;
    u16 encounter_rate;
    u16 species;
};

struct ModEncounterDefinition;
typedef u8 (*ModEncounterHook)(const struct ModEncounterDefinition *definition, struct ModEncounterContext *context);

struct ModEncounterDefinition
{
    const char *key;
    const char *hook_key;
    u8 map_group;
    u8 map_num;
    u8 area;
    u8 rod_mask;
    u8 encounter_rate;
    u8 slot_count;
    s16 priority;
    u16 flags;
    ModEncounterHook hook;
    struct ModEncounterSlot slots[MOD_ENCOUNTER_MAX_SLOTS];
};

bool8 EncounterApi_IsDefinitionValid(const struct ModEncounterDefinition *definition, bool8 allow_empty_default);
bool8 EncounterApi_HasDefinition(u8 area, u8 rod);
u16 EncounterApi_GetEncounterRate(u8 area, u8 rod, u16 vanilla_encounter_rate);
bool8 EncounterApi_TrySelectWildMon(u8 area, u8 rod, u8 flags, struct WildPokemon *wild_pokemon);
ModEncounterHook EncounterApi_FindCompiledHook(const char *source_key, const char *hook_key);

#endif // GUARD_MOD_ENCOUNTER_H
