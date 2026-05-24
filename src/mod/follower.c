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
    bool8 hasPlayerCoords;
    u8 spriteId;
    u8 graphicsId;
    u8 paletteNum;
    const char *assetKey;
    const struct ObjectEventGraphicsInfo *graphicsInfo;
    u16 graphicsRevision;
    u8 elevation;
    u8 facingDirection;
    u8 mapGroup;
    u8 mapNum;
    s16 x;
    s16 y;
    s16 lastPlayerX;
    s16 lastPlayerY;
    u16 species;
    bool8 shiny;
};

static EWRAM_DATA struct ModFollowerState sFollower = {};

static void ResetFollowerState(void);
static void DestroyFollowerSprite(void);
static void ReleaseFollowerAssets(void);
static void HideFollower(void);
static bool8 TryGetPlayerObjectEvent(struct ObjectEvent **objectEvent);
static bool8 TryGetLeadPokemonFollower(const struct ModFollowerSpriteDefinition **definition, u16 *species, bool8 *shiny);
static bool8 TryGetPokemonFollower(u8 partyIndex, const struct ModFollowerSpriteDefinition **definition, u16 *species, bool8 *shiny);
static bool8 IsDirectionCardinal(u8 direction);
static u8 GetDirectionFromDelta(s16 dx, s16 dy, u8 fallbackDirection);
static s16 AbsS16(s16 value);
static bool8 HasFollowerChangedMap(void);
static bool8 IsFollowerTooFarFrom(s16 x, s16 y);
static bool8 IsPlayerInUnsupportedFollowerState(struct ObjectEvent *player);
static bool8 IsFollowerTileBehaviorBlocked(u8 metatileBehavior);
static bool8 IsFollowerTileValid(s16 x, s16 y, u8 elevation);
static void SetCoordsBehindPlayer(struct ObjectEvent *player);
static bool8 TryLoadFollowerAssets(const struct ModFollowerSpriteDefinition *definition, u8 *paletteNum);
static bool8 SyncFollowerGraphics(const struct ModFollowerSpriteDefinition *definition, u16 species, bool8 shiny);
static void SpawnOrUpdateFollowerSprite(s16 x, s16 y, u8 elevation, u8 direction, bool8 moving);
static void RefreshPlayerTrail(struct ObjectEvent *player, bool8 forceSnap);

static void ResetFollowerState(void)
{
    sFollower.active = FALSE;
    sFollower.suspended = FALSE;
    sFollower.hasPlayerCoords = FALSE;
    sFollower.spriteId = MAX_SPRITES;
    sFollower.graphicsId = OVERWORLD_SPRITE_API_INVALID_GFX;
    sFollower.paletteNum = MOD_FOLLOWER_INVALID_PALETTE;
    sFollower.assetKey = NULL;
    sFollower.graphicsInfo = NULL;
    sFollower.graphicsRevision = 0;
    sFollower.elevation = ELEVATION_DEFAULT;
    sFollower.facingDirection = DIR_SOUTH;
    sFollower.mapGroup = 0;
    sFollower.mapNum = 0;
    sFollower.x = 0;
    sFollower.y = 0;
    sFollower.lastPlayerX = 0;
    sFollower.lastPlayerY = 0;
    sFollower.species = SPECIES_NONE;
    sFollower.shiny = FALSE;
}

static void DestroyFollowerSprite(void)
{
    DestroyVirtualObject(OVERWORLD_SPRITE_API_FOLLOWER_VIRTUAL_ID);
    sFollower.active = FALSE;
    sFollower.spriteId = MAX_SPRITES;
}

static void ReleaseFollowerAssets(void)
{
    if (sFollower.assetKey != NULL)
        SpriteAssetApi_Release(sFollower.assetKey);

    sFollower.graphicsId = OVERWORLD_SPRITE_API_INVALID_GFX;
    sFollower.paletteNum = MOD_FOLLOWER_INVALID_PALETTE;
    sFollower.assetKey = NULL;
    sFollower.graphicsInfo = NULL;
    sFollower.graphicsRevision = 0;
    sFollower.species = SPECIES_NONE;
    sFollower.shiny = FALSE;
}

