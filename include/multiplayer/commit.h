#ifndef GUARD_MULTIPLAYER_COMMIT_H
#define GUARD_MULTIPLAYER_COMMIT_H

#include "global.h"
#include "multiplayer/protocol.h"
#include "multiplayer/types.h"

struct Pokemon;

void MultiplayerCommit_Init(void);
void MultiplayerCommit_BuildKey(struct MultiplayerTransactionKey *transaction_key, u32 session_epoch, u8 player_id, u8 packet_type, u8 subsession_id, u32 action_sequence);
u32 MultiplayerCommit_GetTransactionId(const struct MultiplayerTransactionKey *transaction_key);
u8 MultiplayerCommit_Prepare(const struct MultiplayerTransactionKey *transaction_key, u8 commit_type, const void *commit_payload, u16 commit_payload_size, struct NetCommitResult *commit_result);
u8 MultiplayerCommit_Commit(const struct MultiplayerTransactionKey *transaction_key, u8 commit_type, const void *commit_payload, u16 commit_payload_size, struct NetCommitResult *commit_result);
u8 MultiplayerCommit_Rollback(const struct MultiplayerTransactionKey *transaction_key, u8 commit_type, const void *commit_payload, u16 commit_payload_size, struct NetCommitResult *commit_result);
void MultiplayerCommit_ApplyServerResult(const struct NetCommitResult *commit_result);
bool8 MultiplayerCommit_IsFailClosedType(u8 commit_type);
void MultiplayerCommit_PayMoney(u32 amount);
void MultiplayerCommit_ReceiveMoney(u32 amount);
void MultiplayerCommit_WriteMonData(struct Pokemon *party_mon, s32 mon_data_field, const void *mon_data_value);

#endif // GUARD_MULTIPLAYER_COMMIT_H
