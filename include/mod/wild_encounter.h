#ifndef GUARD_MOD_WILD_ENCOUNTER_H
#define GUARD_MOD_WILD_ENCOUNTER_H

#include "global.h"
#include "mod/domain.h"

u16 WildEncounterApi_GetFileCount(void);
const struct ModDomainFileDefinition *WildEncounterApi_GetFileByIndex(u16 index);
const struct ModDomainFileDefinition *WildEncounterApi_FindFile(const char *key);

#endif // GUARD_MOD_WILD_ENCOUNTER_H
