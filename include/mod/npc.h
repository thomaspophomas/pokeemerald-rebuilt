#ifndef GUARD_MOD_NPC_H
#define GUARD_MOD_NPC_H

#include "global.h"

#define MOD_NPC_INSTANCE_NONE 0xFF
#define MOD_NPC_DYNAMIC_LOCAL_ID_BASE 0xD0
#define MOD_NPC_DYNAMIC_LOCAL_ID_END  0xEF

struct ModNpcDefinition
{
    const char *key;
    u16 npc_definition_id;
    u8 graphics_id;
    u8 movement_type;
    u8 local_id;
    u8 elevation;
    u16 flag_id;
    const u8 *script;
};

u8 NpcApi_Spawn(u16 npc_definition_id, u8 map_group, u8 map_number, s16 x, s16 y);
void NpcApi_Despawn(u8 npc_instance_id);
bool8 NpcApi_SetBehavior(u8 npc_instance_id, u8 movement_type);
bool8 NpcApi_SetVisible(u8 npc_instance_id, bool8 visible);
bool8 NpcApi_FindByLocalId(u8 map_group, u8 map_number, u8 object_event_local_id, u8 *npc_instance_id);
const struct ModNpcDefinition *NpcApi_FindDefinition(const char *key);

#endif // GUARD_MOD_NPC_H
