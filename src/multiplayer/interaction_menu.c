#include "global.h"
#include "menu.h"
#include "multiplayer/interaction_menu.h"
#include "script.h"
#include "script_menu.h"
#include "sound.h"
#include "task.h"
#include "constants/songs.h"

#if FEATURE_MULTIPLAYER

enum
{
    INTERACTION_MENU_FIGHT,
    INTERACTION_MENU_POKETRADE,
    INTERACTION_MENU_ITEMTRADE,
    INTERACTION_MENU_EXIT,
};

static void Task_HandleInteractionMenu(u8 taskId);

static const u8 sText_Fight[] = _("Fight");
static const u8 sText_Poketrade[] = _("Poketrade");
static const u8 sText_Itemtrade[] = _("Itemtrade");
static const u8 sText_Exit[] = _("Exit");

static const struct MenuAction sInteractionMenuActions[] =
{
    [INTERACTION_MENU_FIGHT] = {sText_Fight, {0}},
    [INTERACTION_MENU_POKETRADE] = {sText_Poketrade, {0}},
    [INTERACTION_MENU_ITEMTRADE] = {sText_Itemtrade, {0}},
    [INTERACTION_MENU_EXIT] = {sText_Exit, {0}},
};

static EWRAM_DATA bool8 sMenuActive = FALSE;
static EWRAM_DATA u8 sWindowId = WINDOW_NONE;
static EWRAM_DATA u8 sInitiatorPlayerId = NET_PLAYER_NONE;
static EWRAM_DATA u8 sTargetPlayerId = NET_PLAYER_NONE;
static EWRAM_DATA bool8 sRemoteMenuSeen[MAX_NET_PLAYERS] = {0};

#define tWindowId data[0]

static void MoveCoordsOneStep(u8 direction, s16 *x, s16 *y)
{
    switch (direction)
    {
    case DIR_NORTH:
        (*y)--;
        break;
    case DIR_SOUTH:
        (*y)++;
        break;
    case DIR_WEST:
        (*x)--;
        break;
    case DIR_EAST:
        (*x)++;
        break;
    }
}

static bool8 SnapshotCanOpenRemoteMenu(const struct NetPlayerSnapshot *snapshot, const struct NetPlayerSnapshot *local)
{
    s16 x = snapshot->x;
    s16 y = snapshot->y;
    s32 dx;
    s32 dy;

    if (!snapshot->active || !local->active)
        return FALSE;
    if (snapshot->mapGroup != local->mapGroup || snapshot->mapNum != local->mapNum)
        return FALSE;

    MoveCoordsOneStep(snapshot->facingDirection, &x, &y);
    if (x == local->x && y == local->y)
        return TRUE;

    dx = (s32)snapshot->x - local->x;
    dy = (s32)snapshot->y - local->y;
    if (dx < 0)
        dx = -dx;
    if (dy < 0)
        dy = -dy;

    return dx + dy <= 1;
}

static void CloseInteractionMenu(u8 taskId)
{
    if (sWindowId != WINDOW_NONE)
    {
        ClearToTransparentAndRemoveWindow(sWindowId);
        sWindowId = WINDOW_NONE;
    }

    sMenuActive = FALSE;
    sInitiatorPlayerId = NET_PLAYER_NONE;
    sTargetPlayerId = NET_PLAYER_NONE;
    UnlockPlayerFieldControls();
    DestroyTask(taskId);
}

static bool8 StartInteractionMenu(u8 initiatorPlayerId, u8 targetPlayerId)
{
    u8 taskId;

    if (sMenuActive)
        return FALSE;
    if (ArePlayerFieldControlsLocked())
        return FALSE;

    sWindowId = CreateWindowFromRect(18, 6, 11, ARRAY_COUNT(sInteractionMenuActions) * 2);
    SetStandardWindowBorderStyle(sWindowId, FALSE);
    PrintMenuTable(sWindowId, ARRAY_COUNT(sInteractionMenuActions), sInteractionMenuActions);
    InitMenuInUpperLeftCornerNormal(sWindowId, ARRAY_COUNT(sInteractionMenuActions), 0);
    ScheduleBgCopyTilemapToVram(0);

    sMenuActive = TRUE;
    sInitiatorPlayerId = initiatorPlayerId;
    sTargetPlayerId = targetPlayerId;
    LockPlayerFieldControls();
    PlaySE(SE_SELECT);

    taskId = CreateTask(Task_HandleInteractionMenu, 80);
    gTasks[taskId].tWindowId = sWindowId;
    return TRUE;
}

