#include "global.h"
#include "constants/characters.h"
#include "event_object_movement.h"
#include "generated/mod_registry.h"
#include "global.fieldmap.h"
#include "malloc.h"
#include "multiplayer/constants.h"
#include "mod/runtime_profile.h"

#define MOD_RUNTIME_PROFILE_MAX_CHUNKS ((MOD_RUNTIME_PROFILE_MAX_BLOB_SIZE + NET_PROFILE_CHUNK_DATA_SIZE - 1) / NET_PROFILE_CHUNK_DATA_SIZE)

struct RuntimeProfileText
{
    char language[8];
    char key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    u8 *text;
};

struct RuntimeProfileNpc
{
    struct ModRuntimeProfileNpcRecord record;
};

struct RuntimeProfileAsset
{
    char key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char sourceKey[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    struct ModSpriteAssetDefinition definition;
    struct SpriteSheet sheet;
    struct SpritePalette palette;
};

struct ModRuntimeProfileState
{
    bool8 active;
    u32 profileHash;
    u16 expectedSize;
    u16 expectedChunks;
    u16 receivedBytes;
    u8 *blob;
    u8 *textBytes;
    u8 *assetBytes;
    u16 textByteCapacity;
    u16 assetByteCapacity;
    u16 textByteCount;
    u8 chunkReceived[MOD_RUNTIME_PROFILE_MAX_CHUNKS];
    struct RuntimeProfileText texts[MOD_RUNTIME_PROFILE_MAX_TEXTS];
    u16 textCount;
    bool8 hasWeather;
    struct ModWeatherDisplay weather;
    char engineRulesetId[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    struct RuntimeProfileNpc npcs[MOD_RUNTIME_PROFILE_MAX_NPCS];
    u16 npcCount;
    struct RuntimeProfileAsset assets[MOD_RUNTIME_PROFILE_MAX_ASSETS];
    u16 assetCount;
    u16 assetByteCount;
};

static EWRAM_DATA struct ModRuntimeProfileState *sProfile = NULL;

static void CopyBoundedString(char *dest, const char *src, u16 capacity)
{
    u16 i;

    if (capacity == 0)
        return;
    if (src == NULL)
    {
        dest[0] = '\0';
        return;
    }

    for (i = 0; i < capacity - 1 && src[i] != '\0'; i++)
        dest[i] = src[i];
    dest[i] = '\0';
}

static u32 CalcProfileHash(const u8 *data, u16 size)
{
    u16 i;
    u32 hash = 2166136261U;

    for (i = 0; i < size; i++)
    {
        hash ^= data[i];
        hash *= 16777619U;
    }

    return hash;
}

static u16 AlignAssetOffset(u16 offset)
{
    return (offset + 1) & ~1;
}

static void RemoveRuntimeNpcs(void)
{
    u16 i;
    u8 localId;
    u8 mapGroup;
    u8 mapNum;

    if (sProfile == NULL || gSaveBlock1Ptr == NULL)
        return;

    mapGroup = gSaveBlock1Ptr->location.mapGroup;
    mapNum = gSaveBlock1Ptr->location.mapNum;
    for (i = 0; i < sProfile->npcCount; i++)
    {
        if (sProfile->npcs[i].record.mapGroup != mapGroup || sProfile->npcs[i].record.mapNum != mapNum)
            continue;
        localId = sProfile->npcs[i].record.localId;
        if (localId == 0)
            localId = MOD_NPC_DYNAMIC_LOCAL_ID_BASE + (i % (MOD_NPC_DYNAMIC_LOCAL_ID_END - MOD_NPC_DYNAMIC_LOCAL_ID_BASE + 1));
        RemoveObjectEventByLocalIdAndMap(localId, mapNum, mapGroup);
    }
}

static struct RuntimeProfileAsset *FindRuntimeAssetSlot(const char *key)
{
    u16 i;

    if (sProfile == NULL || key == NULL)
        return NULL;

    for (i = 0; i < sProfile->assetCount; i++)
    {
        if (strcmp(sProfile->assets[i].key, key) == 0)
            return &sProfile->assets[i];
    }

    if (sProfile->assetCount >= MOD_RUNTIME_PROFILE_MAX_ASSETS)
        return NULL;

