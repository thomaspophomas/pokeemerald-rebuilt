#include "global.h"
#include "international_string_util.h"
#include "main.h"
#include "menu.h"
#include "money.h"
#include "multiplayer/battle.h"
#include "multiplayer/interaction_menu.h"
#include "multiplayer/session.h"
#include "multiplayer/trade.h"
#include "palette.h"
#include "script.h"
#include "script_menu.h"
#include "sound.h"
#include "text.h"
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

static void Task_HandleInteractionMenu(u8 task_id);
static bool8 TryStartFight(void);
static bool8 TryStartItemTrade(void);

static const u8 sText_Fight[] = _("Fight");
static const u8 sText_Poketrade[] = _("Poketrade");
static const u8 sText_Itemtrade[] = _("Itemtrade");
static const u8 sText_Exit[] = _("Exit");
static const u8 sText_Money[] = _("Money");

static const struct MenuAction sInteractionMenuActions[] =
{
    [INTERACTION_MENU_FIGHT] = {sText_Fight, {0}},
    [INTERACTION_MENU_POKETRADE] = {sText_Poketrade, {0}},
    [INTERACTION_MENU_ITEMTRADE] = {sText_Itemtrade, {0}},
    [INTERACTION_MENU_EXIT] = {sText_Exit, {0}},
};

static EWRAM_DATA bool8 sMenuActive = FALSE;
static EWRAM_DATA u8 sWindowId = WINDOW_NONE;
static EWRAM_DATA u8 sMoneyWindowId = WINDOW_NONE;
static EWRAM_DATA u8 sInitiatorPlayerId = NET_PLAYER_NONE;
static EWRAM_DATA u8 sTargetPlayerId = NET_PLAYER_NONE;
static EWRAM_DATA u32 sItemTradeMoneyAmount = 0;
static EWRAM_DATA bool8 sRemoteMenuSeen[MAX_NET_PLAYERS] = {0};

#define tWindowId data[0]
#define tInputDelay data[1]
#define tMode data[2]

#define INTERACTION_MENU_LEFT 18
#define INTERACTION_MENU_TOP 6
#define INTERACTION_MENU_INPUT_DELAY 8
#define INTERACTION_MENU_MODE_OPTIONS 0
#define INTERACTION_MENU_MODE_MONEY 1
#define INTERACTION_MENU_MONEY_STEP 100
#define INTERACTION_MENU_MONEY_BIG_STEP 1000

static u8 CreateInteractionMenuWindow(void)
{
    u8 windowId;
    u8 width = GetMaxWidthInMenuTable(sInteractionMenuActions, ARRAY_COUNT(sInteractionMenuActions));
    struct WindowTemplate template;

    template = CreateWindowTemplate(
        0,
        INTERACTION_MENU_LEFT + 1,
        INTERACTION_MENU_TOP + 1,
        width,
        ARRAY_COUNT(sInteractionMenuActions) * 2,
        15,
        100);

    windowId = AddWindow(&template);
    if (windowId == WINDOW_NONE)
        return WINDOW_NONE;

    PutWindowTilemap(windowId);
    return windowId;
}

static u8 CreateMoneyWindow(void)
{
    struct WindowTemplate template = CreateWindowTemplate(0, 16, 10, 13, 4, 15, 132);
    u8 windowId = AddWindow(&template);

    if (windowId == WINDOW_NONE)
        return WINDOW_NONE;

    PutWindowTilemap(windowId);
    SetStandardWindowBorderStyle(windowId, FALSE);
    return windowId;
}

static void PrintMoneyInputWindow(void)
{
    if (sMoneyWindowId == WINDOW_NONE)
        return;

    FillWindowPixelBuffer(sMoneyWindowId, PIXEL_FILL(1));
    AddTextPrinterParameterized(sMoneyWindowId, FONT_NORMAL, sText_Money, 8, 1, TEXT_SKIP_DRAW, NULL);
    PrintMoneyAmount(sMoneyWindowId, 8, 17, sItemTradeMoneyAmount, TEXT_SKIP_DRAW);
    CopyWindowToVram(sMoneyWindowId, COPYWIN_GFX);
    ScheduleBgCopyTilemapToVram(0);
}

static bool8 AdjustItemTradeMoney(s32 delta)
{
    u32 money = GetMoney(&gSaveBlock1Ptr->money);
    u32 amount = sItemTradeMoneyAmount;

    if (delta > 0)
    {
        if (amount + delta > money || amount + delta < amount)
            amount = money;
        else
            amount += delta;
    }
    else if (delta < 0)
    {
        u32 magnitude = (u32)-delta;

        if (amount < magnitude)
            amount = 0;
        else
            amount -= magnitude;
    }

    if (amount == sItemTradeMoneyAmount)
        return FALSE;

    sItemTradeMoneyAmount = amount;
    return TRUE;
}

