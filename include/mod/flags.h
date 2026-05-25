#ifndef GUARD_MOD_FLAGS_H
#define GUARD_MOD_FLAGS_H

#include "global.h"

#define MOD_FLAG_INVALID 0xFFFF
#define MOD_FLAG_VANILLA_BASE 0x8000
#define MOD_FLAG_TO_VANILLA(flag_id) ((flag_id) | MOD_FLAG_VANILLA_BASE)
#define MOD_FLAG_FROM_VANILLA(flag_id) ((flag_id) & ~MOD_FLAG_VANILLA_BASE)

struct ModFlagDefinition
{
    const char *key;
    u16 mod_flag_id;
    bool8 initial_value;
};

void ModFlag_Init(void);
u16 ModFlag_GetByName(const char *key);
bool8 ModFlag_Get(u16 mod_flag_id);
bool8 ModFlag_Set(u16 mod_flag_id);
bool8 ModFlag_Clear(u16 mod_flag_id);
bool8 ModFlag_SetLogged(u16 mod_flag_id, const char *reason);
bool8 ModFlag_ClearLogged(u16 mod_flag_id, const char *reason);

#endif // GUARD_MOD_FLAGS_H
