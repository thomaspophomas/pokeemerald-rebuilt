#ifndef GUARD_MOD_SHOP_H
#define GUARD_MOD_SHOP_H

#include "global.h"
#include "mod/domain.h"

u16 ShopApi_GetFileCount(void);
const struct ModDomainFileDefinition *ShopApi_GetFileByIndex(u16 index);
const struct ModDomainFileDefinition *ShopApi_FindFile(const char *key);

#endif // GUARD_MOD_SHOP_H
