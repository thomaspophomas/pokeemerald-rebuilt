#include "global.h"
#include "multiplayer/battle.h"
#include "multiplayer/session.h"

static bool8 RequestHasUniquePlayers(const struct MultiplayerBattleRequest *request)
{
    u8 i;
    u8 j;

    for (i = 0; i < request->playerCount; i++)
    {
        if (request->players[i] >= MAX_NET_PLAYERS)
            return FALSE;

        for (j = i + 1; j < request->playerCount; j++)
        {
            if (request->players[i] == request->players[j])
                return FALSE;
        }
    }

    return TRUE;
}

bool8 MultiplayerBattle_CanStart(const struct MultiplayerBattleRequest *request)
{
#if FEATURE_MULTIPLAYER
    if (request == NULL)
        return FALSE;
    if (!MultiplayerSession_IsOnline())
        return FALSE;
    if (!RequestHasUniquePlayers(request))
        return FALSE;

    switch (request->type)
    {
    case MULTIPLAYER_SUBSESSION_PVE_BATTLE:
        return request->playerCount > 0 && request->playerCount <= MAX_NET_PVE_PLAYERS;
    case MULTIPLAYER_SUBSESSION_PVP_BATTLE:
        return request->playerCount == MAX_NET_BATTLE_PLAYERS;
    default:
        return FALSE;
    }
#else
    return FALSE;
#endif
}

bool8 MultiplayerBattle_Start(const struct MultiplayerBattleRequest *request)
{
#if FEATURE_MULTIPLAYER
    if (!MultiplayerBattle_CanStart(request))
        return FALSE;

    return MultiplayerSession_StartSubsession(request->type, request->playerCount, request->players);
#else
    return FALSE;
#endif
}

void MultiplayerBattle_OnBattleStart(u32 battleTypeFlags)
{
}

void MultiplayerBattle_OnBattleEnd(u32 battleOutcome)
{
}
