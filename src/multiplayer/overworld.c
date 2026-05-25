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
#include "pokemon.h"
#include "sprite.h"
#include "constants/characters.h"
#include "constants/event_object_movement.h"
#include "constants/event_objects.h"
#include "constants/species.h"

#if FEATURE_MULTIPLAYER

struct RemotePlayerActor
{
    bool8 active;
    u8 player_id;
    u8 spriteId;
    u8 virtualObjId;
    u8 graphicsId;
    u8 mapGroup;
    u8 mapNum;
    u32 sessionEpoch;
    u32 lastSequence;
    u16 graphicsRevision;
    u16 missingFrames;
    s16 currentX;
    s16 currentY;
    s16 targetX;
    s16 targetY;
    u8 targetElevation;
    u8 targetFacingDirection;
    bool8 targetWalking;
};

static EWRAM_DATA struct RemotePlayerActor sRemoteActors[MAX_NET_REMOTE_PLAYERS] = {0};

#define REMOTE_ACTOR_MISSING_GRACE_FRAMES 60

static void SpawnRemoteActor(u8 actor_index, const struct NetPlayerSnapshot *snapshot);
static bool8 RemoteActorIsVisible(const struct RemotePlayerActor *actor);

static void BuildLocalPartyProfile(struct NetPlayerBattleProfile *profile)
{
    u8 source_party_index;
    u8 profile_party_index = 0;

    if (gSaveBlock2Ptr != NULL)
    {
        profile->trainerGender = gSaveBlock2Ptr->playerGender;
        memcpy(profile->playerName, gSaveBlock2Ptr->playerName, PLAYER_NAME_LENGTH + 1);
        profile->playerName[PLAYER_NAME_LENGTH] = EOS;
    }

    for (source_party_index = 0; source_party_index < PARTY_SIZE && profile_party_index < NET_PLAYER_PARTY_SNAPSHOT_SIZE; source_party_index++)
    {
        u8 profile_move_index;
        u32 hp;
        u32 species = GetMonData(&gPlayerParty[source_party_index], MON_DATA_SPECIES_OR_EGG, NULL);

        if (species == SPECIES_NONE || species == SPECIES_EGG)
            continue;

        hp = GetMonData(&gPlayerParty[source_party_index], MON_DATA_HP, NULL);
        if (hp == 0)
            continue;

        profile->partySpecies[profile_party_index] = species;
        profile->partyHeldItems[profile_party_index] = GetMonData(&gPlayerParty[source_party_index], MON_DATA_HELD_ITEM, NULL);
        profile->partyLevels[profile_party_index] = GetMonData(&gPlayerParty[source_party_index], MON_DATA_LEVEL, NULL);
        profile->partyHp[profile_party_index] = min(hp, 0xFFFF);

        for (profile_move_index = 0; profile_move_index < MAX_MON_MOVES; profile_move_index++)
            profile->partyMoves[profile_party_index][profile_move_index] = GetMonData(&gPlayerParty[source_party_index], MON_DATA_MOVE1 + profile_move_index, NULL);

        profile_party_index++;
    }

    profile->partyCount = profile_party_index;
}

static u8 GetRemoteActorIndexByPlayerId(u8 player_id)
{
    u8 remote_actor_index;

    for (remote_actor_index = 0; remote_actor_index < MAX_NET_REMOTE_PLAYERS; remote_actor_index++)
    {
        if (sRemoteActors[remote_actor_index].active && sRemoteActors[remote_actor_index].player_id == player_id)
            return remote_actor_index;
    }

    return MAX_NET_REMOTE_PLAYERS;
}

