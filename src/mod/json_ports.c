#include "global.h"
#include "mod/domain.h"
#include "mod/item.h"
#include "mod/move.h"
#include "mod/pokemon.h"
#include "mod/quest.h"
#include "mod/shop.h"
#include "mod/trainer.h"
#include "mod/trainer_party.h"
#include "mod/wild_encounter.h"

u16 TrainerApi_GetFileCount(void)
{
    return ModDomainApi_CountFiles("trainers");
}

const struct ModDomainFileDefinition *TrainerApi_GetFileByIndex(u16 index)
{
    return ModDomainApi_GetFileByIndex("trainers", index);
}

const struct ModDomainFileDefinition *TrainerApi_FindFile(const char *key)
{
    return ModDomainApi_FindFile("trainers", key);
}

u16 TrainerPartyApi_GetFileCount(void)
{
    return ModDomainApi_CountFiles("trainer_parties");
}

const struct ModDomainFileDefinition *TrainerPartyApi_GetFileByIndex(u16 index)
{
    return ModDomainApi_GetFileByIndex("trainer_parties", index);
}

const struct ModDomainFileDefinition *TrainerPartyApi_FindFile(const char *key)
{
    return ModDomainApi_FindFile("trainer_parties", key);
}

u16 QuestApi_GetFileCount(void)
{
    return ModDomainApi_CountFiles("quests");
}

const struct ModDomainFileDefinition *QuestApi_GetFileByIndex(u16 index)
{
    return ModDomainApi_GetFileByIndex("quests", index);
}

const struct ModDomainFileDefinition *QuestApi_FindFile(const char *key)
{
    return ModDomainApi_FindFile("quests", key);
}

u16 ItemApi_GetFileCount(void)
{
    return ModDomainApi_CountFiles("items");
}

const struct ModDomainFileDefinition *ItemApi_GetFileByIndex(u16 index)
{
    return ModDomainApi_GetFileByIndex("items", index);
}

const struct ModDomainFileDefinition *ItemApi_FindFile(const char *key)
{
    return ModDomainApi_FindFile("items", key);
}

u16 PokemonApi_GetFileCount(void)
{
    return ModDomainApi_CountFiles("pokemon");
}

const struct ModDomainFileDefinition *PokemonApi_GetFileByIndex(u16 index)
{
    return ModDomainApi_GetFileByIndex("pokemon", index);
}

const struct ModDomainFileDefinition *PokemonApi_FindFile(const char *key)
{
    return ModDomainApi_FindFile("pokemon", key);
}

u16 MoveApi_GetFileCount(void)
{
    return ModDomainApi_CountFiles("moves");
}

const struct ModDomainFileDefinition *MoveApi_GetFileByIndex(u16 index)
{
    return ModDomainApi_GetFileByIndex("moves", index);
}

const struct ModDomainFileDefinition *MoveApi_FindFile(const char *key)
{
    return ModDomainApi_FindFile("moves", key);
}

u16 ShopApi_GetFileCount(void)
{
    return ModDomainApi_CountFiles("shops");
}

const struct ModDomainFileDefinition *ShopApi_GetFileByIndex(u16 index)
{
    return ModDomainApi_GetFileByIndex("shops", index);
}

const struct ModDomainFileDefinition *ShopApi_FindFile(const char *key)
{
    return ModDomainApi_FindFile("shops", key);
}

u16 WildEncounterApi_GetFileCount(void)
{
    return ModDomainApi_CountFiles("wild_encounters");
}

const struct ModDomainFileDefinition *WildEncounterApi_GetFileByIndex(u16 index)
{
    return ModDomainApi_GetFileByIndex("wild_encounters", index);
}

const struct ModDomainFileDefinition *WildEncounterApi_FindFile(const char *key)
{
    return ModDomainApi_FindFile("wild_encounters", key);
}
