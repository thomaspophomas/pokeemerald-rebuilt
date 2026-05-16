#include "global.h"
#include "engine/runtime_state.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "event_scripts.h"
#include "field_player_avatar.h"
#include "field_screen_effect.h"
#include "fieldmap.h"
#include "global.fieldmap.h"
#include "multiplayer/overworld_interaction.h"
#include "multiplayer/session.h"
#include "overworld.h"
#include "script.h"
#include "sound.h"
#include "constants/event_objects.h"
#include "constants/songs.h"

static bool8 IsCurrentMap(const struct MultiplayerPendingOverworldAction *action)
{
    return gSaveBlock1Ptr != NULL
        && gSaveBlock1Ptr->location.mapGroup == action->target.mapGroup
        && gSaveBlock1Ptr->location.mapNum == action->target.mapNum;
}

static bool8 PlayerStillAtActionOrigin(const struct MultiplayerPendingOverworldAction *action)
{
    s16 x;
    s16 y;
    s32 dx;
    s32 dy;

    PlayerGetDestCoords(&x, &y);
    dx = (s32)x - action->target.x;
    dy = (s32)y - action->target.y;
    if (dx < 0)
        dx = -dx;
    if (dy < 0)
        dy = -dy;

    return dx + dy <= NET_PLAYER_INTERACTION_RANGE_TILES
        && (action->target.elevation == ELEVATION_TRANSITION || PlayerGetElevation() == action->target.elevation);
}

static bool8 RevalidateNpcTarget(const struct MultiplayerPendingOverworldAction *action, u8 *objectEventId)
{
    if (action->objectEventId < OBJECT_EVENTS_COUNT
     && gObjectEvents[action->objectEventId].active
     && gObjectEvents[action->objectEventId].localId == action->target.localId
     && gObjectEvents[action->objectEventId].mapGroup == action->target.mapGroup
     && gObjectEvents[action->objectEventId].mapNum == action->target.mapNum)
    {
        *objectEventId = action->objectEventId;
        return TRUE;
    }

    return TryGetObjectEventIdByLocalIdAndMap(
        action->target.localId,
        action->target.mapNum,
        action->target.mapGroup,
        objectEventId);
}

static bool8 RevalidatePendingAction(const struct MultiplayerPendingOverworldAction *action)
{
    u8 objectEventId;

    if (action == NULL || !action->active)
        return FALSE;
    if (!IsCurrentMap(action))
        return FALSE;
    if (!PlayerStillAtActionOrigin(action))
        return FALSE;
    if (action->facing != 0 && GetPlayerFacingDirection() != action->facing)
        return FALSE;
    if (action->target.targetKind == MULTIPLAYER_INTERACTION_TARGET_NPC
     && !RevalidateNpcTarget(action, &objectEventId))
        return FALSE;

    return TRUE;
}

static bool8 ResumeScript(const struct MultiplayerPendingOverworldAction *action)
{
    u8 objectEventId;

    if (action->script == NULL || !RevalidatePendingAction(action))
        return FALSE;

    if (action->target.targetKind == MULTIPLAYER_INTERACTION_TARGET_NPC)
    {
        if (!RevalidateNpcTarget(action, &objectEventId))
            return FALSE;
        gSelectedObjectEvent = objectEventId;
        gSpecialVar_LastTalked = action->target.localId;
        gSpecialVar_Facing = action->facing;
    }

    ScriptContext_SetupScript(action->script);
    return TRUE;
}

static bool8 ResumeWarp(const struct MultiplayerPendingOverworldAction *action)
{
    if (!RevalidatePendingAction(action))
        return FALSE;

    StoreInitialPlayerAvatarState();
    SetWarpDestination(action->destMapGroup, action->destMapNum, action->destWarpId, action->destX, action->destY);
    switch (action->resumeKind)
    {
    case MULTIPLAYER_OW_RESUME_DOOR_WARP:
        DoDoorWarp();
        break;
    case MULTIPLAYER_OW_RESUME_DIVE_WARP:
        DoDiveWarp();
        PlaySE(SE_M_DIVE);
        break;
    case MULTIPLAYER_OW_RESUME_WARP:
    default:
        DoWarp();
        break;
    }
    return TRUE;
}

