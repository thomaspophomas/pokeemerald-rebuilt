#ifndef GUARD_MULTIPLAYER_OVERWORLD_H
#define GUARD_MULTIPLAYER_OVERWORLD_H

#include "global.h"
#include "multiplayer/types.h"

void MultiplayerOverworld_Init(void);
bool8 MultiplayerOverworld_CanTick(void);
void MultiplayerOverworld_Reset(void);
void MultiplayerOverworld_OnMapLoad(void);
void MultiplayerOverworld_OnPlayerStep(u8 direction, u16 newKeys, u16 heldKeys);
bool8 MultiplayerOverworld_TryInteractWithRemotePlayer(s16 x, s16 y, u8 elevation, u8 direction);
bool8 MultiplayerOverworld_HasVisibleRemotePlayer(void);
bool8 MultiplayerOverworld_TryGetVisibleRemotePlayer(u8 *playerId);
void MultiplayerOverworld_BuildLocalSnapshot(struct NetPlayerSnapshot *snapshot, u8 playerId, u32 tick);
void MultiplayerOverworld_Tick(const struct MultiplayerSession *session);
u8 MultiplayerOverworld_GetRemoteAvatarCapacity(void);
u8 MultiplayerOverworld_GetActiveRemoteAvatarCount(void);

#endif // GUARD_MULTIPLAYER_OVERWORLD_H
