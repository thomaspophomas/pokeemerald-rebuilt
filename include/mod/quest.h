#ifndef GUARD_MOD_QUEST_H
#define GUARD_MOD_QUEST_H

#include "global.h"
#include "mod/domain.h"

u16 QuestApi_GetFileCount(void);
const struct ModDomainFileDefinition *QuestApi_GetFileByIndex(u16 index);
const struct ModDomainFileDefinition *QuestApi_FindFile(const char *key);

#endif // GUARD_MOD_QUEST_H
