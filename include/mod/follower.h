#ifndef GUARD_MOD_FOLLOWER_H
#define GUARD_MOD_FOLLOWER_H

#include "mod/base.h"

void ModFollower_Init(void);
void ModFollower_RunFrame(void);
void ModFollower_OnMapLoad(void);
void ModFollower_OnPlayerStep(u8 direction, u16 new_keys, u16 held_keys);
void ModFollower_OnBattleStart(void);
void ModFollower_OnBattleEnd(void);

#endif // GUARD_MOD_FOLLOWER_H
