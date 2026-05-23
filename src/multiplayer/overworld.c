#include "global.h"
#include "global.fieldmap.h"
#include "event_object_movement.h"
#include "field_player_avatar.h"
#include "fieldmap.h"
#include "main.h"
#include "multiplayer/interaction_menu.h"
#include "multiplayer/session.h"
#include "multiplayer/overworld.h"
#include "overworld.h"
#include "sprite.h"
#include "constants/event_object_movement.h"
#include "constants/event_objects.h"

#if FEATURE_MULTIPLAYER

struct RemotePlayerActor
{
    bool8 active;
    u8 playerId;
    u8 spriteId;
    u8 virtualObjId;
    u8 graphicsId;
    u8 mapGroup;
    u8 mapNum;
    u32 sessionEpoch;
    u16 graphicsRevision;
    u16 missingFrames;
};

static EWRAM_DATA struct RemotePlayerActor sRemoteActors[MAX_NET_REMOTE_PLAYERS] = {0};

#define REMOTE_ACTOR_MISSING_GRACE_FRAMES 60

static void SpawnRemoteActor(u8 actorIndex, const struct NetPlayerSnapshot *snapshot);

static u8 GetRemoteActorIndexByPlayerId(u8 playerId)
{
    u8 i;

    for (i = 0; i < MAX_NET_REMOTE_PLAYERS; i++)
    {
        if (sRemoteActors[i].active && sRemoteActors[i].playerId == playerId)
            return i;
    }

    return MAX_NET_REMOTE_PLAYERS;
}

static void ResetRemoteActor(u8 i)
{
    sRemoteActors[i].active = FALSE;
    sRemoteActors[i].playerId = NET_PLAYER_NONE;
    sRemoteActors[i].spriteId = MAX_SPRITES;
    sRemoteActors[i].virtualObjId = NET_REMOTE_PLAYER_VIRTUAL_ID_BASE + i;
    sRemoteActors[i].graphicsId = 0;
    sRemoteActors[i].mapGroup = 0;
    sRemoteActors[i].mapNum = 0;
    sRemoteActors[i].sessionEpoch = 0;
    sRemoteActors[i].graphicsRevision = 0;
    sRemoteActors[i].missingFrames = 0;
}

static void DespawnRemoteActor(u8 i)
{
    if (sRemoteActors[i].active)
        DestroyVirtualObject(sRemoteActors[i].virtualObjId);
    ResetRemoteActor(i);
}

static void MarkRemoteActorMissing(u8 i)
{
    if (!sRemoteActors[i].active)
        return;

    if (sRemoteActors[i].missingFrames < REMOTE_ACTOR_MISSING_GRACE_FRAMES)
    {
        sRemoteActors[i].missingFrames++;
        return;
    }

    DespawnRemoteActor(i);
}

static void RespawnRemoteActor(u8 actorIndex, const struct NetPlayerSnapshot *snapshot)
{
    if (sRemoteActors[actorIndex].active)
        DestroyVirtualObject(sRemoteActors[actorIndex].virtualObjId);

    ResetRemoteActor(actorIndex);
    sRemoteActors[actorIndex].active = TRUE;
    sRemoteActors[actorIndex].playerId = snapshot->playerId;
    SpawnRemoteActor(actorIndex, snapshot);
}

static bool8 DirectionIsValid(u8 direction)
{
    return direction == DIR_NORTH
        || direction == DIR_SOUTH
        || direction == DIR_WEST
        || direction == DIR_EAST;
}

