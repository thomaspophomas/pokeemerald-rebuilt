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
    u8 sprite_id;
    u8 virtual_object_id;
    u8 graphics_id;
    u8 map_group;
    u8 map_num;
    u32 session_epoch;
    u32 last_sequence;
    u16 graphics_revision;
    u16 missing_frames;
    s16 current_x;
    s16 current_y;
    s16 target_x;
    s16 target_y;
    u8 target_elevation;
    u8 target_facing_direction;
    bool8 target_walking;
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
        profile->trainer_gender = gSaveBlock2Ptr->playerGender;
        memcpy(profile->player_name, gSaveBlock2Ptr->player_name, PLAYER_NAME_LENGTH + 1);
        profile->player_name[PLAYER_NAME_LENGTH] = EOS;
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

        profile->party_species[profile_party_index] = species;
        profile->party_held_items[profile_party_index] = GetMonData(&gPlayerParty[source_party_index], MON_DATA_HELD_ITEM, NULL);
        profile->party_levels[profile_party_index] = GetMonData(&gPlayerParty[source_party_index], MON_DATA_LEVEL, NULL);
        profile->party_hp[profile_party_index] = min(hp, 0xFFFF);

        for (profile_move_index = 0; profile_move_index < MAX_MON_MOVES; profile_move_index++)
            profile->party_moves[profile_party_index][profile_move_index] = GetMonData(&gPlayerParty[source_party_index], MON_DATA_MOVE1 + profile_move_index, NULL);

        profile_party_index++;
    }

    profile->party_count = profile_party_index;
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
    sRemoteActors[remote_actor_index].sprite_id = MAX_SPRITES;
    sRemoteActors[remote_actor_index].virtual_object_id = NET_REMOTE_PLAYER_VIRTUAL_ID_BASE + remote_actor_index;
    sRemoteActors[remote_actor_index].graphics_id = 0;
    sRemoteActors[remote_actor_index].map_group = 0;
    sRemoteActors[remote_actor_index].map_num = 0;
    sRemoteActors[remote_actor_index].session_epoch = 0;
    sRemoteActors[remote_actor_index].last_sequence = 0;
    sRemoteActors[remote_actor_index].graphics_revision = 0;
    sRemoteActors[remote_actor_index].missing_frames = 0;
    sRemoteActors[remote_actor_index].current_x = 0;
    sRemoteActors[remote_actor_index].current_y = 0;
    sRemoteActors[remote_actor_index].target_x = 0;
    sRemoteActors[remote_actor_index].target_y = 0;
    sRemoteActors[remote_actor_index].target_elevation = 0;
    sRemoteActors[remote_actor_index].target_facing_direction = DIR_SOUTH;
    sRemoteActors[remote_actor_index].target_walking = FALSE;
}

static void DespawnRemoteActor(u8 remote_actor_index)
{
    if (sRemoteActors[remote_actor_index].active)
        DestroyVirtualObject(sRemoteActors[remote_actor_index].virtual_object_id);
    ResetRemoteActor(remote_actor_index);
}

static void MarkRemoteActorMissing(u8 remote_actor_index)
{
    if (!sRemoteActors[remote_actor_index].active)
        return;

    if (sRemoteActors[remote_actor_index].missing_frames < REMOTE_ACTOR_MISSING_GRACE_FRAMES)
    {
        sRemoteActors[remote_actor_index].missing_frames++;
        return;
    }

    DespawnRemoteActor(remote_actor_index);
}

static void RespawnRemoteActor(u8 actor_index, const struct NetPlayerSnapshot *snapshot)
{
    if (sRemoteActors[actor_index].active)
        DestroyVirtualObject(sRemoteActors[actor_index].virtual_object_id);

    ResetRemoteActor(actor_index);
    sRemoteActors[actor_index].active = TRUE;
    sRemoteActors[actor_index].player_id = snapshot->player_id;
    SpawnRemoteActor(actor_index, snapshot);
}

