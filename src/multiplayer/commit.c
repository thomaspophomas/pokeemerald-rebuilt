#include "global.h"
#include "money.h"
#include "multiplayer/commit.h"
#include "multiplayer/trade.h"
#include "pokemon.h"

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

static bool8 CommitEntryIsTerminal(const struct MultiplayerCommitLogEntry *entry)
{
    return entry->state == MULTIPLAYER_COMMIT_STATE_COMMITTED
        || entry->state == MULTIPLAYER_COMMIT_STATE_ROLLED_BACK;
}

static struct MultiplayerCommitLogEntry *FindCommitEntry(const struct MultiplayerTransactionKey *transaction_key)
{
    u8 commit_log_index;

    for (commit_log_index = 0; commit_log_index < NET_COMMIT_LOG_SIZE; commit_log_index++)
    {
        if (sCommitLog[commit_log_index].active && TransactionKeysEqual(&sCommitLog[commit_log_index].key, transaction_key))
            return &sCommitLog[commit_log_index];
    }

    return NULL;
}

static struct MultiplayerCommitLogEntry *AllocCommitEntry(void)
{
    u8 commit_log_index;

    for (commit_log_index = 0; commit_log_index < NET_COMMIT_LOG_SIZE; commit_log_index++)
    {
        if (!sCommitLog[commit_log_index].active)
            return &sCommitLog[commit_log_index];
    }

    for (commit_log_index = 0; commit_log_index < NET_COMMIT_LOG_SIZE; commit_log_index++)
    {
        if (CommitEntryIsTerminal(&sCommitLog[commit_log_index]))
            return &sCommitLog[commit_log_index];
    }

    return NULL;
}

static struct MultiplayerCommitLogEntry *FindCommitEntryByTransactionId(u32 transactionId)
{
    u8 commit_log_index;

    for (commit_log_index = 0; commit_log_index < NET_COMMIT_LOG_SIZE; commit_log_index++)
    {
        if (sCommitLog[commit_log_index].active && MultiplayerCommit_GetTransactionId(&sCommitLog[commit_log_index].key) == transactionId)
            return &sCommitLog[commit_log_index];
    }

    return NULL;
}

static void FillCommitResult(const struct MultiplayerCommitLogEntry *entry, struct NetCommitResult *commit_result)
{
    if (commit_result == NULL)
        return;

    memset(commit_result, 0, sizeof(*commit_result));
    commit_result->transactionId = MultiplayerCommit_GetTransactionId(&entry->key);
    commit_result->serverRevision = entry->serverRevision;
    commit_result->payloadChecksum = entry->payloadChecksum;
    commit_result->commitType = entry->commitType;
    commit_result->result_code = entry->result_code;
    commit_result->detail = entry->detail;
}

static u8 StoreTerminalResult(struct MultiplayerCommitLogEntry *entry, u8 commit_state, u8 commit_result_code, u16 detail)
{
    entry->state = commit_state;
    entry->result_code = commit_result_code;
    entry->detail = detail;
    if (entry->serverRevision == 0)
    {
        sServerRevision++;
        if (sServerRevision == 0)
            sServerRevision++;
        entry->serverRevision = sServerRevision;
    }

    return entry->result_code;
}

#endif

void MultiplayerCommit_Init(void)
{
#if FEATURE_MULTIPLAYER
    memset(sCommitLog, 0, sizeof(sCommitLog));
    sServerRevision = 0;
#endif
}

void MultiplayerCommit_BuildKey(struct MultiplayerTransactionKey *transaction_key, u32 session_epoch, u8 player_id, u8 packet_type, u8 subsession_id, u32 action_sequence)
{
    if (transaction_key == NULL)
        return;

    memset(transaction_key, 0, sizeof(*transaction_key));
    transaction_key->sessionEpoch = session_epoch;
    transaction_key->playerId = player_id;
    transaction_key->packetType = packet_type;
    transaction_key->subsessionId = subsession_id;
    transaction_key->actionSequence = action_sequence;
}

u32 MultiplayerCommit_GetTransactionId(const struct MultiplayerTransactionKey *transaction_key)
{
    if (transaction_key == NULL || transaction_key->actionSequence == 0)
        return 0;

    return NetProtocol_MakeTransactionId(
        transaction_key->sessionEpoch,
        transaction_key->playerId,
        transaction_key->packetType,
        transaction_key->subsessionId,
        transaction_key->actionSequence);
}

