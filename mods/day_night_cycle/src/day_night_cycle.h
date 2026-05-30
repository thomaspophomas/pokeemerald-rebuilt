#ifndef GUARD_DAY_NIGHT_CYCLE_H
#define GUARD_DAY_NIGHT_CYCLE_H

#include "mod_sdk.h"

bool8 DayNightCycle_DebugIsTintApplied(void);
bool8 DayNightCycle_DebugIsTintSuspended(void);
bool8 DayNightCycle_DebugGetAppliedTint(u16 *r, u16 *g, u16 *b);
void DayNightCycle_DebugResetTint(void);
void DayNightCycle_DebugSuspendTint(void);
void DayNightCycle_DebugSetMinuteOfDay(u16 minuteOfDay);
void DayNightCycle_DebugSetMinuteStep(u16 minutesPerTick);
u16 DayNightCycle_DebugGetMinuteStep(void);
void DayNightCycle_DebugClearMinuteOverride(void);
bool8 DayNightCycle_DebugGetMinuteOverride(u16 *minuteOfDay);

#endif // GUARD_DAY_NIGHT_CYCLE_H