static bool8 MovementActionShowsWalking(u8 movementActionId)
{
    return (movementActionId >= MOVEMENT_ACTION_WALK_SLOW_DOWN
         && movementActionId <= MOVEMENT_ACTION_WALK_NORMAL_RIGHT)
        || (movementActionId >= MOVEMENT_ACTION_WALK_FAST_DOWN
         && movementActionId <= MOVEMENT_ACTION_WALK_IN_PLACE_FASTER_RIGHT)
        || (movementActionId >= MOVEMENT_ACTION_RIDE_WATER_CURRENT_DOWN
         && movementActionId <= MOVEMENT_ACTION_PLAYER_RUN_RIGHT)
        || (movementActionId >= MOVEMENT_ACTION_JUMP_SPECIAL_DOWN
         && movementActionId <= MOVEMENT_ACTION_JUMP_SPECIAL_RIGHT)
        || (movementActionId >= MOVEMENT_ACTION_JUMP_DOWN
         && movementActionId <= MOVEMENT_ACTION_JUMP_IN_PLACE_RIGHT)
        || movementActionId == MOVEMENT_ACTION_WALK_DOWN_START_AFFINE
        || movementActionId == MOVEMENT_ACTION_WALK_DOWN_AFFINE
        || (movementActionId >= MOVEMENT_ACTION_ACRO_WHEELIE_HOP_DOWN
         && movementActionId <= MOVEMENT_ACTION_WALK_SLOW_DIAGONAL_DOWN_RIGHT);
}

static bool8 SnapshotIsFresh(const struct NetPlayerSnapshot *snapshot, u32 currentTick)
{
    if (snapshot->tick == 0 || currentTick == 0)
        return TRUE;
    if (snapshot->tick > currentTick)
        return snapshot->tick - currentTick <= NET_PLAYER_SNAPSHOT_FUTURE_SKEW_FRAMES;

    return currentTick - snapshot->tick <= NET_PLAYER_SNAPSHOT_TTL_FRAMES;
}

static bool8 SnapshotCoordsAreInBounds(const struct NetPlayerSnapshot *snapshot)
{
    s16 x = snapshot->x - MAP_OFFSET;
    s16 y = snapshot->y - MAP_OFFSET;

    if (gMapHeader.mapLayout == NULL)
        return FALSE;
    if (x < 0 || y < 0)
        return FALSE;
    if (x >= gMapHeader.mapLayout->width || y >= gMapHeader.mapLayout->height)
        return FALSE;

    return TRUE;
}

static bool8 SnapshotIsOnCurrentMap(const struct NetPlayerSnapshot *snapshot, u32 currentTick)
{
    if (!snapshot->active)
        return FALSE;
    if (snapshot->playerId >= MAX_NET_PLAYERS)
        return FALSE;
    if (snapshot->flags & ~NET_PLAYER_FLAGS_MASK)
        return FALSE;
    if (snapshot->flags & (NET_PLAYER_FLAG_HIDDEN | NET_PLAYER_FLAG_STALE | NET_PLAYER_FLAG_RESYNC))
        return FALSE;
    if (snapshot->staleFrames >= NET_PLAYER_STALE_FRAMES)
        return FALSE;
    if (snapshot->sequence == 0 || snapshot->sessionEpoch == 0)
        return FALSE;
    if (!DirectionIsValid(snapshot->facingDirection))
        return FALSE;
    if (!SnapshotIsFresh(snapshot, currentTick))
        return FALSE;
    if (snapshot->mapGroup != gSaveBlock1Ptr->location.mapGroup)
        return FALSE;
    if (snapshot->mapNum != gSaveBlock1Ptr->location.mapNum)
        return FALSE;
    if (!SnapshotCoordsAreInBounds(snapshot))
        return FALSE;

    return TRUE;
}

static u8 AllocRemoteActor(u8 playerId)
{
    u8 i;

    for (i = 0; i < MAX_NET_REMOTE_PLAYERS; i++)
    {
        if (!sRemoteActors[i].active)
        {
            ResetRemoteActor(i);
            sRemoteActors[i].active = TRUE;
            sRemoteActors[i].playerId = playerId;
            return i;
        }
    }

    return MAX_NET_REMOTE_PLAYERS;
}

static u8 CountFreeSpriteSlots(void)
{
    u8 i;
    u8 count = 0;

    for (i = 0; i < MAX_SPRITES; i++)
    {
        if (!gSprites[i].inUse)
            count++;
    }

    return count;
}

