#ifndef GUARD_MULTIPLAYER_COMMIT_H
#define GUARD_MULTIPLAYER_COMMIT_H

#include "global.h"
#include "multiplayer/protocol.h"
#include "multiplayer/types.h"

void MultiplayerCommit_Init(void);
void MultiplayerCommit_BuildKey(struct MultiplayerTransactionKey *key, u32 sessionEpoch, u8 playerId, u8 packetType, u8 subsessionId, u32 actionSequence);
u32 MultiplayerCommit_GetTransactionId(const struct MultiplayerTransactionKey *key);
u8 MultiplayerCommit_Prepare(const struct MultiplayerTransactionKey *key, u8 commitType, const void *payload, u16 payloadSize, struct NetCommitResult *result);
u8 MultiplayerCommit_Commit(const struct MultiplayerTransactionKey *key, u8 commitType, const void *payload, u16 payloadSize, struct NetCommitResult *result);
u8 MultiplayerCommit_Rollback(const struct MultiplayerTransactionKey *key, u8 commitType, const void *payload, u16 payloadSize, struct NetCommitResult *result);
void MultiplayerCommit_ApplyServerResult(const struct NetCommitResult *result);
bool8 MultiplayerCommit_IsFailClosedType(u8 commitType);

#endif // GUARD_MULTIPLAYER_COMMIT_H
