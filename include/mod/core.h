#ifndef GUARD_MOD_CORE_H
#define GUARD_MOD_CORE_H

#include "global.h"

void ModApi_Init(void);
void ModApi_RunFrame(void);
void ModApi_OnMapLoad(void);
void ModApi_OnPlayerStep(u8 direction, u16 newKeys, u16 heldKeys);
void ModApi_OnBattleStart(u32 battleTypeFlags);
void ModApi_OnBattleEnd(u32 battleOutcome);

#endif // GUARD_MOD_CORE_H
