#ifndef GUARD_MULTIPLAYER_BATTLE_H
#define GUARD_MULTIPLAYER_BATTLE_H

#include "global.h"
#include "multiplayer/types.h"

struct MultiplayerBattleRequest
{
    u8 type;
    u8 player_count;
    u8 players[MAX_NET_BATTLE_PLAYERS];
    u32 battle_type_flags;
    u16 trainer_a;
    u16 trainer_b;
};

bool8 MultiplayerBattle_CanStart(const struct MultiplayerBattleRequest *request);
bool8 MultiplayerBattle_Start(const struct MultiplayerBattleRequest *request);
bool8 MultiplayerBattle_LocalPlayerCanFight(void);
bool8 MultiplayerBattle_StartTrainerPveBattle(u8 partner_player_id, u16 trainer_id);
bool8 MultiplayerBattle_RemotePlayerCanPartner(u8 partner_player_id);
bool8 MultiplayerBattle_PrepareTrainerPvePartnerParty(u8 partner_player_id);
void MultiplayerBattle_CancelPendingTrainerPveBattle(void);
bool8 MultiplayerBattle_IsTrainerPvePartnerBattle(void);
u8 MultiplayerBattle_GetPartnerTrainerBackPicId(void);
const u8 *MultiplayerBattle_GetPartnerName(void);
bool8 MultiplayerBattle_SendAction(u8 subsession_id, u8 battler_slot, u8 action, u8 target, u16 parameter);
void MultiplayerBattle_Tick(const struct MultiplayerSession *session);
bool8 MultiplayerBattle_TryCreateSecretBaseEnemyParty(void);
void MultiplayerBattle_ApplyCurrentWeather(void);
void MultiplayerBattle_OnBattleStart(u32 battle_type_flags);
void MultiplayerBattle_OnBattleEnd(u32 battle_outcome);

#endif // GUARD_MULTIPLAYER_BATTLE_H
