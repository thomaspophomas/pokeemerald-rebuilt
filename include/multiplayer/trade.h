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
bool8 MultiplayerTrade_StartItemTrade(u8 other_player_id, u32 money_amount);
bool8 MultiplayerTrade_SendAction(u8 subsession_id, u8 action, u16 party_slot, u32 trade_checksum, u32 money_amount);
void MultiplayerTrade_Tick(const struct MultiplayerSession *session);
void MultiplayerTrade_OnCommitResult(const struct NetCommitResult *commit_result);
void MultiplayerTrade_ApplyRemoteAction(u8 sender_player_id, const struct NetTradeAction *trade_action);
void MultiplayerTrade_End(u8 subsession_id, bool8 committed);

#endif // GUARD_MULTIPLAYER_TRADE_H
