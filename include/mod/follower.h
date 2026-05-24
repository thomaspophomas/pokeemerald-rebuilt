#ifndef GUARD_MOD_FOLLOWER_H
#define GUARD_MOD_FOLLOWER_H

#include "global.h"

void ModFollower_Init(void);
void ModFollower_RunFrame(void);
void ModFollower_OnMapLoad(void);
void ModFollower_OnPlayerStep(u8 direction, u16 newKeys, u16 heldKeys);
void ModFollower_OnBattleStart(void);
void ModFollower_OnBattleEnd(void);

#endif // GUARD_MOD_FOLLOWER_H
