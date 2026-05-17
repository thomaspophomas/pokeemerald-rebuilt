#ifndef GUARD_MOD_MOVE_H
#define GUARD_MOD_MOVE_H

#include "global.h"
#include "mod/domain.h"

u16 MoveApi_GetFileCount(void);
const struct ModDomainFileDefinition *MoveApi_GetFileByIndex(u16 index);
const struct ModDomainFileDefinition *MoveApi_FindFile(const char *key);

#endif // GUARD_MOD_MOVE_H
