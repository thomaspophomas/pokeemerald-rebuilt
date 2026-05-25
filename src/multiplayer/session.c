#include "global.h"
#include "engine/extension_profile.h"
#include "engine/runtime_state.h"
#include "global.fieldmap.h"
#include "main.h"
#include "multiplayer/session.h"
#include "multiplayer/companion_save_beacon.h"
#include "multiplayer/commit.h"
#include "multiplayer/interaction_menu.h"
#include "multiplayer/transport.h"
#include "multiplayer/overworld.h"
#include "multiplayer/battle.h"
#include "multiplayer/trade.h"
#include "constants/moves.h"
#include "constants/species.h"

#if FEATURE_MULTIPLAYER_SMOKE_STATUS
#include "multiplayer/smoke_status.h"
#endif

#if FEATURE_MULTIPLAYER

static EWRAM_DATA struct MultiplayerSession sSession = {0};
static EWRAM_DATA bool8 sConnectRequested = FALSE;
static EWRAM_DATA bool8 sClientHelloSent = FALSE;
static EWRAM_DATA u16 sHeartbeatTimer = 0;
static EWRAM_DATA u16 sTransportLossFrames = 0;

#define NET_TRANSPORT_VIEW_LOSS_GRACE_FRAMES 30

#if FEATURE_MULTIPLAYER_SMOKE_STATUS
EWRAM_DATA volatile struct NetMultiplayerSmokeStatus gNetMultiplayerSmokeStatus = {0};
static EWRAM_DATA u32 sLastSmokeProfileAckHash = 0;
static EWRAM_DATA u8 sLastSmokeProfileAckResult = 0;

static void UpdateSmokeStatus(void);
#endif

static bool8 RuntimeAllowsOnline(void)
{
    return EngineRuntimeState_IsMultiplayerOnlineEnabled();
}

static bool8 RuntimeCanPollTransport(void)
{
    return gMain.inBattle || MultiplayerOverworld_CanTick();
}

static void ResetSession(void)
{
    MultiplayerInteractionMenu_Reset();
    memset(&sSession, 0, sizeof(sSession));
    sSession.state = MULTIPLAYER_SESSION_OFFLINE;
    sSession.local_player_id = NET_PLAYER_NONE;
    sSession.host_player_id = NET_PLAYER_NONE;
    sSession.transport_mode = NET_ACTIVE_TRANSPORT_MODE;
    sSession.health_state = MULTIPLAYER_HEALTH_DISCONNECTED;
    sClientHelloSent = FALSE;
    sHeartbeatTimer = 0;
    sTransportLossFrames = 0;
    EngineExtensionProfile_Clear();
    MultiplayerTrade_Reset();
#if FEATURE_MULTIPLAYER_SMOKE_STATUS
    sLastSmokeProfileAckHash = 0;
    sLastSmokeProfileAckResult = 0;
    UpdateSmokeStatus();
#endif
}

#if FEATURE_MULTIPLAYER_SMOKE_STATUS
static void UpdateSmokeStatus(void)
{
    gNetMultiplayerSmokeStatus.magic = NET_SMOKE_STATUS_MAGIC;
    gNetMultiplayerSmokeStatus.session_state = sSession.state;
    gNetMultiplayerSmokeStatus.health_state = sSession.health_state;
    gNetMultiplayerSmokeStatus.local_player_id = sSession.local_player_id;
    gNetMultiplayerSmokeStatus.player_count = sSession.player_count;
    gNetMultiplayerSmokeStatus.session_epoch = sSession.session_epoch;
    gNetMultiplayerSmokeStatus.active_profile_hash = EngineExtensionProfile_GetActiveHash();
    gNetMultiplayerSmokeStatus.last_profile_ack_hash = sLastSmokeProfileAckHash;
    gNetMultiplayerSmokeStatus.last_profile_ack_result = sLastSmokeProfileAckResult;
    gNetMultiplayerSmokeStatus.flags = RuntimeCanPollTransport() ? NET_SMOKE_STATUS_FLAG_RUNTIME_READY : 0;
}
#endif

static u8 GetMaxPlayersForSubsession(u8 subsession_type)
{
    switch (subsession_type)
    {
    case MULTIPLAYER_SUBSESSION_PVE_BATTLE:
        return MAX_NET_PVE_PLAYERS;
    case MULTIPLAYER_SUBSESSION_PVP_BATTLE:
        return MAX_NET_PVP_PLAYERS;
    case MULTIPLAYER_SUBSESSION_TRADE:
        return MAX_NET_TRADE_PLAYERS;
    default:
        return 0;
    }
}

static bool8 IsValidPlayerId(u8 player_id)
{
    return player_id < MAX_NET_PLAYERS;
}

static bool8 IsValidSubsessionState(u8 subsession_state)
{
    return subsession_state == MULTIPLAYER_SUBSESSION_STATE_INVITING
        || subsession_state == MULTIPLAYER_SUBSESSION_STATE_READY
        || subsession_state == MULTIPLAYER_SUBSESSION_STATE_ACTIVE
        || subsession_state == MULTIPLAYER_SUBSESSION_STATE_ENDING
        || subsession_state == MULTIPLAYER_SUBSESSION_STATE_ERROR;
}

static bool8 IsValidInteractionState(u8 interaction_state)
{
    return interaction_state <= MULTIPLAYER_INTERACTION_OPTIONS_MENU;
}

static bool8 DirectionIsValid(u8 direction)
{
    return direction == DIR_NORTH
        || direction == DIR_SOUTH
        || direction == DIR_WEST
        || direction == DIR_EAST;
}

static bool8 ElevationIsValid(u8 elevation)
{
    return elevation == ELEVATION_TRANSITION
        || elevation == ELEVATION_SURF
        || elevation == ELEVATION_DEFAULT
        || elevation == ELEVATION_MULTI_LEVEL;
}

static bool8 MapLocationIsValid(u8 map_group, u8 map_number)
{
    return map_group != 0xFF && map_number != 0xFF;
}

static bool8 SessionIdentityIsValid(const struct NetTransportSessionView *view)
{
    return view->transport_mode == NET_ACTIVE_TRANSPORT_MODE
        && view->session_id != 0
        && view->session_epoch != 0
        && view->player_token != 0
        && view->join_nonce != 0;
}

static bool8 SnapshotCoordsAreSane(const struct NetPlayerSnapshot *snapshot)
{
    return snapshot->x >= NET_PLAYER_COORD_MIN
        && snapshot->x <= NET_PLAYER_COORD_MAX
        && snapshot->y >= NET_PLAYER_COORD_MIN
        && snapshot->y <= NET_PLAYER_COORD_MAX;
}

static bool8 SubsessionStateBlocksInteraction(u8 subsession_state)
{
    return subsession_state == MULTIPLAYER_SUBSESSION_STATE_INVITING
        || subsession_state == MULTIPLAYER_SUBSESSION_STATE_READY
        || subsession_state == MULTIPLAYER_SUBSESSION_STATE_ACTIVE;
}

static bool8 SubsessionStateIsTerminal(u8 subsession_state)
{
    return subsession_state == MULTIPLAYER_SUBSESSION_STATE_ENDING
        || subsession_state == MULTIPLAYER_SUBSESSION_STATE_ERROR;
}

static bool8 BarrierTypeIsValid(u8 barrier_type)
{
    return barrier_type == MULTIPLAYER_BARRIER_SCRIPT
        || barrier_type == MULTIPLAYER_BARRIER_WARP
        || barrier_type == MULTIPLAYER_BARRIER_BATTLE_INVITE
        || barrier_type == MULTIPLAYER_BARRIER_TRADE_INVITE;
}

static bool8 SnapshotTickIsFresh(const struct NetPlayerSnapshot *snapshot, u32 currentTick)
{
    if (snapshot->tick == 0 || currentTick == 0)
        return TRUE;
    if (snapshot->tick > currentTick)
        return snapshot->tick - currentTick <= NET_PLAYER_SNAPSHOT_FUTURE_SKEW_FRAMES;

    return currentTick - snapshot->tick <= NET_PLAYER_SNAPSHOT_TTL_FRAMES;
}

static bool8 SnapshotIsStale(const struct NetPlayerSnapshot *snapshot)
{
    return (snapshot->flags & (NET_PLAYER_FLAG_STALE | NET_PLAYER_FLAG_RESYNC))
        || snapshot->stale_frames >= NET_PLAYER_STALE_FRAMES;
}

static bool8 SnapshotHasValidOnlineIdentity(const struct NetPlayerSnapshot *snapshot, const struct NetTransportSessionView *view)
{
    return snapshot->session_epoch == view->session_epoch
        && snapshot->sequence != 0;
}

static bool8 SnapshotServerTickIsFresh(const struct NetPlayerSnapshot *snapshot, u32 currentTick)
{
    if (snapshot->server_tick_seen == 0 || currentTick == 0)
        return TRUE;
    if (snapshot->server_tick_seen > currentTick)
        return snapshot->server_tick_seen - currentTick <= NET_PLAYER_SNAPSHOT_FUTURE_SKEW_FRAMES;

    return currentTick - snapshot->server_tick_seen <= NET_PLAYER_DISCONNECT_FRAMES;
}

