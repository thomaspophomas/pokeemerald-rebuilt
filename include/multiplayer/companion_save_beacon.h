#ifndef GUARD_MULTIPLAYER_COMPANION_SAVE_BEACON_H
#define GUARD_MULTIPLAYER_COMPANION_SAVE_BEACON_H

#include "global.h"

void MultiplayerCompanionSaveBeacon_Init(void);
void MultiplayerCompanionSaveBeacon_Tick(u32 tick, u8 sessionState, u8 healthState, u8 localPlayerId, u8 playerCount);

#endif // GUARD_MULTIPLAYER_COMPANION_SAVE_BEACON_H
