#include "global.h"
#include "generated/mod_registry.h"
#include "mod/event.h"
#include "mod/time.h"
#include "rtc.h"

#if FEATURE_MULTIPLAYER
#include "multiplayer/clock.h"
#endif

static EWRAM_DATA bool8 sTimeInitialized = FALSE;
static EWRAM_DATA u8 sLastSegment = MOD_TIME_DAY;
static EWRAM_DATA u16 sLastDayCount = 0;

static u8 GetSegmentFromMinute(u16 minute_of_day)
{
    u16 time_segment_index;

    for (time_segment_index = 0; time_segment_index < gModTimeSegmentCount; time_segment_index++)
    {
        if (gModTimeSegments[time_segment_index].start_minute <= gModTimeSegments[time_segment_index].end_minute)
        {
            if (minute_of_day >= gModTimeSegments[time_segment_index].start_minute && minute_of_day <= gModTimeSegments[time_segment_index].end_minute)
                return gModTimeSegments[time_segment_index].segment;
        }
        else
        {
            if (minute_of_day >= gModTimeSegments[time_segment_index].start_minute || minute_of_day <= gModTimeSegments[time_segment_index].end_minute)
                return gModTimeSegments[time_segment_index].segment;
        }
    }

    if (minute_of_day < 6 * 60)
        return MOD_TIME_NIGHT;
    if (minute_of_day < 12 * 60)
        return MOD_TIME_MORNING;
    if (minute_of_day < 18 * 60)
        return MOD_TIME_DAY;
    if (minute_of_day < 21 * 60)
        return MOD_TIME_EVENING;
    return MOD_TIME_NIGHT;
}

void ModTime_Now(struct ModTimeSnapshot *snapshot)
{
    u32 epoch;

    memset(snapshot, 0, sizeof(*snapshot));

#if FEATURE_MULTIPLAYER
    if (MultiplayerClock_IsServerClockActive())
    {
        epoch = MultiplayerClock_GetServerEpochSeconds();
        snapshot->epoch_seconds = epoch;
        snapshot->day_count = epoch / (24 * 60 * 60);
        snapshot->minute_of_day = (epoch / 60) % (24 * 60);
        snapshot->segment = GetSegmentFromMinute(snapshot->minute_of_day);
        snapshot->uses_server_clock = TRUE;
        return;
    }
#endif

    RtcCalcLocalTime();
    snapshot->day_count = RtcGetLocalDayCount();
    snapshot->minute_of_day = gLocalTime.hours * 60 + gLocalTime.minutes;
    snapshot->segment = GetSegmentFromMinute(snapshot->minute_of_day);
    snapshot->uses_server_clock = FALSE;
}

void ModTime_Init(void)
{
    struct ModTimeSnapshot snapshot;

    ModTime_Now(&snapshot);
    sLastSegment = snapshot.segment;
    sLastDayCount = snapshot.day_count;
    sTimeInitialized = TRUE;
}

void ModTime_RunFrame(void)
{
    struct ModTimeSnapshot snapshot;

    if (!sTimeInitialized)
        ModTime_Init();

    ModTime_Now(&snapshot);
    if (snapshot.day_count != sLastDayCount)
        ModEvent_Emit(MOD_EVENT_DAY_CHANGED, &snapshot, sizeof(snapshot));

    if (snapshot.segment != sLastSegment)
    {
        struct ModEventTimeChanged payload;

        payload.old_segment = sLastSegment;
        payload.new_segment = snapshot.segment;
        payload.day_count = snapshot.day_count;
        ModEvent_Emit(MOD_EVENT_TIME_SEGMENT_CHANGED, &payload, sizeof(payload));
    }

    sLastSegment = snapshot.segment;
    sLastDayCount = snapshot.day_count;
}

u8 ModTime_GetSegment(void)
{
    struct ModTimeSnapshot snapshot;

    ModTime_Now(&snapshot);
    return snapshot.segment;
}

bool8 ModTime_IsNight(void)
{
    return ModTime_GetSegment() == MOD_TIME_NIGHT;
}

u16 ModTime_GetDayCount(void)
{
    struct ModTimeSnapshot snapshot;

    ModTime_Now(&snapshot);
    return snapshot.day_count;
}

u16 ModTime_GetMinuteOfDay(void)
{
    struct ModTimeSnapshot snapshot;

    ModTime_Now(&snapshot);
    return snapshot.minute_of_day;
}
