#include "global.h"
#include "constants/species.h"
#include "generated/mod_registry.h"
#include "mod/encounter.h"
#include "mod/runtime_profile.h"
#include "random.h"
#include "string_util.h"

static u8 RodToMask(u8 rod)
{
    if (rod > SUPER_ROD)
        return 0;
    return 1 << rod;
}

static bool8 DefinitionIsEmptyDefault(const struct ModEncounterDefinition *definition)
{
    u8 encounter_slot_index;

    if (definition == NULL)
        return FALSE;
    if (definition->key != NULL || definition->hook_key != NULL || definition->hook != NULL)
        return FALSE;
    if (definition->map_group != 0 || definition->map_num != 0 || definition->area != 0 || definition->rod_mask != 0)
        return FALSE;
    if (definition->encounter_rate != 0 || definition->slot_count != 0 || definition->priority != 0 || definition->flags != 0)
        return FALSE;
    for (encounter_slot_index = 0; encounter_slot_index < MOD_ENCOUNTER_MAX_SLOTS; encounter_slot_index++)
    {
        if (definition->slots[encounter_slot_index].species != SPECIES_NONE || definition->slots[encounter_slot_index].weight != 0)
            return FALSE;
    }
    return TRUE;
}

static bool8 DefinitionMatchesContext(const struct ModEncounterDefinition *definition, u8 map_group, u8 map_num, u8 area, u8 rod)
{
    if (!EncounterApi_IsDefinitionValid(definition, TRUE) || DefinitionIsEmptyDefault(definition))
        return FALSE;
    if (definition->map_group != map_group || definition->map_num != map_num || definition->area != area)
        return FALSE;
    if (area == MOD_ENCOUNTER_AREA_FISHING && (definition->rod_mask & RodToMask(rod)) == 0)
        return FALSE;
    return TRUE;
}

static bool8 RuntimeEncounterKeyExists(const char *key, const struct ModEncounterDefinition *encounters, u16 count)
{
    u16 encounter_index;

    if (key == NULL)
        return FALSE;

    for (encounter_index = 0; encounter_index < count; encounter_index++)
    {
        if (encounters[encounter_index].key != NULL && strcmp(encounters[encounter_index].key, key) == 0)
            return TRUE;
    }

    return FALSE;
}

static const struct ModEncounterDefinition *FindBestEncounter(
    const struct ModEncounterDefinition *encounters,
    u16 count,
    u8 map_group,
    u8 map_num,
    u8 area,
    u8 rod,
    const struct ModEncounterDefinition *shadowingEncounters,
    u16 shadowingCount)
{
    const struct ModEncounterDefinition *best_definition = NULL;
    u16 encounter_index;

    if (encounters == NULL)
        return NULL;

    for (encounter_index = 0; encounter_index < count; encounter_index++)
    {
        const struct ModEncounterDefinition *definition = &encounters[encounter_index];

        if (shadowingEncounters != NULL && RuntimeEncounterKeyExists(definition->key, shadowingEncounters, shadowingCount))
            continue;
        if (!DefinitionMatchesContext(definition, map_group, map_num, area, rod))
            continue;
        if (best_definition == NULL || definition->priority < best_definition->priority)
            best_definition = definition;
    }

    return best_definition;
}

static const struct ModEncounterDefinition *FindCurrentEncounter(u8 area, u8 rod)
{
    const struct ModEncounterDefinition *runtimeEncounters;
    const struct ModEncounterDefinition *best;
    u16 runtimeCount;
    u8 map_group;
    u8 map_num;

    if (gSaveBlock1Ptr == NULL)
        return NULL;

    map_group = gSaveBlock1Ptr->location.mapGroup;
    map_num = gSaveBlock1Ptr->location.mapNum;
    runtimeEncounters = ModRuntimeProfile_GetEncounters(&runtimeCount);
    best = FindBestEncounter(runtimeEncounters, runtimeCount, map_group, map_num, area, rod, NULL, 0);
    if (best != NULL)
        return best;

    return FindBestEncounter(gModEncounterDefinitions, gModEncounterDefinitionCount, map_group, map_num, area, rod, runtimeEncounters, runtimeCount);
}

static bool8 EncounterSlotMatchesContext(const struct ModEncounterSlot *encounter_slot, u8 area, u8 rod)
{
    if (encounter_slot->weight == 0 || encounter_slot->species == SPECIES_NONE)
        return FALSE;
    if (area == MOD_ENCOUNTER_AREA_FISHING && (encounter_slot->rod_mask & RodToMask(rod)) == 0)
        return FALSE;
    return TRUE;
}

static bool8 PickWeightedSlot(const struct ModEncounterDefinition *definition, u8 area, u8 rod, u8 *selected_slot_index)
{
    u16 totalWeight = 0;
    u16 roll;
    u8 encounter_slot_index;

    for (encounter_slot_index = 0; encounter_slot_index < definition->slot_count; encounter_slot_index++)
    {
        if (EncounterSlotMatchesContext(&definition->slots[encounter_slot_index], area, rod))
            totalWeight += definition->slots[encounter_slot_index].weight;
    }
    if (totalWeight == 0)
        return FALSE;

    roll = Random() % totalWeight;
    for (encounter_slot_index = 0; encounter_slot_index < definition->slot_count; encounter_slot_index++)
    {
        if (!EncounterSlotMatchesContext(&definition->slots[encounter_slot_index], area, rod))
            continue;
        if (roll < definition->slots[encounter_slot_index].weight)
        {
            *selected_slot_index = encounter_slot_index;
            return TRUE;
        }
        roll -= definition->slots[encounter_slot_index].weight;
    }

    return FALSE;
}