static u32 GetSnapshotDistanceFromLocalPlayer(const struct NetPlayerSnapshot *snapshot)
{
    struct ObjectEvent *playerObjEvent = &gObjectEvents[gPlayerAvatar.objectEventId];
    s32 dx = (s32)snapshot->x - playerObjEvent->currentCoords.x;
    s32 dy = (s32)snapshot->y - playerObjEvent->currentCoords.y;

    if (dx < 0)
        dx = -dx;
    if (dy < 0)
        dy = -dy;

    return dx + dy;
}

static bool8 SnapshotIsInteractionTarget(const struct NetPlayerSnapshot *snapshot, s16 targetX, s16 targetY)
{
    struct ObjectEvent *playerObjEvent = &gObjectEvents[gPlayerAvatar.objectEventId];
    s32 dx;
    s32 dy;

    if (snapshot->x == targetX && snapshot->y == targetY)
        return TRUE;

    dx = (s32)snapshot->x - playerObjEvent->currentCoords.x;
    dy = (s32)snapshot->y - playerObjEvent->currentCoords.y;
    if (dx < 0)
        dx = -dx;
    if (dy < 0)
        dy = -dy;

    return dx + dy <= 1;
}

static u8 GetRemoteSpriteBudget(void)
{
    u8 budget = CountFreeSpriteSlots() + MultiplayerOverworld_GetActiveRemoteAvatarCount();

    if (budget > MAX_NET_REMOTE_PLAYERS)
        budget = MAX_NET_REMOTE_PLAYERS;

    return budget;
}

static void SelectVisibleRemotePlayers(const struct MultiplayerSession *session, bool8 *selected)
{
    u8 selectedCount = 0;
    u8 budget = GetRemoteSpriteBudget();

    memset(selected, 0, MAX_NET_PLAYERS * sizeof(*selected));
    while (selectedCount < budget)
    {
        u8 i;
        u8 bestPlayerId = NET_PLAYER_NONE;
        u32 bestDistance = 0xFFFFFFFF;

        for (i = 0; i < MAX_NET_PLAYERS; i++)
        {
            u32 distance;

            if (i == session->localPlayerId || selected[i])
                continue;
            if (!SnapshotIsOnCurrentMap(&session->players[i], session->tick))
                continue;

            distance = GetSnapshotDistanceFromLocalPlayer(&session->players[i]);
            if (bestPlayerId == NET_PLAYER_NONE || distance < bestDistance || (distance == bestDistance && i < bestPlayerId))
            {
                bestPlayerId = i;
                bestDistance = distance;
            }
        }

        if (bestPlayerId == NET_PLAYER_NONE)
            return;

        selected[bestPlayerId] = TRUE;
        selectedCount++;
    }
}

static void SpawnRemoteActor(u8 actorIndex, const struct NetPlayerSnapshot *snapshot)
{
    u8 graphicsId = snapshot->avatarGraphicsId;
    u8 spriteId;

    if (graphicsId == 0)
        graphicsId = GetPlayerAvatarGraphicsIdByCurrentState();

    spriteId = CreateOrUpdateVirtualObject(
        graphicsId,
        sRemoteActors[actorIndex].virtualObjId,
        snapshot->x - MAP_OFFSET,
        snapshot->y - MAP_OFFSET,
        snapshot->elevation,
        snapshot->facingDirection,
        snapshot->graphicsRevision);

    if (spriteId == MAX_SPRITES)
    {
        ResetRemoteActor(actorIndex);
        return;
    }
    TurnVirtualObjectWithMovement(
        sRemoteActors[actorIndex].virtualObjId,
        snapshot->facingDirection,
        MovementActionShowsWalking(snapshot->movementActionId));

    sRemoteActors[actorIndex].spriteId = spriteId;
    sRemoteActors[actorIndex].graphicsId = graphicsId;
    sRemoteActors[actorIndex].mapGroup = snapshot->mapGroup;
    sRemoteActors[actorIndex].mapNum = snapshot->mapNum;
    sRemoteActors[actorIndex].sessionEpoch = snapshot->sessionEpoch;
    sRemoteActors[actorIndex].graphicsRevision = snapshot->graphicsRevision;
    sRemoteActors[actorIndex].missingFrames = 0;
}

