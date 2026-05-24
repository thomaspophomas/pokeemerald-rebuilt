#ifndef GUARD_MULTIPLAYER_TRADE_H
#define GUARD_MULTIPLAYER_TRADE_H

#include "global.h"
#include "multiplayer/protocol.h"

#define MULTIPLAYER_TRADE_ACTION_ITEMTRADE_OFFER 1

struct MultiplayerSession;

struct MultiplayerTradeRequest
{
    u8 playerA;
    u8 playerB;
};

bool8 MultiplayerTrade_CanStart(const struct MultiplayerTradeRequest *request);
bool8 MultiplayerTrade_Start(const struct MultiplayerTradeRequest *request);
void MultiplayerTrade_Reset(void);
bool8 MultiplayerTrade_StartItemTrade(u8 otherPlayerId, u32 moneyAmount);
bool8 MultiplayerTrade_SendAction(u8 subsessionId, u8 action, u16 partySlot, u32 tradeChecksum, u32 moneyAmount);
void MultiplayerTrade_Tick(const struct MultiplayerSession *session);
void MultiplayerTrade_OnCommitResult(const struct NetCommitResult *result);
void MultiplayerTrade_ApplyRemoteAction(u8 senderPlayerId, const struct NetTradeAction *action);
void MultiplayerTrade_End(u8 subsessionId, bool8 committed);

#endif // GUARD_MULTIPLAYER_TRADE_H
