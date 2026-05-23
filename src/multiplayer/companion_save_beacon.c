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
    u8 sessionState;
    u8 healthState;
    u8 localPlayerId;
    u8 playerCount;
    u8 active;
    u8 mapGroup;
    u8 mapNum;
    u8 elevation;
    s16 x;
    s16 y;
    u8 facingDirection;
    u8 movementActionId;
    u8 avatarGraphicsId;
    u8 reserved0;
    u16 graphicsRevision;
    u16 flags;
} __attribute__((packed));

static EWRAM_DATA u32 sBeaconSequence = 0;
static EWRAM_DATA u16 sBeaconTimer = 0;

static u16 CalcBeaconChecksum(const struct NetCompanionSaveBeacon *beacon)
{
    const u8 *bytes = (const u8 *)beacon;
    u16 checksum = 0;
    u16 i;

    for (i = 0; i < sizeof(*beacon); i++)
        checksum = (checksum << 1) ^ (checksum >> 15) ^ bytes[i];

    return checksum;
}

static u8 GetSafeLocalPlayerId(u8 localPlayerId)
{
    if (localPlayerId < MAX_NET_PLAYERS)
        return localPlayerId;

    return 0;
}

static void WriteBeacon(u32 tick, u8 sessionState, u8 healthState, u8 localPlayerId, u8 playerCount)
{
    struct NetPlayerSnapshot snapshot;
    struct NetCompanionSaveBeacon beacon;

    MultiplayerOverworld_BuildLocalSnapshot(&snapshot, GetSafeLocalPlayerId(localPlayerId), tick);
    if (!snapshot.active)
        return;

    memset(&beacon, 0, sizeof(beacon));
    beacon.magic = NET_COMPANION_SAVE_BEACON_MAGIC;
    beacon.version = NET_COMPANION_SAVE_BEACON_VERSION;
    beacon.size = sizeof(beacon);
    beacon.sequence = ++sBeaconSequence;
    beacon.tick = tick;
    beacon.sessionState = sessionState;
    beacon.healthState = healthState;
    beacon.localPlayerId = snapshot.playerId;
    beacon.playerCount = playerCount;
    beacon.active = snapshot.active;
    beacon.mapGroup = snapshot.mapGroup;
    beacon.mapNum = snapshot.mapNum;
    beacon.elevation = snapshot.elevation;
    beacon.x = snapshot.x;
    beacon.y = snapshot.y;
    beacon.facingDirection = snapshot.facingDirection;
    beacon.movementActionId = snapshot.movementActionId;
    beacon.avatarGraphicsId = snapshot.avatarGraphicsId;
    beacon.graphicsRevision = snapshot.graphicsRevision;
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

void MultiplayerCompanionSaveBeacon_Tick(u32 tick, u8 sessionState, u8 healthState, u8 localPlayerId, u8 playerCount)
{
#if FEATURE_MULTIPLAYER && FEATURE_MULTIPLAYER_COMPANION_SAVE_BEACON
    if (sBeaconTimer < NET_COMPANION_SAVE_BEACON_INTERVAL_FRAMES)
    {
        sBeaconTimer++;
        return;
    }

    sBeaconTimer = 0;
    WriteBeacon(tick, sessionState, healthState, localPlayerId, playerCount);
#else
    (void)tick;
    (void)sessionState;
    (void)healthState;
    (void)localPlayerId;
    (void)playerCount;
#endif
}
