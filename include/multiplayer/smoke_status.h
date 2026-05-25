#ifndef GUARD_MULTIPLAYER_SMOKE_STATUS_H
#define GUARD_MULTIPLAYER_SMOKE_STATUS_H

#include "global.h"

#define NET_SMOKE_STATUS_MAGIC 0x454B4D53 // "SMKE"
#define NET_SMOKE_STATUS_FLAG_RUNTIME_READY (1 << 0)

struct NetMultiplayerSmokeStatus
{
    u32 magic;
    u8 session_state;
    u8 health_state;
    u8 local_player_id;
    u8 player_count;
    u32 session_epoch;
    u32 active_profile_hash;
    u32 last_profile_ack_hash;
    u8 last_profile_ack_result;
    u8 flags;
    u8 reserved[2];
} __attribute__((packed));

extern volatile struct NetMultiplayerSmokeStatus gNetMultiplayerSmokeStatus;

#endif // GUARD_MULTIPLAYER_SMOKE_STATUS_H