static void ResetRemoteActor(u8 remote_actor_index)
{
    sRemoteActors[remote_actor_index].active = FALSE;
    sRemoteActors[remote_actor_index].player_id = NET_PLAYER_NONE;
    sRemoteActors[remote_actor_index].spriteId = MAX_SPRITES;
    sRemoteActors[remote_actor_index].virtualObjId = NET_REMOTE_PLAYER_VIRTUAL_ID_BASE + remote_actor_index;
    sRemoteActors[remote_actor_index].graphicsId = 0;
    sRemoteActors[remote_actor_index].mapGroup = 0;
    sRemoteActors[remote_actor_index].mapNum = 0;
    sRemoteActors[remote_actor_index].sessionEpoch = 0;
    sRemoteActors[remote_actor_index].lastSequence = 0;
    sRemoteActors[remote_actor_index].graphicsRevision = 0;
    sRemoteActors[remote_actor_index].missingFrames = 0;
    sRemoteActors[remote_actor_index].currentX = 0;
    sRemoteActors[remote_actor_index].currentY = 0;
    sRemoteActors[remote_actor_index].targetX = 0;
    sRemoteActors[remote_actor_index].targetY = 0;
    sRemoteActors[remote_actor_index].targetElevation = 0;
    sRemoteActors[remote_actor_index].targetFacingDirection = DIR_SOUTH;
    sRemoteActors[remote_actor_index].targetWalking = FALSE;
}

static void DespawnRemoteActor(u8 remote_actor_index)
{
    if (sRemoteActors[remote_actor_index].active)
        DestroyVirtualObject(sRemoteActors[remote_actor_index].virtualObjId);
    ResetRemoteActor(remote_actor_index);
}

static void MarkRemoteActorMissing(u8 remote_actor_index)
{
    if (!sRemoteActors[remote_actor_index].active)
        return;

    if (sRemoteActors[remote_actor_index].missingFrames < REMOTE_ACTOR_MISSING_GRACE_FRAMES)
    {
        sRemoteActors[remote_actor_index].missingFrames++;
        return;
    }

    DespawnRemoteActor(remote_actor_index);
}

