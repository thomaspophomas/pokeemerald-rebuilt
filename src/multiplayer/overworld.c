#include "global.h"
#include "global.fieldmap.h"
#include "event_object_movement.h"
#include "field_player_avatar.h"
#include "fieldmap.h"
#include "multiplayer/overworld.h"
#include "constants/event_object_movement.h"
#include "constants/event_objects.h"

#if FEATURE_MULTIPLAYER

struct RemotePlayerActor
{
    bool8 active;
    u8 playerId;
    u8 objectEventId;
    u8 localId;
    u8 mapGroup;
    u8 mapNum;
};

static EWRAM_DATA struct RemotePlayerActor sRemoteActors[MAX_NET_REMOTE_PLAYERS] = {0};

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
    sRemoteActors[i].objectEventId = OBJECT_EVENTS_COUNT;
    sRemoteActors[i].localId = NET_REMOTE_PLAYER_LOCAL_ID_BASE + i;
    sRemoteActors[i].mapGroup = 0;
    sRemoteActors[i].mapNum = 0;
}

static void DespawnRemoteActor(u8 i)
{
    if (sRemoteActors[i].active)
        RemoveObjectEventByLocalIdAndMap(sRemoteActors[i].localId, sRemoteActors[i].mapNum, sRemoteActors[i].mapGroup);
    ResetRemoteActor(i);
}

static bool8 SnapshotIsOnCurrentMap(const struct NetPlayerSnapshot *snapshot)
{
    if (!snapshot->active)
        return FALSE;
    if (snapshot->flags & NET_PLAYER_FLAG_HIDDEN)
        return FALSE;
    if (snapshot->mapGroup != gSaveBlock1Ptr->location.mapGroup)
        return FALSE;
    if (snapshot->mapNum != gSaveBlock1Ptr->location.mapNum)
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

static u8 GetMovementTypeForFacing(u8 direction)
{
    switch (direction)
    {
    case DIR_NORTH:
        return MOVEMENT_TYPE_FACE_UP;
    case DIR_SOUTH:
        return MOVEMENT_TYPE_FACE_DOWN;
    case DIR_WEST:
        return MOVEMENT_TYPE_FACE_LEFT;
    case DIR_EAST:
        return MOVEMENT_TYPE_FACE_RIGHT;
    default:
        return MOVEMENT_TYPE_FACE_DOWN;
    }
}

static void SpawnRemoteActor(u8 actorIndex, const struct NetPlayerSnapshot *snapshot)
{
    u8 graphicsId = snapshot->avatarGraphicsId;
    u8 movementType = GetMovementTypeForFacing(snapshot->facingDirection);
    u8 objectEventId;

    if (graphicsId == 0)
        graphicsId = GetPlayerAvatarGraphicsIdByCurrentState();

    objectEventId = SpawnSpecialObjectEventParameterized(
        graphicsId,
        movementType,
        sRemoteActors[actorIndex].localId,
        snapshot->x,
        snapshot->y,
        snapshot->elevation);

    if (objectEventId == OBJECT_EVENTS_COUNT)
    {
        ResetRemoteActor(actorIndex);
        return;
    }

    sRemoteActors[actorIndex].objectEventId = objectEventId;
    sRemoteActors[actorIndex].mapGroup = snapshot->mapGroup;
    sRemoteActors[actorIndex].mapNum = snapshot->mapNum;
}

static void MoveRemoteActor(u8 actorIndex, const struct NetPlayerSnapshot *snapshot)
{
    u8 objectEventId;
    s16 x = snapshot->x - MAP_OFFSET;
    s16 y = snapshot->y - MAP_OFFSET;

    if (sRemoteActors[actorIndex].mapGroup != snapshot->mapGroup
     || sRemoteActors[actorIndex].mapNum != snapshot->mapNum)
    {
        DespawnRemoteActor(actorIndex);
        return;
    }

    TryMoveObjectEventToMapCoords(sRemoteActors[actorIndex].localId, snapshot->mapNum, snapshot->mapGroup, x, y);
    objectEventId = GetObjectEventIdByLocalIdAndMap(sRemoteActors[actorIndex].localId, snapshot->mapNum, snapshot->mapGroup);
    if (objectEventId == OBJECT_EVENTS_COUNT)
    {
        DespawnRemoteActor(actorIndex);
        return;
    }

    sRemoteActors[actorIndex].objectEventId = objectEventId;
    ObjectEventTurn(&gObjectEvents[objectEventId], snapshot->facingDirection);
}

static void SyncRemoteActor(const struct NetPlayerSnapshot *snapshot)
{
    u8 actorIndex;

    actorIndex = GetRemoteActorIndexByPlayerId(snapshot->playerId);
    if (!SnapshotIsOnCurrentMap(snapshot))
    {
        if (actorIndex != MAX_NET_REMOTE_PLAYERS)
            DespawnRemoteActor(actorIndex);
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

void MultiplayerOverworld_OnMapLoad(void)
{
#if FEATURE_MULTIPLAYER
    u8 i;

    for (i = 0; i < MAX_NET_REMOTE_PLAYERS; i++)
        DespawnRemoteActor(i);
#endif
}

void MultiplayerOverworld_OnPlayerStep(u8 direction, u16 newKeys, u16 heldKeys)
{
}

void MultiplayerOverworld_BuildLocalSnapshot(struct NetPlayerSnapshot *snapshot, u8 playerId, u32 tick)
{
#if FEATURE_MULTIPLAYER
    struct ObjectEvent *playerObjEvent;

    memset(snapshot, 0, sizeof(*snapshot));
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
#else
    memset(snapshot, 0, sizeof(*snapshot));
#endif
}

void MultiplayerOverworld_Tick(const struct MultiplayerSession *session)
{
#if FEATURE_MULTIPLAYER
    u8 i;

    if (session == NULL || session->localPlayerId >= MAX_NET_PLAYERS)
        return;

    for (i = 0; i < MAX_NET_PLAYERS; i++)
    {
        if (i == session->localPlayerId)
            continue;
        SyncRemoteActor(&session->players[i]);
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