static bool8 BattleProfileIsValid(const struct NetPlayerBattleProfile *profile)
{
    u8 party_mon_index;
    u8 move_index;

    if (profile == NULL)
        return FALSE;
    if (profile->party_count > NET_PLAYER_PARTY_SNAPSHOT_SIZE)
        return FALSE;

    for (party_mon_index = 0; party_mon_index < profile->party_count; party_mon_index++)
    {
        if (profile->party_species[party_mon_index] >= NUM_SPECIES)
            return FALSE;
        if (profile->party_species[party_mon_index] != SPECIES_NONE && profile->party_levels[party_mon_index] > MAX_LEVEL)
            return FALSE;
        for (move_index = 0; move_index < MAX_MON_MOVES; move_index++)
        {
            if (profile->party_moves[party_mon_index][move_index] >= MOVES_COUNT)
                return FALSE;
        }
    }

    return TRUE;
}

static bool8 SnapshotIsValid(const struct NetPlayerSnapshot *snapshot, u8 player_id_for_snapshot, const struct NetTransportSessionView *view)
{
    if (!snapshot->active)
        return TRUE;
    if (snapshot->player_id != player_id_for_snapshot)
        return FALSE;
    if (!SnapshotHasValidOnlineIdentity(snapshot, view))
        return FALSE;
    if (!MapLocationIsValid(snapshot->map_group, snapshot->map_num))
        return FALSE;
    if (!SnapshotCoordsAreSane(snapshot))
        return FALSE;
    if (!ElevationIsValid(snapshot->elevation))
        return FALSE;
    if (!DirectionIsValid(snapshot->facing_direction))
        return FALSE;
    if (snapshot->flags & ~NET_PLAYER_FLAGS_MASK)
        return FALSE;
    if (!IsValidInteractionState(snapshot->interaction_state))
        return FALSE;
    if (snapshot->subsession_state > MULTIPLAYER_SUBSESSION_STATE_ERROR)
        return FALSE;
    if (snapshot->subsession_id > MAX_NET_SUBSESSIONS)
        return FALSE;
    if ((snapshot->flags & NET_PLAYER_FLAG_IN_SUBSESSION) && snapshot->subsession_id == NET_SUBSESSION_NONE)
        return FALSE;
    if ((snapshot->flags & NET_PLAYER_FLAG_IN_SUBSESSION) && !IsValidSubsessionState(snapshot->subsession_state))
        return FALSE;
    if (!(snapshot->flags & NET_PLAYER_FLAG_IN_SUBSESSION)
     && (snapshot->subsession_id != NET_SUBSESSION_NONE || snapshot->subsession_state != MULTIPLAYER_SUBSESSION_STATE_NONE))
        return FALSE;
    if ((snapshot->flags & NET_PLAYER_FLAG_IN_SUBSESSION)
     && snapshot->interaction_state != MULTIPLAYER_INTERACTION_BATTLE
     && snapshot->interaction_state != MULTIPLAYER_INTERACTION_TRADE)
        return FALSE;
    if ((snapshot->interaction_state == MULTIPLAYER_INTERACTION_BATTLE
      || snapshot->interaction_state == MULTIPLAYER_INTERACTION_TRADE)
     && !(snapshot->flags & NET_PLAYER_FLAG_IN_SUBSESSION))
        return FALSE;
    if ((snapshot->interaction_state == MULTIPLAYER_INTERACTION_SCRIPT
      || snapshot->interaction_state == MULTIPLAYER_INTERACTION_WARP)
     && !(snapshot->flags & NET_PLAYER_FLAG_BUSY))
        return FALSE;
    if (snapshot->interaction_state == MULTIPLAYER_INTERACTION_OPTIONS_MENU
     && !(snapshot->flags & NET_PLAYER_FLAG_BUSY))
        return FALSE;
    if (snapshot->stale_frames > NET_PLAYER_DISCONNECT_FRAMES)
        return FALSE;
    if (snapshot->anomaly_score >= NET_ANOMALY_THRESHOLD_KICK)
        return FALSE;
    if (!SnapshotTickIsFresh(snapshot, view->bridge_tick))
        return FALSE;
    if (!SnapshotServerTickIsFresh(snapshot, view->bridge_tick))
        return FALSE;

    return TRUE;
}

static bool8 SubsessionIncludesPlayer(const struct MultiplayerSubsession *subsession, u8 player_id)
{
    u8 player_index_in_subsession;
    u8 max_players;

    if (!subsession->active)
        return FALSE;

    max_players = GetMaxPlayersForSubsession(subsession->type);
    if (max_players == 0 || subsession->player_count > max_players)
        return FALSE;

    for (player_index_in_subsession = 0; player_index_in_subsession < subsession->player_count; player_index_in_subsession++)
    {
        if (subsession->players[player_index_in_subsession] == player_id)
            return TRUE;
    }

    return FALSE;
}

static bool8 BlockingSubsessionIncludesPlayer(const struct MultiplayerSubsession *subsession, u8 player_id)
{
    return SubsessionStateBlocksInteraction(subsession->state)
        && SubsessionIncludesPlayer(subsession, player_id);
}

static bool8 SubsessionIsValid(const struct MultiplayerSubsession *subsession, const struct NetPlayerSnapshot *players)
{
    u8 player_index_in_subsession;
    u8 duplicate_check_index;
    u8 max_players;

    if (!subsession->active)
        return TRUE;
    if (subsession->subsession_id == NET_SUBSESSION_NONE || subsession->subsession_id > MAX_NET_SUBSESSIONS)
        return FALSE;
    if (!IsValidSubsessionState(subsession->state))
        return FALSE;
    if (SubsessionStateBlocksInteraction(subsession->state) && subsession->timeout_frames == 0)
        return FALSE;
    if (!IsValidPlayerId(subsession->host_player_id) || !players[subsession->host_player_id].active)
        return FALSE;

    max_players = GetMaxPlayersForSubsession(subsession->type);
    if (max_players == 0 || subsession->player_count == 0 || subsession->player_count > max_players)
        return FALSE;

    for (player_index_in_subsession = 0; player_index_in_subsession < subsession->player_count; player_index_in_subsession++)
    {
        if (!IsValidPlayerId(subsession->players[player_index_in_subsession])
         || !players[subsession->players[player_index_in_subsession]].active
         || SnapshotIsStale(&players[subsession->players[player_index_in_subsession]]))
            return FALSE;

        for (duplicate_check_index = player_index_in_subsession + 1; duplicate_check_index < subsession->player_count; duplicate_check_index++)
        {
            if (subsession->players[player_index_in_subsession] == subsession->players[duplicate_check_index])
                return FALSE;
        }
    }

    return TRUE;
}

static bool8 SanitizeSessionView(struct NetTransportSessionView *view)
{
    u8 player_index;
    u8 subsession_index;
    u8 player_index_in_subsession;
    bool8 claimed_players[MAX_NET_PLAYERS] = {0};

    if (!view->connected)
        return FALSE;
    if (!IsValidPlayerId(view->local_player_id))
        return FALSE;
    if (!IsValidPlayerId(view->host_player_id))
        return FALSE;
    if (view->player_count == 0 || view->player_count > MAX_NET_PLAYERS)
        return FALSE;
    if (!SessionIdentityIsValid(view))
        return FALSE;

    for (player_index = 0; player_index < MAX_NET_PLAYERS; player_index++)
    {
        if (!SnapshotIsValid(&view->players[player_index], player_index, view))
            memset(&view->players[player_index], 0, sizeof(view->players[player_index]));
    }

    for (subsession_index = 0; subsession_index < MAX_NET_SUBSESSIONS; subsession_index++)
    {
        if (!view->subsessions[subsession_index].active)
        {
            memset(&view->subsessions[subsession_index], 0, sizeof(view->subsessions[subsession_index]));
            continue;
        }
        if (!SubsessionIsValid(&view->subsessions[subsession_index], view->players))
        {
            memset(&view->subsessions[subsession_index], 0, sizeof(view->subsessions[subsession_index]));
            continue;
        }

        if (SubsessionStateBlocksInteraction(view->subsessions[subsession_index].state))
        {
            for (player_index_in_subsession = 0; player_index_in_subsession < view->subsessions[subsession_index].player_count; player_index_in_subsession++)
            {
                if (claimed_players[view->subsessions[subsession_index].players[player_index_in_subsession]])
                    break;
            }
            if (player_index_in_subsession != view->subsessions[subsession_index].player_count)
            {
                memset(&view->subsessions[subsession_index], 0, sizeof(view->subsessions[subsession_index]));
                continue;
            }
        }

        if (SubsessionStateBlocksInteraction(view->subsessions[subsession_index].state))
        {
            for (player_index_in_subsession = 0; player_index_in_subsession < view->subsessions[subsession_index].player_count; player_index_in_subsession++)
                claimed_players[view->subsessions[subsession_index].players[player_index_in_subsession]] = TRUE;
        }
    }

    return TRUE;
}

