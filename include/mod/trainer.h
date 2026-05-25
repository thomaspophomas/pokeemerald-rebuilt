#ifndef GUARD_MOD_TRAINER_H
#define GUARD_MOD_TRAINER_H

#include "global.h"
#include "data.h"

#define MOD_TRAINER_MAX_PARTY_SIZE PARTY_SIZE

#define MOD_TRAINER_OVERRIDE_CLASS         (1 << 0)
#define MOD_TRAINER_OVERRIDE_PIC           (1 << 1)
#define MOD_TRAINER_OVERRIDE_MUSIC_GENDER  (1 << 2)
#define MOD_TRAINER_OVERRIDE_ITEMS         (1 << 3)
#define MOD_TRAINER_OVERRIDE_DOUBLE        (1 << 4)
#define MOD_TRAINER_OVERRIDE_AI            (1 << 5)
#define MOD_TRAINER_OVERRIDE_PARTY         (1 << 6)

struct ModTrainerMon
{
    u16 iv;
    u8 lvl;
    u16 species;
    u16 held_item;
    u16 moves[MAX_MON_MOVES];
} __attribute__((packed));

struct ModTrainerDefinition
{
    const char *key;
    u16 trainer_id;
    s16 priority;
    u16 flags;
    u8 trainer_class;
    u8 encounter_music_gender;
    u8 trainer_pic;
    u16 items[MAX_TRAINER_ITEMS];
    bool8 double_battle;
    u32 ai_flags;
    u8 party_size;
    struct ModTrainerMon party[MOD_TRAINER_MAX_PARTY_SIZE];
};

bool8 TrainerApi_IsDefinitionValid(const struct ModTrainerDefinition *definition, bool8 allow_empty_default);
const struct ModTrainerDefinition *TrainerApi_GetDefinition(u16 trainer_id);
u8 TrainerApi_GetPartySize(u16 trainer_id, u8 vanilla);
u8 TrainerApi_GetPartyLevel(u16 trainer_id, u8 party_index, u8 vanilla);
bool8 TrainerApi_CreateParty(u16 trainer_id, struct Pokemon *party, u8 max_count, u8 *party_size);
bool8 TrainerApi_IsDoubleBattle(u16 trainer_id, bool8 vanilla);
u8 TrainerApi_GetTrainerClass(u16 trainer_id, u8 vanilla);

#endif // GUARD_MOD_TRAINER_H