static void MoveRemoteActor(u8 actorIndex, const struct NetPlayerSnapshot *snapshot)
{
    u8 graphicsId = snapshot->avatarGraphicsId;
    s16 x = snapshot->x - MAP_OFFSET;
    s16 y = snapshot->y - MAP_OFFSET;

    if (sRemoteActors[actorIndex].mapGroup != snapshot->mapGroup
     || sRemoteActors[actorIndex].mapNum != snapshot->mapNum
     || sRemoteActors[actorIndex].sessionEpoch != snapshot->sessionEpoch)
    {
        RespawnRemoteActor(actorIndex, snapshot);
        return;
    }

    if (graphicsId == 0)
        graphicsId = GetPlayerAvatarGraphicsIdByCurrentState();
    if (sRemoteActors[actorIndex].graphicsId != graphicsId
     || sRemoteActors[actorIndex].graphicsRevision != snapshot->graphicsRevision)
    {
        SetVirtualObjectGraphics(sRemoteActors[actorIndex].virtualObjId, graphicsId);
        sRemoteActors[actorIndex].graphicsId = graphicsId;
        sRemoteActors[actorIndex].graphicsRevision = snapshot->graphicsRevision;
    }

    SetVirtualObjectMapCoords(sRemoteActors[actorIndex].virtualObjId, x, y, snapshot->elevation);
    TurnVirtualObjectWithMovement(
        sRemoteActors[actorIndex].virtualObjId,
        snapshot->facingDirection,
        MovementActionShowsWalking(snapshot->movementActionId));
    sRemoteActors[actorIndex].missingFrames = 0;
}

static void SyncRemoteActor(const struct NetPlayerSnapshot *snapshot, u32 currentTick)
{
    u8 actorIndex;

    actorIndex = GetRemoteActorIndexByPlayerId(snapshot->playerId);
    if (!SnapshotIsOnCurrentMap(snapshot, currentTick))
    {
        if (actorIndex != MAX_NET_REMOTE_PLAYERS)
            MarkRemoteActorMissing(actorIndex);
        return;
    }

    if (actorIndex == MAX_NET_REMOTE_PLAYERS)
    {
        actorIndex = AllocRemoteActor(snapshot->playerId);
        if (actorIndex == MAX_NET_REMOTE_PLAYERS)
            return;
        SpawnRemoteActor(actorIndex, snapshot);
    }
    else
    {
        MoveRemoteActor(actorIndex, snapshot);
    }
}

#endif

void MultiplayerOverworld_Init(void)
{
#if FEATURE_MULTIPLAYER
    u8 i;

    for (i = 0; i < MAX_NET_REMOTE_PLAYERS; i++)
        ResetRemoteActor(i);
#endif
}

bool8 MultiplayerOverworld_CanTick(void)
{
#if FEATURE_MULTIPLAYER
    if (gMain.inBattle)
        return FALSE;
    if (gMain.callback1 != CB1_Overworld || gMain.callback2 != CB2_Overworld)
        return FALSE;
    if (gSaveBlock1Ptr == NULL || gMapHeader.mapLayout == NULL)
        return FALSE;
    if (gPlayerAvatar.objectEventId >= OBJECT_EVENTS_COUNT)
        return FALSE;
    if (!gObjectEvents[gPlayerAvatar.objectEventId].active || !gObjectEvents[gPlayerAvatar.objectEventId].isPlayer)
        return FALSE;

    return TRUE;
#else
    return FALSE;
#endif
}

void MultiplayerOverworld_Reset(void)
{
#if FEATURE_MULTIPLAYER
    u8 i;

    for (i = 0; i < MAX_NET_REMOTE_PLAYERS; i++)
        DespawnRemoteActor(i);
#endif
}

void MultiplayerOverworld_OnMapLoad(void)
{
    MultiplayerInteractionMenu_Reset();
    MultiplayerOverworld_Reset();
}

void MultiplayerOverworld_OnPlayerStep(u8 direction, u16 newKeys, u16 heldKeys)
{
    (void)direction;
    (void)newKeys;
    (void)heldKeys;
}

