#include "global.h"
#include "generated/mod_registry.h"
#include "mod/event.h"

static EWRAM_DATA u8 sEventDepth = 0;
static EWRAM_DATA u8 sFrameBudget = MOD_EVENT_FRAME_BUDGET;

void ModEvent_Init(void)
{
    sEventDepth = 0;
    sFrameBudget = MOD_EVENT_FRAME_BUDGET;
}

void ModEvent_BeginFrame(void)
{
    sFrameBudget = MOD_EVENT_FRAME_BUDGET;
}

s8 ModEvent_Emit(u16 type, const void *payload, u16 size)
{
    u16 i;
    struct ModEvent event;
    s8 result;

    if (type >= MOD_EVENT_COUNT)
        return MOD_EVENT_RESULT_ERROR;
    if (sEventDepth >= MOD_EVENT_MAX_RECURSION)
        return MOD_EVENT_RESULT_ERROR;
    if (sFrameBudget == 0)
        return MOD_EVENT_RESULT_DEFER;

    event.type = type;
    event.size = size;
    event.payload = payload;

    sEventDepth++;
    for (i = 0; i < gModEventSubscriptionCount; i++)
    {
        if (gModEventSubscriptions[i].type != type)
            continue;
        if (gModEventSubscriptions[i].handler == NULL)
            continue;
        if (sFrameBudget == 0)
        {
            sEventDepth--;
            return MOD_EVENT_RESULT_DEFER;
        }

        sFrameBudget--;
        result = gModEventSubscriptions[i].handler(&event);
        if (result == MOD_EVENT_RESULT_STOP || result == MOD_EVENT_RESULT_ERROR)
        {
            sEventDepth--;
            return result;
        }
    }

    sEventDepth--;
    return MOD_EVENT_RESULT_CONTINUE;
}
