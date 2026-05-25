#include "global.h"
#include "event_data.h"
#include "generated/mod_registry.h"
#include "mod/event.h"
#include "mod/flags.h"
#include "mod/state.h"

static bool8 IsVanillaFlag(u16 mod_flag_id)
{
    return (mod_flag_id & MOD_FLAG_VANILLA_BASE) != 0;
}

static bool8 ModFlagIdInRange(u16 mod_flag_id)
{
    return mod_flag_id < gModFlagDefinitionCount && mod_flag_id < MOD_SAVE_FLAG_BYTES * 8;
}

static bool8 GetModFlagBit(u16 mod_flag_id)
{
    struct ModSaveState *state;
    u8 mask;

    if (!ModFlagIdInRange(mod_flag_id))
        return FALSE;

    state = ModState_Get();
    mask = 1 << (mod_flag_id & 7);
    return (state->flags[mod_flag_id / 8] & mask) != 0;
}

static void SetModFlagBit(u16 mod_flag_id, bool8 value)
{
    struct ModSaveState *state;
    u8 mask;

    if (!ModFlagIdInRange(mod_flag_id))
        return;

    state = ModState_Get();
    mask = 1 << (mod_flag_id & 7);
    if (value)
        state->flags[mod_flag_id / 8] |= mask;
    else
        state->flags[mod_flag_id / 8] &= ~mask;
}

static void EmitFlagChanged(u16 mod_flag_id, bool8 value, const char *reason)
{
    struct ModEventFlagChanged payload;

    payload.flagId = mod_flag_id;
    payload.value = value;
    payload.reason = reason;
    ModEvent_Emit(MOD_EVENT_FLAG_CHANGED, &payload, sizeof(payload));
}

void ModFlag_Init(void)
{
    u16 flag_index;
    bool8 changed = FALSE;

    ModState_Init();
    for (flag_index = 0; flag_index < gModFlagDefinitionCount; flag_index++)
    {
        if (gModFlagDefinitions[flag_index].initialValue && !GetModFlagBit(gModFlagDefinitions[flag_index].mod_flag_id))
        {
            SetModFlagBit(gModFlagDefinitions[flag_index].mod_flag_id, TRUE);
            changed = TRUE;
        }
    }
    if (changed)
        ModState_BumpRevision();
}

u16 ModFlag_GetByName(const char *key)
{
    u16 flag_index;

    if (key == NULL)
        return MOD_FLAG_INVALID;

    for (flag_index = 0; flag_index < gModFlagDefinitionCount; flag_index++)
    {
        if (strcmp(gModFlagDefinitions[flag_index].key, key) == 0)
            return gModFlagDefinitions[flag_index].mod_flag_id;
    }

    return MOD_FLAG_INVALID;
}

bool8 ModFlag_Get(u16 mod_flag_id)
{
    if (IsVanillaFlag(mod_flag_id))
        return FlagGet(MOD_FLAG_FROM_VANILLA(mod_flag_id));

    return GetModFlagBit(mod_flag_id);
}

bool8 ModFlag_Set(u16 mod_flag_id)
{
    return ModFlag_SetLogged(mod_flag_id, NULL);
}

bool8 ModFlag_Clear(u16 mod_flag_id)
{
    return ModFlag_ClearLogged(mod_flag_id, NULL);
}

bool8 ModFlag_SetLogged(u16 mod_flag_id, const char *reason)
{
    if (IsVanillaFlag(mod_flag_id))
    {
        bool8 oldValue = FlagGet(MOD_FLAG_FROM_VANILLA(mod_flag_id));
        FlagSet(MOD_FLAG_FROM_VANILLA(mod_flag_id));
        if (!oldValue)
            EmitFlagChanged(mod_flag_id, TRUE, reason);
        return TRUE;
    }

    if (!ModFlagIdInRange(mod_flag_id))
        return FALSE;
    if (!GetModFlagBit(mod_flag_id))
    {
        SetModFlagBit(mod_flag_id, TRUE);
        ModState_BumpRevision();
        EmitFlagChanged(mod_flag_id, TRUE, reason);
    }
    return TRUE;
}

bool8 ModFlag_ClearLogged(u16 mod_flag_id, const char *reason)
{
    if (IsVanillaFlag(mod_flag_id))
    {
        bool8 oldValue = FlagGet(MOD_FLAG_FROM_VANILLA(mod_flag_id));
        FlagClear(MOD_FLAG_FROM_VANILLA(mod_flag_id));
        if (oldValue)
            EmitFlagChanged(mod_flag_id, FALSE, reason);
        return TRUE;
    }

    if (!ModFlagIdInRange(mod_flag_id))
        return FALSE;
    if (GetModFlagBit(mod_flag_id))
    {
        SetModFlagBit(mod_flag_id, FALSE);
        ModState_BumpRevision();
        EmitFlagChanged(mod_flag_id, FALSE, reason);
    }
    return TRUE;
}
