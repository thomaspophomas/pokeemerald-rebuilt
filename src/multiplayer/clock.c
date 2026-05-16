#include "global.h"
#include "multiplayer/clock.h"
#include "multiplayer/session.h"

void MultiplayerClock_Init(void)
{
}

bool8 MultiplayerClock_IsServerClockActive(void)
{
    return MultiplayerSession_IsOnline() && MultiplayerSession_GetServerClockSeconds() != 0;
}

u32 MultiplayerClock_GetServerEpochSeconds(void)
{
    return MultiplayerSession_GetServerClockSeconds();
}
