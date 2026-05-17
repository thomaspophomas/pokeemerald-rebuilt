#include "global.h"
#include "generated/mod_registry.h"
#include "mod/domain.h"

u16 ModDomainApi_CountFiles(const char *domain)
{
    u16 i;
    u16 count = 0;

    if (domain == NULL)
        return 0;

    for (i = 0; i < gModDomainFileCount; i++)
    {
        if (strcmp(gModDomainFiles[i].domain, domain) == 0)
            count++;
    }

    return count;
}

const struct ModDomainFileDefinition *ModDomainApi_GetFileByIndex(const char *domain, u16 index)
{
    u16 i;
    u16 seen = 0;

    if (domain == NULL)
        return NULL;

    for (i = 0; i < gModDomainFileCount; i++)
    {
        if (strcmp(gModDomainFiles[i].domain, domain) != 0)
            continue;
        if (seen == index)
            return &gModDomainFiles[i];
        seen++;
    }

    return NULL;
}

const struct ModDomainFileDefinition *ModDomainApi_FindFile(const char *domain, const char *key)
{
    u16 i;

    if (domain == NULL || key == NULL)
        return NULL;

    for (i = 0; i < gModDomainFileCount; i++)
    {
        if (strcmp(gModDomainFiles[i].domain, domain) == 0 && strcmp(gModDomainFiles[i].key, key) == 0)
            return &gModDomainFiles[i];
    }

    return NULL;
}
