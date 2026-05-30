#include "global.h"
#include "constants/event_objects.h"
#include "constants/global.h"
#include "constants/species.h"
#include "event_object_movement.h"
#include "fieldmap.h"
#include "global.fieldmap.h"
#include "metatile_behavior.h"
#include "mod/follower.h"
#include "mod/overworld_sprite.h"
#include "mod/sprite_asset.h"
#include "pokemon.h"
#include "sprite.h"

#define MOD_FOLLOWER_FORM_DEFAULT 0
#define MOD_FOLLOWER_MAX_SYNC_DISTANCE 3
#define MOD_FOLLOWER_INVALID_PALETTE 0xFF

struct ModFollowerState
{
    bool8 active;
    bool8 suspended;
    bool8 has_player_coords;
    u8 sprite_id;
    u8 graphics_id;
    u8 palette_num;
    const char *asset_key;
    const struct ObjectEventGraphicsInfo *graphics_info;
    u16 graphics_revision;
    u8 elevation;
    u8 facing_direction;
    u8 map_group;
    u8 map_num;
    s16 x;
    s16 y;
    s16 last_player_x;
    s16 last_player_y;
    u16 species;
    bool8 shiny;
};

static EWRAM_DATA struct ModFollowerState sFollower = {};

static void ResetFollowerState(void)
{
    sFollower.active = FALSE;
    sFollower.suspended = FALSE;
    sFollower.has_player_coords = FALSE;
    sFollower.sprite_id = MAX_SPRITES;
    sFollower.graphics_id = OVERWORLD_SPRITE_API_INVALID_GFX;
    sFollower.palette_num = MOD_FOLLOWER_INVALID_PALETTE;
    sFollower.asset_key = NULL;
    sFollower.graphics_info = NULL;
    sFollower.graphics_revision = 0;
    sFollower.elevation = ELEVATION_DEFAULT;
    sFollower.facing_direction = DIR_SOUTH;
    sFollower.map_group = 0;
    sFollower.map_num = 0;
    sFollower.x = 0;
    sFollower.y = 0;
    sFollower.last_player_x = 0;
    sFollower.last_player_y = 0;
    sFollower.species = SPECIES_NONE;
    sFollower.shiny = FALSE;
}

static void DestroyFollowerSprite(void)
{
    DestroyVirtualObject(OVERWORLD_SPRITE_API_FOLLOWER_VIRTUAL_ID);
    sFollower.active = FALSE;
    sFollower.sprite_id = MAX_SPRITES;
}

static void ReleaseFollowerAssets(void)
{
    if (sFollower.asset_key != NULL)
        SpriteAssetApi_Release(sFollower.asset_key);

    sFollower.graphics_id = OVERWORLD_SPRITE_API_INVALID_GFX;
    sFollower.palette_num = MOD_FOLLOWER_INVALID_PALETTE;
    sFollower.asset_key = NULL;
    sFollower.graphics_info = NULL;
    sFollower.graphics_revision = 0;
    sFollower.species = SPECIES_NONE;
    sFollower.shiny = FALSE;
}

static void HideFollower(void)
{
    DestroyFollowerSprite();
    ReleaseFollowerAssets();
    sFollower.has_player_coords = FALSE;
}

static bool8 TryGetPlayerObjectEvent(struct ObjectEvent **object_event)
{
    if (gPlayerAvatar.objectEventId >= OBJECT_EVENTS_COUNT)
        return FALSE;
    if (!gObjectEvents[gPlayerAvatar.objectEventId].active)
        return FALSE;

    *object_event = &gObjectEvents[gPlayerAvatar.objectEventId];
    return TRUE;
}

static bool8 TryGetPokemonFollower(u8 party_index, const struct ModFollowerSpriteDefinition **definition, u16 *species, bool8 *shiny)
{
    u16 mon_species;
    bool8 mon_shiny;
    const struct ModFollowerSpriteDefinition *follower;

    mon_species = GetMonData(&gPlayerParty[party_index], MON_DATA_SPECIES_OR_EGG, NULL);
    if (mon_species == SPECIES_NONE || mon_species == SPECIES_EGG)
        return FALSE;
    if (GetMonData(&gPlayerParty[party_index], MON_DATA_HP, NULL) == 0)
        return FALSE;

    mon_shiny = IsMonShiny(&gPlayerParty[party_index]);
    follower = OverworldSpriteApi_FindFollowerSprite(mon_species, MOD_FOLLOWER_FORM_DEFAULT, mon_shiny);
    if (follower == NULL && mon_shiny)
        follower = OverworldSpriteApi_FindFollowerSprite(mon_species, MOD_FOLLOWER_FORM_DEFAULT, FALSE);
    if (follower == NULL)
        return FALSE;

    *definition = follower;
    *species = mon_species;
    *shiny = mon_shiny;
    return TRUE;
}

