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
};

struct NetPlayerSnapshot
{
    bool8 active;
    u8 playerId;
    u8 mapGroup;
    u8 mapNum;
    s16 x;
    s16 y;
    u8 elevation;
    u8 facingDirection;
    u8 movementActionId;
    u8 avatarGraphicsId;
    u8 outfitId;
    u8 interactionState;
    u8 subsessionId;
    u8 subsessionState;
    u16 flags;
    u32 tick;
};

struct MultiplayerSubsession
{
    bool8 active;
    u8 id;
    u8 type;
    u8 state;
    u8 hostPlayerId;
    u8 playerCount;
    u8 players[MAX_NET_BATTLE_PLAYERS];
    u16 timeoutFrames;
};

struct MultiplayerSession
{
    u8 state;
    u8 localPlayerId;
    u8 hostPlayerId;
    u8 playerCount;
    u16 lastError;
    u32 sessionId;
    u32 tick;
    struct NetPlayerSnapshot players[MAX_NET_PLAYERS];
    struct MultiplayerSubsession subsessions[MAX_NET_SUBSESSIONS];
};

#endif // GUARD_MULTIPLAYER_TYPES_H
