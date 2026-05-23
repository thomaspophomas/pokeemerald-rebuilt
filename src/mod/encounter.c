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
    u8 i;

    if (definition == NULL)
        return FALSE;
    if (definition->key != NULL || definition->hookKey != NULL || definition->hook != NULL)
        return FALSE;
    if (definition->mapGroup != 0 || definition->mapNum != 0 || definition->area != 0 || definition->rodMask != 0)
        return FALSE;
    if (definition->encounterRate != 0 || definition->slotCount != 0 || definition->priority != 0 || definition->flags != 0)
        return FALSE;
    for (i = 0; i < MOD_ENCOUNTER_MAX_SLOTS; i++)
    {
        if (definition->slots[i].species != SPECIES_NONE || definition->slots[i].weight != 0)
            return FALSE;
    }
    return TRUE;
}

static bool8 DefinitionMatchesContext(const struct ModEncounterDefinition *definition, u8 mapGroup, u8 mapNum, u8 area, u8 rod)
{
    if (!EncounterApi_IsDefinitionValid(definition, TRUE) || DefinitionIsEmptyDefault(definition))
        return FALSE;
    if (definition->mapGroup != mapGroup || definition->mapNum != mapNum || definition->area != area)
        return FALSE;
    if (area == MOD_ENCOUNTER_AREA_FISHING && (definition->rodMask & RodToMask(rod)) == 0)
        return FALSE;
    return TRUE;
}

static bool8 RuntimeEncounterKeyExists(const char *key, const struct ModEncounterDefinition *encounters, u16 count)
{
    u16 i;

    if (key == NULL)
        return FALSE;

    for (i = 0; i < count; i++)
    {
        if (encounters[i].key != NULL && strcmp(encounters[i].key, key) == 0)
            return TRUE;
    }

    return FALSE;
}

static const struct ModEncounterDefinition *FindBestEncounter(
    const struct ModEncounterDefinition *encounters,
    u16 count,
    u8 mapGroup,
    u8 mapNum,
    u8 area,
    u8 rod,
    const struct ModEncounterDefinition *shadowingEncounters,
    u16 shadowingCount)
{
    const struct ModEncounterDefinition *best = NULL;
    u16 i;

    if (encounters == NULL)
        return NULL;

    for (i = 0; i < count; i++)
    {
        const struct ModEncounterDefinition *definition = &encounters[i];

        if (shadowingEncounters != NULL && RuntimeEncounterKeyExists(definition->key, shadowingEncounters, shadowingCount))
            continue;
        if (!DefinitionMatchesContext(definition, mapGroup, mapNum, area, rod))
            continue;
        if (best == NULL || definition->priority < best->priority)
            best = definition;
    }

    return best;
}

static const struct ModEncounterDefinition *FindCurrentEncounter(u8 area, u8 rod)
{
    const struct ModEncounterDefinition *runtimeEncounters;
    const struct ModEncounterDefinition *best;
    u16 runtimeCount;
    u8 mapGroup;
    u8 mapNum;

    if (gSaveBlock1Ptr == NULL)
        return NULL;

    mapGroup = gSaveBlock1Ptr->location.mapGroup;
    mapNum = gSaveBlock1Ptr->location.mapNum;
    runtimeEncounters = ModRuntimeProfile_GetEncounters(&runtimeCount);
    best = FindBestEncounter(runtimeEncounters, runtimeCount, mapGroup, mapNum, area, rod, NULL, 0);
    if (best != NULL)
        return best;

    return FindBestEncounter(gModEncounterDefinitions, gModEncounterDefinitionCount, mapGroup, mapNum, area, rod, runtimeEncounters, runtimeCount);
}

static bool8 SlotMatchesContext(const struct ModEncounterSlot *slot, u8 area, u8 rod)
{
    if (slot->weight == 0 || slot->species == SPECIES_NONE)
        return FALSE;
    if (area == MOD_ENCOUNTER_AREA_FISHING && (slot->rodMask & RodToMask(rod)) == 0)
        return FALSE;
    return TRUE;
}

static bool8 PickWeightedSlot(const struct ModEncounterDefinition *definition, u8 area, u8 rod, u8 *slotIndex)
{
    u16 totalWeight = 0;
    u16 roll;
    u8 i;

    for (i = 0; i < definition->slotCount; i++)
    {
        if (SlotMatchesContext(&definition->slots[i], area, rod))
            totalWeight += definition->slots[i].weight;
    }
    if (totalWeight == 0)
        return FALSE;

    roll = Random() % totalWeight;
    for (i = 0; i < definition->slotCount; i++)
    {
        if (!SlotMatchesContext(&definition->slots[i], area, rod))
            continue;
        if (roll < definition->slots[i].weight)
        {
            *slotIndex = i;
            return TRUE;
        }
        roll -= definition->slots[i].weight;
    }

    return FALSE;
}

