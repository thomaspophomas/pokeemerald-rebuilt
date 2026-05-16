#ifndef GUARD_MULTIPLAYER_SESSION_H
#define GUARD_MULTIPLAYER_SESSION_H

#include "global.h"
#include "multiplayer/types.h"

void MultiplayerSession_Init(void);
void MultiplayerSession_Tick(void);
void MultiplayerSession_OnMapLoad(void);
void MultiplayerSession_OnPlayerStep(u8 direction, u16 newKeys, u16 heldKeys);
void MultiplayerSession_OnBattleStart(u32 battleTypeFlags);
void MultiplayerSession_OnBattleEnd(u32 battleOutcome);

void MultiplayerSession_RequestConnect(void);
void MultiplayerSession_RequestDisconnect(void);
void MultiplayerSession_RefreshRuntimeMode(void);
void MultiplayerSession_SuspendForNewGame(void);
void MultiplayerSession_MarkPlayerControlReady(void);
bool8 MultiplayerSession_CanRunOnlineLifecycle(void);
u8 MultiplayerSession_GetState(void);
u8 MultiplayerSession_GetLocalPlayerId(void);
u8 MultiplayerSession_GetPlayerCount(void);
const struct MultiplayerSession *MultiplayerSession_Get(void);
bool8 MultiplayerSession_IsOnline(void);
bool8 MultiplayerSession_IsHost(void);
u32 MultiplayerSession_GetSessionEpoch(void);
u32 MultiplayerSession_GetServerClockSeconds(void);
u32 MultiplayerSession_NextActionSequence(void);
bool8 MultiplayerSession_BuildTransactionKey(struct MultiplayerTransactionKey *key, u8 packetType, u8 subsessionId, u32 actionSequence);
bool8 MultiplayerSession_SendReliableAction(u8 packetType, u8 commitType, const struct MultiplayerTransactionKey *key, const void *payload, u16 payloadSize);
bool8 MultiplayerSession_IsPlayerActive(u8 playerId);
bool8 MultiplayerSession_IsPlayerBusy(u8 playerId);
bool8 MultiplayerSession_IsPlayerInSubsession(u8 playerId);
bool8 MultiplayerSession_IsPlayerInteractionBlocked(u8 playerId);
bool8 MultiplayerSession_ArePlayersOnSameMap(u8 playerCount, const u8 *players);
bool8 MultiplayerSession_ArePlayersWithinRange(u8 playerCount, const u8 *players, u16 maxDistance);
u8 MultiplayerSession_PreflightInteraction(const struct MultiplayerInteractionTarget *target, const u8 *script, u8 objectEventId, u8 facing);
u8 MultiplayerSession_PreflightOverworldAction(const struct MultiplayerPendingOverworldAction *action);
bool8 MultiplayerSession_TryStartWarpBarrier(u8 mapGroup, u8 mapNum, u8 warpId, s16 x, s16 y);
bool8 MultiplayerSession_StartInteractionBarrier(u8 type, u8 playerCount, const u8 *players);
void MultiplayerSession_ClearInteractionBarrier(u8 type);
void MultiplayerSession_OnScriptReleased(void);
void MultiplayerSession_OnMapWarpCompleted(void);
bool8 MultiplayerSession_StartSubsession(u8 type, u8 playerCount, const u8 *players);
void MultiplayerSession_EndSubsession(u8 subsessionId, u8 state);

#endif // GUARD_MULTIPLAYER_SESSION_H
