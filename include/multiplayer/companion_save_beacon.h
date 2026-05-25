#ifndef GUARD_MULTIPLAYER_COMPANION_SAVE_BEACON_H
#define GUARD_MULTIPLAYER_COMPANION_SAVE_BEACON_H

#include "global.h"

void MultiplayerCompanionSaveBeacon_Init(void);
void MultiplayerCompanionSaveBeacon_Tick(u32 tick, u8 session_state, u8 health_state, u8 local_player_id, u8 player_count);

#endif // GUARD_MULTIPLAYER_COMPANION_SAVE_BEACON_H
