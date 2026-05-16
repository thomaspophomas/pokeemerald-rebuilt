#ifndef GUARD_MULTIPLAYER_OVERWORLD_INTERACTION_H
#define GUARD_MULTIPLAYER_OVERWORLD_INTERACTION_H

#include "global.h"
#include "multiplayer/types.h"

u8 OverworldInteraction_Preflight(const struct MultiplayerPendingOverworldAction *action);
bool8 OverworldInteraction_Resume(const struct MultiplayerPendingOverworldAction *action);
void OverworldInteraction_ShowDeniedMessage(void);
u16 OverworldInteraction_HashScript(const u8 *script);
u16 OverworldInteraction_CalcResourceChecksum(const struct MultiplayerPendingOverworldAction *action);
void OverworldInteraction_AddResource(struct MultiplayerPendingOverworldAction *action, u8 kind, u8 mapGroup, u8 mapNum, u8 localId, u8 elevation, s16 x, s16 y, u16 id);
bool8 OverworldInteraction_IsReadonlyScriptAllowed(const u8 *script);

#endif // GUARD_MULTIPLAYER_OVERWORLD_INTERACTION_H