static u8 PickEncounterLevel(const struct ModEncounterSlot *encounter_slot)
{
    u8 lower_level_bound = encounter_slot->min_level;
    u8 upper_level_bound = encounter_slot->max_level;

    if (upper_level_bound < lower_level_bound)
    {
        u8 original_upper_level_bound = upper_level_bound;
        upper_level_bound = lower_level_bound;
        lower_level_bound = original_upper_level_bound;
    }
    return lower_level_bound + (Random() % (upper_level_bound - lower_level_bound + 1));
}

static u8 RunHook(const struct ModEncounterDefinition *definition, struct ModEncounterContext *context)
{
    if (definition->hook == NULL)
        return MOD_ENCOUNTER_HOOK_CONTINUE;
    return definition->hook(definition, context);
}

bool8 EncounterApi_IsDefinitionValid(const struct ModEncounterDefinition *definition, bool8 allow_empty_default)
{
    u8 encounter_slot_index;

    if (definition == NULL)
        return FALSE;
    if (allow_empty_default && DefinitionIsEmptyDefault(definition))
        return TRUE;
    if (definition->key == NULL || definition->key[0] == '\0')
        return FALSE;
    if (definition->area > MOD_ENCOUNTER_AREA_FISHING)
        return FALSE;
    if (definition->rod_mask == 0 || (definition->rod_mask & ~MOD_ENCOUNTER_ROD_ALL) != 0)
        return FALSE;
    if (definition->slot_count > MOD_ENCOUNTER_MAX_SLOTS)
        return FALSE;
    if (definition->slot_count == 0 && definition->hook == NULL)
        return FALSE;

    for (encounter_slot_index = 0; encounter_slot_index < definition->slot_count; encounter_slot_index++)
    {
        const struct ModEncounterSlot *encounter_slot = &definition->slots[encounter_slot_index];

        if (encounter_slot->species == SPECIES_NONE || encounter_slot->species >= NUM_SPECIES)
            return FALSE;
        if (encounter_slot->min_level == 0 || encounter_slot->min_level > 100 || encounter_slot->max_level == 0 || encounter_slot->max_level > 100)
            return FALSE;
        if (encounter_slot->weight == 0)
            return FALSE;
        if (encounter_slot->rod_mask == 0 || (encounter_slot->rod_mask & ~MOD_ENCOUNTER_ROD_ALL) != 0)
            return FALSE;
    }

    return TRUE;
}

bool8 EncounterApi_HasDefinition(u8 area, u8 rod)
{
    return FindCurrentEncounter(area, rod) != NULL;
}

u16 EncounterApi_GetEncounterRate(u8 area, u8 rod, u16 vanilla_encounter_rate)
{
    const struct ModEncounterDefinition *definition = FindCurrentEncounter(area, rod);
    struct ModEncounterContext context;
    u8 encounter_hook_result;

    if (definition == NULL)
        return vanilla_encounter_rate;

    memset(&context, 0, sizeof(context));
    context.map_group = gSaveBlock1Ptr->location.mapGroup;
    context.map_num = gSaveBlock1Ptr->location.mapNum;
    context.area = area;
    context.rod = rod;
    context.phase = MOD_ENCOUNTER_PHASE_RATE;
    context.encounter_rate = definition->encounter_rate;

    encounter_hook_result = RunHook(definition, &context);
    if (encounter_hook_result == MOD_ENCOUNTER_HOOK_CANCEL)
        return 0;
    return context.encounter_rate;
}

bool8 EncounterApi_TrySelectWildMon(u8 area, u8 rod, u8 flags, struct WildPokemon *wild_pokemon)
{
    const struct ModEncounterDefinition *definition = FindCurrentEncounter(area, rod);
    const struct ModEncounterSlot *selected_encounter_slot = NULL;
    struct ModEncounterContext context;
    u8 selected_slot_index = 0;
    u8 encounter_hook_result;

    if (definition == NULL || wild_pokemon == NULL)
        return FALSE;

    memset(&context, 0, sizeof(context));
    context.map_group = gSaveBlock1Ptr->location.mapGroup;
    context.map_num = gSaveBlock1Ptr->location.mapNum;
    context.area = area;
    context.rod = rod;
    context.phase = MOD_ENCOUNTER_PHASE_SELECT;
    context.flags = flags;
    context.slot_index = 0xFF;

    if (PickWeightedSlot(definition, area, rod, &selected_slot_index))
    {
        selected_encounter_slot = &definition->slots[selected_slot_index];
        context.slot_index = selected_slot_index;
        context.species = selected_encounter_slot->species;
        context.level = PickEncounterLevel(selected_encounter_slot);
    }

    encounter_hook_result = RunHook(definition, &context);
    if (encounter_hook_result == MOD_ENCOUNTER_HOOK_CANCEL)
        return FALSE;
    if (context.species == SPECIES_NONE || context.species >= NUM_SPECIES || context.level == 0 || context.level > 100)
        return FALSE;

    wild_pokemon->species = context.species;
    wild_pokemon->minLevel = context.level;
    wild_pokemon->maxLevel = context.level;
    return TRUE;
}

ModEncounterHook EncounterApi_FindCompiledHook(const char *source_key, const char *hook_key)
{
    u16 encounter_index;

    if (source_key == NULL || hook_key == NULL)
        return NULL;

    for (encounter_index = 0; encounter_index < gModEncounterDefinitionCount; encounter_index++)
    {
        if (gModEncounterDefinitions[encounter_index].key == NULL || gModEncounterDefinitions[encounter_index].hook_key == NULL)
            continue;
        if (strcmp(gModEncounterDefinitions[encounter_index].key, source_key) == 0
         && strcmp(gModEncounterDefinitions[encounter_index].hook_key, hook_key) == 0)
            return gModEncounterDefinitions[encounter_index].hook;
    }

    return NULL;
}