bool8 MultiplayerOverworld_TryInteractWithRemotePlayer(s16 x, s16 y, u8 elevation, u8 direction)
{
#if FEATURE_MULTIPLAYER
    const struct MultiplayerSession *session = MultiplayerSession_Get();
    u8 i;

    (void)elevation;
    (void)direction;

    if (session == NULL || session->localPlayerId >= MAX_NET_PLAYERS)
        return FALSE;
    if (!MultiplayerSession_IsOnline())
        return FALSE;
    if (MultiplayerInteractionMenu_IsActive())
        return FALSE;

    for (i = 0; i < MAX_NET_PLAYERS; i++)
    {
        const struct NetPlayerSnapshot *snapshot;

        if (i == session->localPlayerId)
            continue;

        snapshot = &session->players[i];
        if (!SnapshotIsOnCurrentMap(snapshot, session->tick))
            continue;
        if (!SnapshotIsInteractionTarget(snapshot, x, y))
            continue;
        if (MultiplayerSession_IsPlayerInteractionBlocked(i)
         || MultiplayerSession_IsPlayerInteractionBlocked(session->localPlayerId))
            return FALSE;

        return MultiplayerInteractionMenu_StartLocal(i);
    }
#else
    (void)x;
    (void)y;
    (void)elevation;
    (void)direction;
#endif

    return FALSE;
}

void MultiplayerOverworld_BuildLocalSnapshot(struct NetPlayerSnapshot *snapshot, u8 playerId, u32 tick)
{
#if FEATURE_MULTIPLAYER
    struct ObjectEvent *playerObjEvent;

    memset(snapshot, 0, sizeof(*snapshot));
    if (!MultiplayerOverworld_CanTick())
        return;

    snapshot->active = TRUE;
    snapshot->playerId = playerId;
    snapshot->mapGroup = gSaveBlock1Ptr->location.mapGroup;
    snapshot->mapNum = gSaveBlock1Ptr->location.mapNum;
    snapshot->tick = tick;

    playerObjEvent = &gObjectEvents[gPlayerAvatar.objectEventId];
    snapshot->x = playerObjEvent->currentCoords.x;
    snapshot->y = playerObjEvent->currentCoords.y;
    snapshot->elevation = playerObjEvent->previousElevation;
    snapshot->facingDirection = playerObjEvent->facingDirection;
    snapshot->movementActionId = playerObjEvent->movementActionId;
    snapshot->avatarGraphicsId = GetPlayerAvatarGraphicsIdByCurrentState();
    snapshot->graphicsRevision = ((u16)snapshot->avatarGraphicsId << 8) | snapshot->outfitId;
#else
    memset(snapshot, 0, sizeof(*snapshot));
#endif
}

void MultiplayerOverworld_Tick(const struct MultiplayerSession *session)
{
#if FEATURE_MULTIPLAYER
    u8 i;
    bool8 selected[MAX_NET_PLAYERS];

    if (session == NULL || session->localPlayerId >= MAX_NET_PLAYERS)
        return;
    if (!MultiplayerOverworld_CanTick())
    {
        MultiplayerOverworld_Reset();
        return;
    }

    SelectVisibleRemotePlayers(session, selected);

    for (i = 0; i < MAX_NET_PLAYERS; i++)
    {
        u8 actorIndex;

        if (i == session->localPlayerId)
            continue;

        actorIndex = GetRemoteActorIndexByPlayerId(i);
        if (!selected[i])
        {
            if (actorIndex != MAX_NET_REMOTE_PLAYERS)
                MarkRemoteActorMissing(actorIndex);
            continue;
        }

        SyncRemoteActor(&session->players[i], session->tick);
    }
#endif
}

u8 MultiplayerOverworld_GetRemoteAvatarCapacity(void)
{
    return MAX_NET_REMOTE_PLAYERS;
}

u8 MultiplayerOverworld_GetActiveRemoteAvatarCount(void)
{
#if FEATURE_MULTIPLAYER
    u8 i;
    u8 count = 0;

    for (i = 0; i < MAX_NET_REMOTE_PLAYERS; i++)
    {
        if (sRemoteActors[i].active)
            count++;
    }

    return count;
#else
    return 0;
#endif
}