bool8 MultiplayerCommit_IsFailClosedType(u8 commit_type)
{
    return commit_type == MULTIPLAYER_COMMIT_TRADE
        || commit_type == MULTIPLAYER_COMMIT_ITEM
        || commit_type == MULTIPLAYER_COMMIT_BATTLE
        || commit_type == MULTIPLAYER_COMMIT_STORY_FLAG
        || commit_type == MULTIPLAYER_COMMIT_OUTFIT
        || commit_type == MULTIPLAYER_COMMIT_WEATHER_REWARD;
}

void MultiplayerCommit_PayMoney(u32 amount)
{
#if FEATURE_MULTIPLAYER
    RemoveMoney(&gSaveBlock1Ptr->money, amount);
#else
    (void)amount;
#endif
}

void MultiplayerCommit_ReceiveMoney(u32 amount)
{
#if FEATURE_MULTIPLAYER
    AddMoney(&gSaveBlock1Ptr->money, amount);
#else
    (void)amount;
#endif
}

void MultiplayerCommit_WriteMonData(struct Pokemon *party_mon, s32 mon_data_field, const void *mon_data_value)
{
#if FEATURE_MULTIPLAYER
    SetMonData(party_mon, mon_data_field, mon_data_value);
#else
    (void)party_mon;
    (void)mon_data_field;
    (void)mon_data_value;
#endif
}

u8 MultiplayerCommit_Prepare(const struct MultiplayerTransactionKey *transaction_key, u8 commit_type, const void *commit_payload, u16 commit_payload_size, struct NetCommitResult *commit_result)
{
#if FEATURE_MULTIPLAYER
    struct MultiplayerCommitLogEntry *commit_log_entry;

    if (transaction_key == NULL || transaction_key->sessionEpoch == 0 || transaction_key->actionSequence == 0)
        return MULTIPLAYER_COMMIT_RESULT_REJECTED;

    commit_log_entry = FindCommitEntry(transaction_key);
    if (commit_log_entry != NULL)
    {
        FillCommitResult(commit_log_entry, commit_result);
        return commit_log_entry->result_code;
    }

    commit_log_entry = AllocCommitEntry();
    if (commit_log_entry == NULL)
        return MULTIPLAYER_COMMIT_RESULT_REJECTED;

    memset(commit_log_entry, 0, sizeof(*commit_log_entry));
    commit_log_entry->active = TRUE;
    commit_log_entry->state = MULTIPLAYER_COMMIT_STATE_PREPARED;
    commit_log_entry->result_code = MULTIPLAYER_COMMIT_RESULT_PENDING;
    commit_log_entry->commitType = commit_type;
    commit_log_entry->key = *transaction_key;
    commit_log_entry->payloadChecksum = NetProtocol_CalcChecksum(commit_payload, commit_payload_size);
    FillCommitResult(commit_log_entry, commit_result);
    return commit_log_entry->result_code;
#else
    (void)transaction_key;
    (void)commit_type;
    (void)commit_payload;
    (void)commit_payload_size;
    (void)commit_result;
    return MULTIPLAYER_COMMIT_RESULT_REJECTED;
#endif
}

