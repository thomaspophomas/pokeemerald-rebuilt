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

static u8 GetSegmentFromMinute(u16 minuteOfDay)
{
    u16 time_segment_index;

    for (time_segment_index = 0; time_segment_index < gModTimeSegmentCount; time_segment_index++)
    {
        if (gModTimeSegments[time_segment_index].startMinute <= gModTimeSegments[time_segment_index].endMinute)
        {
            if (minuteOfDay >= gModTimeSegments[time_segment_index].startMinute && minuteOfDay <= gModTimeSegments[time_segment_index].endMinute)
                return gModTimeSegments[time_segment_index].segment;
        }
        else
        {
            if (minuteOfDay >= gModTimeSegments[time_segment_index].startMinute || minuteOfDay <= gModTimeSegments[time_segment_index].endMinute)
                return gModTimeSegments[time_segment_index].segment;
        }
    }

    if (minuteOfDay < 6 * 60)
        return MOD_TIME_NIGHT;
    if (minuteOfDay < 12 * 60)
        return MOD_TIME_MORNING;
    if (minuteOfDay < 18 * 60)
        return MOD_TIME_DAY;
    if (minuteOfDay < 21 * 60)
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
        snapshot->epochSeconds = epoch;
        snapshot->dayCount = epoch / (24 * 60 * 60);
        snapshot->minuteOfDay = (epoch / 60) % (24 * 60);
        snapshot->segment = GetSegmentFromMinute(snapshot->minuteOfDay);
        snapshot->usesServerClock = TRUE;
        return;
    }
#endif

    RtcCalcLocalTime();
    snapshot->dayCount = RtcGetLocalDayCount();
    snapshot->minuteOfDay = gLocalTime.hours * 60 + gLocalTime.minutes;
    snapshot->segment = GetSegmentFromMinute(snapshot->minuteOfDay);
    snapshot->usesServerClock = FALSE;
}

void ModTime_Init(void)
{
    struct ModTimeSnapshot snapshot;

    ModTime_Now(&snapshot);
    sLastSegment = snapshot.segment;
    sLastDayCount = snapshot.dayCount;
    sTimeInitialized = TRUE;
}

void ModTime_RunFrame(void)
{
    struct ModTimeSnapshot snapshot;

    if (!sTimeInitialized)
        ModTime_Init();

    ModTime_Now(&snapshot);
    if (snapshot.dayCount != sLastDayCount)
        ModEvent_Emit(MOD_EVENT_DAY_CHANGED, &snapshot, sizeof(snapshot));

    if (snapshot.segment != sLastSegment)
    {
        struct ModEventTimeChanged payload;

        payload.oldSegment = sLastSegment;
        payload.newSegment = snapshot.segment;
        payload.dayCount = snapshot.dayCount;
        ModEvent_Emit(MOD_EVENT_TIME_SEGMENT_CHANGED, &payload, sizeof(payload));
    }

    sLastSegment = snapshot.segment;
    sLastDayCount = snapshot.dayCount;
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
    return snapshot.dayCount;
}

u16 ModTime_GetMinuteOfDay(void)
{
    struct ModTimeSnapshot snapshot;

    ModTime_Now(&snapshot);
    return snapshot.minuteOfDay;
}
