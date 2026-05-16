#include "global.h"
#include "event_object_movement.h"
#include "generated/mod_registry.h"
#include "global.fieldmap.h"
#include "mod/npc.h"

static const struct ModNpcDefinition *FindDefinitionById(u16 id)
{
    u16 i;

    for (i = 0; i < gModNpcDefinitionCount; i++)
    {
        if (gModNpcDefinitions[i].id == id)
            return &gModNpcDefinitions[i];
    }

    return NULL;
}

u8 NpcApi_Spawn(u16 defId, u8 mapGroup, u8 mapNum, s16 x, s16 y)
{
    const struct ModNpcDefinition *definition;
    u8 localId;
    u8 objectEventId;

    definition = FindDefinitionById(defId);
    if (definition == NULL)
        return MOD_NPC_INSTANCE_NONE;
    if (gSaveBlock1Ptr == NULL
     || gSaveBlock1Ptr->location.mapGroup != mapGroup
     || gSaveBlock1Ptr->location.mapNum != mapNum)
        return MOD_NPC_INSTANCE_NONE;

    localId = definition->localId;
    if (localId == 0)
        localId = MOD_NPC_DYNAMIC_LOCAL_ID_BASE + (defId % (MOD_NPC_DYNAMIC_LOCAL_ID_END - MOD_NPC_DYNAMIC_LOCAL_ID_BASE + 1));

    if (TryGetObjectEventIdByLocalIdAndMap(localId, mapNum, mapGroup, &objectEventId))
        return objectEventId;

    objectEventId = SpawnSpecialObjectEventParameterized(
        definition->graphicsId,
        definition->movementType,
        localId,
        x,
        y,
        definition->elevation);
    if (objectEventId >= OBJECT_EVENTS_COUNT)
        return MOD_NPC_INSTANCE_NONE;

    return objectEventId;
}

void NpcApi_Despawn(u8 instanceId)
{
    if (instanceId >= OBJECT_EVENTS_COUNT || !gObjectEvents[instanceId].active)
        return;

    RemoveObjectEventByLocalIdAndMap(gObjectEvents[instanceId].localId, gObjectEvents[instanceId].mapNum, gObjectEvents[instanceId].mapGroup);
}

bool8 NpcApi_SetBehavior(u8 instanceId, u8 movementType)
{
    if (instanceId >= OBJECT_EVENTS_COUNT || !gObjectEvents[instanceId].active)
        return FALSE;

    gObjectEvents[instanceId].movementType = movementType;
    SetTrainerMovementType(&gObjectEvents[instanceId], movementType);
    return TRUE;
}

bool8 NpcApi_SetVisible(u8 instanceId, bool8 visible)
{
    if (instanceId >= OBJECT_EVENTS_COUNT || !gObjectEvents[instanceId].active)
        return FALSE;

    SetObjectInvisibility(gObjectEvents[instanceId].localId, gObjectEvents[instanceId].mapNum, gObjectEvents[instanceId].mapGroup, !visible);
    return TRUE;
}

bool8 NpcApi_FindByLocalId(u8 mapGroup, u8 mapNum, u8 localId, u8 *instanceId)
{
    if (instanceId == NULL)
        return FALSE;

    return TryGetObjectEventIdByLocalIdAndMap(localId, mapNum, mapGroup, instanceId);
}

const struct ModNpcDefinition *NpcApi_FindDefinition(const char *key)
{
    u16 i;

    if (key == NULL)
        return NULL;

    for (i = 0; i < gModNpcDefinitionCount; i++)
    {
        if (strcmp(gModNpcDefinitions[i].key, key) == 0)
            return &gModNpcDefinitions[i];
    }

    return NULL;
}
