#include "global.h"
#include "multiplayer/commit.h"

#if FEATURE_MULTIPLAYER

static EWRAM_DATA struct MultiplayerCommitLogEntry sCommitLog[NET_COMMIT_LOG_SIZE] = {0};
static EWRAM_DATA u32 sServerRevision = 0;

static bool8 TransactionKeysEqual(const struct MultiplayerTransactionKey *left, const struct MultiplayerTransactionKey *right)
{
    return left->sessionEpoch == right->sessionEpoch
        && left->actionSequence == right->actionSequence
        && left->playerId == right->playerId
        && left->packetType == right->packetType
        && left->subsessionId == right->subsessionId;
}

static struct MultiplayerCommitLogEntry *FindCommitEntry(const struct MultiplayerTransactionKey *key)
{
    u8 i;

    for (i = 0; i < NET_COMMIT_LOG_SIZE; i++)
    {
        if (sCommitLog[i].active && TransactionKeysEqual(&sCommitLog[i].key, key))
            return &sCommitLog[i];
    }

    return NULL;
}

static bool8 CommitEntryIsTerminal(const struct MultiplayerCommitLogEntry *entry)
{
    return entry->state == MULTIPLAYER_COMMIT_STATE_COMMITTED
        || entry->state == MULTIPLAYER_COMMIT_STATE_ROLLED_BACK;
}

static bool8 CommitEntryIsOlderThan(const struct MultiplayerCommitLogEntry *left, const struct MultiplayerCommitLogEntry *right)
{
    if (left->serverRevision == 0)
        return right->serverRevision != 0;
    if (right->serverRevision == 0)
        return FALSE;

    return left->serverRevision < right->serverRevision;
}

static struct MultiplayerCommitLogEntry *AllocCommitEntry(const struct MultiplayerTransactionKey *key)
{
    u8 i;
    struct MultiplayerCommitLogEntry *candidate = NULL;

    for (i = 0; i < NET_COMMIT_LOG_SIZE; i++)
    {
        if (!sCommitLog[i].active)
            return &sCommitLog[i];
    }

    for (i = 0; i < NET_COMMIT_LOG_SIZE; i++)
    {
        if (!CommitEntryIsTerminal(&sCommitLog[i]))
            continue;
        if (key != NULL && sCommitLog[i].key.sessionEpoch == key->sessionEpoch)
            continue;
        if (candidate == NULL || CommitEntryIsOlderThan(&sCommitLog[i], candidate))
            candidate = &sCommitLog[i];
    }

    if (candidate != NULL)
        return candidate;

    for (i = 0; i < NET_COMMIT_LOG_SIZE; i++)
    {
        if (!CommitEntryIsTerminal(&sCommitLog[i]))
            continue;
        if (candidate == NULL || CommitEntryIsOlderThan(&sCommitLog[i], candidate))
            candidate = &sCommitLog[i];
    }

    return candidate;
}

static void FillCommitResult(const struct MultiplayerCommitLogEntry *entry, struct NetCommitResult *result)
{
    if (result == NULL)
        return;

    memset(result, 0, sizeof(*result));
    result->transactionId = MultiplayerCommit_GetTransactionId(&entry->key);
    result->sessionEpoch = entry->key.sessionEpoch;
    result->actionSequence = entry->key.actionSequence;
    result->serverRevision = entry->serverRevision;
    result->payloadChecksum = entry->payloadChecksum;
    result->playerId = entry->key.playerId;
    result->packetType = entry->key.packetType;
    result->subsessionId = entry->key.subsessionId;
    result->commitType = entry->commitType;
    result->result = entry->result;
    result->detail = entry->detail;
}

static u8 StoreTerminalResult(struct MultiplayerCommitLogEntry *entry, u8 state, u8 result, u16 detail)
{
    entry->state = state;
    entry->result = result;
    entry->detail = detail;
    if (entry->serverRevision == 0)
    {
        sServerRevision++;
        if (sServerRevision == 0)
            sServerRevision++;
        entry->serverRevision = sServerRevision;
    }

    return entry->result;
}

#endif

