#include "global.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/species.h"
#include "constants/trainers.h"
#include "generated/mod_registry.h"
#include "mod/battle_data.h"
#include "mod/runtime_profile.h"
#include "mod/trainer.h"
#include "pokemon.h"

static bool8 DefinitionIsEmptyDefault(const struct ModTrainerDefinition *definition)
{
    return definition->key == NULL
        && definition->trainerId == 0
        && definition->priority == 0
        && definition->flags == 0
        && definition->partySize == 0;
}

static bool8 RuntimeTrainerKeyExists(const char *key, const struct ModTrainerDefinition *trainers, u16 count)
{
    u16 i;

    if (key == NULL || trainers == NULL)
        return FALSE;

    for (i = 0; i < count; i++)
    {
        if (trainers[i].key != NULL && strcmp(trainers[i].key, key) == 0)
            return TRUE;
    }

    return FALSE;
}

static const struct ModTrainerDefinition *FindBestTrainer(
    const struct ModTrainerDefinition *trainers,
    u16 count,
    u16 trainerId,
    const struct ModTrainerDefinition *shadowingTrainers,
    u16 shadowingCount)
{
    const struct ModTrainerDefinition *best = NULL;
    u16 i;

    if (trainers == NULL)
        return NULL;

    for (i = 0; i < count; i++)
    {
        const struct ModTrainerDefinition *definition = &trainers[i];

        if (shadowingTrainers != NULL && RuntimeTrainerKeyExists(definition->key, shadowingTrainers, shadowingCount))
            continue;
        if (!TrainerApi_IsDefinitionValid(definition, TRUE) || DefinitionIsEmptyDefault(definition))
            continue;
        if (definition->trainerId != trainerId)
            continue;
        if (best == NULL || definition->priority < best->priority)
            best = definition;
    }

    return best;
}

const struct ModTrainerDefinition *TrainerApi_GetDefinition(u16 trainerId)
{
    const struct ModTrainerDefinition *runtimeTrainers;
    const struct ModTrainerDefinition *best;
    u16 runtimeCount;

    runtimeTrainers = ModRuntimeProfile_GetTrainers(&runtimeCount);
    best = FindBestTrainer(runtimeTrainers, runtimeCount, trainerId, NULL, 0);
    if (best != NULL)
        return best;

    return FindBestTrainer(gModTrainerDefinitions, gModTrainerDefinitionCount, trainerId, runtimeTrainers, runtimeCount);
}

bool8 TrainerApi_IsDefinitionValid(const struct ModTrainerDefinition *definition, bool8 allowEmptyDefault)
{
    u8 i;
    u8 j;

    if (definition == NULL)
        return FALSE;
    if (allowEmptyDefault && DefinitionIsEmptyDefault(definition))
        return TRUE;
    if (definition->key == NULL || definition->key[0] == '\0')
        return FALSE;
    if ((definition->flags & MOD_TRAINER_OVERRIDE_PARTY) && (definition->partySize == 0 || definition->partySize > MOD_TRAINER_MAX_PARTY_SIZE))
        return FALSE;
    for (i = 0; i < definition->partySize; i++)
    {
        if (definition->party[i].lvl == 0 || definition->party[i].lvl > MAX_LEVEL)
            return FALSE;
        if (definition->party[i].species == SPECIES_NONE || definition->party[i].species >= NUM_SPECIES)
            return FALSE;
        for (j = 0; j < MAX_MON_MOVES; j++)
        {
            if (definition->party[i].moves[j] >= MOVES_COUNT)
                return FALSE;
        }
    }
    return TRUE;
}

u8 TrainerApi_GetPartySize(u16 trainerId, u8 vanilla)
{
    const struct ModTrainerDefinition *definition = TrainerApi_GetDefinition(trainerId);

    if (definition != NULL && (definition->flags & MOD_TRAINER_OVERRIDE_PARTY))
        return definition->partySize;
    return vanilla;
}

u8 TrainerApi_GetPartyLevel(u16 trainerId, u8 partyIndex, u8 vanilla)
{
    const struct ModTrainerDefinition *definition = TrainerApi_GetDefinition(trainerId);

    if (definition != NULL && (definition->flags & MOD_TRAINER_OVERRIDE_PARTY) && partyIndex < definition->partySize)
        return definition->party[partyIndex].lvl;
    return vanilla;
}

bool8 TrainerApi_CreateParty(u16 trainerId, struct Pokemon *party, u8 maxCount, u8 *partySize)
{
    const struct ModTrainerDefinition *definition = TrainerApi_GetDefinition(trainerId);
    u8 count;
    u8 i;
    u8 j;

    if (definition == NULL || !(definition->flags & MOD_TRAINER_OVERRIDE_PARTY) || party == NULL)
        return FALSE;

    count = definition->partySize;
    if (count > maxCount)
        count = maxCount;

    for (i = 0; i < count; i++)
    {
        const struct ModTrainerMon *mon = &definition->party[i];
        u8 fixedIV = mon->iv * MAX_PER_STAT_IVS / 255;

        CreateMon(&party[i], mon->species, mon->lvl, fixedIV, TRUE, 0x88 + (i << 8), OT_ID_RANDOM_NO_SHINY, 0);
        if (mon->heldItem != ITEM_NONE)
            SetMonData(&party[i], MON_DATA_HELD_ITEM, &mon->heldItem);
        for (j = 0; j < MAX_MON_MOVES; j++)
        {
            u8 pp;

            if (mon->moves[j] == MOVE_NONE)
                continue;
            SetMonData(&party[i], MON_DATA_MOVE1 + j, &mon->moves[j]);
            pp = BattleDataApi_GetMovePP(mon->moves[j]);
            SetMonData(&party[i], MON_DATA_PP1 + j, &pp);
        }
    }

    if (partySize != NULL)
        *partySize = definition->partySize;
    return TRUE;
}

bool8 TrainerApi_IsDoubleBattle(u16 trainerId, bool8 vanilla)
{
    const struct ModTrainerDefinition *definition = TrainerApi_GetDefinition(trainerId);

    if (definition != NULL && (definition->flags & MOD_TRAINER_OVERRIDE_DOUBLE))
        return definition->doubleBattle;
    return vanilla;
}

u8 TrainerApi_GetTrainerClass(u16 trainerId, u8 vanilla)
{
    const struct ModTrainerDefinition *definition = TrainerApi_GetDefinition(trainerId);

    if (definition != NULL && (definition->flags & MOD_TRAINER_OVERRIDE_CLASS))
        return definition->trainerClass;
    return vanilla;
}
