#include "global.h"
#include "constants/event_objects.h"
#include "constants/global.h"
#include "constants/species.h"
#include "event_object_movement.h"
#include "global.fieldmap.h"
#include "mod/follower.h"
#include "mod/overworld_sprite.h"
#include "mod/sprite_asset.h"
#include "pokemon.h"
#include "sprite.h"

#define MOD_FOLLOWER_FORM_DEFAULT 0
#define MOD_FOLLOWER_MAX_SYNC_DISTANCE 3

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
static bool8 TryGetPlayerObjectEvent(struct ObjectEvent **objectEvent);
static bool8 TryGetLeadPokemonFollower(const struct ModFollowerSpriteDefinition **definition, u16 *species, bool8 *shiny);
static bool8 IsDirectionCardinal(u8 direction);
static u8 GetDirectionFromDelta(s16 dx, s16 dy, u8 fallbackDirection);
static s16 AbsS16(s16 value);
static bool8 HasFollowerChangedMap(void);
static bool8 IsFollowerTooFarFrom(s16 x, s16 y);
static void SetCoordsBehindPlayer(struct ObjectEvent *player);
static void SyncFollowerGraphics(const struct ModFollowerSpriteDefinition *definition, u16 species, bool8 shiny);
static void SpawnOrUpdateFollowerSprite(s16 x, s16 y, u8 elevation, u8 direction, bool8 moving);
static void RefreshPlayerTrail(struct ObjectEvent *player, bool8 forceSnap);

static void ResetFollowerState(void)
{
    sFollower.active = FALSE;
    sFollower.suspended = FALSE;
    sFollower.hasPlayerCoords = FALSE;
    sFollower.spriteId = MAX_SPRITES;
    sFollower.graphicsId = OVERWORLD_SPRITE_API_INVALID_GFX;
    sFollower.paletteNum = 0xFF;
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
    u16 leadSpecies;
    bool8 leadShiny;
    const struct ModFollowerSpriteDefinition *follower;

    leadSpecies = GetMonData(&gPlayerParty[0], MON_DATA_SPECIES_OR_EGG, NULL);
    if (leadSpecies == SPECIES_NONE || leadSpecies == SPECIES_EGG)
        return FALSE;

    leadShiny = IsMonShiny(&gPlayerParty[0]);
    follower = OverworldSpriteApi_FindFollowerSprite(leadSpecies, MOD_FOLLOWER_FORM_DEFAULT, leadShiny);
    if (follower == NULL && leadShiny)
        follower = OverworldSpriteApi_FindFollowerSprite(leadSpecies, MOD_FOLLOWER_FORM_DEFAULT, FALSE);
    if (follower == NULL)
        return FALSE;

    *definition = follower;
    *species = leadSpecies;
    *shiny = leadShiny;
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

static void SyncFollowerGraphics(const struct ModFollowerSpriteDefinition *definition, u16 species, bool8 shiny)
{
    if (sFollower.active
     && sFollower.species == species
     && sFollower.shiny == shiny
     && sFollower.graphicsId == definition->graphicsId
     && sFollower.assetKey == definition->assetKey
     && sFollower.graphicsInfo == definition->graphicsInfo
     && sFollower.graphicsRevision == definition->graphicsRevision)
        return;

    if (sFollower.active)
        DestroyFollowerSprite();
    if (sFollower.assetKey != NULL && sFollower.assetKey != definition->assetKey)
        SpriteAssetApi_Release(sFollower.assetKey);

    sFollower.paletteNum = 0xFF;
    if (definition->assetKey != NULL)
    {
        SpriteAssetApi_LoadSheet(definition->assetKey);
        sFollower.paletteNum = SpriteAssetApi_LoadPaletteNum(definition->assetKey);
    }

    sFollower.species = species;
    sFollower.shiny = shiny;
    sFollower.graphicsId = definition->graphicsId;
    sFollower.assetKey = definition->assetKey;
    sFollower.graphicsInfo = definition->graphicsInfo;
    sFollower.graphicsRevision = definition->graphicsRevision;
}

static void SpawnOrUpdateFollowerSprite(s16 x, s16 y, u8 elevation, u8 direction, bool8 moving)
{
    if (!IsDirectionCardinal(direction))
        direction = sFollower.facingDirection;
    if (!IsDirectionCardinal(direction))
        direction = DIR_SOUTH;

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
    if (!TryGetLeadPokemonFollower(&definition, &species, &shiny))
    {
        DestroyFollowerSprite();
        return;
    }

    SyncFollowerGraphics(definition, species, shiny);
    RefreshPlayerTrail(player, FALSE);
}

void ModFollower_OnMapLoad(void)
{
    DestroyFollowerSprite();
    sFollower.hasPlayerCoords = FALSE;
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
    if (IsDirectionCardinal(direction))
        sFollower.facingDirection = direction;

    RefreshPlayerTrail(player, FALSE);
}

void ModFollower_OnBattleStart(void)
{
    sFollower.suspended = TRUE;
    DestroyFollowerSprite();
}

void ModFollower_OnBattleEnd(void)
{
    sFollower.suspended = FALSE;
    sFollower.hasPlayerCoords = FALSE;
}