static bool8 DirectionIsValid(u8 direction)
{
    return direction == DIR_NORTH
        || direction == DIR_SOUTH
        || direction == DIR_WEST
        || direction == DIR_EAST;
}

static bool8 MovementActionShowsWalking(u8 movement_action_id)
{
    return (movement_action_id >= MOVEMENT_ACTION_WALK_SLOW_DOWN
         && movement_action_id <= MOVEMENT_ACTION_WALK_NORMAL_RIGHT)
        || (movement_action_id >= MOVEMENT_ACTION_WALK_FAST_DOWN
         && movement_action_id <= MOVEMENT_ACTION_WALK_IN_PLACE_FASTER_RIGHT)
        || (movement_action_id >= MOVEMENT_ACTION_RIDE_WATER_CURRENT_DOWN
         && movement_action_id <= MOVEMENT_ACTION_PLAYER_RUN_RIGHT)
        || (movement_action_id >= MOVEMENT_ACTION_JUMP_SPECIAL_DOWN
         && movement_action_id <= MOVEMENT_ACTION_JUMP_SPECIAL_RIGHT)
        || (movement_action_id >= MOVEMENT_ACTION_JUMP_DOWN
         && movement_action_id <= MOVEMENT_ACTION_JUMP_IN_PLACE_RIGHT)
        || movement_action_id == MOVEMENT_ACTION_WALK_DOWN_START_AFFINE
        || movement_action_id == MOVEMENT_ACTION_WALK_DOWN_AFFINE
        || (movement_action_id >= MOVEMENT_ACTION_ACRO_WHEELIE_HOP_DOWN
         && movement_action_id <= MOVEMENT_ACTION_WALK_SLOW_DIAGONAL_DOWN_RIGHT);
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
    if (snapshot->player_id >= MAX_NET_PLAYERS)
        return FALSE;
    if (snapshot->flags & ~NET_PLAYER_FLAGS_MASK)
        return FALSE;
    if (snapshot->flags & (NET_PLAYER_FLAG_HIDDEN | NET_PLAYER_FLAG_STALE | NET_PLAYER_FLAG_RESYNC))
        return FALSE;
    if (snapshot->stale_frames >= NET_PLAYER_STALE_FRAMES)
        return FALSE;
    if (snapshot->sequence == 0 || snapshot->session_epoch == 0)
        return FALSE;
    if (!DirectionIsValid(snapshot->facing_direction))
        return FALSE;
    if (!SnapshotIsFresh(snapshot, currentTick))
        return FALSE;
    if (snapshot->map_group != gSaveBlock1Ptr->location.mapGroup)
        return FALSE;
    if (snapshot->map_num != gSaveBlock1Ptr->location.mapNum)
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

static bool8 SnapshotIsInteractionTarget(const struct NetPlayerSnapshot *snapshot, s16 target_x, s16 target_y)
{
    struct ObjectEvent *playerObjEvent = &gObjectEvents[gPlayerAvatar.objectEventId];
    s32 dx;
    s32 dy;

    if (snapshot->x == target_x && snapshot->y == target_y)
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

            if (player_id == session->local_player_id || selected[player_id])
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
    u8 graphics_id = snapshot->avatar_graphics_id;
    u8 sprite_id;
    s16 x = snapshot->x - MAP_OFFSET;
    s16 y = snapshot->y - MAP_OFFSET;

    if (graphics_id == 0)
        graphics_id = GetPlayerAvatarGraphicsIdByCurrentState();

    sprite_id = CreateOrUpdateVirtualObject(
        graphics_id,
        sRemoteActors[actor_index].virtual_object_id,
        x,
        y,
        snapshot->elevation,
        snapshot->facing_direction,
        snapshot->graphics_revision);

    if (sprite_id == MAX_SPRITES)
    {
        ResetRemoteActor(actor_index);
        return;
    }
    TurnVirtualObjectWithMovement(
        sRemoteActors[actor_index].virtual_object_id,
        snapshot->facing_direction,
        MovementActionShowsWalking(snapshot->movement_action_id));

    sRemoteActors[actor_index].sprite_id = sprite_id;
    sRemoteActors[actor_index].graphics_id = graphics_id;
    sRemoteActors[actor_index].map_group = snapshot->map_group;
    sRemoteActors[actor_index].map_num = snapshot->map_num;
    sRemoteActors[actor_index].session_epoch = snapshot->session_epoch;
    sRemoteActors[actor_index].last_sequence = snapshot->sequence;
    sRemoteActors[actor_index].graphics_revision = snapshot->graphics_revision;
    sRemoteActors[actor_index].missing_frames = 0;
    sRemoteActors[actor_index].current_x = x;
    sRemoteActors[actor_index].current_y = y;
    sRemoteActors[actor_index].target_x = x;
    sRemoteActors[actor_index].target_y = y;
    sRemoteActors[actor_index].target_elevation = snapshot->elevation;
    sRemoteActors[actor_index].target_facing_direction = snapshot->facing_direction;
    sRemoteActors[actor_index].target_walking = MovementActionShowsWalking(snapshot->movement_action_id);
}

static s16 AbsS16(s16 value)
{
    return value < 0 ? -value : value;
}

static bool8 RemoteActorSpriteIsIdle(const struct RemotePlayerActor *actor)
{
    if (actor->sprite_id >= MAX_SPRITES || !gSprites[actor->sprite_id].inUse)
        return TRUE;

    return gSprites[actor->sprite_id].x2 == 0 && gSprites[actor->sprite_id].y2 == 0;
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
    s16 dx = actor->target_x - actor->current_x;
    s16 dy = actor->target_y - actor->current_y;
    u8 direction;

    if (AbsS16(dx) + AbsS16(dy) > 4)
    {
        actor->current_x = actor->target_x;
        actor->current_y = actor->target_y;
        SetVirtualObjectMapCoords(actor->virtual_object_id, actor->current_x, actor->current_y, actor->target_elevation);
        TurnVirtualObjectWithMovement(actor->virtual_object_id, actor->target_facing_direction, actor->target_walking);
        return;
    }

    if (!RemoteActorSpriteIsIdle(actor))
        return;

    if (dx == 0 && dy == 0)
    {
        TurnVirtualObjectWithMovement(actor->virtual_object_id, actor->target_facing_direction, actor->target_walking);
        return;
    }

    direction = GetStepDirection(dx, dy);
    switch (direction)
    {
    case DIR_NORTH:
        actor->current_y--;
        break;
    case DIR_SOUTH:
        actor->current_y++;
        break;
    case DIR_WEST:
        actor->current_x--;
        break;
    case DIR_EAST:
        actor->current_x++;
        break;
    }

    SetVirtualObjectMapCoords(actor->virtual_object_id, actor->current_x, actor->current_y, actor->target_elevation);
    TurnVirtualObjectWithMovement(actor->virtual_object_id, direction, TRUE);
}

static void MoveRemoteActor(u8 actor_index, const struct NetPlayerSnapshot *snapshot)
{
    u8 graphics_id = snapshot->avatar_graphics_id;
    s16 x = snapshot->x - MAP_OFFSET;
    s16 y = snapshot->y - MAP_OFFSET;

    if (sRemoteActors[actor_index].map_group != snapshot->map_group
     || sRemoteActors[actor_index].map_num != snapshot->map_num
     || sRemoteActors[actor_index].session_epoch != snapshot->session_epoch)
    {
        RespawnRemoteActor(actor_index, snapshot);
        return;
    }

    if (graphics_id == 0)
        graphics_id = GetPlayerAvatarGraphicsIdByCurrentState();
    if (sRemoteActors[actor_index].graphics_id != graphics_id
     || sRemoteActors[actor_index].graphics_revision != snapshot->graphics_revision)
    {
        SetVirtualObjectGraphics(sRemoteActors[actor_index].virtual_object_id, graphics_id);
        sRemoteActors[actor_index].graphics_id = graphics_id;
        sRemoteActors[actor_index].graphics_revision = snapshot->graphics_revision;
    }

    if (snapshot->sequence > sRemoteActors[actor_index].last_sequence)
    {
        sRemoteActors[actor_index].target_x = x;
        sRemoteActors[actor_index].target_y = y;
        sRemoteActors[actor_index].target_elevation = snapshot->elevation;
        sRemoteActors[actor_index].target_facing_direction = snapshot->facing_direction;
        sRemoteActors[actor_index].target_walking = MovementActionShowsWalking(snapshot->movement_action_id);
        sRemoteActors[actor_index].last_sequence = snapshot->sequence;
    }
    StepRemoteActorTowardTarget(actor_index);
    sRemoteActors[actor_index].missing_frames = 0;
}

static void SyncRemoteActor(const struct NetPlayerSnapshot *snapshot, u32 currentTick)
{
    u8 actor_index;

    actor_index = GetRemoteActorIndexByPlayerId(snapshot->player_id);
    if (!SnapshotIsOnCurrentMap(snapshot, currentTick))
    {
        if (actor_index != MAX_NET_REMOTE_PLAYERS)
            MarkRemoteActorMissing(actor_index);
        return;
    }

    if (actor_index == MAX_NET_REMOTE_PLAYERS)
    {
        actor_index = AllocRemoteActor(snapshot->player_id);
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

    if (session == NULL || session->local_player_id >= MAX_NET_PLAYERS)
        return FALSE;
    if (!MultiplayerSession_IsOnline())
        return FALSE;
    if (MultiplayerInteractionMenu_IsActive())
        return FALSE;

    for (remote_player_id = 0; remote_player_id < MAX_NET_PLAYERS; remote_player_id++)
    {
        const struct NetPlayerSnapshot *snapshot;

        if (remote_player_id == session->local_player_id)
            continue;

        snapshot = &session->players[remote_player_id];
        if (!SnapshotIsOnCurrentMap(snapshot, session->tick))
            continue;
        if (!SnapshotIsInteractionTarget(snapshot, x, y))
            continue;
        if (MultiplayerSession_IsPlayerInteractionBlocked(remote_player_id)
         || MultiplayerSession_IsPlayerInteractionBlocked(session->local_player_id))
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

    if (actor == NULL || !actor->active || actor->sprite_id >= MAX_SPRITES)
        return FALSE;

    sprite = &gSprites[actor->sprite_id];
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
            s32 x_distance_from_player = (s32)sRemoteActors[remote_actor_index].current_x + MAP_OFFSET - player_object_event->currentCoords.x;
            s32 y_distance_from_player = (s32)sRemoteActors[remote_actor_index].current_y + MAP_OFFSET - player_object_event->currentCoords.y;

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
    snapshot->player_id = local_player_id;
    snapshot->map_group = gSaveBlock1Ptr->location.mapGroup;
    snapshot->map_num = gSaveBlock1Ptr->location.mapNum;
    snapshot->tick = tick;

    player_object_event = &gObjectEvents[gPlayerAvatar.objectEventId];
    snapshot->x = player_object_event->currentCoords.x;
    snapshot->y = player_object_event->currentCoords.y;
    snapshot->elevation = player_object_event->previousElevation;
    snapshot->facing_direction = player_object_event->facing_direction;
    snapshot->movement_action_id = player_object_event->movement_action_id;
    snapshot->avatar_graphics_id = GetPlayerAvatarGraphicsIdByCurrentState();
    snapshot->graphics_revision = ((u16)snapshot->avatar_graphics_id << 8) | snapshot->outfit_id;
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

    if (session == NULL || session->local_player_id >= MAX_NET_PLAYERS)
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

        if (remote_player_id == session->local_player_id)
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