static void ClearInactiveBattleProfiles(void)
{
    u8 player_index;

    for (player_index = 0; player_index < MAX_NET_PLAYERS; player_index++)
    {
        if (!sSession.players[player_index].active || SnapshotIsStale(&sSession.players[player_index]))
            memset(&sSession.player_profiles[player_index], 0, sizeof(sSession.player_profiles[player_index]));
    }
}

static bool8 BarrierIncludesPlayer(const struct MultiplayerInteractionBarrier *barrier, u8 player_id)
{
    u8 player_index_in_barrier;

    if (!barrier->active)
        return FALSE;

    for (player_index_in_barrier = 0; player_index_in_barrier < barrier->player_count; player_index_in_barrier++)
    {
        if (barrier->players[player_index_in_barrier] == player_id)
            return TRUE;
    }

    return FALSE;
}

static u8 GetBarrierTypeForSubsession(u8 subsession_type)
{
    if (subsession_type == MULTIPLAYER_SUBSESSION_TRADE)
        return MULTIPLAYER_BARRIER_TRADE_INVITE;
    if (subsession_type == MULTIPLAYER_SUBSESSION_PVE_BATTLE
     || subsession_type == MULTIPLAYER_SUBSESSION_PVP_BATTLE)
        return MULTIPLAYER_BARRIER_BATTLE_INVITE;

    return MULTIPLAYER_BARRIER_NONE;
}

static bool8 SubsessionMatchesBarrier(const struct MultiplayerSubsession *subsession, const struct MultiplayerInteractionBarrier *barrier)
{
    u8 player_index_in_barrier;

    if (!subsession->active || !barrier->active)
        return FALSE;
    if (GetBarrierTypeForSubsession(subsession->type) != barrier->type)
        return FALSE;
    if (subsession->player_count != barrier->player_count)
        return FALSE;

    for (player_index_in_barrier = 0; player_index_in_barrier < barrier->player_count; player_index_in_barrier++)
    {
        if (!SubsessionIncludesPlayer(subsession, barrier->players[player_index_in_barrier]))
            return FALSE;
    }

    return TRUE;
}

static void ResetInteractionBarrier(void)
{
    memset(&sSession.interaction_barrier, 0, sizeof(sSession.interaction_barrier));
}

static void ClearInviteBarrierWhenSubsessionLeavesInvite(void)
{
    u8 subsession_index;

    if (sSession.interaction_barrier.type != MULTIPLAYER_BARRIER_BATTLE_INVITE
     && sSession.interaction_barrier.type != MULTIPLAYER_BARRIER_TRADE_INVITE)
        return;

    for (subsession_index = 0; subsession_index < MAX_NET_SUBSESSIONS; subsession_index++)
    {
        if (!SubsessionMatchesBarrier(&sSession.subsessions[subsession_index], &sSession.interaction_barrier))
            continue;
        if (sSession.subsessions[subsession_index].state == MULTIPLAYER_SUBSESSION_STATE_INVITING
         || sSession.subsessions[subsession_index].state == MULTIPLAYER_SUBSESSION_STATE_READY)
            return;

        ResetInteractionBarrier();
        return;
    }
}

static void ExpireInteractionBarrier(void)
{
    if (!sSession.interaction_barrier.active)
        return;
    if (sSession.interaction_barrier.timeout_frames != 0)
        sSession.interaction_barrier.timeout_frames--;
    if (sSession.interaction_barrier.timeout_frames == 0)
        ResetInteractionBarrier();
}

static bool8 SnapshotInteractionBlocks(const struct NetPlayerSnapshot *snapshot)
{
    if (!snapshot->active)
        return FALSE;
    if (SnapshotIsStale(snapshot))
        return TRUE;
    if (snapshot->interaction_state == MULTIPLAYER_INTERACTION_SCRIPT
     || snapshot->interaction_state == MULTIPLAYER_INTERACTION_WARP
     || snapshot->interaction_state == MULTIPLAYER_INTERACTION_OPTIONS_MENU)
        return TRUE;
    if ((snapshot->flags & NET_PLAYER_FLAG_IN_SUBSESSION)
     && SubsessionStateBlocksInteraction(snapshot->subsession_state))
        return TRUE;

    return FALSE;
}

static bool8 SnapshotBlocksSubsessionStart(const struct NetPlayerSnapshot *snapshot)
{
    if (!snapshot->active)
        return FALSE;
    if (SnapshotIsStale(snapshot))
        return TRUE;
    if (snapshot->interaction_state == MULTIPLAYER_INTERACTION_SCRIPT
     || snapshot->interaction_state == MULTIPLAYER_INTERACTION_WARP)
        return TRUE;
    if ((snapshot->flags & NET_PLAYER_FLAG_IN_SUBSESSION)
     && SubsessionStateBlocksInteraction(snapshot->subsession_state))
        return TRUE;

    return FALSE;
}

static bool8 PlayerBlocksSubsessionStart(u8 player_id)
{
    if (!IsValidPlayerId(player_id))
        return TRUE;
    if (!MultiplayerSession_IsPlayerActive(player_id))
        return TRUE;

    return SnapshotBlocksSubsessionStart(&sSession.players[player_id])
        || MultiplayerSession_IsPlayerInSubsession(player_id)
        || BarrierIncludesPlayer(&sSession.interaction_barrier, player_id);
}

static bool8 PlayerBlocksBarrierStart(u8 barrier_type, u8 player_id)
{
    if (barrier_type == MULTIPLAYER_BARRIER_BATTLE_INVITE
     || barrier_type == MULTIPLAYER_BARRIER_TRADE_INVITE)
        return PlayerBlocksSubsessionStart(player_id);

    return MultiplayerSession_IsPlayerInteractionBlocked(player_id);
}

static bool8 SessionHasActiveLocalSubsession(void)
{
    u8 subsession_index;

    for (subsession_index = 0; subsession_index < MAX_NET_SUBSESSIONS; subsession_index++)
    {
        if (BlockingSubsessionIncludesPlayer(&sSession.subsessions[subsession_index], sSession.local_player_id))
            return TRUE;
    }

    return FALSE;
}

static const struct MultiplayerSubsession *GetLocalSubsession(void)
{
    u8 subsession_index;

    for (subsession_index = 0; subsession_index < MAX_NET_SUBSESSIONS; subsession_index++)
    {
        if (BlockingSubsessionIncludesPlayer(&sSession.subsessions[subsession_index], sSession.local_player_id))
            return &sSession.subsessions[subsession_index];
    }

    return NULL;
}

static void AbortLocalSubsessionsForDisconnect(void)
{
    u8 subsession_index;

    for (subsession_index = 0; subsession_index < MAX_NET_SUBSESSIONS; subsession_index++)
    {
        if (!sSession.subsessions[subsession_index].active)
            continue;
        if (!SubsessionIncludesPlayer(&sSession.subsessions[subsession_index], sSession.local_player_id))
            continue;

        sSession.subsessions[subsession_index].state = MULTIPLAYER_SUBSESSION_STATE_ERROR;
        memset(&sSession.subsessions[subsession_index], 0, sizeof(sSession.subsessions[subsession_index]));
    }

    ResetInteractionBarrier();
}

static void ExpireLocalSubsessions(void)
{
    u8 subsession_index;

    ExpireInteractionBarrier();

    if (sSession.local_player_id == NET_PLAYER_NONE || sSession.local_player_id != sSession.host_player_id)
        return;

    for (subsession_index = 0; subsession_index < MAX_NET_SUBSESSIONS; subsession_index++)
    {
        if (!sSession.subsessions[subsession_index].active)
            continue;
        if (!SubsessionStateBlocksInteraction(sSession.subsessions[subsession_index].state))
            continue;

        if (sSession.subsessions[subsession_index].timeout_frames != 0)
            sSession.subsessions[subsession_index].timeout_frames--;
        if (sSession.subsessions[subsession_index].timeout_frames == 0)
        {
            sSession.subsessions[subsession_index].state = MULTIPLAYER_SUBSESSION_STATE_ERROR;
            NetTransport_SendPacket(NET_PACKET_SUBSESSION_ABORT, &sSession.subsessions[subsession_index], sizeof(sSession.subsessions[subsession_index]));
            memset(&sSession.subsessions[subsession_index], 0, sizeof(sSession.subsessions[subsession_index]));
            ResetInteractionBarrier();
        }
    }
}

