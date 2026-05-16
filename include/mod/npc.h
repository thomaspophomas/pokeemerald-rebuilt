#ifndef GUARD_MOD_NPC_H
#define GUARD_MOD_NPC_H

#include "global.h"

#define MOD_NPC_INSTANCE_NONE 0xFF
#define MOD_NPC_DYNAMIC_LOCAL_ID_BASE 0xD0
#define MOD_NPC_DYNAMIC_LOCAL_ID_END  0xEF

enum ModNpcInteractionPolicy
{
    MOD_NPC_INTERACTION_EXCLUSIVE,
    MOD_NPC_INTERACTION_SHARED_READONLY,
    MOD_NPC_INTERACTION_DISABLED_ONLINE,
};

struct ModNpcDefinition
{
    const char *key;
    u16 id;
    u8 graphicsId;
    u8 movementType;
    u8 localId;
    u8 elevation;
    u8 interactionPolicy;
    u8 reserved;
    u16 flagId;
    const u8 *script;
};

u8 NpcApi_Spawn(u16 defId, u8 mapGroup, u8 mapNum, s16 x, s16 y);
void NpcApi_Despawn(u8 instanceId);
bool8 NpcApi_SetBehavior(u8 instanceId, u8 movementType);
bool8 NpcApi_SetVisible(u8 instanceId, bool8 visible);
bool8 NpcApi_FindByLocalId(u8 mapGroup, u8 mapNum, u8 localId, u8 *instanceId);
u8 NpcApi_GetInteractionPolicy(u8 mapGroup, u8 mapNum, u8 localId, const u8 *script);
const struct ModNpcDefinition *NpcApi_FindDefinition(const char *key);

#endif // GUARD_MOD_NPC_H