static bool8 TryGetLeadPokemonFollower(const struct ModFollowerSpriteDefinition **definition, u16 *species, bool8 *shiny)
{
    u8 i;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (TryGetPokemonFollower(i, definition, species, shiny))
            return TRUE;
    }

    return FALSE;
}

static bool8 IsDirectionCardinal(u8 direction)
{
    return direction == DIR_SOUTH
        || direction == DIR_NORTH
        || direction == DIR_WEST
        || direction == DIR_EAST;
}

static s16 AbsS16(s16 value)
{
    return value < 0 ? -value : value;
}

static u8 GetDirectionFromDelta(s16 dx, s16 dy, u8 fallback_direction)
{
    if (AbsS16(dx) > AbsS16(dy))
        return dx < 0 ? DIR_WEST : DIR_EAST;
    if (dy != 0)
        return dy < 0 ? DIR_NORTH : DIR_SOUTH;
    if (dx != 0)
        return dx < 0 ? DIR_WEST : DIR_EAST;
    return fallback_direction;
}

static bool8 HasFollowerChangedMap(void)
{
    return sFollower.map_group != gSaveBlock1Ptr->location.mapGroup
        || sFollower.map_num != gSaveBlock1Ptr->location.mapNum;
}

static bool8 IsFollowerTooFarFrom(s16 x, s16 y)
{
    return AbsS16(sFollower.x - x) > MOD_FOLLOWER_MAX_SYNC_DISTANCE
        || AbsS16(sFollower.y - y) > MOD_FOLLOWER_MAX_SYNC_DISTANCE;
}

static bool8 IsPlayerInUnsupportedFollowerState(struct ObjectEvent *player)
{
    if (player->invisible)
        return TRUE;
    if (gPlayerAvatar.flags & (PLAYER_AVATAR_FLAG_SURFING | PLAYER_AVATAR_FLAG_UNDERWATER))
        return TRUE;

    switch (player->graphicsId)
    {
    case OBJ_EVENT_GFX_BRENDAN_SURFING:
    case OBJ_EVENT_GFX_MAY_SURFING:
    case OBJ_EVENT_GFX_BRENDAN_UNDERWATER:
    case OBJ_EVENT_GFX_MAY_UNDERWATER:
    case OBJ_EVENT_GFX_RIVAL_BRENDAN_FIELD_MOVE:
    case OBJ_EVENT_GFX_RIVAL_MAY_FIELD_MOVE:
    case OBJ_EVENT_GFX_BRENDAN_FISHING:
    case OBJ_EVENT_GFX_MAY_FISHING:
    case OBJ_EVENT_GFX_BRENDAN_WATERING:
    case OBJ_EVENT_GFX_MAY_WATERING:
        return TRUE;
    }

    return FALSE;
}

static bool8 IsFollowerTileBehaviorBlocked(u8 metatile_behavior)
{
    return MetatileBehavior_IsSurfableWaterOrUnderwater(metatile_behavior)
        || MetatileBehavior_IsWarpDoor(metatile_behavior)
        || MetatileBehavior_IsDoor(metatile_behavior)
        || MetatileBehavior_IsNonAnimDoor(metatile_behavior)
        || MetatileBehavior_IsDeepSouthWarp(metatile_behavior)
        || MetatileBehavior_IsEastArrowWarp(metatile_behavior)
        || MetatileBehavior_IsWestArrowWarp(metatile_behavior)
        || MetatileBehavior_IsNorthArrowWarp(metatile_behavior)
        || MetatileBehavior_IsSouthArrowWarp(metatile_behavior)
        || MetatileBehavior_IsLavaridgeB1FWarp(metatile_behavior)
        || MetatileBehavior_IsLavaridge1FWarp(metatile_behavior)
        || MetatileBehavior_IsAquaHideoutWarp(metatile_behavior)
        || MetatileBehavior_IsUnionRoomWarp(metatile_behavior)
        || MetatileBehavior_IsMossdeepGymWarp(metatile_behavior)
        || MetatileBehavior_IsBattlePyramidWarp(metatile_behavior)
        || MetatileBehavior_IsMtPyreHole(metatile_behavior)
        || MetatileBehavior_IsCrackedFloorHole(metatile_behavior)
        || MetatileBehavior_IsEscalator(metatile_behavior)
        || MetatileBehavior_IsLadder(metatile_behavior)
        || MetatileBehavior_IsForcedMovementTile(metatile_behavior)
        || MetatileBehavior_IsJumpSouth(metatile_behavior)
        || MetatileBehavior_IsJumpNorth(metatile_behavior)
        || MetatileBehavior_IsJumpWest(metatile_behavior)
        || MetatileBehavior_IsJumpEast(metatile_behavior);
}