static void RespawnRemoteActor(u8 actor_index, const struct NetPlayerSnapshot *snapshot)
{
    if (sRemoteActors[actor_index].active)
        DestroyVirtualObject(sRemoteActors[actor_index].virtualObjId);

    ResetRemoteActor(actor_index);
    sRemoteActors[actor_index].active = TRUE;
    sRemoteActors[actor_index].player_id = snapshot->playerId;
    SpawnRemoteActor(actor_index, snapshot);
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

static u8 AllocRemoteActor(u8 player_id)
{
    u8 remote_actor_index;

    for (remote_actor_index = 0; remote_actor_index < MAX_NET_REMOTE_PLAYERS; remote_actor_index++)
    {
        if (!sRemoteActors[remote_actor_index].active)
        {
            ResetRemoteActor(remote_actor_index);
            sRemoteActors[remote_actor_index].active = TRUE;
            sRemoteActors[remote_actor_index].player_id = player_id;
            return remote_actor_index;
        }
    }

    return MAX_NET_REMOTE_PLAYERS;
}

static u8 CountFreeSpriteSlots(void)
{
    u8 sprite_index;
    u8 count = 0;

    for (sprite_index = 0; sprite_index < MAX_SPRITES; sprite_index++)
    {
        if (!gSprites[sprite_index].inUse)
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
        u8 player_id;
        u8 bestPlayerId = NET_PLAYER_NONE;
        u32 bestDistance = 0xFFFFFFFF;

        for (player_id = 0; player_id < MAX_NET_PLAYERS; player_id++)
        {
            u32 distance;

            if (player_id == session->localPlayerId || selected[player_id])
                continue;
            if (!SnapshotIsOnCurrentMap(&session->players[player_id], session->tick))
                continue;

            distance = GetSnapshotDistanceFromLocalPlayer(&session->players[player_id]);
            if (bestPlayerId == NET_PLAYER_NONE || distance < bestDistance || (distance == bestDistance && player_id < bestPlayerId))
            {
                bestPlayerId = player_id;
                bestDistance = distance;
            }
        }

        if (bestPlayerId == NET_PLAYER_NONE)
            return;

        selected[bestPlayerId] = TRUE;
        selectedCount++;
    }
}

static void SpawnRemoteActor(u8 actor_index, const struct NetPlayerSnapshot *snapshot)
{
    u8 graphicsId = snapshot->avatarGraphicsId;
    u8 spriteId;
    s16 x = snapshot->x - MAP_OFFSET;
    s16 y = snapshot->y - MAP_OFFSET;

    if (graphicsId == 0)
        graphicsId = GetPlayerAvatarGraphicsIdByCurrentState();

    spriteId = CreateOrUpdateVirtualObject(
        graphicsId,
        sRemoteActors[actor_index].virtualObjId,
        x,
        y,
        snapshot->elevation,
        snapshot->facingDirection,
        snapshot->graphicsRevision);

    if (spriteId == MAX_SPRITES)
    {
        ResetRemoteActor(actor_index);
        return;
    }
    TurnVirtualObjectWithMovement(
        sRemoteActors[actor_index].virtualObjId,
        snapshot->facingDirection,
        MovementActionShowsWalking(snapshot->movementActionId));

    sRemoteActors[actor_index].spriteId = spriteId;
    sRemoteActors[actor_index].graphicsId = graphicsId;
    sRemoteActors[actor_index].mapGroup = snapshot->mapGroup;
    sRemoteActors[actor_index].mapNum = snapshot->mapNum;
    sRemoteActors[actor_index].sessionEpoch = snapshot->sessionEpoch;
    sRemoteActors[actor_index].lastSequence = snapshot->sequence;
    sRemoteActors[actor_index].graphicsRevision = snapshot->graphicsRevision;
    sRemoteActors[actor_index].missingFrames = 0;
    sRemoteActors[actor_index].currentX = x;
    sRemoteActors[actor_index].currentY = y;
    sRemoteActors[actor_index].targetX = x;
    sRemoteActors[actor_index].targetY = y;
    sRemoteActors[actor_index].targetElevation = snapshot->elevation;
    sRemoteActors[actor_index].targetFacingDirection = snapshot->facingDirection;
    sRemoteActors[actor_index].targetWalking = MovementActionShowsWalking(snapshot->movementActionId);
}

static s16 AbsS16(s16 value)
{
    return value < 0 ? -value : value;
}

static bool8 RemoteActorSpriteIsIdle(const struct RemotePlayerActor *actor)
{
    if (actor->spriteId >= MAX_SPRITES || !gSprites[actor->spriteId].inUse)
        return TRUE;

    return gSprites[actor->spriteId].x2 == 0 && gSprites[actor->spriteId].y2 == 0;
}

static u8 GetStepDirection(s16 dx, s16 dy)
{
    if (AbsS16(dx) >= AbsS16(dy) && dx != 0)
        return dx < 0 ? DIR_WEST : DIR_EAST;
    if (dy != 0)
        return dy < 0 ? DIR_NORTH : DIR_SOUTH;

    return DIR_SOUTH;
}

static void StepRemoteActorTowardTarget(u8 actor_index)
{
    struct RemotePlayerActor *actor = &sRemoteActors[actor_index];
    s16 dx = actor->targetX - actor->currentX;
    s16 dy = actor->targetY - actor->currentY;
    u8 direction;

    if (AbsS16(dx) + AbsS16(dy) > 4)
    {
        actor->currentX = actor->targetX;
        actor->currentY = actor->targetY;
        SetVirtualObjectMapCoords(actor->virtualObjId, actor->currentX, actor->currentY, actor->targetElevation);
        TurnVirtualObjectWithMovement(actor->virtualObjId, actor->targetFacingDirection, actor->targetWalking);
        return;
    }

    if (!RemoteActorSpriteIsIdle(actor))
        return;

    if (dx == 0 && dy == 0)
    {
        TurnVirtualObjectWithMovement(actor->virtualObjId, actor->targetFacingDirection, actor->targetWalking);
        return;
    }

    direction = GetStepDirection(dx, dy);
    switch (direction)
    {
    case DIR_NORTH:
        actor->currentY--;
        break;
    case DIR_SOUTH:
        actor->currentY++;
        break;
    case DIR_WEST:
        actor->currentX--;
        break;
    case DIR_EAST:
        actor->currentX++;
        break;
    }

    SetVirtualObjectMapCoords(actor->virtualObjId, actor->currentX, actor->currentY, actor->targetElevation);
    TurnVirtualObjectWithMovement(actor->virtualObjId, direction, TRUE);
}

static void MoveRemoteActor(u8 actor_index, const struct NetPlayerSnapshot *snapshot)
{
    u8 graphicsId = snapshot->avatarGraphicsId;
    s16 x = snapshot->x - MAP_OFFSET;
    s16 y = snapshot->y - MAP_OFFSET;

    if (sRemoteActors[actor_index].mapGroup != snapshot->mapGroup
     || sRemoteActors[actor_index].mapNum != snapshot->mapNum
     || sRemoteActors[actor_index].sessionEpoch != snapshot->sessionEpoch)
    {
        RespawnRemoteActor(actor_index, snapshot);
        return;
    }

    if (graphicsId == 0)
        graphicsId = GetPlayerAvatarGraphicsIdByCurrentState();
    if (sRemoteActors[actor_index].graphicsId != graphicsId
     || sRemoteActors[actor_index].graphicsRevision != snapshot->graphicsRevision)
    {
        SetVirtualObjectGraphics(sRemoteActors[actor_index].virtualObjId, graphicsId);
        sRemoteActors[actor_index].graphicsId = graphicsId;
        sRemoteActors[actor_index].graphicsRevision = snapshot->graphicsRevision;
    }

    if (snapshot->sequence > sRemoteActors[actor_index].lastSequence)
    {
        sRemoteActors[actor_index].targetX = x;
        sRemoteActors[actor_index].targetY = y;
        sRemoteActors[actor_index].targetElevation = snapshot->elevation;
        sRemoteActors[actor_index].targetFacingDirection = snapshot->facingDirection;
        sRemoteActors[actor_index].targetWalking = MovementActionShowsWalking(snapshot->movementActionId);
        sRemoteActors[actor_index].lastSequence = snapshot->sequence;
    }
    StepRemoteActorTowardTarget(actor_index);
    sRemoteActors[actor_index].missingFrames = 0;
}

static void SyncRemoteActor(const struct NetPlayerSnapshot *snapshot, u32 currentTick)
{
    u8 actor_index;

    actor_index = GetRemoteActorIndexByPlayerId(snapshot->playerId);
    if (!SnapshotIsOnCurrentMap(snapshot, currentTick))
    {
        if (actor_index != MAX_NET_REMOTE_PLAYERS)
            MarkRemoteActorMissing(actor_index);
        return;
    }

    if (actor_index == MAX_NET_REMOTE_PLAYERS)
    {
        actor_index = AllocRemoteActor(snapshot->playerId);
        if (actor_index == MAX_NET_REMOTE_PLAYERS)
            return;
        SpawnRemoteActor(actor_index, snapshot);
    }
    else
    {
        MoveRemoteActor(actor_index, snapshot);
    }
}

#endif

void MultiplayerOverworld_Init(void)
{
#if FEATURE_MULTIPLAYER
    u8 remote_actor_index;

    for (remote_actor_index = 0; remote_actor_index < MAX_NET_REMOTE_PLAYERS; remote_actor_index++)
        ResetRemoteActor(remote_actor_index);
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
    u8 remote_actor_index;

    for (remote_actor_index = 0; remote_actor_index < MAX_NET_REMOTE_PLAYERS; remote_actor_index++)
        DespawnRemoteActor(remote_actor_index);
#endif
}

void MultiplayerOverworld_OnMapLoad(void)
{
    MultiplayerInteractionMenu_Reset();
    MultiplayerOverworld_Reset();
}

void MultiplayerOverworld_OnPlayerStep(u8 direction, u16 new_keys, u16 held_keys)
{
    (void)direction;
    (void)new_keys;
    (void)held_keys;
}

bool8 MultiplayerOverworld_TryInteractWithRemotePlayer(s16 x, s16 y, u8 elevation, u8 direction)
{
#if FEATURE_MULTIPLAYER
    const struct MultiplayerSession *session = MultiplayerSession_Get();
    u8 remote_player_id;

    (void)elevation;
    (void)direction;

    if (session == NULL || session->localPlayerId >= MAX_NET_PLAYERS)
        return FALSE;
    if (!MultiplayerSession_IsOnline())
        return FALSE;
    if (MultiplayerInteractionMenu_IsActive())
        return FALSE;

    for (remote_player_id = 0; remote_player_id < MAX_NET_PLAYERS; remote_player_id++)
    {
        const struct NetPlayerSnapshot *snapshot;

        if (remote_player_id == session->localPlayerId)
            continue;

        snapshot = &session->players[remote_player_id];
        if (!SnapshotIsOnCurrentMap(snapshot, session->tick))
            continue;
        if (!SnapshotIsInteractionTarget(snapshot, x, y))
            continue;
        if (MultiplayerSession_IsPlayerInteractionBlocked(remote_player_id)
         || MultiplayerSession_IsPlayerInteractionBlocked(session->localPlayerId))
            return FALSE;

        return MultiplayerInteractionMenu_StartLocal(remote_player_id);
    }
#else
    (void)x;
    (void)y;
    (void)elevation;
    (void)direction;
#endif

    return FALSE;
}

bool8 MultiplayerOverworld_HasVisibleRemotePlayer(void)
{
#if FEATURE_MULTIPLAYER
    return MultiplayerOverworld_TryGetVisibleRemotePlayer(NULL);
#else
    return FALSE;
#endif
}

#if FEATURE_MULTIPLAYER
static bool8 RemoteActorIsVisible(const struct RemotePlayerActor *actor)
{
    const struct Sprite *sprite;
    s16 x;
    s16 y;

    if (actor == NULL || !actor->active || actor->spriteId >= MAX_SPRITES)
        return FALSE;

    sprite = &gSprites[actor->spriteId];
    if (!sprite->inUse)
        return FALSE;

    x = sprite->x + sprite->x2;
    y = sprite->y + sprite->y2;
    return x >= -16 && x <= DISPLAY_WIDTH + 16 && y >= -16 && y <= DISPLAY_HEIGHT + 16;
}
#endif

bool8 MultiplayerOverworld_TryGetVisibleRemotePlayer(u8 *remote_player_id)
{
#if FEATURE_MULTIPLAYER
    u8 remote_actor_index;
    u8 best_player_id = NET_PLAYER_NONE;
    u32 best_distance = 0xFFFFFFFF;
    struct ObjectEvent *player_object_event = NULL;

    if (remote_player_id != NULL)
        *remote_player_id = NET_PLAYER_NONE;
    if (gPlayerAvatar.objectEventId < OBJECT_EVENTS_COUNT)
        player_object_event = &gObjectEvents[gPlayerAvatar.objectEventId];

    for (remote_actor_index = 0; remote_actor_index < MAX_NET_REMOTE_PLAYERS; remote_actor_index++)
    {
        u32 distance = 0;

        if (!RemoteActorIsVisible(&sRemoteActors[remote_actor_index]))
            continue;
        if (player_object_event != NULL)
        {
            s32 x_distance_from_player = (s32)sRemoteActors[remote_actor_index].currentX + MAP_OFFSET - player_object_event->currentCoords.x;
            s32 y_distance_from_player = (s32)sRemoteActors[remote_actor_index].currentY + MAP_OFFSET - player_object_event->currentCoords.y;

            if (x_distance_from_player < 0)
                x_distance_from_player = -x_distance_from_player;
            if (y_distance_from_player < 0)
                y_distance_from_player = -y_distance_from_player;
            distance = x_distance_from_player + y_distance_from_player;
        }
        if (best_player_id == NET_PLAYER_NONE || distance < best_distance || (distance == best_distance && sRemoteActors[remote_actor_index].player_id < best_player_id))
        {
            best_player_id = sRemoteActors[remote_actor_index].player_id;
            best_distance = distance;
        }
    }

    if (best_player_id == NET_PLAYER_NONE)
        return FALSE;
    if (remote_player_id != NULL)
        *remote_player_id = best_player_id;
    return TRUE;
#else
    if (remote_player_id != NULL)
        *remote_player_id = NET_PLAYER_NONE;
    return FALSE;
#endif
}

void MultiplayerOverworld_BuildLocalSnapshot(struct NetPlayerSnapshot *snapshot, u8 local_player_id, u32 tick)
{
#if FEATURE_MULTIPLAYER
    struct ObjectEvent *player_object_event;

    memset(snapshot, 0, sizeof(*snapshot));
    if (!MultiplayerOverworld_CanTick())
        return;

    snapshot->active = TRUE;
    snapshot->playerId = local_player_id;
    snapshot->mapGroup = gSaveBlock1Ptr->location.mapGroup;
    snapshot->mapNum = gSaveBlock1Ptr->location.mapNum;
    snapshot->tick = tick;

    player_object_event = &gObjectEvents[gPlayerAvatar.objectEventId];
    snapshot->x = player_object_event->currentCoords.x;
    snapshot->y = player_object_event->currentCoords.y;
    snapshot->elevation = player_object_event->previousElevation;
    snapshot->facingDirection = player_object_event->facingDirection;
    snapshot->movementActionId = player_object_event->movementActionId;
    snapshot->avatarGraphicsId = GetPlayerAvatarGraphicsIdByCurrentState();
    snapshot->graphicsRevision = ((u16)snapshot->avatarGraphicsId << 8) | snapshot->outfitId;
#else
    memset(snapshot, 0, sizeof(*snapshot));
#endif
}

void MultiplayerOverworld_BuildLocalBattleProfile(struct NetPlayerBattleProfile *profile)
{
#if FEATURE_MULTIPLAYER
    memset(profile, 0, sizeof(*profile));
    if (!MultiplayerOverworld_CanTick())
        return;

    BuildLocalPartyProfile(profile);
#else
    memset(profile, 0, sizeof(*profile));
#endif
}

void MultiplayerOverworld_Tick(const struct MultiplayerSession *session)
{
#if FEATURE_MULTIPLAYER
    u8 remote_player_id;
    bool8 selected[MAX_NET_PLAYERS];

    if (session == NULL || session->localPlayerId >= MAX_NET_PLAYERS)
        return;
    if (!MultiplayerOverworld_CanTick())
    {
        MultiplayerOverworld_Reset();
        return;
    }

    SelectVisibleRemotePlayers(session, selected);

    for (remote_player_id = 0; remote_player_id < MAX_NET_PLAYERS; remote_player_id++)
    {
        u8 actor_index;

        if (remote_player_id == session->localPlayerId)
            continue;

        actor_index = GetRemoteActorIndexByPlayerId(remote_player_id);
        if (!selected[remote_player_id])
        {
            if (actor_index != MAX_NET_REMOTE_PLAYERS)
                MarkRemoteActorMissing(actor_index);
            continue;
        }

        SyncRemoteActor(&session->players[remote_player_id], session->tick);
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
    u8 remote_actor_index;
    u8 count = 0;

    for (remote_actor_index = 0; remote_actor_index < MAX_NET_REMOTE_PLAYERS; remote_actor_index++)
    {
        if (sRemoteActors[remote_actor_index].active)
            count++;
    }

    return count;
#else
    return 0;
#endif
}