static void Task_HandleInteractionMenu(u8 taskId)
{
    s8 selection = Menu_ProcessInputNoWrap();

    if (selection == MENU_NOTHING_CHOSEN)
        return;
    if (selection == MENU_B_PRESSED || selection == INTERACTION_MENU_EXIT)
        PlaySE(SE_SELECT);

    CloseInteractionMenu(taskId);
}

void MultiplayerInteractionMenu_Init(void)
{
    memset(sRemoteMenuSeen, 0, sizeof(sRemoteMenuSeen));
    sMenuActive = FALSE;
    sWindowId = WINDOW_NONE;
    sInitiatorPlayerId = NET_PLAYER_NONE;
    sTargetPlayerId = NET_PLAYER_NONE;
}

void MultiplayerInteractionMenu_Reset(void)
{
    if (sMenuActive && sWindowId != WINDOW_NONE)
    {
        ClearToTransparentAndRemoveWindow(sWindowId);
        sWindowId = WINDOW_NONE;
        UnlockPlayerFieldControls();
    }

    MultiplayerInteractionMenu_Init();
}

bool8 MultiplayerInteractionMenu_IsActive(void)
{
    return sMenuActive;
}

bool8 MultiplayerInteractionMenu_StartLocal(u8 targetPlayerId)
{
    return StartInteractionMenu(NET_PLAYER_NONE, targetPlayerId);
}

void MultiplayerInteractionMenu_ApplySnapshotState(struct NetPlayerSnapshot *snapshot)
{
    if (!sMenuActive || snapshot == NULL)
        return;

    snapshot->flags |= NET_PLAYER_FLAG_BUSY;
    snapshot->interactionState = MULTIPLAYER_INTERACTION_OPTIONS_MENU;
}

void MultiplayerInteractionMenu_UpdateRemoteRequests(const struct MultiplayerSession *session)
{
    const struct NetPlayerSnapshot *local;
    u8 i;

    if (session == NULL || session->localPlayerId >= MAX_NET_PLAYERS)
        return;

    local = &session->players[session->localPlayerId];
    for (i = 0; i < MAX_NET_PLAYERS; i++)
    {
        const struct NetPlayerSnapshot *snapshot;

        if (i == session->localPlayerId)
            continue;

        snapshot = &session->players[i];
        if (!snapshot->active || snapshot->interactionState != MULTIPLAYER_INTERACTION_OPTIONS_MENU)
        {
            sRemoteMenuSeen[i] = FALSE;
            continue;
        }
        if (sRemoteMenuSeen[i])
            continue;

        sRemoteMenuSeen[i] = TRUE;
        if (SnapshotCanOpenRemoteMenu(snapshot, local))
            StartInteractionMenu(i, session->localPlayerId);
    }
}

#undef tWindowId

#else

void MultiplayerInteractionMenu_Init(void)
{
}

void MultiplayerInteractionMenu_Reset(void)
{
}

bool8 MultiplayerInteractionMenu_IsActive(void)
{
    return FALSE;
}

bool8 MultiplayerInteractionMenu_StartLocal(u8 targetPlayerId)
{
    (void)targetPlayerId;
    return FALSE;
}

void MultiplayerInteractionMenu_ApplySnapshotState(struct NetPlayerSnapshot *snapshot)
{
    (void)snapshot;
}

void MultiplayerInteractionMenu_UpdateRemoteRequests(const struct MultiplayerSession *session)
{
    (void)session;
}

#endif
