#ifndef GUARD_MOD_MAP_H
#define GUARD_MOD_MAP_H

#include "global.h"
#include "global.fieldmap.h"

#define MOD_MAP_INVALID_ID 0xFFFF

struct ModMapDefinition
{
    const char *key;
    u16 map_id;
    u8 map_group;
    u8 map_num;
    const char *json_path;
};

const struct MapHeader *MapApi_GetHeader(u16 map_id);
const struct MapHeader *MapApi_FindByName(const char *key);
const struct MapConnection *MapApi_GetConnection(u16 map_id, u8 direction);
const struct WarpEvent *MapApi_GetWarpTarget(u16 map_id, u8 warp_id);
bool8 MapApi_IsPositionValid(u16 map_id, s16 x, s16 y);
const struct ModMapDefinition *MapApi_FindDefinition(const char *key);

#endif // GUARD_MOD_MAP_H
