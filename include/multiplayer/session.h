#ifndef GUARD_MULTIPLAYER_SESSION_H
#define GUARD_MULTIPLAYER_SESSION_H

#include "global.h"
#include "multiplayer/types.h"

void MultiplayerSession_Init(void);
void MultiplayerSession_Tick(void);
void MultiplayerSession_OnMapLoad(void);
void MultiplayerSession_OnPlayerStep(u8 direction, u16 new_keys, u16 held_keys);
void MultiplayerSession_OnBattleStart(u32 battle_type_flags);
void MultiplayerSession_OnBattleEnd(u32 battle_outcome);

void MultiplayerSession_RequestConnect(void);
void MultiplayerSession_RequestDisconnect(void);
void MultiplayerSession_RefreshRuntimeMode(void);
u8 MultiplayerSession_GetState(void);
u8 MultiplayerSession_GetLocalPlayerId(void);
u8 MultiplayerSession_GetPlayerCount(void);
const struct MultiplayerSession *MultiplayerSession_Get(void);
const struct NetPlayerBattleProfile *MultiplayerSession_GetPlayerBattleProfile(u8 player_id);
bool8 MultiplayerSession_IsOnline(void);
bool8 MultiplayerSession_IsHost(void);
u32 MultiplayerSession_GetSessionEpoch(void);
u32 MultiplayerSession_GetServerClockSeconds(void);
u32 MultiplayerSession_NextActionSequence(void);
bool8 MultiplayerSession_BuildTransactionKey(struct MultiplayerTransactionKey *transaction_key, u8 packet_type, u8 subsession_id, u32 action_sequence);
bool8 MultiplayerSession_IsPlayerActive(u8 player_id);
bool8 MultiplayerSession_IsPlayerBusy(u8 player_id);
bool8 MultiplayerSession_IsPlayerInSubsession(u8 player_id);
bool8 MultiplayerSession_IsPlayerInteractionBlocked(u8 player_id);
bool8 MultiplayerSession_ArePlayersOnSameMap(u8 player_count, const u8 *player_ids);
bool8 MultiplayerSession_ArePlayersWithinRange(u8 player_count, const u8 *player_ids, u16 max_distance);
bool8 MultiplayerSession_StartInteractionBarrier(u8 barrier_type, u8 player_count, const u8 *player_ids);
void MultiplayerSession_ClearInteractionBarrier(u8 barrier_type);
bool8 MultiplayerSession_StartSubsession(u8 subsession_type, u8 player_count, const u8 *player_ids);
void MultiplayerSession_EndSubsession(u8 subsession_id, u8 final_state);

#endif // GUARD_MULTIPLAYER_SESSION_H