static void CopyViewIntoSession(const struct NetTransportSessionView *view)
{
    u32 oldSessionEpoch = sSession.session_epoch;

    sSession.local_player_id = view->local_player_id;
    sSession.host_player_id = view->host_player_id;
    sSession.player_count = view->player_count;
    sSession.transport_mode = view->transport_mode;
    sSession.session_id = view->session_id;
    sSession.session_epoch = view->session_epoch;
    sSession.player_token = view->player_token;
    sSession.join_nonce = view->join_nonce;
    sSession.bridge_tick = view->bridge_tick;
    sSession.server_clock_seconds = view->server_clock_seconds;
    if (oldSessionEpoch != sSession.session_epoch)
    {
        sClientHelloSent = FALSE;
        sHeartbeatTimer = 0;
        sSession.local_client_frame = 0;
        sSession.local_snapshot_sequence = 0;
        sSession.local_snapshot_hot_hash = 0;
        sSession.local_action_sequence = 0;
        sSession.local_profile_publish_timer = 0;
        memset(sSession.player_profiles, 0, sizeof(sSession.player_profiles));
        MultiplayerCommit_Init();
        MultiplayerInteractionMenu_Reset();
        MultiplayerTrade_Reset();
        EngineExtensionProfile_Clear();
    }
    sSession.health_state = MULTIPLAYER_HEALTH_HEALTHY;
    sTransportLossFrames = 0;
    if (view->bridge_tick != 0)
        sSession.tick = view->bridge_tick;
    else
        sSession.tick++;
    memcpy(sSession.players, view->players, sizeof(sSession.players));
    memcpy(sSession.subsessions, view->subsessions, sizeof(sSession.subsessions));
    ClearInactiveBattleProfiles();
    ClearInviteBarrierWhenSubsessionLeavesInvite();
    ExpireLocalSubsessions();

    if (SessionHasActiveLocalSubsession())
        sSession.state = MULTIPLAYER_SESSION_SUBSESSION;
    else if (view->player_count > 1)
        sSession.state = MULTIPLAYER_SESSION_OVERWORLD_SYNC;
    else
        sSession.state = MULTIPLAYER_SESSION_LOBBY;
}

static u32 BuildFeatureFlags(void)
{
    u32 flags = 0;

#if FEATURE_MULTIPLAYER
    flags |= NET_FEATURE_FLAG_MULTIPLAYER;
#endif
#if FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT
    flags |= NET_FEATURE_FLAG_EMULATOR_TRANSPORT;
#endif
#if FEATURE_MULTIPLAYER_LINK_TRANSPORT
    flags |= NET_FEATURE_FLAG_LINK_TRANSPORT;
#endif
#if FEATURE_ENGINE_GEN1
    flags |= NET_FEATURE_FLAG_ENGINE_GEN1;
#endif
#if FEATURE_ENGINE_GEN2
    flags |= NET_FEATURE_FLAG_ENGINE_GEN2;
#endif
#if FEATURE_ENGINE_GEN3
    flags |= NET_FEATURE_FLAG_ENGINE_GEN3;
#endif

    return flags;
}

static bool8 PublishClientHello(void)
{
    struct NetClientHello hello;

    if (sClientHelloSent)
        return FALSE;
    if (sSession.session_epoch == 0 || sSession.player_token == 0 || sSession.join_nonce == 0)
        return FALSE;

    memset(&hello, 0, sizeof(hello));
    hello.protocol_version = NET_PROTOCOL_VERSION;
    hello.bridge_version = NET_EMULATOR_BRIDGE_VERSION;
    hello.build_id = NET_PROTOCOL_BUILD_ID;
    hello.ruleset_hash = NET_RULESET_HASH;
    hello.feature_flags = BuildFeatureFlags();
    hello.profile_protocol_version = EngineExtensionProfile_GetProtocolVersion();
    hello.profile_capability_flags = EngineExtensionProfile_GetCapabilityFlags();
    hello.profile_capability_hash = EngineExtensionProfile_GetCapabilityHash();
    hello.mod_catalog_hash = EngineExtensionProfile_GetCatalogHash();
    hello.mod_catalog_count = EngineExtensionProfile_GetCatalogEntryCount();
    hello.transport_mode = NET_ACTIVE_TRANSPORT_MODE;

    if (NetTransport_SendPacket(NET_PACKET_CLIENT_HELLO, &hello, sizeof(hello)))
    {
        sClientHelloSent = TRUE;
        sHeartbeatTimer = NET_HEARTBEAT_INTERVAL_FRAMES;
        return TRUE;
    }

    return FALSE;
}

static u32 NextActionSequence(void)
{
    sSession.local_action_sequence++;
    if (sSession.local_action_sequence == 0)
        sSession.local_action_sequence++;

    return sSession.local_action_sequence;
}

static void AdvanceLocalClientFrame(void)
{
    sSession.local_client_frame++;
    if (sSession.local_client_frame == 0)
        sSession.local_client_frame++;
}

static u32 NextLocalSnapshotSequence(void)
{
    sSession.local_snapshot_sequence++;
    if (sSession.local_snapshot_sequence == 0)
        sSession.local_snapshot_sequence++;

    return sSession.local_snapshot_sequence;
}

static void PublishHeartbeat(void)
{
    struct NetHeartbeat heartbeat;

    if (sSession.session_epoch == 0 || sSession.player_token == 0 || sSession.join_nonce == 0)
        return;
    if (sHeartbeatTimer != 0)
    {
        sHeartbeatTimer--;
        return;
    }

    memset(&heartbeat, 0, sizeof(heartbeat));
    heartbeat.client_frame = sSession.local_client_frame;
    heartbeat.server_tick_seen = sSession.bridge_tick;
    heartbeat.session_epoch = sSession.session_epoch;
    heartbeat.player_token = sSession.player_token;
    heartbeat.join_nonce = sSession.join_nonce;
    if (NetTransport_SendPacket(NET_PACKET_HEARTBEAT, &heartbeat, sizeof(heartbeat)))
        sHeartbeatTimer = NET_HEARTBEAT_INTERVAL_FRAMES;
}

static u32 AddSnapshotFieldToHotHash(u32 hash, u32 value)
{
    return hash ^ (value + 0x9E3779B9 + (hash << 6) + (hash >> 2));
}

static u32 CalcSnapshotHotStateHash(const struct NetPlayerSnapshot *snapshot)
{
    u32 hash = 0x811C9DC5;

    hash = AddSnapshotFieldToHotHash(hash, snapshot->active);
    hash = AddSnapshotFieldToHotHash(hash, snapshot->player_id);
    hash = AddSnapshotFieldToHotHash(hash, snapshot->map_group);
    hash = AddSnapshotFieldToHotHash(hash, snapshot->map_num);
    hash = AddSnapshotFieldToHotHash(hash, (u16)snapshot->x);
    hash = AddSnapshotFieldToHotHash(hash, (u16)snapshot->y);
    hash = AddSnapshotFieldToHotHash(hash, snapshot->elevation);
    hash = AddSnapshotFieldToHotHash(hash, snapshot->facing_direction);
    hash = AddSnapshotFieldToHotHash(hash, snapshot->movement_action_id);
    hash = AddSnapshotFieldToHotHash(hash, snapshot->avatar_graphics_id);
    hash = AddSnapshotFieldToHotHash(hash, snapshot->outfit_id);
    hash = AddSnapshotFieldToHotHash(hash, snapshot->interaction_state);
    hash = AddSnapshotFieldToHotHash(hash, snapshot->subsession_id);
    hash = AddSnapshotFieldToHotHash(hash, snapshot->subsession_state);
    hash = AddSnapshotFieldToHotHash(hash, snapshot->graphics_revision);
    hash = AddSnapshotFieldToHotHash(hash, snapshot->flags);

    return hash == 0 ? 1 : hash;
}

static bool8 LocalSnapshotHasNeverBeenPublished(void)
{
    return sSession.local_snapshot_sequence == 0;
}

static bool8 LocalSnapshotHotStateChanged(u32 hotHash)
{
    return hotHash != sSession.local_snapshot_hot_hash;
}

static bool8 LocalSnapshotNeedsIdleRefresh(void)
{
#if NET_PLAYER_SNAPSHOT_IDLE_REPUBLISH_FRAMES > 0
    return sSession.tick % NET_PLAYER_SNAPSHOT_IDLE_REPUBLISH_FRAMES == 0;
#else
    return FALSE;
#endif
}

static bool8 LocalSnapshotShouldBePublished(u32 hotHash)
{
    return LocalSnapshotHasNeverBeenPublished()
        || LocalSnapshotHotStateChanged(hotHash)
        || LocalSnapshotNeedsIdleRefresh();
}

static void AddSessionStateToLocalSnapshot(struct NetPlayerSnapshot *snapshot)
{
    snapshot->server_tick_seen = sSession.bridge_tick;
    snapshot->session_epoch = sSession.session_epoch;
    snapshot->stale_frames = 0;
    snapshot->anomaly_score = sSession.anomaly_score;
}

static void AddSubsessionStateToLocalSnapshot(struct NetPlayerSnapshot *snapshot, const struct MultiplayerSubsession *subsession)
{
    snapshot->flags |= NET_PLAYER_FLAG_BUSY | NET_PLAYER_FLAG_IN_SUBSESSION;
    snapshot->subsession_id = subsession->subsession_id;
    snapshot->subsession_state = subsession->state;
    if (subsession->type == MULTIPLAYER_SUBSESSION_TRADE)
        snapshot->interaction_state = MULTIPLAYER_INTERACTION_TRADE;
    else
        snapshot->interaction_state = MULTIPLAYER_INTERACTION_BATTLE;
}