static u8 PickLevel(const struct ModEncounterSlot *slot)
{
    u8 minLevel = slot->minLevel;
    u8 maxLevel = slot->maxLevel;

    if (maxLevel < minLevel)
    {
        u8 temp = maxLevel;
        maxLevel = minLevel;
        minLevel = temp;
    }
    return minLevel + (Random() % (maxLevel - minLevel + 1));
}

static u8 RunHook(const struct ModEncounterDefinition *definition, struct ModEncounterContext *context)
{
    if (definition->hook == NULL)
        return MOD_ENCOUNTER_HOOK_CONTINUE;
    return definition->hook(definition, context);
}

bool8 EncounterApi_IsDefinitionValid(const struct ModEncounterDefinition *definition, bool8 allowEmptyDefault)
{
    u8 i;

    if (definition == NULL)
        return FALSE;
    if (allowEmptyDefault && DefinitionIsEmptyDefault(definition))
        return TRUE;
    if (definition->key == NULL || definition->key[0] == '\0')
        return FALSE;
    if (definition->area > MOD_ENCOUNTER_AREA_FISHING)
        return FALSE;
    if (definition->rodMask == 0 || (definition->rodMask & ~MOD_ENCOUNTER_ROD_ALL) != 0)
        return FALSE;
    if (definition->slotCount > MOD_ENCOUNTER_MAX_SLOTS)
        return FALSE;
    if (definition->slotCount == 0 && definition->hook == NULL)
        return FALSE;

    for (i = 0; i < definition->slotCount; i++)
    {
        const struct ModEncounterSlot *slot = &definition->slots[i];

        if (slot->species == SPECIES_NONE || slot->species >= NUM_SPECIES)
            return FALSE;
        if (slot->minLevel == 0 || slot->minLevel > 100 || slot->maxLevel == 0 || slot->maxLevel > 100)
            return FALSE;
        if (slot->weight == 0)
            return FALSE;
        if (slot->rodMask == 0 || (slot->rodMask & ~MOD_ENCOUNTER_ROD_ALL) != 0)
            return FALSE;
    }

    return TRUE;
}

bool8 EncounterApi_HasDefinition(u8 area, u8 rod)
{
    return FindCurrentEncounter(area, rod) != NULL;
}

u16 EncounterApi_GetEncounterRate(u8 area, u8 rod, u16 vanillaRate)
{
    const struct ModEncounterDefinition *definition = FindCurrentEncounter(area, rod);
    struct ModEncounterContext context;
    u8 result;

    if (definition == NULL)
        return vanillaRate;

    memset(&context, 0, sizeof(context));
    context.mapGroup = gSaveBlock1Ptr->location.mapGroup;
    context.mapNum = gSaveBlock1Ptr->location.mapNum;
    context.area = area;
    context.rod = rod;
    context.phase = MOD_ENCOUNTER_PHASE_RATE;
    context.encounterRate = definition->encounterRate;

    result = RunHook(definition, &context);
    if (result == MOD_ENCOUNTER_HOOK_CANCEL)
        return 0;
    return context.encounterRate;
}

bool8 EncounterApi_TrySelectWildMon(u8 area, u8 rod, u8 flags, struct WildPokemon *wildPokemon)
{
    const struct ModEncounterDefinition *definition = FindCurrentEncounter(area, rod);
    const struct ModEncounterSlot *slot = NULL;
    struct ModEncounterContext context;
    u8 slotIndex = 0;
    u8 result;

    if (definition == NULL || wildPokemon == NULL)
        return FALSE;

    memset(&context, 0, sizeof(context));
    context.mapGroup = gSaveBlock1Ptr->location.mapGroup;
    context.mapNum = gSaveBlock1Ptr->location.mapNum;
    context.area = area;
    context.rod = rod;
    context.phase = MOD_ENCOUNTER_PHASE_SELECT;
    context.flags = flags;
    context.slotIndex = 0xFF;

    if (PickWeightedSlot(definition, area, rod, &slotIndex))
    {
        slot = &definition->slots[slotIndex];
        context.slotIndex = slotIndex;
        context.species = slot->species;
        context.level = PickLevel(slot);
    }

    result = RunHook(definition, &context);
    if (result == MOD_ENCOUNTER_HOOK_CANCEL)
        return FALSE;
    if (context.species == SPECIES_NONE || context.species >= NUM_SPECIES || context.level == 0 || context.level > 100)
        return FALSE;

    wildPokemon->species = context.species;
    wildPokemon->minLevel = context.level;
    wildPokemon->maxLevel = context.level;
    return TRUE;
}

ModEncounterHook EncounterApi_FindCompiledHook(const char *sourceKey, const char *hookKey)
{
    u16 i;

    if (sourceKey == NULL || hookKey == NULL)
        return NULL;

    for (i = 0; i < gModEncounterDefinitionCount; i++)
    {
        if (gModEncounterDefinitions[i].key == NULL || gModEncounterDefinitions[i].hookKey == NULL)
            continue;
        if (strcmp(gModEncounterDefinitions[i].key, sourceKey) == 0
         && strcmp(gModEncounterDefinitions[i].hookKey, hookKey) == 0)
            return gModEncounterDefinitions[i].hook;
    }

    return NULL;
}
