#ifndef GUARD_MOD_POKEMON_H
#define GUARD_MOD_POKEMON_H

#include "global.h"
#include "mod/domain.h"

u16 PokemonApi_GetFileCount(void);
const struct ModDomainFileDefinition *PokemonApi_GetFileByIndex(u16 index);
const struct ModDomainFileDefinition *PokemonApi_FindFile(const char *key);

#endif // GUARD_MOD_POKEMON_H
