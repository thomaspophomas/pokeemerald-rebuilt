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
u8 MultiplayerSession_GetState(void);
u8 MultiplayerSession_GetLocalPlayerId(void);
u8 MultiplayerSession_GetPlayerCount(void);
const struct MultiplayerSession *MultiplayerSession_Get(void);
bool8 MultiplayerSession_IsOnline(void);
bool8 MultiplayerSession_IsHost(void);
bool8 MultiplayerSession_IsPlayerInSubsession(u8 playerId);
bool8 MultiplayerSession_StartSubsession(u8 type, u8 playerCount, const u8 *players);
void MultiplayerSession_EndSubsession(u8 subsessionId, u8 state);

#endif // GUARD_MULTIPLAYER_SESSION_H