u8 OverworldInteraction_Preflight(const struct MultiplayerPendingOverworldAction *action)
{
    if (action == NULL || !action->active)
        return MULTIPLAYER_INTERACTION_PREFLIGHT_DENIED;
    if (!EngineRuntimeState_IsMultiplayerOnlineEnabled())
        return MULTIPLAYER_INTERACTION_PREFLIGHT_START;
    if (action->actionType == MULTIPLAYER_OW_ACTION_DISABLED_ONLINE)
        return MULTIPLAYER_INTERACTION_PREFLIGHT_DENIED;
    if (action->actionType == MULTIPLAYER_OW_ACTION_READONLY_SCRIPT)
    {
        if (OverworldInteraction_IsReadonlyScriptAllowed(action->script))
            return MULTIPLAYER_INTERACTION_PREFLIGHT_START;
        return MULTIPLAYER_INTERACTION_PREFLIGHT_DENIED;
    }
    if (!MultiplayerSession_IsOnline())
        return MULTIPLAYER_INTERACTION_PREFLIGHT_BUSY;

    return MultiplayerSession_PreflightOverworldAction(action);
}

bool8 OverworldInteraction_Resume(const struct MultiplayerPendingOverworldAction *action)
{
    if (action == NULL || !action->active)
        return FALSE;

    switch (action->resumeKind)
    {
    case MULTIPLAYER_OW_RESUME_SCRIPT:
    case MULTIPLAYER_OW_RESUME_DIVE_DOWN_SCRIPT:
    case MULTIPLAYER_OW_RESUME_DIVE_EMERGE_SCRIPT:
        return ResumeScript(action);
    case MULTIPLAYER_OW_RESUME_WARP:
    case MULTIPLAYER_OW_RESUME_DOOR_WARP:
    case MULTIPLAYER_OW_RESUME_DIVE_WARP:
        return ResumeWarp(action);
    default:
        return FALSE;
    }
}

void OverworldInteraction_ShowDeniedMessage(void)
{
    PlaySE(SE_BOO);
    ScriptContext_SetupScript(EventScript_MultiplayerNpcBusy);
}

u16 OverworldInteraction_HashScript(const u8 *script)
{
    u32 value = (u32)script;
    u16 hash;

    value ^= value >> 16;
    value ^= value >> 8;
    hash = value;
    return hash == 0 ? 1 : hash;
}

u16 OverworldInteraction_CalcResourceChecksum(const struct MultiplayerPendingOverworldAction *action)
{
    u8 i;
    u16 checksum;

    if (action == NULL)
        return 0;

    checksum = action->target.scriptHash ^ ((u16)action->actionType << 8) ^ action->target.targetKind;
    for (i = 0; i < action->resourceCount && i < NET_INTERACTION_RESOURCE_LOCK_MAX; i++)
    {
        const struct MultiplayerResourceLockKey *resource = &action->resources[i];

        checksum ^= ((u16)resource->kind << 11)
            ^ ((u16)resource->mapGroup << 8)
            ^ ((u16)resource->mapNum << 4)
            ^ resource->localId
            ^ resource->id
            ^ (u16)resource->x
            ^ (u16)resource->y;
    }
    if (checksum == 0)
        checksum = 1;
    return checksum;
}

void OverworldInteraction_AddResource(struct MultiplayerPendingOverworldAction *action, u8 kind, u8 mapGroup, u8 mapNum, u8 localId, u8 elevation, s16 x, s16 y, u16 id)
{
    struct MultiplayerResourceLockKey *resource;

    if (action == NULL || action->resourceCount >= NET_INTERACTION_RESOURCE_LOCK_MAX)
        return;

    resource = &action->resources[action->resourceCount++];
    memset(resource, 0, sizeof(*resource));
    resource->kind = kind;
    resource->mapGroup = mapGroup;
    resource->mapNum = mapNum;
    resource->localId = localId;
    resource->elevation = elevation;
    resource->x = x;
    resource->y = y;
    resource->id = id;
}

bool8 OverworldInteraction_IsReadonlyScriptAllowed(const u8 *script)
{
    return script == EventScript_TestSignpostMsg;
}