static void AddBarrierStateToLocalSnapshot(struct NetPlayerSnapshot *snapshot)
{
    snapshot->flags |= NET_PLAYER_FLAG_BUSY;
    if (sSession.interaction_barrier.type == MULTIPLAYER_BARRIER_SCRIPT)
        snapshot->interaction_state = MULTIPLAYER_INTERACTION_SCRIPT;
    else if (sSession.interaction_barrier.type == MULTIPLAYER_BARRIER_WARP)
        snapshot->interaction_state = MULTIPLAYER_INTERACTION_WARP;
}

static void AddInteractionStateToLocalSnapshot(struct NetPlayerSnapshot *snapshot)
{
    const struct MultiplayerSubsession *subsession = GetLocalSubsession();

    if (subsession != NULL)
        AddSubsessionStateToLocalSnapshot(snapshot, subsession);
    else if (BarrierIncludesPlayer(&sSession.interaction_barrier, sSession.local_player_id))
        AddBarrierStateToLocalSnapshot(snapshot);
    else
        MultiplayerInteractionMenu_ApplySnapshotState(snapshot);
}

static bool8 SendLocalSnapshotIfNeeded(struct NetPlayerSnapshot *snapshot, u32 hotHash)
{
    if (!LocalSnapshotShouldBePublished(hotHash))
        return FALSE;

    snapshot->sequence = NextLocalSnapshotSequence();
    if (!NetTransport_WriteLocalSnapshot(snapshot))
        return FALSE;

    sSession.local_snapshot_hot_hash = hotHash;
    return TRUE;
}

static void PublishLocalSnapshot(void)
{
    struct NetPlayerSnapshot snapshot;
    u32 hotHash;

    if (sSession.local_player_id >= MAX_NET_PLAYERS)
        return;
    if (!MultiplayerOverworld_CanTick())
        return;

    MultiplayerOverworld_BuildLocalSnapshot(&snapshot, sSession.local_player_id, sSession.tick);
    AdvanceLocalClientFrame();
    AddSessionStateToLocalSnapshot(&snapshot);
    AddInteractionStateToLocalSnapshot(&snapshot);
    hotHash = CalcSnapshotHotStateHash(&snapshot);
    snapshot.sequence = sSession.local_snapshot_sequence;
    SendLocalSnapshotIfNeeded(&snapshot, hotHash);

    sSession.players[sSession.local_player_id] = snapshot;
}

static void PublishLocalBattleProfile(void)
{
    struct NetPlayerBattleProfile profile;

    if (sSession.local_player_id >= MAX_NET_PLAYERS)
        return;
    if (!MultiplayerOverworld_CanTick())
        return;
    if (sSession.local_profile_publish_timer != 0)
    {
        sSession.local_profile_publish_timer--;
        return;
    }

    MultiplayerOverworld_BuildLocalBattleProfile(&profile);
    if (!BattleProfileIsValid(&profile))
    {
        sSession.local_profile_publish_timer = NET_PLAYER_PROFILE_PUBLISH_RETRY_FRAMES;
        return;
    }
    if (NetTransport_SendPacket(NET_PACKET_PLAYER_BATTLE_PROFILE, &profile, sizeof(profile)))
    {
        sSession.player_profiles[sSession.local_player_id] = profile;
        sSession.local_profile_publish_timer = NET_PLAYER_PROFILE_PUBLISH_INTERVAL_FRAMES;
    }
    else
    {
        sSession.local_profile_publish_timer = NET_PLAYER_PROFILE_PUBLISH_RETRY_FRAMES;
    }
}

static void PublishMoveIntent(u8 direction, u16 new_keys, u16 held_keys)
{
    struct NetMoveIntent intent;
    const struct NetPlayerSnapshot *snapshot;

    if (!MultiplayerSession_IsOnline())
        return;
    if (sSession.local_player_id >= MAX_NET_PLAYERS)
        return;
    snapshot = &sSession.players[sSession.local_player_id];
    if (!snapshot->active)
        return;

    memset(&intent, 0, sizeof(intent));
    intent.header.client_frame = sSession.local_client_frame;
    intent.header.server_tick_seen = sSession.bridge_tick;
    intent.header.action_sequence = sSession.local_snapshot_sequence;
    intent.held_keys = held_keys;
    intent.new_keys = new_keys;
    intent.direction = direction;
    intent.movement_action_id = snapshot->movement_action_id;
    intent.map_group = snapshot->map_group;
    intent.map_num = snapshot->map_num;
    intent.x = snapshot->x;
    intent.y = snapshot->y;
    NetTransport_SendUnreliablePacket(NET_PACKET_MOVE_INTENT, &intent, sizeof(intent));
}

static void PublishInteractIntent(u8 barrier_type, u8 target_player_id, u8 map_group, u8 map_number)
{
    struct NetInteractIntent intent;
    struct MultiplayerTransactionKey transaction_key;
    const struct NetPlayerSnapshot *snapshot;
    u32 action_sequence;

    if (!MultiplayerSession_IsOnline())
        return;
    if (sSession.local_player_id >= MAX_NET_PLAYERS)
        return;
    snapshot = &sSession.players[sSession.local_player_id];
    if (!snapshot->active)
        return;

    memset(&intent, 0, sizeof(intent));
    action_sequence = NextActionSequence();
    MultiplayerCommit_BuildKey(&transaction_key, sSession.session_epoch, sSession.local_player_id, NET_PACKET_INTERACT_INTENT, NET_SUBSESSION_NONE, action_sequence);
    intent.header.client_frame = sSession.local_client_frame;
    intent.header.server_tick_seen = sSession.bridge_tick;
    intent.header.action_sequence = action_sequence;
    intent.header.transaction_id = MultiplayerCommit_GetTransactionId(&transaction_key);
    intent.target_player_id = target_player_id;
    intent.interaction_type = barrier_type;
    intent.map_group = map_group;
    intent.map_num = map_number;
    intent.x = snapshot->x;
    intent.y = snapshot->y;
    NetTransport_SendPacket(NET_PACKET_INTERACT_INTENT, &intent, sizeof(intent));
}

static void SendProfileAck(u32 profile_hash, u8 profile_ack_result, u16 detail);

static u16 GetCatalogChunkCount(void)
{
    if (EngineExtensionProfile_GetCatalogEntryCount() == 0)
        return 0;

    return (EngineExtensionProfile_GetCatalogEntryCount() + NET_CATALOG_CHUNK_ENTRY_COUNT - 1) / NET_CATALOG_CHUNK_ENTRY_COUNT;
}

static u8 GetCatalogChunkEntryCount(u16 first_entry, u16 entryTotal)
{
    u16 remaining = entryTotal - first_entry;

    if (remaining > NET_CATALOG_CHUNK_ENTRY_COUNT)
        return NET_CATALOG_CHUNK_ENTRY_COUNT;

    return remaining;
}

static bool8 SendModCatalogBegin(u16 entryTotal, u16 chunk_count)
{
    struct NetClientCatalogBegin begin;

    memset(&begin, 0, sizeof(begin));
    begin.catalog_hash = EngineExtensionProfile_GetCatalogHash();
    begin.entry_count = entryTotal;
    begin.chunk_count = chunk_count;
    begin.schema_hash = EngineExtensionProfile_GetCatalogSchemaHash();

    return NetTransport_SendPacket(NET_PACKET_CLIENT_CATALOG_BEGIN, &begin, sizeof(begin));
}

static void BuildModCatalogChunk(struct NetClientCatalogChunk *chunk, const struct ModCatalogEntry *entries, u16 chunk_index, u16 entryTotal)
{
    u16 first_entry = chunk_index * NET_CATALOG_CHUNK_ENTRY_COUNT;
    u8 entry_count = GetCatalogChunkEntryCount(first_entry, entryTotal);
    u8 entry_index_in_chunk;

    memset(chunk, 0, sizeof(*chunk));
    chunk->catalog_hash = EngineExtensionProfile_GetCatalogHash();
    chunk->chunk_index = chunk_index;
    chunk->first_entry = first_entry;
    chunk->entry_count = entry_count;
    for (entry_index_in_chunk = 0; entry_index_in_chunk < entry_count; entry_index_in_chunk++)
        chunk->entries[entry_index_in_chunk] = entries[first_entry + entry_index_in_chunk];
}

static bool8 SendModCatalogChunk(const struct ModCatalogEntry *entries, u16 chunk_index, u16 entryTotal)
{
    struct NetClientCatalogChunk chunk;

    BuildModCatalogChunk(&chunk, entries, chunk_index, entryTotal);
    return NetTransport_SendPacket(NET_PACKET_CLIENT_CATALOG_CHUNK, &chunk, sizeof(chunk));
}

static void SendModCatalog(void)
{
    const struct ModCatalogEntry *entries = EngineExtensionProfile_GetCatalogEntries();
    u16 entryTotal = EngineExtensionProfile_GetCatalogEntryCount();
    u16 chunk_count = GetCatalogChunkCount();
    u16 chunk_index;

    if (!SendModCatalogBegin(entryTotal, chunk_count))
        return;
    if (entries == NULL)
        return;

    for (chunk_index = 0; chunk_index < chunk_count; chunk_index++)
    {
        if (!SendModCatalogChunk(entries, chunk_index, entryTotal))
            return;
    }
}

