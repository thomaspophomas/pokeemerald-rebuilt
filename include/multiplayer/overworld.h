#ifndef GUARD_MULTIPLAYER_OVERWORLD_H
#define GUARD_MULTIPLAYER_OVERWORLD_H

#include "global.h"
#include "multiplayer/types.h"

void MultiplayerOverworld_Init(void);
void MultiplayerOverworld_OnMapLoad(void);
void MultiplayerOverworld_OnPlayerStep(u8 direction, u16 newKeys, u16 heldKeys);
void MultiplayerOverworld_BuildLocalSnapshot(struct NetPlayerSnapshot *snapshot, u8 playerId, u32 tick);
void MultiplayerOverworld_Tick(const struct MultiplayerSession *session);
u8 MultiplayerOverworld_GetRemoteAvatarCapacity(void);
u8 MultiplayerOverworld_GetActiveRemoteAvatarCount(void);

#endif // GUARD_MULTIPLAYER_OVERWORLD_H