static void HideFollower(void)
{
    DestroyFollowerSprite();
    ReleaseFollowerAssets();
    sFollower.hasPlayerCoords = FALSE;
}

static bool8 TryGetPlayerObjectEvent(struct ObjectEvent **objectEvent)
{
    if (gPlayerAvatar.objectEventId >= OBJECT_EVENTS_COUNT)
        return FALSE;
    if (!gObjectEvents[gPlayerAvatar.objectEventId].active)
        return FALSE;

    *objectEvent = &gObjectEvents[gPlayerAvatar.objectEventId];
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

static bool8 TryGetPokemonFollower(u8 partyIndex, const struct ModFollowerSpriteDefinition **definition, u16 *species, bool8 *shiny)
{
    u16 monSpecies;
    bool8 monShiny;
    const struct ModFollowerSpriteDefinition *follower;

    monSpecies = GetMonData(&gPlayerParty[partyIndex], MON_DATA_SPECIES_OR_EGG, NULL);
    if (monSpecies == SPECIES_NONE || monSpecies == SPECIES_EGG)
        return FALSE;
    if (GetMonData(&gPlayerParty[partyIndex], MON_DATA_HP, NULL) == 0)
        return FALSE;

    monShiny = IsMonShiny(&gPlayerParty[partyIndex]);
    follower = OverworldSpriteApi_FindFollowerSprite(monSpecies, MOD_FOLLOWER_FORM_DEFAULT, monShiny);
    if (follower == NULL && monShiny)
        follower = OverworldSpriteApi_FindFollowerSprite(monSpecies, MOD_FOLLOWER_FORM_DEFAULT, FALSE);
    if (follower == NULL)
        return FALSE;

    *definition = follower;
    *species = monSpecies;
    *shiny = monShiny;
    return TRUE;
}

static bool8 IsDirectionCardinal(u8 direction)
{
    return direction == DIR_SOUTH
        || direction == DIR_NORTH
        || direction == DIR_WEST
        || direction == DIR_EAST;
}

static u8 GetDirectionFromDelta(s16 dx, s16 dy, u8 fallbackDirection)
{
    if (AbsS16(dx) > AbsS16(dy))
        return dx < 0 ? DIR_WEST : DIR_EAST;
    if (dy != 0)
        return dy < 0 ? DIR_NORTH : DIR_SOUTH;
    if (dx != 0)
        return dx < 0 ? DIR_WEST : DIR_EAST;
    return fallbackDirection;
}

static s16 AbsS16(s16 value)
{
    return value < 0 ? -value : value;
}

static bool8 HasFollowerChangedMap(void)
{
    return sFollower.mapGroup != gSaveBlock1Ptr->location.mapGroup
        || sFollower.mapNum != gSaveBlock1Ptr->location.mapNum;
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

static bool8 IsFollowerTileBehaviorBlocked(u8 metatileBehavior)
{
    return MetatileBehavior_IsSurfableWaterOrUnderwater(metatileBehavior)
        || MetatileBehavior_IsWarpDoor(metatileBehavior)
        || MetatileBehavior_IsDoor(metatileBehavior)
        || MetatileBehavior_IsNonAnimDoor(metatileBehavior)
        || MetatileBehavior_IsDeepSouthWarp(metatileBehavior)
        || MetatileBehavior_IsEastArrowWarp(metatileBehavior)
        || MetatileBehavior_IsWestArrowWarp(metatileBehavior)
        || MetatileBehavior_IsNorthArrowWarp(metatileBehavior)
        || MetatileBehavior_IsSouthArrowWarp(metatileBehavior)
        || MetatileBehavior_IsLavaridgeB1FWarp(metatileBehavior)
        || MetatileBehavior_IsLavaridge1FWarp(metatileBehavior)
        || MetatileBehavior_IsAquaHideoutWarp(metatileBehavior)
        || MetatileBehavior_IsUnionRoomWarp(metatileBehavior)
        || MetatileBehavior_IsMossdeepGymWarp(metatileBehavior)
        || MetatileBehavior_IsBattlePyramidWarp(metatileBehavior)
        || MetatileBehavior_IsMtPyreHole(metatileBehavior)
        || MetatileBehavior_IsCrackedFloorHole(metatileBehavior)
        || MetatileBehavior_IsEscalator(metatileBehavior)
        || MetatileBehavior_IsLadder(metatileBehavior)
        || MetatileBehavior_IsForcedMovementTile(metatileBehavior)
        || MetatileBehavior_IsJumpSouth(metatileBehavior)
        || MetatileBehavior_IsJumpNorth(metatileBehavior)
        || MetatileBehavior_IsJumpWest(metatileBehavior)
        || MetatileBehavior_IsJumpEast(metatileBehavior);
}

static bool8 IsFollowerTileValid(s16 x, s16 y, u8 elevation)
{
    u8 metatileBehavior;

    if (MapGridGetCollisionAt(x, y) != COLLISION_NONE)
        return FALSE;
    if (GetMapBorderIdAt(x, y) == CONNECTION_INVALID)
        return FALSE;
    if (GetObjectEventIdByPosition(x, y, elevation) != OBJECT_EVENTS_COUNT)
        return FALSE;

    metatileBehavior = MapGridGetMetatileBehaviorAt(x, y);
    if (IsFollowerTileBehaviorBlocked(metatileBehavior))
        return FALSE;

    return TRUE;
}

static void SetCoordsBehindPlayer(struct ObjectEvent *player)
{
    u8 direction;

    direction = player->facingDirection;
    if (!IsDirectionCardinal(direction))
        direction = DIR_SOUTH;

    sFollower.x = player->currentCoords.x;
    sFollower.y = player->currentCoords.y;
    MoveCoords(GetOppositeDirection(direction), &sFollower.x, &sFollower.y);
    sFollower.lastPlayerX = player->currentCoords.x;
    sFollower.lastPlayerY = player->currentCoords.y;
    sFollower.hasPlayerCoords = TRUE;
    sFollower.facingDirection = direction;
}

static bool8 TryLoadFollowerAssets(const struct ModFollowerSpriteDefinition *definition, u8 *paletteNum)
{
    *paletteNum = MOD_FOLLOWER_INVALID_PALETTE;
    if (definition->assetKey == NULL)
        return TRUE;

    if (definition->graphicsInfo == NULL && !SpriteAssetApi_LoadSheet(definition->assetKey))
        return FALSE;
    if (definition->graphicsInfo != NULL)
        SpriteAssetApi_LoadSheet(definition->assetKey);

    *paletteNum = SpriteAssetApi_LoadPaletteNum(definition->assetKey);
    return *paletteNum != MOD_FOLLOWER_INVALID_PALETTE;
}

static bool8 SyncFollowerGraphics(const struct ModFollowerSpriteDefinition *definition, u16 species, bool8 shiny)
{
    u8 paletteNum;

    if (sFollower.species == species
     && sFollower.shiny == shiny
     && sFollower.graphicsId == definition->graphicsId
     && sFollower.assetKey == definition->assetKey
     && sFollower.graphicsInfo == definition->graphicsInfo
     && sFollower.graphicsRevision == definition->graphicsRevision)
        return TRUE;

    if (sFollower.active)
        DestroyFollowerSprite();
    if (sFollower.assetKey != NULL && sFollower.assetKey != definition->assetKey)
        ReleaseFollowerAssets();

    if (!TryLoadFollowerAssets(definition, &paletteNum))
    {
        if (definition->assetKey != NULL)
            SpriteAssetApi_Release(definition->assetKey);
        ReleaseFollowerAssets();
        return FALSE;
    }

    sFollower.species = species;
    sFollower.shiny = shiny;
    sFollower.graphicsId = definition->graphicsId;
    sFollower.assetKey = definition->assetKey;
    sFollower.graphicsInfo = definition->graphicsInfo;
    sFollower.graphicsRevision = definition->graphicsRevision;
    sFollower.paletteNum = paletteNum;
    return TRUE;
}

static void SpawnOrUpdateFollowerSprite(s16 x, s16 y, u8 elevation, u8 direction, bool8 moving)
{
    if (!IsDirectionCardinal(direction))
        direction = sFollower.facingDirection;
    if (!IsDirectionCardinal(direction))
        direction = DIR_SOUTH;

    if (!IsFollowerTileValid(x, y, elevation))
    {
        DestroyFollowerSprite();
        return;
    }

    if (sFollower.graphicsInfo != NULL)
    {
        sFollower.spriteId = CreateOrUpdateVirtualObjectFromGraphicsInfo(
            sFollower.graphicsInfo,
            sFollower.graphicsId,
            OVERWORLD_SPRITE_API_FOLLOWER_VIRTUAL_ID,
            x,
            y,
            elevation,
            direction,
            sFollower.graphicsRevision,
            sFollower.paletteNum);
    }
    else
    {
        sFollower.spriteId = CreateOrUpdateVirtualObject(
            sFollower.graphicsId,
            OVERWORLD_SPRITE_API_FOLLOWER_VIRTUAL_ID,
            x,
            y,
            elevation,
            direction,
            sFollower.graphicsRevision);
    }
    if (sFollower.spriteId == MAX_SPRITES)
    {
        sFollower.active = FALSE;
        return;
    }

    sFollower.active = TRUE;
    sFollower.elevation = elevation;
    sFollower.facingDirection = direction;
    sFollower.mapGroup = gSaveBlock1Ptr->location.mapGroup;
    sFollower.mapNum = gSaveBlock1Ptr->location.mapNum;
    TurnVirtualObjectWithMovement(OVERWORLD_SPRITE_API_FOLLOWER_VIRTUAL_ID, direction, moving);
}

static void RefreshPlayerTrail(struct ObjectEvent *player, bool8 forceSnap)
{
    s16 playerX;
    s16 playerY;
    u8 playerDirection;
    u8 elevation;
    bool8 moved;
    bool8 snap;

    playerX = player->currentCoords.x;
    playerY = player->currentCoords.y;
    playerDirection = player->facingDirection;
    if (!IsDirectionCardinal(playerDirection))
        playerDirection = sFollower.facingDirection;
    if (!IsDirectionCardinal(playerDirection))
        playerDirection = DIR_SOUTH;

    elevation = player->currentElevation;
    if (elevation == 0)
        elevation = ELEVATION_DEFAULT;

    snap = forceSnap || !sFollower.active || !sFollower.hasPlayerCoords || HasFollowerChangedMap();
    if (!snap)
        snap = IsFollowerTooFarFrom(playerX, playerY);

    if (snap)
    {
        SetCoordsBehindPlayer(player);
        SpawnOrUpdateFollowerSprite(sFollower.x, sFollower.y, elevation, playerDirection, FALSE);
        return;
    }

    moved = playerX != sFollower.lastPlayerX || playerY != sFollower.lastPlayerY;
    if (moved)
    {
        s16 targetX;
        s16 targetY;
        u8 moveDirection;

        targetX = sFollower.lastPlayerX;
        targetY = sFollower.lastPlayerY;
        moveDirection = GetDirectionFromDelta(targetX - sFollower.x, targetY - sFollower.y, playerDirection);
        sFollower.x = targetX;
        sFollower.y = targetY;
        sFollower.lastPlayerX = playerX;
        sFollower.lastPlayerY = playerY;
        SpawnOrUpdateFollowerSprite(sFollower.x, sFollower.y, elevation, moveDirection, TRUE);
    }
    else
    {
        SpawnOrUpdateFollowerSprite(sFollower.x, sFollower.y, elevation, playerDirection, FALSE);
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

void ModFollower_OnPlayerStep(u8 direction, u16 newKeys, u16 heldKeys)
{
    struct ObjectEvent *player;

    (void)newKeys;
    (void)heldKeys;

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
        sFollower.facingDirection = direction;

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
    sFollower.hasPlayerCoords = FALSE;
}