void MultiplayerCommit_Init(void)
{
#if FEATURE_MULTIPLAYER
    memset(sCommitLog, 0, sizeof(sCommitLog));
    sServerRevision = 0;
#endif
}

void MultiplayerCommit_BuildKey(struct MultiplayerTransactionKey *key, u32 sessionEpoch, u8 playerId, u8 packetType, u8 subsessionId, u32 actionSequence)
{
    if (key == NULL)
        return;

    memset(key, 0, sizeof(*key));
    key->sessionEpoch = sessionEpoch;
    key->playerId = playerId;
    key->packetType = packetType;
    key->subsessionId = subsessionId;
    key->actionSequence = actionSequence;
}

u32 MultiplayerCommit_GetTransactionId(const struct MultiplayerTransactionKey *key)
{
    if (key == NULL || key->actionSequence == 0)
        return 0;

    return NetProtocol_MakeTransactionId(
        key->sessionEpoch,
        key->playerId,
        key->packetType,
        key->subsessionId,
        key->actionSequence);
}

bool8 MultiplayerCommit_IsFailClosedType(u8 commitType)
{
    return commitType == MULTIPLAYER_COMMIT_TRADE
        || commitType == MULTIPLAYER_COMMIT_ITEM
        || commitType == MULTIPLAYER_COMMIT_BATTLE
        || commitType == MULTIPLAYER_COMMIT_STORY_FLAG
        || commitType == MULTIPLAYER_COMMIT_OUTFIT
        || commitType == MULTIPLAYER_COMMIT_WEATHER_REWARD
        || commitType == MULTIPLAYER_COMMIT_MONEY
        || commitType == MULTIPLAYER_COMMIT_POKEMON;
}

u8 MultiplayerCommit_Prepare(const struct MultiplayerTransactionKey *key, u8 commitType, const void *payload, u16 payloadSize, struct NetCommitResult *result)
{
#if FEATURE_MULTIPLAYER
    struct MultiplayerCommitLogEntry *entry;

    if (key == NULL || key->sessionEpoch == 0 || key->actionSequence == 0)
        return MULTIPLAYER_COMMIT_RESULT_REJECTED;

    entry = FindCommitEntry(key);
    if (entry != NULL)
    {
        FillCommitResult(entry, result);
        return entry->result;
    }

    entry = AllocCommitEntry(key);
    if (entry == NULL)
        return MULTIPLAYER_COMMIT_RESULT_REJECTED;

    memset(entry, 0, sizeof(*entry));
    entry->active = TRUE;
    entry->state = MULTIPLAYER_COMMIT_STATE_PREPARED;
    entry->result = MULTIPLAYER_COMMIT_RESULT_PENDING;
    entry->commitType = commitType;
    entry->key = *key;
    entry->payloadChecksum = NetProtocol_CalcChecksum(payload, payloadSize);
    FillCommitResult(entry, result);
    return entry->result;
#else
    (void)key;
    (void)commitType;
    (void)payload;
    (void)payloadSize;
    (void)result;
    return MULTIPLAYER_COMMIT_RESULT_REJECTED;
#endif
}

u8 MultiplayerCommit_Commit(const struct MultiplayerTransactionKey *key, u8 commitType, const void *payload, u16 payloadSize, struct NetCommitResult *result)
{
#if FEATURE_MULTIPLAYER
    struct MultiplayerCommitLogEntry *entry;
    u16 checksum;

    if (key == NULL || key->sessionEpoch == 0 || key->actionSequence == 0)
        return MULTIPLAYER_COMMIT_RESULT_REJECTED;

    entry = FindCommitEntry(key);
    if (entry == NULL)
    {
        MultiplayerCommit_Prepare(key, commitType, payload, payloadSize, result);
        entry = FindCommitEntry(key);
        if (entry == NULL)
            return MULTIPLAYER_COMMIT_RESULT_REJECTED;
    }

    if (entry->state == MULTIPLAYER_COMMIT_STATE_COMMITTED
     || entry->state == MULTIPLAYER_COMMIT_STATE_ROLLED_BACK)
    {
        FillCommitResult(entry, result);
        return entry->result;
    }

    checksum = NetProtocol_CalcChecksum(payload, payloadSize);
    if (entry->payloadChecksum != checksum)
        StoreTerminalResult(entry, MULTIPLAYER_COMMIT_STATE_ROLLED_BACK, MULTIPLAYER_COMMIT_RESULT_REJECTED, 1);
    else if (MultiplayerCommit_IsFailClosedType(commitType))
        StoreTerminalResult(entry, MULTIPLAYER_COMMIT_STATE_ROLLED_BACK, MULTIPLAYER_COMMIT_RESULT_REJECTED, 2);
    else
        StoreTerminalResult(entry, MULTIPLAYER_COMMIT_STATE_COMMITTED, MULTIPLAYER_COMMIT_RESULT_OK, 0);

    FillCommitResult(entry, result);
    return entry->result;
#else
    (void)key;
    (void)commitType;
    (void)payload;
    (void)payloadSize;
    (void)result;
    return MULTIPLAYER_COMMIT_RESULT_REJECTED;
#endif
}

