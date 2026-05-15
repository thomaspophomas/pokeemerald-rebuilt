#include "global.h"
#include "multiplayer/trade.h"
#include "multiplayer/session.h"

bool8 MultiplayerTrade_CanStart(const struct MultiplayerTradeRequest *request)
{
#if FEATURE_MULTIPLAYER
    if (request == NULL)
        return FALSE;
    if (!MultiplayerSession_IsOnline())
        return FALSE;
    if (request->playerA >= MAX_NET_PLAYERS || request->playerB >= MAX_NET_PLAYERS)
        return FALSE;
    if (request->playerA == request->playerB)
        return FALSE;

    return TRUE;
#else
    return FALSE;
#endif
}

bool8 MultiplayerTrade_Start(const struct MultiplayerTradeRequest *request)
{
#if FEATURE_MULTIPLAYER
    u8 players[MAX_NET_TRADE_PLAYERS];

    if (!MultiplayerTrade_CanStart(request))
        return FALSE;

    players[0] = request->playerA;
    players[1] = request->playerB;
    return MultiplayerSession_StartSubsession(MULTIPLAYER_SUBSESSION_TRADE, MAX_NET_TRADE_PLAYERS, players);
#else
    return FALSE;
#endif
}

void MultiplayerTrade_End(u8 subsessionId, bool8 committed)
{
#if FEATURE_MULTIPLAYER
    MultiplayerSession_EndSubsession(
        subsessionId,
        committed ? MULTIPLAYER_SUBSESSION_STATE_ENDING : MULTIPLAYER_SUBSESSION_STATE_ERROR);
#endif
}
