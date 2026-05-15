#ifndef GUARD_MULTIPLAYER_TRADE_H
#define GUARD_MULTIPLAYER_TRADE_H

#include "global.h"

struct MultiplayerTradeRequest
{
    u8 playerA;
    u8 playerB;
};

bool8 MultiplayerTrade_CanStart(const struct MultiplayerTradeRequest *request);
bool8 MultiplayerTrade_Start(const struct MultiplayerTradeRequest *request);
void MultiplayerTrade_End(u8 subsessionId, bool8 committed);

#endif // GUARD_MULTIPLAYER_TRADE_H