static bool8 IsFollowerTileValid(s16 x, s16 y, u8 elevation)
{
    u8 metatile_behavior;

    if (MapGridGetCollisionAt(x, y) != COLLISION_NONE)
        return FALSE;
    if (GetMapBorderIdAt(x, y) == CONNECTION_INVALID)
        return FALSE;
    if (GetObjectEventIdByPosition(x, y, elevation) != OBJECT_EVENTS_COUNT)
        return FALSE;

    metatile_behavior = MapGridGetMetatileBehaviorAt(x, y);
    if (IsFollowerTileBehaviorBlocked(metatile_behavior))
        return FALSE;

    return TRUE;
}

static void SetCoordsBehindPlayer(struct ObjectEvent *player)
{
    u8 direction = player->facingDirection;

    if (!IsDirectionCardinal(direction))
        direction = DIR_SOUTH;

    sFollower.x = player->currentCoords.x;
    sFollower.y = player->currentCoords.y;
    MoveCoords(GetOppositeDirection(direction), &sFollower.x, &sFollower.y);
    sFollower.last_player_x = player->currentCoords.x;
    sFollower.last_player_y = player->currentCoords.y;
    sFollower.has_player_coords = TRUE;
    sFollower.facing_direction = direction;
}

static bool8 TryLoadFollowerAssets(const struct ModFollowerSpriteDefinition *definition, u8 *palette_num)
{
    *palette_num = MOD_FOLLOWER_INVALID_PALETTE;
    if (definition->asset_key == NULL)
        return TRUE;

    if (definition->graphics_info == NULL && !SpriteAssetApi_LoadSheet(definition->asset_key))
        return FALSE;
    if (definition->graphics_info != NULL)
        SpriteAssetApi_LoadSheet(definition->asset_key);

    *palette_num = SpriteAssetApi_LoadPaletteNum(definition->asset_key);
    return *palette_num != MOD_FOLLOWER_INVALID_PALETTE;
}

static bool8 SyncFollowerGraphics(const struct ModFollowerSpriteDefinition *definition, u16 species, bool8 shiny)
{
    u8 palette_num;

    if (sFollower.species == species
     && sFollower.shiny == shiny
     && sFollower.graphics_id == definition->graphics_id
     && sFollower.asset_key == definition->asset_key
     && sFollower.graphics_info == definition->graphics_info
     && sFollower.graphics_revision == definition->graphics_revision)
        return TRUE;

    if (sFollower.active)
        DestroyFollowerSprite();
    if (sFollower.asset_key != NULL && sFollower.asset_key != definition->asset_key)
        ReleaseFollowerAssets();

    if (!TryLoadFollowerAssets(definition, &palette_num))
    {
        if (definition->asset_key != NULL)
            SpriteAssetApi_Release(definition->asset_key);
        ReleaseFollowerAssets();
        return FALSE;
    }

    sFollower.species = species;
    sFollower.shiny = shiny;
    sFollower.graphics_id = definition->graphics_id;
    sFollower.asset_key = definition->asset_key;
    sFollower.graphics_info = definition->graphics_info;
    sFollower.graphics_revision = definition->graphics_revision;
    sFollower.palette_num = palette_num;
    return TRUE;
}

