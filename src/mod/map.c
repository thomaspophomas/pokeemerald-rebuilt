#include "global.h"
#include "fieldmap.h"
#include "generated/mod_registry.h"
#include "mod/map.h"
#include "overworld.h"

static const struct ModMapDefinition *FindDefinitionByMapId(u16 map_id)
{
    u16 map_index;

    for (map_index = 0; map_index < gModMapDefinitionCount; map_index++)
    {
        if (gModMapDefinitions[map_index].map_id == map_id)
            return &gModMapDefinitions[map_index];
    }

    return NULL;
}

const struct MapHeader *MapApi_GetHeader(u16 map_id)
{
    const struct ModMapDefinition *definition;

    definition = FindDefinitionByMapId(map_id);
    if (definition == NULL)
        return NULL;

    return Overworld_GetMapHeaderByGroupAndId(definition->mapGroup, definition->mapNum);
}

const struct MapHeader *MapApi_FindByName(const char *key)
{
    const struct ModMapDefinition *definition;

    definition = MapApi_FindDefinition(key);
    if (definition == NULL)
        return NULL;

    return MapApi_GetHeader(definition->map_id);
}

const struct MapConnection *MapApi_GetConnection(u16 map_id, u8 direction)
{
    const struct MapHeader *header;
    s32 connection_index;

    header = MapApi_GetHeader(map_id);
    if (header == NULL || header->connections == NULL)
        return NULL;

    for (connection_index = 0; connection_index < header->connections->count; connection_index++)
    {
        if (header->connections->connections[connection_index].direction == direction)
            return &header->connections->connections[connection_index];
    }

    return NULL;
}

const struct WarpEvent *MapApi_GetWarpTarget(u16 map_id, u8 warp_id)
{
    const struct MapHeader *header;
    u8 warp_index;

    header = MapApi_GetHeader(map_id);
    if (header == NULL || header->events == NULL)
        return NULL;

    for (warp_index = 0; warp_index < header->events->warpCount; warp_index++)
    {
        if (header->events->warps[warp_index].warpId == warp_id)
            return &header->events->warps[warp_index];
    }

    return NULL;
}

bool8 MapApi_IsPositionValid(u16 map_id, s16 x, s16 y)
{
    const struct MapHeader *header;

    header = MapApi_GetHeader(map_id);
    if (header == NULL || header->mapLayout == NULL)
        return FALSE;
    if (x < 0 || y < 0 || x >= header->mapLayout->width || y >= header->mapLayout->height)
        return FALSE;

    return TRUE;
}

const struct ModMapDefinition *MapApi_FindDefinition(const char *key)
{
    u16 map_index;

    if (key == NULL)
        return NULL;

    for (map_index = 0; map_index < gModMapDefinitionCount; map_index++)
    {
        if (strcmp(gModMapDefinitions[map_index].key, key) == 0)
            return &gModMapDefinitions[map_index];
    }

    return NULL;
}
