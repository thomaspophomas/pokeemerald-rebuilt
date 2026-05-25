#ifndef GUARD_MOD_EVENT_H
#define GUARD_MOD_EVENT_H

#include "global.h"

#define MOD_EVENT_MAX_RECURSION 4
#define MOD_EVENT_FRAME_BUDGET 32

enum ModEventType
{
    MOD_EVENT_GAME_INIT,
    MOD_EVENT_SAVE_LOADED,
    MOD_EVENT_MAP_LOAD,
    MOD_EVENT_MAP_ENTER,
    MOD_EVENT_PLAYER_STEP,
    MOD_EVENT_FLAG_CHANGED,
    MOD_EVENT_TIME_SEGMENT_CHANGED,
    MOD_EVENT_DAY_CHANGED,
    MOD_EVENT_WEATHER_CHANGED,
    MOD_EVENT_NPC_INTERACT,
    MOD_EVENT_BATTLE_STARTED,
    MOD_EVENT_BATTLE_ENDED,
    MOD_EVENT_CUSTOM,
    MOD_EVENT_COUNT
};

enum ModEventResult
{
    MOD_EVENT_RESULT_CONTINUE,
    MOD_EVENT_RESULT_STOP,
    MOD_EVENT_RESULT_DEFER,
    MOD_EVENT_RESULT_ERROR
};

struct ModEvent
{
    u16 type;
    u16 size;
    const void *payload;
};

struct ModEventFlagChanged
{
    u16 flag_id;
    bool8 value;
    const char *reason;
};

struct ModEventTimeChanged
{
    u8 old_segment;
    u8 new_segment;
    u16 day_count;
};

struct ModEventWeatherChanged
{
    u8 old_weather;
    u8 new_weather;
    u16 old_layers;
    u16 new_layers;
};

struct ModEventPlayerStep
{
    u8 direction;
    u16 new_keys;
    u16 held_keys;
};

typedef s8 (*ModEventHandler)(const struct ModEvent *event);

struct ModEventSubscription
{
    u16 type;
    s16 priority;
    ModEventHandler handler;
    const char *source;
};

void ModEvent_Init(void);
void ModEvent_BeginFrame(void);
s8 ModEvent_Emit(u16 type, const void *payload, u16 size);

#endif // GUARD_MOD_EVENT_H