static void HandleServerProfileBegin(const struct NetServerProfileBegin *profileBegin)
{
    if (profileBegin->profile_protocol_version != EngineExtensionProfile_GetProtocolVersion())
        SendProfileAck(profileBegin->profile_hash, ENGINE_EXTENSION_PROFILE_RESULT_UNSUPPORTED_VERSION, profileBegin->profile_protocol_version);
    else if (!EngineExtensionProfile_CanReceive(profileBegin->capability_flags, profileBegin->capability_hash))
        SendProfileAck(profileBegin->profile_hash, ENGINE_EXTENSION_PROFILE_RESULT_UNSUPPORTED_CAPABILITY, 0);
    else if (!EngineExtensionProfile_BeginReceive(profileBegin->profile_hash, profileBegin->profile_size, profileBegin->chunk_count))
        SendProfileAck(profileBegin->profile_hash, ENGINE_EXTENSION_PROFILE_RESULT_OUT_OF_MEMORY, profileBegin->profile_size);
}

static void HandleServerProfileChunk(const struct NetServerProfileChunk *profileChunk)
{
    if (!EngineExtensionProfile_ReceiveChunk(profileChunk->profile_hash, profileChunk->chunk_index, profileChunk->offset, profileChunk->data, profileChunk->data_size))
        SendProfileAck(profileChunk->profile_hash, ENGINE_EXTENSION_PROFILE_RESULT_BAD_SIZE, profileChunk->chunk_index);
}

static void HandleServerProfileCommit(const struct NetServerProfileCommit *profileCommit)
{
    u8 profileResult;

    profileResult = EngineExtensionProfile_CommitReceive(profileCommit->profile_hash);
    SendProfileAck(profileCommit->profile_hash, profileResult, 0);
}

static void HandleBattleProfilePacket(u8 player_id, const void *packet_payload, u16 packet_payload_size)
{
    struct NetPlayerBattleProfile battleProfile;

    if (packet_payload_size != sizeof(struct NetPlayerBattleProfile))
        return;
    if (player_id >= MAX_NET_PLAYERS)
        return;

    memcpy(&battleProfile, packet_payload, sizeof(battleProfile));
    if (sSession.players[player_id].active && BattleProfileIsValid(&battleProfile))
        sSession.player_profiles[player_id] = battleProfile;
}

static void ProcessInboundPacket(const struct NetPacketEnvelope *envelope, const void *packet_payload, u16 packet_payload_size)
{
    const struct NetTradeAction *tradeAction;

    switch (envelope->packet_type)
    {
    case NET_PACKET_COMMIT_RESULT:
        if (packet_payload_size == sizeof(struct NetCommitResult))
            MultiplayerCommit_ApplyServerResult((const struct NetCommitResult *)packet_payload);
        break;
    case NET_PACKET_TRADE_INPUT:
        if (packet_payload_size == sizeof(struct NetTradeAction))
        {
            tradeAction = (const struct NetTradeAction *)packet_payload;
            MultiplayerTrade_ApplyRemoteAction(envelope->player_id, tradeAction);
        }
        break;
    case NET_PACKET_RESYNC_REQUEST:
        sSession.health_state = MULTIPLAYER_HEALTH_RESYNCING;
        break;
    case NET_PACKET_DISCONNECT_REASON:
        AbortLocalSubsessionsForDisconnect();
        sSession.health_state = MULTIPLAYER_HEALTH_DISCONNECTED;
        break;
    case NET_PACKET_SERVER_PROFILE_BEGIN:
        if (packet_payload_size == sizeof(struct NetServerProfileBegin))
            HandleServerProfileBegin((const struct NetServerProfileBegin *)packet_payload);
        break;
    case NET_PACKET_SERVER_PROFILE_CHUNK:
        if (packet_payload_size == sizeof(struct NetServerProfileChunk))
            HandleServerProfileChunk((const struct NetServerProfileChunk *)packet_payload);
        break;
    case NET_PACKET_SERVER_PROFILE_COMMIT:
        if (packet_payload_size == sizeof(struct NetServerProfileCommit))
            HandleServerProfileCommit((const struct NetServerProfileCommit *)packet_payload);
        break;
    case NET_PACKET_SERVER_CATALOG_REQUEST:
        if (packet_payload_size == sizeof(struct NetServerCatalogRequest))
            SendModCatalog();
        break;
    case NET_PACKET_PLAYER_BATTLE_PROFILE:
        HandleBattleProfilePacket(envelope->player_id, packet_payload, packet_payload_size);
        break;
    default:
        break;
    }
}

static void ProcessInboundPackets(void)
{
    u8 packet_read_attempt;
    u8 packet_payload[NET_TRANSPORT_PACKET_PAYLOAD_SIZE];
    u16 received_payload_size;
    struct NetPacketEnvelope envelope;

    for (packet_read_attempt = 0; packet_read_attempt < NET_RELIABLE_QUEUE_SIZE; packet_read_attempt++)
    {
        received_payload_size = 0;
        if (!NetTransport_ReceivePacket(&envelope, packet_payload, sizeof(packet_payload), &received_payload_size))
            return;

        ProcessInboundPacket(&envelope, packet_payload, received_payload_size);
    }
}

static void SendProfileAck(u32 profile_hash, u8 profile_ack_result, u16 detail)
{
    struct NetServerProfileAck profile_ack_packet;

#if FEATURE_MULTIPLAYER_SMOKE_STATUS
    sLastSmokeProfileAckHash = profile_hash;
    sLastSmokeProfileAckResult = profile_ack_result;
    UpdateSmokeStatus();
#endif

    memset(&profile_ack_packet, 0, sizeof(profile_ack_packet));
    profile_ack_packet.profile_hash = profile_hash;
    profile_ack_packet.result_code = profile_ack_result;
    profile_ack_packet.detail = detail;
    NetTransport_SendPacket(NET_PACKET_SERVER_PROFILE_ACK, &profile_ack_packet, sizeof(profile_ack_packet));
}

static void HandleTransportLoss(void)
{
    if (sSession.state == MULTIPLAYER_SESSION_OFFLINE || sSession.state == MULTIPLAYER_SESSION_CONNECTING)
    {
        ResetSession();
        if (sConnectRequested)
            sSession.state = MULTIPLAYER_SESSION_CONNECTING;
        return;
    }

    if (sTransportLossFrames < NET_PLAYER_DISCONNECT_FRAMES)
        sTransportLossFrames++;

    if (sTransportLossFrames >= NET_PLAYER_DISCONNECT_FRAMES)
    {
        MultiplayerOverworld_Reset();
        AbortLocalSubsessionsForDisconnect();
        ResetSession();
        if (sConnectRequested)
            sSession.state = MULTIPLAYER_SESSION_CONNECTING;
    }
    else if (sTransportLossFrames >= NET_PLAYER_STALE_FRAMES)
    {
        MultiplayerOverworld_Reset();
        sSession.health_state = MULTIPLAYER_HEALTH_STALE;
    }
    else if (sTransportLossFrames >= NET_TRANSPORT_VIEW_LOSS_GRACE_FRAMES)
    {
        sSession.health_state = MULTIPLAYER_HEALTH_DEGRADED;
    }
}

#endif

void MultiplayerSession_Init(void)
{
#if FEATURE_MULTIPLAYER
    ResetSession();
    NetTransport_Init();
    MultiplayerCommit_Init();
    MultiplayerInteractionMenu_Init();
    MultiplayerOverworld_Init();
    MultiplayerCompanionSaveBeacon_Init();
#if FEATURE_MULTIPLAYER_AUTOCONNECT
    EngineRuntimeState_SetMultiplayerMode(OPTIONS_MULTIPLAYER_MODE_ONLINE);
    MultiplayerSession_RequestConnect();
#endif
#endif
}

void MultiplayerSession_Tick(void)
{
#if FEATURE_MULTIPLAYER
    struct NetTransportSessionView view;

    MultiplayerSession_RefreshRuntimeMode();
    if (!RuntimeAllowsOnline())
    {
#if FEATURE_MULTIPLAYER_SMOKE_STATUS
        UpdateSmokeStatus();
#endif
        return;
    }

    MultiplayerCompanionSaveBeacon_Tick(sSession.tick, sSession.state, sSession.health_state, sSession.local_player_id, sSession.player_count);

    if (!RuntimeCanPollTransport())
    {
#if FEATURE_MULTIPLAYER_SMOKE_STATUS
        UpdateSmokeStatus();
#endif
        return;
    }

    if (!sConnectRequested && sSession.state == MULTIPLAYER_SESSION_OFFLINE)
    {
#if FEATURE_MULTIPLAYER_SMOKE_STATUS
        UpdateSmokeStatus();
#endif
        return;
    }

    NetTransport_Tick();
    if (!NetTransport_ReadSessionView(&view))
    {
        HandleTransportLoss();
#if FEATURE_MULTIPLAYER_SMOKE_STATUS
        UpdateSmokeStatus();
#endif
        return;
    }

    if (!SanitizeSessionView(&view))
    {
        MultiplayerOverworld_Reset();
        sSession.state = MULTIPLAYER_SESSION_ERROR;
#if FEATURE_MULTIPLAYER_SMOKE_STATUS
        UpdateSmokeStatus();
#endif
        return;
    }

    CopyViewIntoSession(&view);
    ProcessInboundPackets();
    MultiplayerBattle_Tick(&sSession);
    MultiplayerTrade_Tick(&sSession);
    MultiplayerInteractionMenu_UpdateRemoteRequests(&sSession);
    if (!PublishClientHello())
        PublishHeartbeat();
    PublishLocalSnapshot();
    PublishLocalBattleProfile();
    MultiplayerOverworld_Tick(&sSession);
#if FEATURE_MULTIPLAYER_SMOKE_STATUS
    UpdateSmokeStatus();
#endif
#endif
}