static void SpawnOrUpdateFollowerSprite(s16 x, s16 y, u8 elevation, u8 direction, bool8 moving)
{
    if (!IsDirectionCardinal(direction))
        direction = sFollower.facing_direction;
    if (!IsDirectionCardinal(direction))
        direction = DIR_SOUTH;

    if (!IsFollowerTileValid(x, y, elevation))
    {
        DestroyFollowerSprite();
        return;
    }

    if (sFollower.graphics_info != NULL)
    {
        sFollower.sprite_id = CreateOrUpdateVirtualObjectFromGraphicsInfo(
            sFollower.graphics_info,
            sFollower.graphics_id,
            OVERWORLD_SPRITE_API_FOLLOWER_VIRTUAL_ID,
            x,
            y,
            elevation,
            direction,
            sFollower.graphics_revision,
            sFollower.palette_num);
    }
    else
    {
        sFollower.sprite_id = CreateOrUpdateVirtualObject(
            sFollower.graphics_id,
            OVERWORLD_SPRITE_API_FOLLOWER_VIRTUAL_ID,
            x,
            y,
            elevation,
            direction,
            sFollower.graphics_revision);
    }
    if (sFollower.sprite_id == MAX_SPRITES)
    {
        sFollower.active = FALSE;
        return;
    }

    sFollower.active = TRUE;
    sFollower.elevation = elevation;
    sFollower.facing_direction = direction;
    sFollower.map_group = gSaveBlock1Ptr->location.mapGroup;
    sFollower.map_num = gSaveBlock1Ptr->location.mapNum;
    TurnVirtualObjectWithMovement(OVERWORLD_SPRITE_API_FOLLOWER_VIRTUAL_ID, direction, moving);
}

static void RefreshPlayerTrail(struct ObjectEvent *player, bool8 force_snap)
{
    s16 player_x;
    s16 player_y;
    u8 player_direction;
    u8 elevation;
    bool8 moved;
    bool8 snap;

    player_x = player->currentCoords.x;
    player_y = player->currentCoords.y;
    player_direction = player->facingDirection;
    if (!IsDirectionCardinal(player_direction))
        player_direction = sFollower.facing_direction;
    if (!IsDirectionCardinal(player_direction))
        player_direction = DIR_SOUTH;

    elevation = player->currentElevation;
    if (elevation == 0)
        elevation = ELEVATION_DEFAULT;

    snap = force_snap || !sFollower.active || !sFollower.has_player_coords || HasFollowerChangedMap();
    if (!snap)
        snap = IsFollowerTooFarFrom(player_x, player_y);

    if (snap)
    {
        SetCoordsBehindPlayer(player);
        SpawnOrUpdateFollowerSprite(sFollower.x, sFollower.y, elevation, player_direction, FALSE);
        return;
    }

    moved = player_x != sFollower.last_player_x || player_y != sFollower.last_player_y;
    if (moved)
    {
        s16 target_x;
        s16 target_y;
        u8 move_direction;

        target_x = sFollower.last_player_x;
        target_y = sFollower.last_player_y;
        move_direction = GetDirectionFromDelta(target_x - sFollower.x, target_y - sFollower.y, player_direction);
        sFollower.x = target_x;
        sFollower.y = target_y;
        sFollower.last_player_x = player_x;
        sFollower.last_player_y = player_y;
        SpawnOrUpdateFollowerSprite(sFollower.x, sFollower.y, elevation, move_direction, TRUE);
    }
    else
    {
        SpawnOrUpdateFollowerSprite(sFollower.x, sFollower.y, elevation, player_direction, FALSE);
    }
}

void ModFollower_Init(void)
{
    ResetFollowerState();
}

void ModFollower_RunFrame(void)
{
    struct ObjectEvent *player;
    const struct ModFollowerSpriteDefinition *definition;
    u16 species;
    bool8 shiny;

    if (sFollower.suspended)
        return;
    if (!TryGetPlayerObjectEvent(&player))
        return;
    if (IsPlayerInUnsupportedFollowerState(player))
    {
        HideFollower();
        return;
    }
    if (!TryGetLeadPokemonFollower(&definition, &species, &shiny))
    {
        HideFollower();
        return;
    }

    if (!SyncFollowerGraphics(definition, species, shiny))
    {
        HideFollower();
        return;
    }
    RefreshPlayerTrail(player, FALSE);
}

void ModFollower_OnMapLoad(void)
{
    HideFollower();
}

void ModFollower_OnPlayerStep(u8 direction, u16 new_keys, u16 held_keys)
{
    struct ObjectEvent *player;

    (void)new_keys;
    (void)held_keys;

    if (sFollower.suspended)
        return;
    if (!TryGetPlayerObjectEvent(&player))
        return;
    if (IsPlayerInUnsupportedFollowerState(player))
    {
        HideFollower();
        return;
    }
    if (IsDirectionCardinal(direction))
        sFollower.facing_direction = direction;

    RefreshPlayerTrail(player, FALSE);
}

void ModFollower_OnBattleStart(void)
{
    sFollower.suspended = TRUE;
    HideFollower();
}

void ModFollower_OnBattleEnd(void)
{
    sFollower.suspended = FALSE;
    sFollower.has_player_coords = FALSE;
}
