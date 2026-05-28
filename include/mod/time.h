#ifndef GUARD_MOD_TIME_H
#define GUARD_MOD_TIME_H

#include "mod/base.h"

enum ModTimeSegment
{
    MOD_TIME_MORNING,
    MOD_TIME_DAY,
    MOD_TIME_EVENING,
    MOD_TIME_NIGHT,
    MOD_TIME_SEGMENT_COUNT
};

struct ModTimeSnapshot
{
    u32 epoch_seconds;
    u16 day_count;
    u16 minute_of_day;
    u8 segment;
    bool8 uses_server_clock;
};

struct ModTimeSegmentDefinition
{
    const char *key;
    u16 start_minute;
    u16 end_minute;
    u8 segment;
};

void ModTime_Init(void);
void ModTime_RunFrame(void);
void ModTime_Now(struct ModTimeSnapshot *snapshot);
u8 ModTime_GetSegment(void);
bool8 ModTime_IsNight(void);
u16 ModTime_GetDayCount(void);
u16 ModTime_GetMinuteOfDay(void);

#endif // GUARD_MOD_TIME_H
