#include "global.h"
#include "decompress.h"
#include "save.h"
#include "multiplayer/companion_save_beacon.h"
#include "multiplayer/constants.h"
#include "multiplayer/overworld.h"
#include "multiplayer/types.h"

#if FEATURE_MULTIPLAYER && FEATURE_MULTIPLAYER_COMPANION_SAVE_BEACON

#define NET_COMPANION_SAVE_BEACON_MAGIC 0x3142434E // "NCB1"
#define NET_COMPANION_SAVE_BEACON_VERSION 1
#define NET_COMPANION_SAVE_BEACON_INTERVAL_FRAMES 60
#define NET_COMPANION_SAVE_BEACON_FLAGS_RECORDED_BATTLE_SECTOR (1 << 0)

struct NetCompanionSaveBeacon
{
    u32 magic;
    u16 version;
    u16 size;
    u32 sequence;
    u32 tick;
    u16 checksum;
    u8 session_state;
    u8 health_state;
    u8 local_player_id;
    u8 player_count;
    u8 active;
    u8 map_group;
    u8 map_num;
    u8 elevation;
    s16 x;
    s16 y;
    u8 facing_direction;
    u8 movement_action_id;
    u8 avatar_graphics_id;
    u8 reserved0;
    u16 graphics_revision;
    u16 flags;
} __attribute__((packed));

static EWRAM_DATA u32 sBeaconSequence = 0;
static EWRAM_DATA u16 sBeaconTimer = 0;

static u16 CalcBeaconChecksum(const struct NetCompanionSaveBeacon *beacon)
{
    const u8 *bytes = (const u8 *)beacon;
    u16 checksum = 0;
    u16 byte_index;

    for (byte_index = 0; byte_index < sizeof(*beacon); byte_index++)
        checksum = (checksum << 1) ^ (checksum >> 15) ^ bytes[byte_index];

    return checksum;
}

static u8 GetSafeLocalPlayerId(u8 local_player_id)
{
    if (local_player_id < MAX_NET_PLAYERS)
        return local_player_id;

    return 0;
}

static void WriteBeacon(u32 tick, u8 session_state, u8 health_state, u8 local_player_id, u8 player_count)
{
    struct NetPlayerSnapshot snapshot;
    struct NetCompanionSaveBeacon beacon;

    MultiplayerOverworld_BuildLocalSnapshot(&snapshot, GetSafeLocalPlayerId(local_player_id), tick);
    if (!snapshot.active)
        return;

    memset(&beacon, 0, sizeof(beacon));
    beacon.magic = NET_COMPANION_SAVE_BEACON_MAGIC;
    beacon.version = NET_COMPANION_SAVE_BEACON_VERSION;
    beacon.size = sizeof(beacon);
    beacon.sequence = ++sBeaconSequence;
    beacon.tick = tick;
    beacon.session_state = session_state;
    beacon.health_state = health_state;
    beacon.local_player_id = snapshot.player_id;
    beacon.player_count = player_count;
    beacon.active = snapshot.active;
    beacon.map_group = snapshot.map_group;
    beacon.map_num = snapshot.map_num;
    beacon.elevation = snapshot.elevation;
    beacon.x = snapshot.x;
    beacon.y = snapshot.y;
    beacon.facing_direction = snapshot.facing_direction;
    beacon.movement_action_id = snapshot.movement_action_id;
    beacon.avatar_graphics_id = snapshot.avatar_graphics_id;
    beacon.graphics_revision = snapshot.graphics_revision;
    beacon.flags = NET_COMPANION_SAVE_BEACON_FLAGS_RECORDED_BATTLE_SECTOR;
    beacon.checksum = CalcBeaconChecksum(&beacon);

    memset(gDecompressionBuffer, 0, SECTOR_COUNTER_OFFSET);
    memcpy(gDecompressionBuffer, &beacon, sizeof(beacon));
    TryWriteSpecialSaveSector(SECTOR_ID_RECORDED_BATTLE, gDecompressionBuffer);
}

#endif

void MultiplayerCompanionSaveBeacon_Init(void)
{
#if FEATURE_MULTIPLAYER && FEATURE_MULTIPLAYER_COMPANION_SAVE_BEACON
    sBeaconSequence = 0;
    sBeaconTimer = 0;
#endif
}

void MultiplayerCompanionSaveBeacon_Tick(u32 tick, u8 session_state, u8 health_state, u8 local_player_id, u8 player_count)
{
#if FEATURE_MULTIPLAYER && FEATURE_MULTIPLAYER_COMPANION_SAVE_BEACON
    if (sBeaconTimer < NET_COMPANION_SAVE_BEACON_INTERVAL_FRAMES)
    {
        sBeaconTimer++;
        return;
    }

    sBeaconTimer = 0;
    WriteBeacon(tick, session_state, health_state, local_player_id, player_count);
#else
    (void)tick;
    (void)session_state;
    (void)health_state;
    (void)local_player_id;
    (void)player_count;
#endif
}
