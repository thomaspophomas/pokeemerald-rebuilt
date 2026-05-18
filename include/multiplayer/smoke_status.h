#ifndef GUARD_MULTIPLAYER_SMOKE_STATUS_H
#define GUARD_MULTIPLAYER_SMOKE_STATUS_H

#include "global.h"

#define NET_SMOKE_STATUS_MAGIC 0x454B4D53 // "SMKE"

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
    u8 reserved[3];
} __attribute__((packed));

extern volatile struct NetMultiplayerSmokeStatus gNetMultiplayerSmokeStatus;

#endif // GUARD_MULTIPLAYER_SMOKE_STATUS_H