u8 MultiplayerCommit_Commit(const struct MultiplayerTransactionKey *transaction_key, u8 commit_type, const void *commit_payload, u16 commit_payload_size, struct NetCommitResult *commit_result)
{
#if FEATURE_MULTIPLAYER
    struct MultiplayerCommitLogEntry *commit_log_entry;
    u16 checksum;

    if (transaction_key == NULL || transaction_key->sessionEpoch == 0 || transaction_key->actionSequence == 0)
        return MULTIPLAYER_COMMIT_RESULT_REJECTED;

    commit_log_entry = FindCommitEntry(transaction_key);
    if (commit_log_entry == NULL)
    {
        MultiplayerCommit_Prepare(transaction_key, commit_type, commit_payload, commit_payload_size, commit_result);
        commit_log_entry = FindCommitEntry(transaction_key);
        if (commit_log_entry == NULL)
            return MULTIPLAYER_COMMIT_RESULT_REJECTED;
    }

    if (CommitEntryIsTerminal(commit_log_entry))
    {
        FillCommitResult(commit_log_entry, commit_result);
        return commit_log_entry->result_code;
    }

    checksum = NetProtocol_CalcChecksum(commit_payload, commit_payload_size);
    if (commit_log_entry->payloadChecksum != checksum)
        StoreTerminalResult(commit_log_entry, MULTIPLAYER_COMMIT_STATE_ROLLED_BACK, MULTIPLAYER_COMMIT_RESULT_REJECTED, 1);
    else if (MultiplayerCommit_IsFailClosedType(commit_type))
        StoreTerminalResult(commit_log_entry, MULTIPLAYER_COMMIT_STATE_ROLLED_BACK, MULTIPLAYER_COMMIT_RESULT_REJECTED, 2);
    else
        StoreTerminalResult(commit_log_entry, MULTIPLAYER_COMMIT_STATE_COMMITTED, MULTIPLAYER_COMMIT_RESULT_OK, 0);

    FillCommitResult(commit_log_entry, commit_result);
    return commit_log_entry->result_code;
#else
    (void)transaction_key;
    (void)commit_type;
    (void)commit_payload;
    (void)commit_payload_size;
    (void)commit_result;
    return MULTIPLAYER_COMMIT_RESULT_REJECTED;
#endif
}

u8 MultiplayerCommit_Rollback(const struct MultiplayerTransactionKey *transaction_key, u8 commit_type, const void *commit_payload, u16 commit_payload_size, struct NetCommitResult *commit_result)
{
#if FEATURE_MULTIPLAYER
    struct MultiplayerCommitLogEntry *commit_log_entry;

    if (transaction_key == NULL || transaction_key->sessionEpoch == 0 || transaction_key->actionSequence == 0)
        return MULTIPLAYER_COMMIT_RESULT_REJECTED;

    commit_log_entry = FindCommitEntry(transaction_key);
    if (commit_log_entry == NULL)
    {
        MultiplayerCommit_Prepare(transaction_key, commit_type, commit_payload, commit_payload_size, commit_result);
        commit_log_entry = FindCommitEntry(transaction_key);
        if (commit_log_entry == NULL)
            return MULTIPLAYER_COMMIT_RESULT_REJECTED;
    }

    if (commit_log_entry->state != MULTIPLAYER_COMMIT_STATE_COMMITTED
     && commit_log_entry->state != MULTIPLAYER_COMMIT_STATE_ROLLED_BACK)
        StoreTerminalResult(commit_log_entry, MULTIPLAYER_COMMIT_STATE_ROLLED_BACK, MULTIPLAYER_COMMIT_RESULT_ROLLED_BACK, 0);

    FillCommitResult(commit_log_entry, commit_result);
    return commit_log_entry->result_code;
#else
    (void)transaction_key;
    (void)commit_type;
    (void)commit_payload;
    (void)commit_payload_size;
    (void)commit_result;
    return MULTIPLAYER_COMMIT_RESULT_REJECTED;
#endif
}

void MultiplayerCommit_ApplyServerResult(const struct NetCommitResult *commit_result)
{
#if FEATURE_MULTIPLAYER
    struct MultiplayerCommitLogEntry *commit_log_entry;

    if (commit_result == NULL || commit_result->transactionId == 0)
        return;

    commit_log_entry = FindCommitEntryByTransactionId(commit_result->transactionId);
    if (commit_log_entry == NULL)
        return;

    commit_log_entry->serverRevision = commit_result->serverRevision;
    commit_log_entry->payloadChecksum = commit_result->payloadChecksum;
    commit_log_entry->commitType = commit_result->commitType;
    commit_log_entry->result_code = commit_result->result_code;
    commit_log_entry->detail = commit_result->detail;
    if (commit_result->result_code == MULTIPLAYER_COMMIT_RESULT_OK)
        commit_log_entry->state = MULTIPLAYER_COMMIT_STATE_COMMITTED;
    else if (commit_result->result_code == MULTIPLAYER_COMMIT_RESULT_ROLLED_BACK
          || commit_result->result_code == MULTIPLAYER_COMMIT_RESULT_REJECTED)
        commit_log_entry->state = MULTIPLAYER_COMMIT_STATE_ROLLED_BACK;

    if (commit_result->commitType == MULTIPLAYER_COMMIT_TRADE)
        MultiplayerTrade_OnCommitResult(commit_result);
#else
    (void)commit_result;
#endif
}