static bool8 StartItemTradeMoneyInput(u8 task_id)
{
    if (sWindowId != WINDOW_NONE)
    {
        ClearToTransparentAndRemoveWindow(sWindowId);
        sWindowId = WINDOW_NONE;
    }

    sMoneyWindowId = CreateMoneyWindow();
    if (sMoneyWindowId == WINDOW_NONE)
        return FALSE;

    sItemTradeMoneyAmount = 0;
    PrintMoneyInputWindow();
    gTasks[task_id].tMode = INTERACTION_MENU_MODE_MONEY;
    return TRUE;
}

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
    if (snapshot->map_group != local->map_group || snapshot->map_num != local->map_num)
        return FALSE;

    MoveCoordsOneStep(snapshot->facing_direction, &x, &y);
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

static void CloseInteractionMenu(u8 task_id)
{
    if (sWindowId != WINDOW_NONE)
    {
        ClearToTransparentAndRemoveWindow(sWindowId);
        sWindowId = WINDOW_NONE;
    }
    if (sMoneyWindowId != WINDOW_NONE)
    {
        ClearToTransparentAndRemoveWindow(sMoneyWindowId);
        sMoneyWindowId = WINDOW_NONE;
    }

    sMenuActive = FALSE;
    sInitiatorPlayerId = NET_PLAYER_NONE;
    sTargetPlayerId = NET_PLAYER_NONE;
    sItemTradeMoneyAmount = 0;
    UnlockPlayerFieldControls();
    DestroyTask(task_id);
}

static bool8 StartInteractionMenu(u8 initiator_player_id, u8 target_player_id)
{
    u8 task_id;

    if (sMenuActive)
        return FALSE;
    if (ArePlayerFieldControlsLocked())
        return FALSE;

    sWindowId = CreateInteractionMenuWindow();
    if (sWindowId == WINDOW_NONE)
        return FALSE;

    SetStandardWindowBorderStyle(sWindowId, FALSE);
    PrintMenuTable(sWindowId, ARRAY_COUNT(sInteractionMenuActions), sInteractionMenuActions);
    InitMenuInUpperLeftCornerNormal(sWindowId, ARRAY_COUNT(sInteractionMenuActions), 0);
    CopyWindowToVram(sWindowId, COPYWIN_MAP);
    ScheduleBgCopyTilemapToVram(0);

    sMenuActive = TRUE;
    sInitiatorPlayerId = initiator_player_id;
    sTargetPlayerId = target_player_id;
    LockPlayerFieldControls();
    PlaySE(SE_SELECT);

    task_id = CreateTask(Task_HandleInteractionMenu, 80);
    gTasks[task_id].tWindowId = sWindowId;
    gTasks[task_id].tInputDelay = INTERACTION_MENU_INPUT_DELAY;
    gTasks[task_id].tMode = INTERACTION_MENU_MODE_OPTIONS;
    return TRUE;
}

static bool8 TryStartFight(void)
{
    struct MultiplayerBattleRequest request;
    u8 local_player_id = MultiplayerSession_GetLocalPlayerId();
    u8 opponent_player_id = sTargetPlayerId;

    if (sInitiatorPlayerId != NET_PLAYER_NONE)
        opponent_player_id = sInitiatorPlayerId;

    if (local_player_id >= MAX_NET_PLAYERS || opponent_player_id >= MAX_NET_PLAYERS)
        return FALSE;
    if (local_player_id == opponent_player_id)
        return FALSE;
    if (!MultiplayerBattle_LocalPlayerCanFight())
        return FALSE;

    memset(&request, 0, sizeof(request));
    request.type = MULTIPLAYER_SUBSESSION_PVP_BATTLE;
    request.player_count = MAX_NET_PVP_PLAYERS;
    request.players[0] = local_player_id;
    request.players[1] = opponent_player_id;

    return MultiplayerBattle_Start(&request);
}

static bool8 TryStartItemTrade(void)
{
    u8 other_player_id = sTargetPlayerId;

    if (sInitiatorPlayerId != NET_PLAYER_NONE)
        other_player_id = sInitiatorPlayerId;

    return MultiplayerTrade_StartItemTrade(other_player_id, sItemTradeMoneyAmount);
}

static void Task_HandleMoneyInput(u8 task_id)
{
    if (JOY_REPEAT(DPAD_RIGHT) || JOY_REPEAT(DPAD_UP))
    {
        if (AdjustItemTradeMoney(INTERACTION_MENU_MONEY_STEP))
        {
            PlaySE(SE_SELECT);
            PrintMoneyInputWindow();
        }
    }
    else if (JOY_REPEAT(DPAD_LEFT) || JOY_REPEAT(DPAD_DOWN))
    {
        if (AdjustItemTradeMoney(-INTERACTION_MENU_MONEY_STEP))
        {
            PlaySE(SE_SELECT);
            PrintMoneyInputWindow();
        }
    }
    else if (JOY_REPEAT(R_BUTTON))
    {
        if (AdjustItemTradeMoney(INTERACTION_MENU_MONEY_BIG_STEP))
        {
            PlaySE(SE_SELECT);
            PrintMoneyInputWindow();
        }
    }
    else if (JOY_REPEAT(L_BUTTON))
    {
        if (AdjustItemTradeMoney(-INTERACTION_MENU_MONEY_BIG_STEP))
        {
            PlaySE(SE_SELECT);
            PrintMoneyInputWindow();
        }
    }
    else if (JOY_NEW(A_BUTTON))
    {
        if (TryStartItemTrade())
            PlaySE(SE_SELECT);
        else
            PlaySE(SE_FAILURE);

        CloseInteractionMenu(task_id);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        CloseInteractionMenu(task_id);
    }
}

