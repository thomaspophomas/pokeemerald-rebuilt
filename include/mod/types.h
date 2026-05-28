#ifndef GUARD_MOD_TYPES_H
#define GUARD_MOD_TYPES_H

#include "mod/base.h"

#define MOD_ID_NONE 0xFFFF
#define MOD_NAME_NONE NULL

struct ModManifest
{
    const char *mod_id;
    const char *name;
    const char *version;
    u16 priority;
    u16 feature_flags;
    u16 state_version;
    u16 state_bytes;
    u16 state_offset;
};

struct ModNamedResource
{
    const char *key;
    u16 resource_id;
};

#endif // GUARD_MOD_TYPES_H
