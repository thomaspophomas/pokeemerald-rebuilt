#include "global.h"
#include "event_object_movement.h"
#include "generated/mod_registry.h"
#include "global.fieldmap.h"
#include "mod/npc.h"
#include "mod/runtime_profile.h"

static const struct ModNpcDefinition *FindDefinitionByNpcDefinitionId(u16 npc_definition_id)
{
    u16 npc_index;

    for (npc_index = 0; npc_index < gModNpcDefinitionCount; npc_index++)
    {
        if (gModNpcDefinitions[npc_index].npc_definition_id == npc_definition_id)
            return &gModNpcDefinitions[npc_index];
    }

    return NULL;
}

u8 NpcApi_Spawn(u16 npc_definition_id, u8 map_group, u8 map_number, s16 x, s16 y)
{
    const struct ModNpcDefinition *definition;
    u8 object_event_local_id;
    u8 object_event_id;

    definition = FindDefinitionByNpcDefinitionId(npc_definition_id);
    if (definition == NULL)
        return MOD_NPC_INSTANCE_NONE;
    if (gSaveBlock1Ptr == NULL
     || gSaveBlock1Ptr->location.mapGroup != map_group
     || gSaveBlock1Ptr->location.mapNum != map_number)
        return MOD_NPC_INSTANCE_NONE;

    object_event_local_id = definition->localId;
    if (object_event_local_id == 0)
        object_event_local_id = MOD_NPC_DYNAMIC_LOCAL_ID_BASE + (npc_definition_id % (MOD_NPC_DYNAMIC_LOCAL_ID_END - MOD_NPC_DYNAMIC_LOCAL_ID_BASE + 1));

    if (TryGetObjectEventIdByLocalIdAndMap(object_event_local_id, map_number, map_group, &object_event_id))
        return object_event_id;

    object_event_id = SpawnSpecialObjectEventParameterized(
        definition->graphicsId,
        definition->movementType,
        object_event_local_id,
        x,
        y,
        definition->elevation);
    if (object_event_id >= OBJECT_EVENTS_COUNT)
        return MOD_NPC_INSTANCE_NONE;

    return object_event_id;
}

void NpcApi_Despawn(u8 npc_instance_id)
{
    if (npc_instance_id >= OBJECT_EVENTS_COUNT || !gObjectEvents[npc_instance_id].active)
        return;

    RemoveObjectEventByLocalIdAndMap(gObjectEvents[npc_instance_id].localId, gObjectEvents[npc_instance_id].mapNum, gObjectEvents[npc_instance_id].mapGroup);
}

bool8 NpcApi_SetBehavior(u8 npc_instance_id, u8 movement_type)
{
    if (npc_instance_id >= OBJECT_EVENTS_COUNT || !gObjectEvents[npc_instance_id].active)
        return FALSE;

    gObjectEvents[npc_instance_id].movementType = movement_type;
    SetTrainerMovementType(&gObjectEvents[npc_instance_id], movement_type);
    return TRUE;
}

bool8 NpcApi_SetVisible(u8 npc_instance_id, bool8 visible)
{
    if (npc_instance_id >= OBJECT_EVENTS_COUNT || !gObjectEvents[npc_instance_id].active)
        return FALSE;

    SetObjectInvisibility(gObjectEvents[npc_instance_id].localId, gObjectEvents[npc_instance_id].mapNum, gObjectEvents[npc_instance_id].mapGroup, !visible);
    return TRUE;
}

bool8 NpcApi_FindByLocalId(u8 map_group, u8 map_number, u8 object_event_local_id, u8 *npc_instance_id)
{
    if (npc_instance_id == NULL)
        return FALSE;

    return TryGetObjectEventIdByLocalIdAndMap(object_event_local_id, map_number, map_group, npc_instance_id);
}

const struct ModNpcDefinition *NpcApi_FindDefinition(const char *key)
{
    const struct ModNpcDefinition *runtimeDefinition;
    u16 npc_index;

    if (key == NULL)
        return NULL;

    runtimeDefinition = ModRuntimeProfile_FindNpc(key);
    if (runtimeDefinition != NULL)
        return runtimeDefinition;

    for (npc_index = 0; npc_index < gModNpcDefinitionCount; npc_index++)
    {
        if (strcmp(gModNpcDefinitions[npc_index].key, key) == 0)
            return &gModNpcDefinitions[npc_index];
    }

    return NULL;
}
