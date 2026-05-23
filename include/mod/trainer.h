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
    u16 heldItem;
    u16 moves[MAX_MON_MOVES];
} __attribute__((packed));

struct ModTrainerDefinition
{
    const char *key;
    u16 trainerId;
    s16 priority;
    u16 flags;
    u8 trainerClass;
    u8 encounterMusicGender;
    u8 trainerPic;
    u16 items[MAX_TRAINER_ITEMS];
    bool8 doubleBattle;
    u32 aiFlags;
    u8 partySize;
    struct ModTrainerMon party[MOD_TRAINER_MAX_PARTY_SIZE];
};

bool8 TrainerApi_IsDefinitionValid(const struct ModTrainerDefinition *definition, bool8 allowEmptyDefault);
const struct ModTrainerDefinition *TrainerApi_GetDefinition(u16 trainerId);
u8 TrainerApi_GetPartySize(u16 trainerId, u8 vanilla);
u8 TrainerApi_GetPartyLevel(u16 trainerId, u8 partyIndex, u8 vanilla);
bool8 TrainerApi_CreateParty(u16 trainerId, struct Pokemon *party, u8 maxCount, u8 *partySize);
bool8 TrainerApi_IsDoubleBattle(u16 trainerId, bool8 vanilla);
u8 TrainerApi_GetTrainerClass(u16 trainerId, u8 vanilla);

#endif // GUARD_MOD_TRAINER_H
