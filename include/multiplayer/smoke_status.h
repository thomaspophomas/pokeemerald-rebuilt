#ifndef GUARD_MULTIPLAYER_SMOKE_STATUS_H
#define GUARD_MULTIPLAYER_SMOKE_STATUS_H

#include "global.h"

#define NET_SMOKE_STATUS_MAGIC 0x454B4D53 // "SMKE"
#define NET_SMOKE_STATUS_FLAG_RUNTIME_READY (1 << 0)

struct NetMultiplayerSmokeStatus
{
    u32 magic;
    u8 sessionState;
    u8 healthState;
    u8 localPlayerId;
    u8 playerCount;
    u32 sessionEpoch;
    u32 activeProfileHash;
    u32 lastProfileAckHash;
    u8 lastProfileAckResult;
    u8 flags;
    u8 reserved[2];
} __attribute__((packed));

extern volatile struct NetMultiplayerSmokeStatus gNetMultiplayerSmokeStatus;

#endif // GUARD_MULTIPLAYER_SMOKE_STATUS_H