    i = sProfile->assetCount++;
    memset(&sProfile->assets[i], 0, sizeof(sProfile->assets[i]));
    CopyBoundedString(sProfile->assets[i].key, key, sizeof(sProfile->assets[i].key));
    sProfile->assets[i].definition.key = sProfile->assets[i].key;
    sProfile->assets[i].definition.tileTag = TAG_NONE;
    sProfile->assets[i].definition.paletteTag = TAG_NONE;
    return &sProfile->assets[i];
}

static const struct ModSpriteAssetDefinition *FindGeneratedAsset(const char *key)
{
    u16 i;

    if (key == NULL)
        return NULL;

    for (i = 0; i < gModSpriteAssetCount; i++)
    {
        if (strcmp(gModSpriteAssets[i].key, key) == 0)
            return &gModSpriteAssets[i];
    }

    return NULL;
}

static u8 ParseTextRecord(const u8 *payload, u16 size)
{
    struct ModRuntimeProfileTextRecord record;
    struct RuntimeProfileText *text;
    const u8 *textBytes;

    if (size < sizeof(record) || sProfile->textCount >= MOD_RUNTIME_PROFILE_MAX_TEXTS)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memcpy(&record, payload, sizeof(record));
    if ((u16)(sizeof(record) + record.textSize) > size)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    if (sProfile->textByteCount + record.textSize + 1 > sProfile->textByteCapacity)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_SIZE;

    textBytes = payload + sizeof(record);
    text = &sProfile->texts[sProfile->textCount++];
    CopyBoundedString(text->language, record.language, sizeof(text->language));
    CopyBoundedString(text->key, record.key, sizeof(text->key));
    text->text = &sProfile->textBytes[sProfile->textByteCount];
    memcpy(text->text, textBytes, record.textSize);
    text->text[record.textSize] = EOS;
    sProfile->textByteCount += record.textSize + 1;
    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

static u8 ParseWeatherRecord(const u8 *payload, u16 size)
{
    struct ModRuntimeProfileWeatherRecord record;

    if (size != sizeof(record))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memcpy(&record, payload, sizeof(record));
    sProfile->hasWeather = TRUE;
    sProfile->weather.vanillaWeather = record.vanillaWeather;
    sProfile->weather.priority = record.priority;
    sProfile->weather.layers = record.layers;
    sProfile->weather.battleWeatherMask = record.battleWeatherMask;
    sProfile->weather.source = MOD_WEATHER_SOURCE_SERVER;
    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

static u8 ParseEngineRecord(const u8 *payload, u16 size)
{
    struct ModRuntimeProfileEngineRecord record;

    if (size != sizeof(record))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memcpy(&record, payload, sizeof(record));
    CopyBoundedString(sProfile->engineRulesetId, record.rulesetId, sizeof(sProfile->engineRulesetId));
    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

static u8 ParseNpcRecord(const u8 *payload, u16 size)
{
    if (size != sizeof(struct ModRuntimeProfileNpcRecord) || sProfile->npcCount >= MOD_RUNTIME_PROFILE_MAX_NPCS)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memcpy(&sProfile->npcs[sProfile->npcCount].record, payload, sizeof(struct ModRuntimeProfileNpcRecord));
    sProfile->npcs[sProfile->npcCount].record.key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    sProfile->npcCount++;
    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

static u8 ParseAssetRefRecord(const u8 *payload, u16 size)
{
    struct ModRuntimeProfileAssetRefRecord record;
    struct RuntimeProfileAsset *asset;

    if (size != sizeof(record))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memcpy(&record, payload, sizeof(record));
    record.key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    record.sourceKey[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    if (FindGeneratedAsset(record.sourceKey) == NULL)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    asset = FindRuntimeAssetSlot(record.key);
    if (asset == NULL)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_SIZE;

    CopyBoundedString(asset->sourceKey, record.sourceKey, sizeof(asset->sourceKey));
    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

static u8 ParseInlineSheetRecord(const u8 *payload, u16 size)
{
    struct ModRuntimeProfileInlineSheetRecord record;
    struct RuntimeProfileAsset *asset;
    const u8 *data;
    u8 *dest;
    u16 assetOffset;

    if (size < sizeof(record))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memcpy(&record, payload, sizeof(record));
    if (record.size == 0 || record.size % TILE_SIZE_4BPP != 0)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    if ((u16)(sizeof(record) + record.size) > size)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    assetOffset = AlignAssetOffset(sProfile->assetByteCount);
    if (assetOffset + record.size > sProfile->assetByteCapacity)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_SIZE;

    record.key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    asset = FindRuntimeAssetSlot(record.key);
    if (asset == NULL)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_SIZE;

    data = payload + sizeof(record);
    dest = &sProfile->assetBytes[assetOffset];
    memcpy(dest, data, record.size);
    asset->sheet.data = dest;
    asset->sheet.size = record.size;
    asset->sheet.tag = record.tileTag;
    asset->definition.sheet = &asset->sheet;
    asset->definition.tileTag = record.tileTag;
    sProfile->assetByteCount = assetOffset + record.size;
    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

static u8 ParseInlinePaletteRecord(const u8 *payload, u16 size)
{
    struct ModRuntimeProfileInlinePaletteRecord record;
    struct RuntimeProfileAsset *asset;
    const u8 *data;
    u8 *dest;
    u16 dataSize;
    u16 assetOffset;

    if (size < sizeof(record))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memcpy(&record, payload, sizeof(record));
    dataSize = record.colorCount * sizeof(u16);
    if (record.colorCount == 0 || record.colorCount > 16)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    if ((u16)(sizeof(record) + dataSize) > size)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    assetOffset = AlignAssetOffset(sProfile->assetByteCount);
    if (assetOffset + PLTT_SIZE_4BPP > sProfile->assetByteCapacity)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_SIZE;

    record.key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    asset = FindRuntimeAssetSlot(record.key);
    if (asset == NULL)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_SIZE;

    data = payload + sizeof(record);
    dest = &sProfile->assetBytes[assetOffset];
    memset(dest, 0, PLTT_SIZE_4BPP);
    memcpy(dest, data, dataSize);
    asset->palette.data = (const u16 *)dest;
    asset->palette.tag = record.paletteTag;
    asset->definition.palette = &asset->palette;
    asset->definition.paletteTag = record.paletteTag;
    sProfile->assetByteCount = assetOffset + PLTT_SIZE_4BPP;
    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

static u8 ParseRecord(u8 type, const u8 *payload, u16 size)
{
    switch (type)
    {
    case MOD_RUNTIME_PROFILE_RECORD_TEXT:
        return ParseTextRecord(payload, size);
    case MOD_RUNTIME_PROFILE_RECORD_WEATHER:
        return ParseWeatherRecord(payload, size);
    case MOD_RUNTIME_PROFILE_RECORD_ENGINE:
        return ParseEngineRecord(payload, size);
    case MOD_RUNTIME_PROFILE_RECORD_NPC:
        return ParseNpcRecord(payload, size);
    case MOD_RUNTIME_PROFILE_RECORD_ASSET_REF:
        return ParseAssetRefRecord(payload, size);
    case MOD_RUNTIME_PROFILE_RECORD_ASSET_INLINE_SHEET:
        return ParseInlineSheetRecord(payload, size);
    case MOD_RUNTIME_PROFILE_RECORD_ASSET_INLINE_PALETTE:
        return ParseInlinePaletteRecord(payload, size);
    default:
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    }
}

static u8 MeasureRecordStorage(u8 type, const u8 *payload, u16 size, u16 *textBytes, u16 *assetBytes)
{
    struct ModRuntimeProfileTextRecord textRecord;
    struct ModRuntimeProfileInlineSheetRecord sheetRecord;
    struct ModRuntimeProfileInlinePaletteRecord paletteRecord;
    u16 dataSize;
    u16 nextAssetOffset;

    switch (type)
    {
    case MOD_RUNTIME_PROFILE_RECORD_TEXT:
        if (size < sizeof(textRecord))
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
        memcpy(&textRecord, payload, sizeof(textRecord));
        if ((u16)(sizeof(textRecord) + textRecord.textSize) > size)
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
        if (*textBytes + textRecord.textSize + 1 > MOD_RUNTIME_PROFILE_MAX_TEXT_BYTES)
            return MOD_RUNTIME_PROFILE_RESULT_BAD_SIZE;
        *textBytes += textRecord.textSize + 1;
        return MOD_RUNTIME_PROFILE_RESULT_OK;
    case MOD_RUNTIME_PROFILE_RECORD_WEATHER:
        return size == sizeof(struct ModRuntimeProfileWeatherRecord) ? MOD_RUNTIME_PROFILE_RESULT_OK : MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    case MOD_RUNTIME_PROFILE_RECORD_ENGINE:
        return size == sizeof(struct ModRuntimeProfileEngineRecord) ? MOD_RUNTIME_PROFILE_RESULT_OK : MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    case MOD_RUNTIME_PROFILE_RECORD_NPC:
        return size == sizeof(struct ModRuntimeProfileNpcRecord) ? MOD_RUNTIME_PROFILE_RESULT_OK : MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    case MOD_RUNTIME_PROFILE_RECORD_ASSET_REF:
        return size == sizeof(struct ModRuntimeProfileAssetRefRecord) ? MOD_RUNTIME_PROFILE_RESULT_OK : MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    case MOD_RUNTIME_PROFILE_RECORD_ASSET_INLINE_SHEET:
        if (size < sizeof(sheetRecord))
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
        memcpy(&sheetRecord, payload, sizeof(sheetRecord));
        if (sheetRecord.size == 0 || sheetRecord.size % TILE_SIZE_4BPP != 0)
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
        if ((u16)(sizeof(sheetRecord) + sheetRecord.size) > size)
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
        nextAssetOffset = AlignAssetOffset(*assetBytes);
        if (nextAssetOffset + sheetRecord.size > MOD_RUNTIME_PROFILE_MAX_ASSET_BYTES)
            return MOD_RUNTIME_PROFILE_RESULT_BAD_SIZE;
        *assetBytes = nextAssetOffset + sheetRecord.size;
        return MOD_RUNTIME_PROFILE_RESULT_OK;
    case MOD_RUNTIME_PROFILE_RECORD_ASSET_INLINE_PALETTE:
        if (size < sizeof(paletteRecord))
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
        memcpy(&paletteRecord, payload, sizeof(paletteRecord));
        dataSize = paletteRecord.colorCount * sizeof(u16);
        if (paletteRecord.colorCount == 0 || paletteRecord.colorCount > 16)
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
        if ((u16)(sizeof(paletteRecord) + dataSize) > size)
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
        nextAssetOffset = AlignAssetOffset(*assetBytes);
        if (nextAssetOffset + PLTT_SIZE_4BPP > MOD_RUNTIME_PROFILE_MAX_ASSET_BYTES)
            return MOD_RUNTIME_PROFILE_RESULT_BAD_SIZE;
        *assetBytes = nextAssetOffset + PLTT_SIZE_4BPP;
        return MOD_RUNTIME_PROFILE_RESULT_OK;
    default:
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    }
}

static u8 PrepareProfileStorage(void)
{
    u16 offset = 0;
    u16 textBytes = 0;
    u16 assetBytes = 0;
    u8 result;
    struct ModRuntimeProfileRecordHeader header;

    while (offset < sProfile->expectedSize)
    {
        if (sProfile->expectedSize - offset < sizeof(header))
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

        memcpy(&header, &sProfile->blob[offset], sizeof(header));
        offset += sizeof(header);
        if (header.size > sProfile->expectedSize - offset)
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

        result = MeasureRecordStorage(header.type, &sProfile->blob[offset], header.size, &textBytes, &assetBytes);
        if (result != MOD_RUNTIME_PROFILE_RESULT_OK)
            return result;

        offset += header.size;
    }

    sProfile->textByteCapacity = textBytes;
    sProfile->assetByteCapacity = assetBytes;
    if (textBytes != 0)
    {
        sProfile->textBytes = AllocZeroed(textBytes);
        if (sProfile->textBytes == NULL)
            return MOD_RUNTIME_PROFILE_RESULT_OUT_OF_MEMORY;
    }
    if (assetBytes != 0)
    {
        sProfile->assetBytes = AllocZeroed(assetBytes);
        if (sProfile->assetBytes == NULL)
            return MOD_RUNTIME_PROFILE_RESULT_OUT_OF_MEMORY;
    }

    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

static u8 ParseProfile(void)
{
    u16 offset = 0;
    u8 result;
    struct ModRuntimeProfileRecordHeader header;

    sProfile->textCount = 0;
    sProfile->textByteCount = 0;
    sProfile->hasWeather = FALSE;
    sProfile->engineRulesetId[0] = '\0';
    sProfile->npcCount = 0;
    sProfile->assetCount = 0;
    sProfile->assetByteCount = 0;
    memset(sProfile->texts, 0, sizeof(sProfile->texts));
    memset(sProfile->npcs, 0, sizeof(sProfile->npcs));
    memset(sProfile->assets, 0, sizeof(sProfile->assets));
    if (sProfile->textBytes != NULL)
        memset(sProfile->textBytes, 0, sProfile->textByteCapacity);
    if (sProfile->assetBytes != NULL)
        memset(sProfile->assetBytes, 0, sProfile->assetByteCapacity);

    while (offset < sProfile->expectedSize)
    {
        if (sProfile->expectedSize - offset < sizeof(header))
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

        memcpy(&header, &sProfile->blob[offset], sizeof(header));
        offset += sizeof(header);
        if (header.size > sProfile->expectedSize - offset)
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

        result = ParseRecord(header.type, &sProfile->blob[offset], header.size);
        if (result != MOD_RUNTIME_PROFILE_RESULT_OK)
            return result;

        offset += header.size;
    }

    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

void ModRuntimeProfile_Init(void)
{
}

void ModRuntimeProfile_Clear(void)
{
    struct ModRuntimeProfileState *profile = sProfile;

    if (profile == NULL)
        return;

    RemoveRuntimeNpcs();
    TRY_FREE_AND_SET_NULL(profile->blob);
    TRY_FREE_AND_SET_NULL(profile->textBytes);
    TRY_FREE_AND_SET_NULL(profile->assetBytes);
    Free(profile);
    sProfile = NULL;
}

bool8 ModRuntimeProfile_IsActive(void)
{
    return sProfile != NULL && sProfile->active;
}

u32 ModRuntimeProfile_GetActiveHash(void)
{
    if (!ModRuntimeProfile_IsActive())
        return 0;

    return sProfile->profileHash;
}

bool8 ModRuntimeProfile_BeginReceive(u32 profileHash, u16 profileSize, u16 chunkCount)
{
    ModRuntimeProfile_Clear();

    if (profileSize == 0 || profileSize > MOD_RUNTIME_PROFILE_MAX_BLOB_SIZE)
        return FALSE;
    if (chunkCount == 0 || chunkCount > MOD_RUNTIME_PROFILE_MAX_CHUNKS)
        return FALSE;

    sProfile = AllocZeroed(sizeof(*sProfile));
    if (sProfile == NULL)
        return FALSE;

    sProfile->blob = AllocZeroed(profileSize);
    if (sProfile->blob == NULL)
    {
        ModRuntimeProfile_Clear();
        return FALSE;
    }

    sProfile->profileHash = profileHash;
    sProfile->expectedSize = profileSize;
    sProfile->expectedChunks = chunkCount;
    return TRUE;
}

bool8 ModRuntimeProfile_ReceiveChunk(u32 profileHash, u16 chunkIndex, u16 offset, const void *data, u8 size)
{
    if (sProfile == NULL || data == NULL)
        return FALSE;
    if (profileHash != sProfile->profileHash)
        return FALSE;
    if (sProfile->blob == NULL)
        return FALSE;
    if (chunkIndex >= sProfile->expectedChunks || chunkIndex >= MOD_RUNTIME_PROFILE_MAX_CHUNKS)
        return FALSE;
    if (size == 0 || size > NET_PROFILE_CHUNK_DATA_SIZE)
        return FALSE;
    if (offset > sProfile->expectedSize || size > sProfile->expectedSize - offset)
        return FALSE;

    memcpy(&sProfile->blob[offset], data, size);
    if (!sProfile->chunkReceived[chunkIndex])
    {
        sProfile->chunkReceived[chunkIndex] = TRUE;
        sProfile->receivedBytes += size;
    }
    return TRUE;
}

u8 ModRuntimeProfile_CommitReceive(u32 profileHash)
{
    u16 i;
    u8 result;

    if (sProfile == NULL || profileHash != sProfile->profileHash)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_HASH;
    if (sProfile->active && sProfile->blob == NULL)
        return MOD_RUNTIME_PROFILE_RESULT_OK;
    for (i = 0; i < sProfile->expectedChunks; i++)
    {
        if (!sProfile->chunkReceived[i])
            return MOD_RUNTIME_PROFILE_RESULT_BAD_SIZE;
    }
    if (CalcProfileHash(sProfile->blob, sProfile->expectedSize) != profileHash)
    {
        ModRuntimeProfile_Clear();
        return MOD_RUNTIME_PROFILE_RESULT_BAD_HASH;
    }

    result = PrepareProfileStorage();
    if (result != MOD_RUNTIME_PROFILE_RESULT_OK)
    {
        ModRuntimeProfile_Clear();
        return result;
    }

    result = ParseProfile();
    if (result != MOD_RUNTIME_PROFILE_RESULT_OK)
    {
        ModRuntimeProfile_Clear();
        return result;
    }

    TRY_FREE_AND_SET_NULL(sProfile->blob);
    sProfile->active = TRUE;
    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

const u8 *ModRuntimeProfile_GetText(const char *key, const char *language)
{
    u16 i;

    if (!ModRuntimeProfile_IsActive() || key == NULL || language == NULL)
        return NULL;

    for (i = 0; i < sProfile->textCount; i++)
    {
        if (strcmp(sProfile->texts[i].key, key) == 0
         && strcmp(sProfile->texts[i].language, language) == 0)
            return sProfile->texts[i].text;
    }

    return NULL;
}

bool8 ModRuntimeProfile_GetWeather(struct ModWeatherDisplay *display)
{
    if (!ModRuntimeProfile_IsActive() || display == NULL || !sProfile->hasWeather)
        return FALSE;

    *display = sProfile->weather;
    return TRUE;
}

const char *ModRuntimeProfile_GetEngineRulesetId(void)
{
    if (!ModRuntimeProfile_IsActive() || sProfile->engineRulesetId[0] == '\0')
        return NULL;

    return sProfile->engineRulesetId;
}

const struct ModNpcDefinition *ModRuntimeProfile_FindNpc(const char *key)
{
    u16 i;
    static struct ModNpcDefinition definition;

    if (!ModRuntimeProfile_IsActive() || key == NULL)
        return NULL;

    for (i = 0; i < sProfile->npcCount; i++)
    {
        if (strcmp(sProfile->npcs[i].record.key, key) == 0)
        {
            memset(&definition, 0, sizeof(definition));
            definition.key = sProfile->npcs[i].record.key;
            definition.id = i;
            definition.graphicsId = sProfile->npcs[i].record.graphicsId;
            definition.movementType = sProfile->npcs[i].record.movementType;
            definition.localId = sProfile->npcs[i].record.localId;
            definition.elevation = sProfile->npcs[i].record.elevation;
            return &definition;
        }
    }

    return NULL;
}

const struct ModSpriteAssetDefinition *ModRuntimeProfile_FindAsset(const char *key)
{
    u16 i;

    if (!ModRuntimeProfile_IsActive() || key == NULL)
        return NULL;

    for (i = 0; i < sProfile->assetCount; i++)
    {
        if (strcmp(sProfile->assets[i].key, key) != 0)
            continue;
        if (sProfile->assets[i].sourceKey[0] != '\0')
            return FindGeneratedAsset(sProfile->assets[i].sourceKey);
        return &sProfile->assets[i].definition;
    }

    return NULL;
}

void ModRuntimeProfile_OnMapLoad(void)
{
    u16 i;
    u8 localId;
    u8 objectEventId;
    const struct ModRuntimeProfileNpcRecord *record;

    if (!ModRuntimeProfile_IsActive() || gSaveBlock1Ptr == NULL)
        return;

    for (i = 0; i < sProfile->npcCount; i++)
    {
        record = &sProfile->npcs[i].record;
        if (record->mapGroup != gSaveBlock1Ptr->location.mapGroup || record->mapNum != gSaveBlock1Ptr->location.mapNum)
            continue;
        localId = record->localId;
        if (localId == 0)
            localId = MOD_NPC_DYNAMIC_LOCAL_ID_BASE + (i % (MOD_NPC_DYNAMIC_LOCAL_ID_END - MOD_NPC_DYNAMIC_LOCAL_ID_BASE + 1));
        if (TryGetObjectEventIdByLocalIdAndMap(localId, record->mapNum, record->mapGroup, &objectEventId))
            continue;
        SpawnSpecialObjectEventParameterized(record->graphicsId, record->movementType, localId, record->x, record->y, record->elevation);
    }
}
