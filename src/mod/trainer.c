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
        && definition->trainer_id == 0
        && definition->priority == 0
        && definition->flags == 0
        && definition->party_size == 0;
}

static bool8 RuntimeTrainerKeyExists(const char *key, const struct ModTrainerDefinition *trainers, u16 count)
{
    u16 trainer_index;

    if (key == NULL || trainers == NULL)
        return FALSE;

    for (trainer_index = 0; trainer_index < count; trainer_index++)
    {
        if (trainers[trainer_index].key != NULL && strcmp(trainers[trainer_index].key, key) == 0)
            return TRUE;
    }

    return FALSE;
}

static const struct ModTrainerDefinition *FindBestTrainer(
    const struct ModTrainerDefinition *trainers,
    u16 count,
    u16 trainer_id,
    const struct ModTrainerDefinition *shadowing_trainers,
    u16 shadowing_count)
{
    const struct ModTrainerDefinition *best_definition = NULL;
    u16 trainer_index;

    if (trainers == NULL)
        return NULL;

    for (trainer_index = 0; trainer_index < count; trainer_index++)
    {
        const struct ModTrainerDefinition *definition = &trainers[trainer_index];

        if (shadowing_trainers != NULL && RuntimeTrainerKeyExists(definition->key, shadowing_trainers, shadowing_count))
            continue;
        if (!TrainerApi_IsDefinitionValid(definition, TRUE) || DefinitionIsEmptyDefault(definition))
            continue;
        if (definition->trainer_id != trainer_id)
            continue;
        if (best_definition == NULL || definition->priority < best_definition->priority)
            best_definition = definition;
    }

    return best_definition;
}

const struct ModTrainerDefinition *TrainerApi_GetDefinition(u16 trainer_id)
{
    const struct ModTrainerDefinition *runtime_trainers;
    const struct ModTrainerDefinition *best;
    u16 runtime_count;

    runtime_trainers = ModRuntimeProfile_GetTrainers(&runtime_count);
    best = FindBestTrainer(runtime_trainers, runtime_count, trainer_id, NULL, 0);
    if (best != NULL)
        return best;

    return FindBestTrainer(gModTrainerDefinitions, gModTrainerDefinitionCount, trainer_id, runtime_trainers, runtime_count);
}

bool8 TrainerApi_IsDefinitionValid(const struct ModTrainerDefinition *definition, bool8 allow_empty_default)
{
    u8 party_mon_index;
    u8 move_index;

    if (definition == NULL)
        return FALSE;
    if (allow_empty_default && DefinitionIsEmptyDefault(definition))
        return TRUE;
    if (definition->key == NULL || definition->key[0] == '\0')
        return FALSE;
    if ((definition->flags & MOD_TRAINER_OVERRIDE_PARTY) && (definition->party_size == 0 || definition->party_size > MOD_TRAINER_MAX_PARTY_SIZE))
        return FALSE;
    for (party_mon_index = 0; party_mon_index < definition->party_size; party_mon_index++)
    {
        if (definition->party[party_mon_index].lvl == 0 || definition->party[party_mon_index].lvl > MAX_LEVEL)
            return FALSE;
        if (definition->party[party_mon_index].species == SPECIES_NONE || definition->party[party_mon_index].species >= NUM_SPECIES)
            return FALSE;
        for (move_index = 0; move_index < MAX_MON_MOVES; move_index++)
        {
            if (definition->party[party_mon_index].moves[move_index] >= MOVES_COUNT)
                return FALSE;
        }
    }
    return TRUE;
}

u8 TrainerApi_GetPartySize(u16 trainer_id, u8 vanilla)
{
    const struct ModTrainerDefinition *definition = TrainerApi_GetDefinition(trainer_id);

    if (definition != NULL && (definition->flags & MOD_TRAINER_OVERRIDE_PARTY))
        return definition->party_size;
    return vanilla;
}

u8 TrainerApi_GetPartyLevel(u16 trainer_id, u8 party_index, u8 vanilla)
{
    const struct ModTrainerDefinition *definition = TrainerApi_GetDefinition(trainer_id);

    if (definition != NULL && (definition->flags & MOD_TRAINER_OVERRIDE_PARTY) && party_index < definition->party_size)
        return definition->party[party_index].lvl;
    return vanilla;
}

bool8 TrainerApi_CreateParty(u16 trainer_id, struct Pokemon *party, u8 max_count, u8 *party_size)
{
    const struct ModTrainerDefinition *definition = TrainerApi_GetDefinition(trainer_id);
    u8 party_count_to_create;
    u8 party_mon_index;
    u8 move_index;

    if (definition == NULL || !(definition->flags & MOD_TRAINER_OVERRIDE_PARTY) || party == NULL)
        return FALSE;

    party_count_to_create = definition->party_size;
    if (party_count_to_create > max_count)
        party_count_to_create = max_count;

    for (party_mon_index = 0; party_mon_index < party_count_to_create; party_mon_index++)
    {
        const struct ModTrainerMon *trainer_party_mon_definition = &definition->party[party_mon_index];
        u8 fixed_iv = trainer_party_mon_definition->iv * MAX_PER_STAT_IVS / 255;

        CreateMon(&party[party_mon_index], trainer_party_mon_definition->species, trainer_party_mon_definition->lvl, fixed_iv, TRUE, 0x88 + (party_mon_index << 8), OT_ID_RANDOM_NO_SHINY, 0);
        if (trainer_party_mon_definition->held_item != ITEM_NONE)
            SetMonData(&party[party_mon_index], MON_DATA_HELD_ITEM, &trainer_party_mon_definition->held_item);
        for (move_index = 0; move_index < MAX_MON_MOVES; move_index++)
        {
            u8 pp;

            if (trainer_party_mon_definition->moves[move_index] == MOVE_NONE)
                continue;
            SetMonData(&party[party_mon_index], MON_DATA_MOVE1 + move_index, &trainer_party_mon_definition->moves[move_index]);
            pp = BattleDataApi_GetMovePP(trainer_party_mon_definition->moves[move_index]);
            SetMonData(&party[party_mon_index], MON_DATA_PP1 + move_index, &pp);
        }
    }

    if (party_size != NULL)
        *party_size = definition->party_size;
    return TRUE;
}

bool8 TrainerApi_IsDoubleBattle(u16 trainer_id, bool8 vanilla)
{
    const struct ModTrainerDefinition *definition = TrainerApi_GetDefinition(trainer_id);

    if (definition != NULL && (definition->flags & MOD_TRAINER_OVERRIDE_DOUBLE))
        return definition->double_battle;
    return vanilla;
}

u8 TrainerApi_GetTrainerClass(u16 trainer_id, u8 vanilla)
{
    const struct ModTrainerDefinition *definition = TrainerApi_GetDefinition(trainer_id);

    if (definition != NULL && (definition->flags & MOD_TRAINER_OVERRIDE_CLASS))
        return definition->trainer_class;
    return vanilla;
}
