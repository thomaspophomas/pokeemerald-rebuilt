#ifndef GUARD_MOD_DOMAIN_H
#define GUARD_MOD_DOMAIN_H

#include "global.h"
#include "mod/types.h"

u16 ModDomainApi_CountFiles(const char *domain);
const struct ModDomainFileDefinition *ModDomainApi_GetFileByIndex(const char *domain, u16 index);
const struct ModDomainFileDefinition *ModDomainApi_FindFile(const char *domain, const char *key);

#endif // GUARD_MOD_DOMAIN_H