void MultiplayerSession_OnMapLoad(void)
{
#if FEATURE_MULTIPLAYER
    if (RuntimeAllowsOnline())
        MultiplayerOverworld_OnMapLoad();
#endif
}

void MultiplayerSession_OnPlayerStep(u8 direction, u16 new_keys, u16 held_keys)
{
#if FEATURE_MULTIPLAYER
    if (!RuntimeAllowsOnline())
        return;
    MultiplayerOverworld_OnPlayerStep(direction, new_keys, held_keys);
    PublishMoveIntent(direction, new_keys, held_keys);
#endif
}

void MultiplayerSession_OnBattleStart(u32 battle_type_flags)
{
#if FEATURE_MULTIPLAYER
    if (RuntimeAllowsOnline())
        MultiplayerBattle_OnBattleStart(battle_type_flags);
#endif
}

void MultiplayerSession_OnBattleEnd(u32 battle_outcome)
{
#if FEATURE_MULTIPLAYER
    if (RuntimeAllowsOnline())
        MultiplayerBattle_OnBattleEnd(battle_outcome);
#endif
}

void MultiplayerSession_RequestConnect(void)
{
#if FEATURE_MULTIPLAYER
    if (!RuntimeAllowsOnline())
        return;
    sConnectRequested = TRUE;
    if (sSession.state == MULTIPLAYER_SESSION_OFFLINE)
        sSession.state = MULTIPLAYER_SESSION_CONNECTING;
#endif
}

void MultiplayerSession_RequestDisconnect(void)
{
#if FEATURE_MULTIPLAYER
    sConnectRequested = FALSE;
    sSession.state = MULTIPLAYER_SESSION_DISCONNECTING;
    AbortLocalSubsessionsForDisconnect();
    MultiplayerOverworld_Reset();
    ResetSession();
#endif
}

void MultiplayerSession_RefreshRuntimeMode(void)
{
#if FEATURE_MULTIPLAYER
    if (RuntimeAllowsOnline())
    {
        if (!sConnectRequested)
            MultiplayerSession_RequestConnect();
    }
    else if (sConnectRequested
          || sSession.state != MULTIPLAYER_SESSION_OFFLINE
          || MultiplayerOverworld_GetActiveRemoteAvatarCount() != 0)
    {
        MultiplayerSession_RequestDisconnect();
    }
#endif
}

u8 MultiplayerSession_GetState(void)
{
#if FEATURE_MULTIPLAYER
    return sSession.state;
#else
    return MULTIPLAYER_SESSION_OFFLINE;
#endif
}

u8 MultiplayerSession_GetLocalPlayerId(void)
{
#if FEATURE_MULTIPLAYER
    return sSession.local_player_id;
#else
    return NET_PLAYER_NONE;
#endif
}

u8 MultiplayerSession_GetPlayerCount(void)
{
#if FEATURE_MULTIPLAYER
    return sSession.player_count;
#else
    return 0;
#endif
}

const struct MultiplayerSession *MultiplayerSession_Get(void)
{
#if FEATURE_MULTIPLAYER
    return &sSession;
#else
    return NULL;
#endif
}

const struct NetPlayerBattleProfile *MultiplayerSession_GetPlayerBattleProfile(u8 player_id)
{
#if FEATURE_MULTIPLAYER
    if (!IsValidPlayerId(player_id))
        return NULL;
    if (!MultiplayerSession_IsPlayerActive(player_id))
        return NULL;
    if (!BattleProfileIsValid(&sSession.player_profiles[player_id]))
        return NULL;

    return &sSession.player_profiles[player_id];
#else
    (void)player_id;
    return NULL;
#endif
}

bool8 MultiplayerSession_IsOnline(void)
{
#if FEATURE_MULTIPLAYER
    if (!RuntimeAllowsOnline())
        return FALSE;
    if (sSession.health_state == MULTIPLAYER_HEALTH_DISCONNECTED
     || sSession.health_state == MULTIPLAYER_HEALTH_RESYNCING)
        return FALSE;

    return sSession.state == MULTIPLAYER_SESSION_LOBBY
        || sSession.state == MULTIPLAYER_SESSION_OVERWORLD_SYNC
        || sSession.state == MULTIPLAYER_SESSION_SUBSESSION;
#else
    return FALSE;
#endif
}

bool8 MultiplayerSession_IsHost(void)
{
#if FEATURE_MULTIPLAYER
    return sSession.local_player_id != NET_PLAYER_NONE && sSession.local_player_id == sSession.host_player_id;
#else
    return FALSE;
#endif
}

u32 MultiplayerSession_GetSessionEpoch(void)
{
#if FEATURE_MULTIPLAYER
    return sSession.session_epoch;
#else
    return 0;
#endif
}

u32 MultiplayerSession_GetServerClockSeconds(void)
{
#if FEATURE_MULTIPLAYER
    return sSession.server_clock_seconds;
#else
    return 0;
#endif
}

u32 MultiplayerSession_NextActionSequence(void)
{
#if FEATURE_MULTIPLAYER
    return NextActionSequence();
#else
    return 0;
#endif
}

bool8 MultiplayerSession_BuildTransactionKey(struct MultiplayerTransactionKey *transaction_key, u8 packet_type, u8 subsession_id, u32 action_sequence)
{
#if FEATURE_MULTIPLAYER
    if (transaction_key == NULL || sSession.local_player_id >= MAX_NET_PLAYERS || sSession.session_epoch == 0 || action_sequence == 0)
        return FALSE;

    MultiplayerCommit_BuildKey(transaction_key, sSession.session_epoch, sSession.local_player_id, packet_type, subsession_id, action_sequence);
    return TRUE;
#else
    (void)transaction_key;
    (void)packet_type;
    (void)subsession_id;
    (void)action_sequence;
    return FALSE;
#endif
}

bool8 MultiplayerSession_IsPlayerActive(u8 player_id)
{
#if FEATURE_MULTIPLAYER
    if (!IsValidPlayerId(player_id))
        return FALSE;

    return sSession.players[player_id].active
        && sSession.players[player_id].player_id == player_id
        && !SnapshotIsStale(&sSession.players[player_id])
        && SnapshotTickIsFresh(&sSession.players[player_id], sSession.tick);
#else
    return FALSE;
#endif
}

bool8 MultiplayerSession_IsPlayerBusy(u8 player_id)
{
#if FEATURE_MULTIPLAYER
    if (!MultiplayerSession_IsPlayerActive(player_id))
        return FALSE;

    return SnapshotInteractionBlocks(&sSession.players[player_id])
        || MultiplayerSession_IsPlayerInSubsession(player_id);
#else
    return FALSE;
#endif
}

bool8 MultiplayerSession_IsPlayerInteractionBlocked(u8 player_id)
{
#if FEATURE_MULTIPLAYER
    if (!IsValidPlayerId(player_id))
        return FALSE;
    if (sSession.players[player_id].active && SnapshotIsStale(&sSession.players[player_id]))
        return TRUE;
    if (!MultiplayerSession_IsPlayerActive(player_id))
        return FALSE;

    return MultiplayerSession_IsPlayerBusy(player_id)
        || BarrierIncludesPlayer(&sSession.interaction_barrier, player_id);
#else
    return FALSE;
#endif
}

bool8 MultiplayerSession_IsPlayerInSubsession(u8 player_id)
{
#if FEATURE_MULTIPLAYER
    u8 subsession_index;

    for (subsession_index = 0; subsession_index < MAX_NET_SUBSESSIONS; subsession_index++)
    {
        if (BlockingSubsessionIncludesPlayer(&sSession.subsessions[subsession_index], player_id))
            return TRUE;
    }
#endif
    return FALSE;
}