u8 MultiplayerCommit_Rollback(const struct MultiplayerTransactionKey *key, u8 commitType, const void *payload, u16 payloadSize, struct NetCommitResult *result)
{
#if FEATURE_MULTIPLAYER
    struct MultiplayerCommitLogEntry *entry;

    if (key == NULL || key->sessionEpoch == 0 || key->actionSequence == 0)
        return MULTIPLAYER_COMMIT_RESULT_REJECTED;

    entry = FindCommitEntry(key);
    if (entry == NULL)
    {
        MultiplayerCommit_Prepare(key, commitType, payload, payloadSize, result);
        entry = FindCommitEntry(key);
        if (entry == NULL)
            return MULTIPLAYER_COMMIT_RESULT_REJECTED;
    }

    if (entry->state != MULTIPLAYER_COMMIT_STATE_COMMITTED
     && entry->state != MULTIPLAYER_COMMIT_STATE_ROLLED_BACK)
        StoreTerminalResult(entry, MULTIPLAYER_COMMIT_STATE_ROLLED_BACK, MULTIPLAYER_COMMIT_RESULT_ROLLED_BACK, 0);

    FillCommitResult(entry, result);
    return entry->result;
#else
    (void)key;
    (void)commitType;
    (void)payload;
    (void)payloadSize;
    (void)result;
    return MULTIPLAYER_COMMIT_RESULT_REJECTED;
#endif
}

bool8 MultiplayerCommit_ApplyServerResult(const struct NetCommitResult *result)
{
#if FEATURE_MULTIPLAYER
    struct MultiplayerCommitLogEntry *entry;
    struct MultiplayerTransactionKey key;

    if (result == NULL || result->transactionId == 0 || result->sessionEpoch == 0 || result->actionSequence == 0 || result->serverRevision == 0)
        return FALSE;

    MultiplayerCommit_BuildKey(&key, result->sessionEpoch, result->playerId, result->packetType, result->subsessionId, result->actionSequence);
    entry = FindCommitEntry(&key);
    if (entry == NULL)
        return FALSE;
    if (MultiplayerCommit_GetTransactionId(&entry->key) != result->transactionId)
        return FALSE;
    if (entry->payloadChecksum != result->payloadChecksum)
        return FALSE;
    if (entry->serverRevision != 0 && result->serverRevision != 0 && result->serverRevision < entry->serverRevision)
        return FALSE;
    if (CommitEntryIsTerminal(entry) && entry->serverRevision != 0)
        return TRUE;

    entry->serverRevision = result->serverRevision;
    entry->commitType = result->commitType;
    entry->result = result->result;
    entry->detail = result->detail;
    if (result->result == MULTIPLAYER_COMMIT_RESULT_OK)
        entry->state = MULTIPLAYER_COMMIT_STATE_COMMITTED;
    else if (result->result == MULTIPLAYER_COMMIT_RESULT_ROLLED_BACK
          || result->result == MULTIPLAYER_COMMIT_RESULT_REJECTED)
        entry->state = MULTIPLAYER_COMMIT_STATE_ROLLED_BACK;
    return TRUE;
#else
    (void)result;
    return FALSE;
#endif
}
