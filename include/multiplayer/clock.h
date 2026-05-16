#ifndef GUARD_MULTIPLAYER_CLOCK_H
#define GUARD_MULTIPLAYER_CLOCK_H

#include "global.h"

void MultiplayerClock_Init(void);
bool8 MultiplayerClock_IsServerClockActive(void);
u32 MultiplayerClock_GetServerEpochSeconds(void);

#endif // GUARD_MULTIPLAYER_CLOCK_H
