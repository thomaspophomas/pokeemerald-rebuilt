#ifndef GUARD_MULTIPLAYER_TYPES_H
#define GUARD_MULTIPLAYER_TYPES_H

#include "global.h"
#include "multiplayer/constants.h"

enum MultiplayerSessionState
{
    MULTIPLAYER_SESSION_OFFLINE,
    MULTIPLAYER_SESSION_CONNECTING,
    MULTIPLAYER_SESSION_LOBBY,
    MULTIPLAYER_SESSION_OVERWORLD_SYNC,
    MULTIPLAYER_SESSION_SUBSESSION,
    MULTIPLAYER_SESSION_DISCONNECTING,
    MULTIPLAYER_SESSION_ERROR,
};

enum MultiplayerSubsessionType
{
    MULTIPLAYER_SUBSESSION_NONE,
    MULTIPLAYER_SUBSESSION_PVE_BATTLE,
    MULTIPLAYER_SUBSESSION_PVP_BATTLE,
    MULTIPLAYER_SUBSESSION_TRADE,
};

enum MultiplayerSubsessionState
{
    MULTIPLAYER_SUBSESSION_STATE_NONE,
    MULTIPLAYER_SUBSESSION_STATE_INVITING,
    MULTIPLAYER_SUBSESSION_STATE_READY,
    MULTIPLAYER_SUBSESSION_STATE_ACTIVE,
    MULTIPLAYER_SUBSESSION_STATE_ENDING,
    MULTIPLAYER_SUBSESSION_STATE_ERROR,
};

enum MultiplayerInteractionState
{
    MULTIPLAYER_INTERACTION_NONE,
    MULTIPLAYER_INTERACTION_SCRIPT,
    MULTIPLAYER_INTERACTION_WARP,
    MULTIPLAYER_INTERACTION_BATTLE,
    MULTIPLAYER_INTERACTION_TRADE,
    MULTIPLAYER_INTERACTION_OPTIONS_MENU,
};

enum MultiplayerInteractionBarrierType
{
    MULTIPLAYER_BARRIER_NONE,
    MULTIPLAYER_BARRIER_SCRIPT,
    MULTIPLAYER_BARRIER_WARP,
    MULTIPLAYER_BARRIER_BATTLE_INVITE,
    MULTIPLAYER_BARRIER_TRADE_INVITE,
};

enum MultiplayerSessionHealth
{
    MULTIPLAYER_HEALTH_DISCONNECTED,
    MULTIPLAYER_HEALTH_HEALTHY,
    MULTIPLAYER_HEALTH_DEGRADED,
    MULTIPLAYER_HEALTH_STALE,
    MULTIPLAYER_HEALTH_RESYNCING,
};

enum MultiplayerCommitState
{
    MULTIPLAYER_COMMIT_STATE_NONE,
    MULTIPLAYER_COMMIT_STATE_PREPARED,
    MULTIPLAYER_COMMIT_STATE_COMMITTED,
    MULTIPLAYER_COMMIT_STATE_ROLLED_BACK,
};

enum MultiplayerCommitType
{
    MULTIPLAYER_COMMIT_NONE,
    MULTIPLAYER_COMMIT_TRADE,
    MULTIPLAYER_COMMIT_ITEM,
    MULTIPLAYER_COMMIT_BATTLE,
    MULTIPLAYER_COMMIT_STORY_FLAG,
    MULTIPLAYER_COMMIT_OUTFIT,
    MULTIPLAYER_COMMIT_WEATHER_REWARD,
};

enum MultiplayerCommitResult
{
    MULTIPLAYER_COMMIT_RESULT_PENDING,
    MULTIPLAYER_COMMIT_RESULT_OK,
    MULTIPLAYER_COMMIT_RESULT_REJECTED,
    MULTIPLAYER_COMMIT_RESULT_ROLLED_BACK,
};

struct MultiplayerTransactionKey
{
    u32 session_epoch;
    u32 action_sequence;
    u8 player_id;
    u8 packet_type;
    u8 subsession_id;
    u8 reserved;
};

struct MultiplayerCommitLogEntry
{
    bool8 active;
    u8 state;
    u8 result_code;
    u8 commit_type;
    struct MultiplayerTransactionKey key;
    u32 server_revision;
    u16 payload_checksum;
    u16 detail;
};

struct NetPlayerSnapshot
{
    bool8 active;
    u8 player_id;
    u8 map_group;
    u8 map_num;
    s16 x;
    s16 y;
    u8 elevation;
    u8 facing_direction;
    u8 movement_action_id;
    u8 avatar_graphics_id;
    u8 outfit_id;
    u8 interaction_state;
    u8 subsession_id;
    u8 subsession_state;
    u16 graphics_revision;
    u16 flags;
    u32 tick;
    u32 server_tick_seen;
    u32 sequence;
    u32 session_epoch;
    u16 stale_frames;
    u8 anomaly_score;
};

STATIC_ASSERT(sizeof(struct NetPlayerSnapshot) <= 40, NetPlayerSnapshotHotPathSize);

struct NetPlayerBattleProfile
{
    u8 trainer_gender;
    u8 party_count;
    u8 party_levels[NET_PLAYER_PARTY_SNAPSHOT_SIZE];
    u8 reserved;
    u16 party_species[NET_PLAYER_PARTY_SNAPSHOT_SIZE];
    u16 party_held_items[NET_PLAYER_PARTY_SNAPSHOT_SIZE];
    u16 party_hp[NET_PLAYER_PARTY_SNAPSHOT_SIZE];
    u16 party_moves[NET_PLAYER_PARTY_SNAPSHOT_SIZE][MAX_MON_MOVES];
    u8 player_name[PLAYER_NAME_LENGTH + 1];
};

struct MultiplayerSubsession
{
    bool8 active;
    u8 subsession_id;
    u8 type;
    u8 state;
    u8 host_player_id;
    u8 player_count;
    u8 players[MAX_NET_BATTLE_PLAYERS];
    u16 timeout_frames;
};

struct MultiplayerInteractionBarrier
{
    bool8 active;
    u8 type;
    u8 owner_player_id;
    u8 player_count;
    u8 players[MAX_NET_BATTLE_PLAYERS];
    u8 map_group;
    u8 map_num;
    u16 timeout_frames;
};

struct MultiplayerSession
{
    u8 state;
    u8 local_player_id;
    u8 host_player_id;
    u8 player_count;
    u16 last_error;
    u32 session_id;
    u32 session_epoch;
    u32 player_token;
    u32 join_nonce;
    u32 tick;
    u32 bridge_tick;
    u32 server_clock_seconds;
    u32 local_client_frame;
    u32 local_snapshot_sequence;
    u32 local_snapshot_hot_hash;
    u32 local_action_sequence;
    u16 local_profile_publish_timer;
    u8 transport_mode;
    u8 anomaly_score;
    u8 health_state;
    u8 reserved;
    struct NetPlayerSnapshot players[MAX_NET_PLAYERS];
    struct NetPlayerBattleProfile player_profiles[MAX_NET_PLAYERS];
    struct MultiplayerSubsession subsessions[MAX_NET_SUBSESSIONS];
    struct MultiplayerInteractionBarrier interaction_barrier;
};

#endif // GUARD_MULTIPLAYER_TYPES_H
