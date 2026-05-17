#ifndef GUARD_MOD_TRAINER_PARTY_H
#define GUARD_MOD_TRAINER_PARTY_H

#include "global.h"
#include "mod/domain.h"

u16 TrainerPartyApi_GetFileCount(void);
const struct ModDomainFileDefinition *TrainerPartyApi_GetFileByIndex(u16 index);
const struct ModDomainFileDefinition *TrainerPartyApi_FindFile(const char *key);

#endif // GUARD_MOD_TRAINER_PARTY_H