static void Task_HandleOptionsInput(u8 task_id)
{
    s8 selection;

    selection = Menu_ProcessInputNoWrap();

    if (selection == MENU_NOTHING_CHOSEN)
        return;
    if (selection == MENU_B_PRESSED || selection == INTERACTION_MENU_EXIT)
    {
        PlaySE(SE_SELECT);
        CloseInteractionMenu(task_id);
        return;
    }

    switch (selection)
    {
    case INTERACTION_MENU_FIGHT:
        if (TryStartFight())
            PlaySE(SE_SELECT);
        else
            PlaySE(SE_FAILURE);
        break;
    case INTERACTION_MENU_ITEMTRADE:
        if (StartItemTradeMoneyInput(task_id))
            PlaySE(SE_SELECT);
        else
        {
            PlaySE(SE_FAILURE);
            CloseInteractionMenu(task_id);
        }
        return;
    default:
        PlaySE(SE_FAILURE);
        break;
    }

    CloseInteractionMenu(task_id);
}

static void Task_HandleInteractionMenu(u8 task_id)
{
    if (gPaletteFade.active)
        return;
    if (gTasks[task_id].tInputDelay > 0)
    {
        gTasks[task_id].tInputDelay--;
        return;
    }

    if (gTasks[task_id].tMode == INTERACTION_MENU_MODE_MONEY)
        Task_HandleMoneyInput(task_id);
    else
        Task_HandleOptionsInput(task_id);
}

void MultiplayerInteractionMenu_Init(void)
{
    memset(sRemoteMenuSeen, 0, sizeof(sRemoteMenuSeen));
    sMenuActive = FALSE;
    sWindowId = WINDOW_NONE;
    sMoneyWindowId = WINDOW_NONE;
    sInitiatorPlayerId = NET_PLAYER_NONE;
    sTargetPlayerId = NET_PLAYER_NONE;
    sItemTradeMoneyAmount = 0;
}

void MultiplayerInteractionMenu_Reset(void)
{
    u8 task_id = FindTaskIdByFunc(Task_HandleInteractionMenu);

    if (sMenuActive && sWindowId != WINDOW_NONE)
    {
        ClearToTransparentAndRemoveWindow(sWindowId);
        sWindowId = WINDOW_NONE;
        UnlockPlayerFieldControls();
    }
    if (sMoneyWindowId != WINDOW_NONE)
    {
        ClearToTransparentAndRemoveWindow(sMoneyWindowId);
        sMoneyWindowId = WINDOW_NONE;
        UnlockPlayerFieldControls();
    }
    if (task_id != TASK_NONE)
        DestroyTask(task_id);

    MultiplayerInteractionMenu_Init();
}

bool8 MultiplayerInteractionMenu_IsActive(void)
{
    return sMenuActive;
}

bool8 MultiplayerInteractionMenu_StartLocal(u8 target_player_id)
{
    return StartInteractionMenu(NET_PLAYER_NONE, target_player_id);
}

void MultiplayerInteractionMenu_ApplySnapshotState(struct NetPlayerSnapshot *snapshot)
{
    if (!sMenuActive || snapshot == NULL)
        return;

    snapshot->flags |= NET_PLAYER_FLAG_BUSY;
    snapshot->interaction_state = MULTIPLAYER_INTERACTION_OPTIONS_MENU;
}

void MultiplayerInteractionMenu_UpdateRemoteRequests(const struct MultiplayerSession *session)
{
    const struct NetPlayerSnapshot *local;
    u8 remote_player_id;

    if (session == NULL || session->local_player_id >= MAX_NET_PLAYERS)
        return;

    local = &session->players[session->local_player_id];
    for (remote_player_id = 0; remote_player_id < MAX_NET_PLAYERS; remote_player_id++)
    {
        const struct NetPlayerSnapshot *snapshot;

        if (remote_player_id == session->local_player_id)
            continue;

        snapshot = &session->players[remote_player_id];
        if (!snapshot->active || snapshot->interaction_state != MULTIPLAYER_INTERACTION_OPTIONS_MENU)
        {
            sRemoteMenuSeen[remote_player_id] = FALSE;
            continue;
        }
        if (sRemoteMenuSeen[remote_player_id])
            continue;

        if (SnapshotCanOpenRemoteMenu(snapshot, local) && StartInteractionMenu(remote_player_id, session->local_player_id))
        {
            sRemoteMenuSeen[remote_player_id] = TRUE;
        }
        else
        {
            sRemoteMenuSeen[remote_player_id] = FALSE;
        }
    }
}

#undef tWindowId
#undef tInputDelay

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

bool8 MultiplayerInteractionMenu_StartLocal(u8 target_player_id)
{
    (void)target_player_id;
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
