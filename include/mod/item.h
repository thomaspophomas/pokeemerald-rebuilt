#ifndef GUARD_MOD_ITEM_H
#define GUARD_MOD_ITEM_H

#include "global.h"
#include "mod/domain.h"

u16 ItemApi_GetFileCount(void);
const struct ModDomainFileDefinition *ItemApi_GetFileByIndex(u16 index);
const struct ModDomainFileDefinition *ItemApi_FindFile(const char *key);

#endif // GUARD_MOD_ITEM_H
