#ifndef GUARD_MOD_TYPES_H
#define GUARD_MOD_TYPES_H

#include "global.h"

#define MOD_ID_NONE 0xFFFF
#define MOD_NAME_NONE NULL

struct ModManifest
{
    const char *id;
    const char *name;
    const char *version;
    u16 priority;
    u16 featureFlags;
};

struct ModNamedResource
{
    const char *key;
    u16 id;
};

struct ModDomainFileDefinition
{
    const char *domain;
    const char *key;
    const char *jsonPath;
};

#endif // GUARD_MOD_TYPES_H