bool8 MultiplayerSession_ArePlayersOnSameMap(u8 player_count, const u8 *player_ids)
{
#if FEATURE_MULTIPLAYER
    u8 player_list_index;
    u8 reference_map_group;
    u8 reference_map_number;

    if (player_count == 0 || player_count > MAX_NET_PLAYERS || player_ids == NULL)
        return FALSE;
    if (!MultiplayerSession_IsPlayerActive(player_ids[0]))
        return FALSE;

    reference_map_group = sSession.players[player_ids[0]].map_group;
    reference_map_number = sSession.players[player_ids[0]].map_num;
    for (player_list_index = 1; player_list_index < player_count; player_list_index++)
    {
        if (!MultiplayerSession_IsPlayerActive(player_ids[player_list_index]))
            return FALSE;
        if (sSession.players[player_ids[player_list_index]].map_group != reference_map_group || sSession.players[player_ids[player_list_index]].map_num != reference_map_number)
            return FALSE;
    }

    return TRUE;
#else
    return FALSE;
#endif
}

bool8 MultiplayerSession_ArePlayersWithinRange(u8 player_count, const u8 *player_ids, u16 max_distance)
{
#if FEATURE_MULTIPLAYER
    u8 player_list_index;
    s32 origin_x;
    s32 origin_y;

    if (!MultiplayerSession_ArePlayersOnSameMap(player_count, player_ids))
        return FALSE;

    origin_x = sSession.players[player_ids[0]].x;
    origin_y = sSession.players[player_ids[0]].y;
    for (player_list_index = 1; player_list_index < player_count; player_list_index++)
    {
        s32 x_distance_from_origin = (s32)sSession.players[player_ids[player_list_index]].x - origin_x;
        s32 y_distance_from_origin = (s32)sSession.players[player_ids[player_list_index]].y - origin_y;

        if (x_distance_from_origin < 0)
            x_distance_from_origin = -x_distance_from_origin;
        if (y_distance_from_origin < 0)
            y_distance_from_origin = -y_distance_from_origin;
        if (x_distance_from_origin + y_distance_from_origin > max_distance)
            return FALSE;
    }

    return TRUE;
#else
    return FALSE;
#endif
}

bool8 MultiplayerSession_StartInteractionBarrier(u8 barrier_type, u8 player_count, const u8 *player_ids)
{
#if FEATURE_MULTIPLAYER
    u8 player_list_index;
    u8 duplicate_check_index;
    u8 target_player_id = NET_PLAYER_NONE;
    u8 barrier_map_group;
    u8 barrier_map_number;

    if (!BarrierTypeIsValid(barrier_type))
        return FALSE;
    if (player_count == 0 || player_count > MAX_NET_BATTLE_PLAYERS || player_ids == NULL)
        return FALSE;
    if (sSession.local_player_id == NET_PLAYER_NONE || !MultiplayerSession_IsOnline())
        return FALSE;
    if (sSession.health_state != MULTIPLAYER_HEALTH_HEALTHY)
        return FALSE;
    if (sSession.interaction_barrier.active)
        return FALSE;
    if (!MultiplayerSession_ArePlayersOnSameMap(player_count, player_ids))
        return FALSE;

    barrier_map_group = sSession.players[player_ids[0]].map_group;
    barrier_map_number = sSession.players[player_ids[0]].map_num;
    for (player_list_index = 0; player_list_index < player_count; player_list_index++)
    {
        if (PlayerBlocksBarrierStart(barrier_type, player_ids[player_list_index]))
            return FALSE;
        if (player_ids[player_list_index] != sSession.local_player_id && target_player_id == NET_PLAYER_NONE)
            target_player_id = player_ids[player_list_index];

        for (duplicate_check_index = player_list_index + 1; duplicate_check_index < player_count; duplicate_check_index++)
        {
            if (player_ids[player_list_index] == player_ids[duplicate_check_index])
                return FALSE;
        }
    }

    memset(&sSession.interaction_barrier, 0, sizeof(sSession.interaction_barrier));
    sSession.interaction_barrier.active = TRUE;
    sSession.interaction_barrier.type = barrier_type;
    sSession.interaction_barrier.owner_player_id = sSession.local_player_id;
    sSession.interaction_barrier.player_count = player_count;
    sSession.interaction_barrier.map_group = barrier_map_group;
    sSession.interaction_barrier.map_num = barrier_map_number;
    sSession.interaction_barrier.timeout_frames = NET_INTERACTION_BARRIER_TIMEOUT_FRAMES;
    memcpy(sSession.interaction_barrier.players, player_ids, player_count);
    if (barrier_type == MULTIPLAYER_BARRIER_SCRIPT || barrier_type == MULTIPLAYER_BARRIER_WARP)
        PublishInteractIntent(barrier_type, target_player_id, barrier_map_group, barrier_map_number);
    return TRUE;
#else
    return FALSE;
#endif
}

void MultiplayerSession_ClearInteractionBarrier(u8 barrier_type)
{
#if FEATURE_MULTIPLAYER
    if (!sSession.interaction_barrier.active)
        return;
    if (barrier_type != MULTIPLAYER_BARRIER_NONE && sSession.interaction_barrier.type != barrier_type)
        return;

    ResetInteractionBarrier();
#endif
}

bool8 MultiplayerSession_StartSubsession(u8 subsession_type, u8 player_count, const u8 *player_ids)
{
#if FEATURE_MULTIPLAYER
    u8 player_list_index;
    u8 duplicate_check_index;
    u8 subsession_index;
    u8 max_players_for_subsession;
    u8 barrier_type;

    max_players_for_subsession = GetMaxPlayersForSubsession(subsession_type);
    if (max_players_for_subsession == 0 || player_count == 0 || player_count > max_players_for_subsession || player_ids == NULL)
        return FALSE;
    if (!MultiplayerSession_ArePlayersOnSameMap(player_count, player_ids))
        return FALSE;

    for (player_list_index = 0; player_list_index < player_count; player_list_index++)
    {
        if (PlayerBlocksSubsessionStart(player_ids[player_list_index]))
            return FALSE;

        for (duplicate_check_index = player_list_index + 1; duplicate_check_index < player_count; duplicate_check_index++)
        {
            if (player_ids[player_list_index] == player_ids[duplicate_check_index])
                return FALSE;
        }
    }

    barrier_type = GetBarrierTypeForSubsession(subsession_type);
    if (!MultiplayerSession_StartInteractionBarrier(barrier_type, player_count, player_ids))
        return FALSE;

    for (subsession_index = 0; subsession_index < MAX_NET_SUBSESSIONS; subsession_index++)
    {
        if (!sSession.subsessions[subsession_index].active)
        {
            memset(&sSession.subsessions[subsession_index], 0, sizeof(sSession.subsessions[subsession_index]));
            sSession.subsessions[subsession_index].active = TRUE;
            sSession.subsessions[subsession_index].subsession_id = subsession_index + 1;
            sSession.subsessions[subsession_index].type = subsession_type;
            sSession.subsessions[subsession_index].state = MULTIPLAYER_SUBSESSION_STATE_INVITING;
            sSession.subsessions[subsession_index].host_player_id = sSession.local_player_id;
            sSession.subsessions[subsession_index].player_count = player_count;
            sSession.subsessions[subsession_index].timeout_frames = NET_SUBSESSION_TIMEOUT_FRAMES;
            memcpy(sSession.subsessions[subsession_index].players, player_ids, player_count);
            if (!NetTransport_SendPacket(NET_PACKET_SUBSESSION_INVITE, &sSession.subsessions[subsession_index], sizeof(sSession.subsessions[subsession_index])))
            {
                memset(&sSession.subsessions[subsession_index], 0, sizeof(sSession.subsessions[subsession_index]));
                MultiplayerSession_ClearInteractionBarrier(barrier_type);
                return FALSE;
            }
            return TRUE;
        }
    }

    MultiplayerSession_ClearInteractionBarrier(barrier_type);
#endif
    return FALSE;
}

void MultiplayerSession_EndSubsession(u8 subsession_id, u8 final_state)
{
#if FEATURE_MULTIPLAYER
    u8 subsession_index;
    u8 packet_type;

    if (subsession_id == NET_SUBSESSION_NONE)
        return;
    if (!SubsessionStateIsTerminal(final_state))
        final_state = MULTIPLAYER_SUBSESSION_STATE_ERROR;

    for (subsession_index = 0; subsession_index < MAX_NET_SUBSESSIONS; subsession_index++)
    {
        if (sSession.subsessions[subsession_index].active && sSession.subsessions[subsession_index].subsession_id == subsession_id)
        {
            if (sSession.subsessions[subsession_index].type == MULTIPLAYER_SUBSESSION_TRADE
             && final_state == MULTIPLAYER_SUBSESSION_STATE_ENDING)
                final_state = MULTIPLAYER_SUBSESSION_STATE_ERROR;
            packet_type = (final_state == MULTIPLAYER_SUBSESSION_STATE_ENDING) ? NET_PACKET_SUBSESSION_COMMIT : NET_PACKET_SUBSESSION_ABORT;
            sSession.subsessions[subsession_index].state = final_state;
            NetTransport_SendPacket(packet_type, &sSession.subsessions[subsession_index], sizeof(sSession.subsessions[subsession_index]));
            memset(&sSession.subsessions[subsession_index], 0, sizeof(sSession.subsessions[subsession_index]));
            MultiplayerSession_ClearInteractionBarrier(MULTIPLAYER_BARRIER_NONE);
            return;
        }
    }
#endif
}
