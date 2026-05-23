#ifndef GUARD_MULTIPLAYER_INTERACTION_MENU_H
#define GUARD_MULTIPLAYER_INTERACTION_MENU_H

#include "global.h"
#include "multiplayer/types.h"

void MultiplayerInteractionMenu_Init(void);
void MultiplayerInteractionMenu_Reset(void);
bool8 MultiplayerInteractionMenu_IsActive(void);
bool8 MultiplayerInteractionMenu_StartLocal(u8 targetPlayerId);
void MultiplayerInteractionMenu_ApplySnapshotState(struct NetPlayerSnapshot *snapshot);
void MultiplayerInteractionMenu_UpdateRemoteRequests(const struct MultiplayerSession *session);

#endif // GUARD_MULTIPLAYER_INTERACTION_MENU_H
