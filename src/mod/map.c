#include "global.h"
#include "fieldmap.h"
#include "generated/mod_registry.h"
#include "mod/map.h"
#include "overworld.h"

static const struct ModMapDefinition *FindDefinitionById(u16 id)
{
    u16 i;

    for (i = 0; i < gModMapDefinitionCount; i++)
    {
        if (gModMapDefinitions[i].id == id)
            return &gModMapDefinitions[i];
    }

    return NULL;
}

const struct MapHeader *MapApi_GetHeader(u16 mapId)
{
    const struct ModMapDefinition *definition;

    definition = FindDefinitionById(mapId);
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

    return MapApi_GetHeader(definition->id);
}

const struct MapConnection *MapApi_GetConnection(u16 mapId, u8 direction)
{
    const struct MapHeader *header;
    s32 i;

    header = MapApi_GetHeader(mapId);
    if (header == NULL || header->connections == NULL)
        return NULL;

    for (i = 0; i < header->connections->count; i++)
    {
        if (header->connections->connections[i].direction == direction)
            return &header->connections->connections[i];
    }

    return NULL;
}

const struct WarpEvent *MapApi_GetWarpTarget(u16 mapId, u8 warpId)
{
    const struct MapHeader *header;
    u8 i;

    header = MapApi_GetHeader(mapId);
    if (header == NULL || header->events == NULL)
        return NULL;

    for (i = 0; i < header->events->warpCount; i++)
    {
        if (header->events->warps[i].warpId == warpId)
            return &header->events->warps[i];
    }

    return NULL;
}

bool8 MapApi_IsPositionValid(u16 mapId, s16 x, s16 y)
{
    const struct MapHeader *header;

    header = MapApi_GetHeader(mapId);
    if (header == NULL || header->mapLayout == NULL)
        return FALSE;
    if (x < 0 || y < 0 || x >= header->mapLayout->width || y >= header->mapLayout->height)
        return FALSE;

    return TRUE;
}

const struct ModMapDefinition *MapApi_FindDefinition(const char *key)
{
    u16 i;

    if (key == NULL)
        return NULL;

    for (i = 0; i < gModMapDefinitionCount; i++)
    {
        if (strcmp(gModMapDefinitions[i].key, key) == 0)
            return &gModMapDefinitions[i];
    }

    return NULL;
}
