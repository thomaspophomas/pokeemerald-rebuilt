#ifndef GUARD_MULTIPLAYER_BATTLE_H
#define GUARD_MULTIPLAYER_BATTLE_H

#include "global.h"
#include "multiplayer/types.h"

struct MultiplayerBattleRequest
{
    u8 type;
    u8 playerCount;
    u8 players[MAX_NET_BATTLE_PLAYERS];
    u32 battleTypeFlags;
    u16 trainerA;
    u16 trainerB;
};

bool8 MultiplayerBattle_CanStart(const struct MultiplayerBattleRequest *request);
bool8 MultiplayerBattle_Start(const struct MultiplayerBattleRequest *request);
bool8 MultiplayerBattle_LocalPlayerCanFight(void);
bool8 MultiplayerBattle_StartTrainerPveBattle(u8 partnerPlayerId, u16 trainerId);
bool8 MultiplayerBattle_RemotePlayerCanPartner(u8 partnerPlayerId);
bool8 MultiplayerBattle_PrepareTrainerPvePartnerParty(u8 partnerPlayerId);
void MultiplayerBattle_CancelPendingTrainerPveBattle(void);
bool8 MultiplayerBattle_IsTrainerPvePartnerBattle(void);
u8 MultiplayerBattle_GetPartnerTrainerBackPicId(void);
const u8 *MultiplayerBattle_GetPartnerName(void);
bool8 MultiplayerBattle_SendAction(u8 subsessionId, u8 battlerSlot, u8 action, u8 target, u16 parameter);
void MultiplayerBattle_Tick(const struct MultiplayerSession *session);
bool8 MultiplayerBattle_TryCreateSecretBaseEnemyParty(void);
void MultiplayerBattle_ApplyCurrentWeather(void);
void MultiplayerBattle_OnBattleStart(u32 battleTypeFlags);
void MultiplayerBattle_OnBattleEnd(u32 battleOutcome);

#endif // GUARD_MULTIPLAYER_BATTLE_H
