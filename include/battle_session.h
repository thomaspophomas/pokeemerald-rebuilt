#ifndef GUARD_BATTLE_SESSION_H
#define GUARD_BATTLE_SESSION_H

#include "global.h"

bool8 BattleSession_IsSecretBaseBattle(void);
u16 BattleSession_GetCurrentFieldBattleWeather(void);
void BattleSession_SetWeather(u16 battleWeather);

#endif // GUARD_BATTLE_SESSION_H
