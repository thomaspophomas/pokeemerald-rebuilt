#include "global.h"
#include "event_data.h"
#include "generated/mod_registry.h"
#include "mod/event.h"
#include "mod/flags.h"
#include "mod/state.h"

static bool8 IsVanillaFlag(u16 id)
{
    return (id & MOD_FLAG_VANILLA_BASE) != 0;
}

static bool8 ModFlagIdInRange(u16 id)
{
    return id < gModFlagDefinitionCount && id < MOD_SAVE_FLAG_BYTES * 8;
}

static bool8 GetModFlagBit(u16 id)
{
    struct ModSaveState *state;
    u8 mask;

    if (!ModFlagIdInRange(id))
        return FALSE;

    state = ModState_Get();
    mask = 1 << (id & 7);
    return (state->flags[id / 8] & mask) != 0;
}

static void SetModFlagBit(u16 id, bool8 value)
{
    struct ModSaveState *state;
    u8 mask;

    if (!ModFlagIdInRange(id))
        return;

    state = ModState_Get();
    mask = 1 << (id & 7);
    if (value)
        state->flags[id / 8] |= mask;
    else
        state->flags[id / 8] &= ~mask;
}

static void EmitFlagChanged(u16 id, bool8 value, const char *reason)
{
    struct ModEventFlagChanged payload;

    payload.flagId = id;
    payload.value = value;
    payload.reason = reason;
    ModEvent_Emit(MOD_EVENT_FLAG_CHANGED, &payload, sizeof(payload));
}

void ModFlag_Init(void)
{
    u16 i;
    bool8 changed = FALSE;

    ModState_Init();
    for (i = 0; i < gModFlagDefinitionCount; i++)
    {
        if (gModFlagDefinitions[i].initialValue && !GetModFlagBit(gModFlagDefinitions[i].id))
        {
            SetModFlagBit(gModFlagDefinitions[i].id, TRUE);
            changed = TRUE;
        }
    }
    if (changed)
        ModState_BumpRevision();
}

u16 ModFlag_GetByName(const char *key)
{
    u16 i;

    if (key == NULL)
        return MOD_FLAG_INVALID;

    for (i = 0; i < gModFlagDefinitionCount; i++)
    {
        if (strcmp(gModFlagDefinitions[i].key, key) == 0)
            return gModFlagDefinitions[i].id;
    }

    return MOD_FLAG_INVALID;
}

bool8 ModFlag_Get(u16 id)
{
    if (IsVanillaFlag(id))
        return FlagGet(MOD_FLAG_FROM_VANILLA(id));

    return GetModFlagBit(id);
}

bool8 ModFlag_Set(u16 id)
{
    return ModFlag_SetLogged(id, NULL);
}

bool8 ModFlag_Clear(u16 id)
{
    return ModFlag_ClearLogged(id, NULL);
}

bool8 ModFlag_SetLogged(u16 id, const char *reason)
{
    if (IsVanillaFlag(id))
    {
        bool8 oldValue = FlagGet(MOD_FLAG_FROM_VANILLA(id));
        FlagSet(MOD_FLAG_FROM_VANILLA(id));
        if (!oldValue)
            EmitFlagChanged(id, TRUE, reason);
        return TRUE;
    }

    if (!ModFlagIdInRange(id))
        return FALSE;
    if (!GetModFlagBit(id))
    {
        SetModFlagBit(id, TRUE);
        ModState_BumpRevision();
        EmitFlagChanged(id, TRUE, reason);
    }
    return TRUE;
}

bool8 ModFlag_ClearLogged(u16 id, const char *reason)
{
    if (IsVanillaFlag(id))
    {
        bool8 oldValue = FlagGet(MOD_FLAG_FROM_VANILLA(id));
        FlagClear(MOD_FLAG_FROM_VANILLA(id));
        if (oldValue)
            EmitFlagChanged(id, FALSE, reason);
        return TRUE;
    }

    if (!ModFlagIdInRange(id))
        return FALSE;
    if (GetModFlagBit(id))
    {
        SetModFlagBit(id, FALSE);
        ModState_BumpRevision();
        EmitFlagChanged(id, FALSE, reason);
    }
    return TRUE;
}
