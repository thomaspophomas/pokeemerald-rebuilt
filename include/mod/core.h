#ifndef GUARD_MOD_CORE_H
#define GUARD_MOD_CORE_H

#include "mod/base.h"

void ModApi_Init(void);
void ModApi_RunFrame(void);
void ModApi_OnMapLoad(void);
void ModApi_OnPlayerStep(u8 direction, u16 new_keys, u16 held_keys);
void ModApi_OnBattleStart(u32 battle_type_flags);
void ModApi_OnBattleEnd(u32 battle_outcome);
u16 ModApi_Random(void);

#endif // GUARD_MOD_CORE_H
