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
    u16 flagId;
    bool8 value;
    const char *reason;
};

struct ModEventTimeChanged
{
    u8 oldSegment;
    u8 newSegment;
    u16 dayCount;
};

struct ModEventWeatherChanged
{
    u8 oldWeather;
    u8 newWeather;
    u16 oldLayers;
    u16 newLayers;
};

struct ModEventPlayerStep
{
    u8 direction;
    u16 newKeys;
    u16 heldKeys;
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
