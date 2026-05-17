#ifndef GUARD_MOD_TRAINER_H
#define GUARD_MOD_TRAINER_H

#include "global.h"
#include "mod/domain.h"

u16 TrainerApi_GetFileCount(void);
const struct ModDomainFileDefinition *TrainerApi_GetFileByIndex(u16 index);
const struct ModDomainFileDefinition *TrainerApi_FindFile(const char *key);

#endif // GUARD_MOD_TRAINER_H
