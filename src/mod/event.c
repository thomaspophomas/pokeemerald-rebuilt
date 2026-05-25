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
    u16 subscription_index;
    struct ModEvent event;
    s8 handler_result;

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
    for (subscription_index = 0; subscription_index < gModEventSubscriptionCount; subscription_index++)
    {
        if (gModEventSubscriptions[subscription_index].type != type)
            continue;
        if (gModEventSubscriptions[subscription_index].handler == NULL)
            continue;
        if (sFrameBudget == 0)
        {
            sEventDepth--;
            return MOD_EVENT_RESULT_DEFER;
        }

        sFrameBudget--;
        handler_result = gModEventSubscriptions[subscription_index].handler(&event);
        if (handler_result == MOD_EVENT_RESULT_STOP || handler_result == MOD_EVENT_RESULT_ERROR)
        {
            sEventDepth--;
            return handler_result;
        }
    }

    sEventDepth--;
    return MOD_EVENT_RESULT_CONTINUE;
}
