#include "global.h"
#include "constants/characters.h"
#include "event_object_movement.h"
#include "generated/mod_registry.h"
#include "global.fieldmap.h"
#include "malloc.h"
#include "multiplayer/constants.h"
#include "mod/badge.h"
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
    char badgeEffectKeys[MOD_RUNTIME_PROFILE_MAX_BADGE_EFFECTS][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    struct ModBadgeEffectDefinition badgeEffects[MOD_RUNTIME_PROFILE_MAX_BADGE_EFFECTS];
    u16 badgeEffectCount;
    char fishingActionKeys[MOD_RUNTIME_PROFILE_MAX_FISHING_ACTIONS][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char fishingActionHookKeys[MOD_RUNTIME_PROFILE_MAX_FISHING_ACTIONS][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char fishingActionPromptKeys[MOD_RUNTIME_PROFILE_MAX_FISHING_ACTIONS][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    struct FishingActionDefinition fishingActions[MOD_RUNTIME_PROFILE_MAX_FISHING_ACTIONS];
    u16 fishingActionCount;
    char encounterKeys[MOD_RUNTIME_PROFILE_MAX_ENCOUNTERS][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char encounterHookKeys[MOD_RUNTIME_PROFILE_MAX_ENCOUNTERS][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    struct ModEncounterDefinition encounters[MOD_RUNTIME_PROFILE_MAX_ENCOUNTERS];
    u16 encounterCount;
    char shopKeys[MOD_RUNTIME_PROFILE_MAX_SHOPS][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    struct ModShopDefinition shops[MOD_RUNTIME_PROFILE_MAX_SHOPS];
    u16 shopCount;
    char itemKeys[MOD_RUNTIME_PROFILE_MAX_ITEMS][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char itemNameKeys[MOD_RUNTIME_PROFILE_MAX_ITEMS][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char itemDescriptionKeys[MOD_RUNTIME_PROFILE_MAX_ITEMS][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char itemFieldHookKeys[MOD_RUNTIME_PROFILE_MAX_ITEMS][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char itemBattleHookKeys[MOD_RUNTIME_PROFILE_MAX_ITEMS][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    struct ModItemDefinition items[MOD_RUNTIME_PROFILE_MAX_ITEMS];
    u16 itemCount;
    char rewardKeys[MOD_RUNTIME_PROFILE_MAX_REWARDS][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char rewardHookKeys[MOD_RUNTIME_PROFILE_MAX_REWARDS][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    struct ModRewardDefinition rewards[MOD_RUNTIME_PROFILE_MAX_REWARDS];
    u16 rewardCount;
    char pokemonDataKeys[MOD_RUNTIME_PROFILE_MAX_POKEMON_DATA][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    struct ModPokemonDataDefinition pokemonData[MOD_RUNTIME_PROFILE_MAX_POKEMON_DATA];
    u16 pokemonDataCount;
    char battleMoveKeys[MOD_RUNTIME_PROFILE_MAX_BATTLE_MOVES][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    struct ModBattleMoveDefinition battleMoves[MOD_RUNTIME_PROFILE_MAX_BATTLE_MOVES];
    u16 battleMoveCount;
    char trainerKeys[MOD_RUNTIME_PROFILE_MAX_TRAINERS][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    struct ModTrainerDefinition trainers[MOD_RUNTIME_PROFILE_MAX_TRAINERS];
    u16 trainerCount;
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

static bool8 RuntimeBadgeEffectKeyExists(const char *key)
{
    u16 i;

    if (key == NULL)
        return FALSE;

    for (i = 0; i < sProfile->badgeEffectCount; i++)
    {
        if (strcmp(sProfile->badgeEffectKeys[i], key) == 0)
            return TRUE;
    }

    return FALSE;
}

static u8 ParseBadgeEffectRecord(const u8 *payload, u16 size)
{
    struct ModRuntimeProfileBadgeEffectRecord record;
    struct ModBadgeEffectDefinition candidate;
    struct ModBadgeEffectDefinition *definition;
    char *key;

    if (size != sizeof(record) || sProfile->badgeEffectCount >= MOD_RUNTIME_PROFILE_MAX_BADGE_EFFECTS)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memcpy(&record, payload, sizeof(record));
    record.key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    if (record.key[0] == '\0')
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    if (RuntimeBadgeEffectKeyExists(record.key))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    candidate.key = record.key;
    candidate.badgeId = record.badgeId;
    candidate.effectKind = record.effectKind;
    candidate.target = record.target;
    candidate.percentPerLevel = record.percentPerLevel;
    candidate.maxLevel = record.maxLevel;
    candidate.flags = record.flags;
    if (!BadgeApi_IsEffectDefinitionValid(&candidate, FALSE))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    key = sProfile->badgeEffectKeys[sProfile->badgeEffectCount];
    CopyBoundedString(key, record.key, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);
    definition = &sProfile->badgeEffects[sProfile->badgeEffectCount++];
    definition->key = key;
    definition->badgeId = record.badgeId;
    definition->effectKind = record.effectKind;
    definition->target = record.target;
    definition->percentPerLevel = record.percentPerLevel;
    definition->maxLevel = record.maxLevel;
    definition->flags = record.flags;
    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

static bool8 RuntimeFishingActionKeyExists(const char *key)
{
    u16 i;

    if (key == NULL)
        return FALSE;

    for (i = 0; i < sProfile->fishingActionCount; i++)
    {
        if (strcmp(sProfile->fishingActionKeys[i], key) == 0)
            return TRUE;
    }

    return FALSE;
}

static u8 ParseFishingActionRecord(const u8 *payload, u16 size)
{
    struct ModRuntimeProfileFishingActionRecord record;
    struct FishingActionDefinition candidate;
    struct FishingActionDefinition *definition;
    FishingActionHook hook;
    u16 i;
    char *key;
    char *hookKey;
    char *promptKey;

    if (size != sizeof(record) || sProfile->fishingActionCount >= MOD_RUNTIME_PROFILE_MAX_FISHING_ACTIONS)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memcpy(&record, payload, sizeof(record));
    record.key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    record.sourceKey[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    record.hookKey[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    record.promptKey[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    if (record.key[0] == '\0' || record.sourceKey[0] == '\0' || record.hookKey[0] == '\0')
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    if (RuntimeFishingActionKeyExists(record.key))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    hook = FishingApi_FindCompiledHook(record.sourceKey, record.hookKey);
    if (hook == NULL)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memset(&candidate, 0, sizeof(candidate));
    candidate.key = record.key;
    candidate.hookKey = record.hookKey;
    candidate.rodMask = record.rodMask;
    candidate.phaseMask = record.phaseMask;
    candidate.priority = record.priority;
    candidate.flags = record.flags;
    candidate.hook = hook;
    candidate.promptKey = record.promptKey[0] == '\0' ? NULL : record.promptKey;
    candidate.buttonMask = record.buttonMask;
    candidate.timeoutFrames = record.timeoutFrames;
    candidate.successOutcome = record.successOutcome;
    candidate.failureOutcome = record.failureOutcome;
    for (i = 0; i < FISHING_ACTION_PARAM_COUNT; i++)
        candidate.params[i] = record.params[i];

    if (!FishingApi_IsDefinitionValid(&candidate, FALSE))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    key = sProfile->fishingActionKeys[sProfile->fishingActionCount];
    hookKey = sProfile->fishingActionHookKeys[sProfile->fishingActionCount];
    promptKey = sProfile->fishingActionPromptKeys[sProfile->fishingActionCount];
    CopyBoundedString(key, record.key, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);
    CopyBoundedString(hookKey, record.hookKey, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);
    CopyBoundedString(promptKey, record.promptKey, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);

    definition = &sProfile->fishingActions[sProfile->fishingActionCount++];
    *definition = candidate;
    definition->key = key;
    definition->hookKey = hookKey;
    definition->promptKey = promptKey[0] == '\0' ? NULL : promptKey;
    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

static bool8 RuntimeEncounterKeyExists(const char *key)
{
    u16 i;

    if (key == NULL)
        return FALSE;

    for (i = 0; i < sProfile->encounterCount; i++)
    {
        if (strcmp(sProfile->encounterKeys[i], key) == 0)
            return TRUE;
    }

    return FALSE;
}

static u8 ParseEncounterRecord(const u8 *payload, u16 size)
{
    struct ModRuntimeProfileEncounterRecord record;
    struct ModEncounterDefinition candidate;
    struct ModEncounterDefinition *definition;
    ModEncounterHook hook = NULL;
    char *key;
    char *hookKey;
    u8 i;

    if (size != sizeof(record) || sProfile->encounterCount >= MOD_RUNTIME_PROFILE_MAX_ENCOUNTERS)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memcpy(&record, payload, sizeof(record));
    record.key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    record.sourceKey[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    record.hookKey[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    if (record.key[0] == '\0')
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    if (RuntimeEncounterKeyExists(record.key))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    if ((record.sourceKey[0] == '\0') != (record.hookKey[0] == '\0'))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    if (record.sourceKey[0] != '\0')
    {
        hook = EncounterApi_FindCompiledHook(record.sourceKey, record.hookKey);
        if (hook == NULL)
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    }

    memset(&candidate, 0, sizeof(candidate));
    candidate.key = record.key;
    candidate.hookKey = record.hookKey[0] == '\0' ? NULL : record.hookKey;
    candidate.mapGroup = record.mapGroup;
    candidate.mapNum = record.mapNum;
    candidate.area = record.area;
    candidate.rodMask = record.rodMask;
    candidate.encounterRate = record.encounterRate;
    candidate.slotCount = record.slotCount;
    candidate.priority = record.priority;
    candidate.flags = record.flags;
    candidate.hook = hook;
    for (i = 0; i < MOD_ENCOUNTER_MAX_SLOTS; i++)
        candidate.slots[i] = record.slots[i];

    if (!EncounterApi_IsDefinitionValid(&candidate, FALSE))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    key = sProfile->encounterKeys[sProfile->encounterCount];
    hookKey = sProfile->encounterHookKeys[sProfile->encounterCount];
    CopyBoundedString(key, record.key, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);
    CopyBoundedString(hookKey, record.hookKey, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);

    definition = &sProfile->encounters[sProfile->encounterCount++];
    *definition = candidate;
    definition->key = key;
    definition->hookKey = hookKey[0] == '\0' ? NULL : hookKey;
    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

static bool8 RuntimeShopKeyExists(const char *key)
{
    u16 i;

    if (key == NULL)
        return FALSE;

    for (i = 0; i < sProfile->shopCount; i++)
    {
        if (strcmp(sProfile->shopKeys[i], key) == 0)
            return TRUE;
    }

    return FALSE;
}

static u8 ParseShopRecord(const u8 *payload, u16 size)
{
    struct ModRuntimeProfileShopRecord record;
    struct ModShopDefinition candidate;
    struct ModShopDefinition *definition;
    char *key;
    u8 i;

    if (size != sizeof(record) || sProfile->shopCount >= MOD_RUNTIME_PROFILE_MAX_SHOPS)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memcpy(&record, payload, sizeof(record));
    record.key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    if (record.key[0] == '\0')
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    if (RuntimeShopKeyExists(record.key))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memset(&candidate, 0, sizeof(candidate));
    candidate.key = record.key;
    candidate.mapGroup = record.mapGroup;
    candidate.mapNum = record.mapNum;
    candidate.martType = record.martType;
    candidate.itemCount = record.itemCount;
    candidate.priority = record.priority;
    candidate.flags = record.flags;
    for (i = 0; i < MOD_SHOP_MAX_ITEMS; i++)
        candidate.items[i] = record.items[i];

    if (!ShopApi_IsDefinitionValid(&candidate, FALSE))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    key = sProfile->shopKeys[sProfile->shopCount];
    CopyBoundedString(key, record.key, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);
    definition = &sProfile->shops[sProfile->shopCount++];
    *definition = candidate;
    definition->key = key;
    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

static bool8 RuntimeItemKeyExists(const char *key)
{
    u16 i;

    if (key == NULL)
        return FALSE;

    for (i = 0; i < sProfile->itemCount; i++)
    {
        if (strcmp(sProfile->itemKeys[i], key) == 0)
            return TRUE;
    }

    return FALSE;
}

static u8 ParseItemRecord(const u8 *payload, u16 size)
{
    struct ModRuntimeProfileItemRecord record;
    struct ModItemDefinition candidate;
    struct ModItemDefinition *definition;
    char *key;
    char *nameKey;
    char *descriptionKey;
    char *fieldHookKey;
    char *battleHookKey;
    ItemUseFunc fieldUseFunc = NULL;
    ItemUseFunc battleUseFunc = NULL;

    if (size != sizeof(record) || sProfile->itemCount >= MOD_RUNTIME_PROFILE_MAX_ITEMS)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memcpy(&record, payload, sizeof(record));
    record.key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    record.nameKey[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    record.descriptionKey[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    record.fieldUseSourceKey[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    record.fieldUseHookKey[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    record.battleUseSourceKey[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    record.battleUseHookKey[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    if (record.key[0] == '\0')
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    if (RuntimeItemKeyExists(record.key))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    if ((record.fieldUseSourceKey[0] == '\0') != (record.fieldUseHookKey[0] == '\0'))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    if ((record.battleUseSourceKey[0] == '\0') != (record.battleUseHookKey[0] == '\0'))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    if (record.fieldUseSourceKey[0] != '\0')
    {
        fieldUseFunc = ItemApi_FindCompiledFieldUseHook(record.fieldUseSourceKey, record.fieldUseHookKey);
        if (fieldUseFunc == NULL)
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    }
    if (record.battleUseSourceKey[0] != '\0')
    {
        battleUseFunc = ItemApi_FindCompiledBattleUseHook(record.battleUseSourceKey, record.battleUseHookKey);
        if (battleUseFunc == NULL)
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    }

    memset(&candidate, 0, sizeof(candidate));
    candidate.key = record.key;
    candidate.nameKey = record.nameKey[0] == '\0' ? NULL : record.nameKey;
    candidate.descriptionKey = record.descriptionKey[0] == '\0' ? NULL : record.descriptionKey;
    candidate.fieldUseHookKey = record.fieldUseHookKey[0] == '\0' ? NULL : record.fieldUseHookKey;
    candidate.battleUseHookKey = record.battleUseHookKey[0] == '\0' ? NULL : record.battleUseHookKey;
    candidate.itemId = record.itemId;
    candidate.price = record.price;
    candidate.flags = record.flags;
    candidate.holdEffect = record.holdEffect;
    candidate.holdEffectParam = record.holdEffectParam;
    candidate.importance = record.importance;
    candidate.pocket = record.pocket;
    candidate.type = record.type;
    candidate.battleUsage = record.battleUsage;
    candidate.secondaryId = record.secondaryId;
    candidate.priority = record.priority;
    candidate.fieldUseFunc = fieldUseFunc;
    candidate.battleUseFunc = battleUseFunc;
    if (!ItemApi_IsDefinitionValid(&candidate, FALSE))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    key = sProfile->itemKeys[sProfile->itemCount];
    nameKey = sProfile->itemNameKeys[sProfile->itemCount];
    descriptionKey = sProfile->itemDescriptionKeys[sProfile->itemCount];
    fieldHookKey = sProfile->itemFieldHookKeys[sProfile->itemCount];
    battleHookKey = sProfile->itemBattleHookKeys[sProfile->itemCount];
    CopyBoundedString(key, record.key, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);
    CopyBoundedString(nameKey, record.nameKey, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);
    CopyBoundedString(descriptionKey, record.descriptionKey, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);
    CopyBoundedString(fieldHookKey, record.fieldUseHookKey, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);
    CopyBoundedString(battleHookKey, record.battleUseHookKey, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);

    definition = &sProfile->items[sProfile->itemCount++];
    *definition = candidate;
    definition->key = key;
    definition->nameKey = nameKey[0] == '\0' ? NULL : nameKey;
    definition->descriptionKey = descriptionKey[0] == '\0' ? NULL : descriptionKey;
    definition->fieldUseHookKey = fieldHookKey[0] == '\0' ? NULL : fieldHookKey;
    definition->battleUseHookKey = battleHookKey[0] == '\0' ? NULL : battleHookKey;
    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

static bool8 RuntimeRewardKeyExists(const char *key)
{
    u16 i;

    if (key == NULL)
        return FALSE;

    for (i = 0; i < sProfile->rewardCount; i++)
    {
        if (strcmp(sProfile->rewardKeys[i], key) == 0)
            return TRUE;
    }

    return FALSE;
}

static u8 ParseRewardRecord(const u8 *payload, u16 size)
{
    struct ModRuntimeProfileRewardRecord record;
    struct ModRewardDefinition candidate;
    struct ModRewardDefinition *definition;
    ModRewardHook hook = NULL;
    char *key;
    char *hookKey;

    if (size != sizeof(record) || sProfile->rewardCount >= MOD_RUNTIME_PROFILE_MAX_REWARDS)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memcpy(&record, payload, sizeof(record));
    record.key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    record.sourceKey[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    record.hookKey[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    if (record.key[0] == '\0')
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    if (RuntimeRewardKeyExists(record.key))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    if ((record.sourceKey[0] == '\0') != (record.hookKey[0] == '\0'))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    if (record.sourceKey[0] != '\0')
    {
        hook = RewardApi_FindCompiledHook(record.sourceKey, record.hookKey);
        if (hook == NULL)
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    }

    memset(&candidate, 0, sizeof(candidate));
    candidate.key = record.key;
    candidate.hookKey = record.hookKey[0] == '\0' ? NULL : record.hookKey;
    candidate.source = record.source;
    candidate.minLevel = record.minLevel;
    candidate.maxLevel = record.maxLevel;
    candidate.priority = record.priority;
    candidate.flags = record.flags;
    candidate.itemId = record.itemId;
    candidate.quantity = record.quantity;
    candidate.hook = hook;
    if (!RewardApi_IsDefinitionValid(&candidate, FALSE))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    key = sProfile->rewardKeys[sProfile->rewardCount];
    hookKey = sProfile->rewardHookKeys[sProfile->rewardCount];
    CopyBoundedString(key, record.key, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);
    CopyBoundedString(hookKey, record.hookKey, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);
    definition = &sProfile->rewards[sProfile->rewardCount++];
    *definition = candidate;
    definition->key = key;
    definition->hookKey = hookKey[0] == '\0' ? NULL : hookKey;
    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

static bool8 RuntimePokemonDataKeyExists(const char *key)
{
    u16 i;

    if (key == NULL)
        return FALSE;

    for (i = 0; i < sProfile->pokemonDataCount; i++)
    {
        if (strcmp(sProfile->pokemonDataKeys[i], key) == 0)
            return TRUE;
    }

    return FALSE;
}

static u8 ParsePokemonDataRecord(const u8 *payload, u16 size)
{
    struct ModRuntimeProfilePokemonDataRecord record;
    struct ModPokemonDataDefinition candidate;
    struct ModPokemonDataDefinition *definition;
    char *key;

    if (size != sizeof(record) || sProfile->pokemonDataCount >= MOD_RUNTIME_PROFILE_MAX_POKEMON_DATA)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memcpy(&record, payload, sizeof(record));
    record.key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    if (record.key[0] == '\0')
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    if (RuntimePokemonDataKeyExists(record.key))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memset(&candidate, 0, sizeof(candidate));
    candidate.key = record.key;
    candidate.species = record.species;
    candidate.priority = record.priority;
    candidate.flags = record.flags;
    candidate.info = record.info;
    candidate.levelUpMoveCount = record.levelUpMoveCount;
    memcpy(candidate.levelUpMoves, record.levelUpMoves, sizeof(candidate.levelUpMoves));
    candidate.evolutionCount = record.evolutionCount;
    memcpy(candidate.evolutions, record.evolutions, sizeof(candidate.evolutions));
    if (!PokemonDataApi_IsDefinitionValid(&candidate, FALSE))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    key = sProfile->pokemonDataKeys[sProfile->pokemonDataCount];
    CopyBoundedString(key, record.key, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);
    definition = &sProfile->pokemonData[sProfile->pokemonDataCount++];
    *definition = candidate;
    definition->key = key;
    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

static bool8 RuntimeBattleMoveKeyExists(const char *key)
{
    u16 i;

    if (key == NULL)
        return FALSE;

    for (i = 0; i < sProfile->battleMoveCount; i++)
    {
        if (strcmp(sProfile->battleMoveKeys[i], key) == 0)
            return TRUE;
    }

    return FALSE;
}

static u8 ParseBattleMoveRecord(const u8 *payload, u16 size)
{
    struct ModRuntimeProfileBattleMoveRecord record;
    struct ModBattleMoveDefinition candidate;
    struct ModBattleMoveDefinition *definition;
    char *key;

    if (size != sizeof(record) || sProfile->battleMoveCount >= MOD_RUNTIME_PROFILE_MAX_BATTLE_MOVES)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memcpy(&record, payload, sizeof(record));
    record.key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    if (record.key[0] == '\0')
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    if (RuntimeBattleMoveKeyExists(record.key))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memset(&candidate, 0, sizeof(candidate));
    candidate.key = record.key;
    candidate.move = record.move;
    candidate.priority = record.priority;
    candidate.overrideFlags = record.overrideFlags;
    candidate.data = record.data;
    if (!BattleDataApi_IsMoveDefinitionValid(&candidate, FALSE))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    key = sProfile->battleMoveKeys[sProfile->battleMoveCount];
    CopyBoundedString(key, record.key, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);
    definition = &sProfile->battleMoves[sProfile->battleMoveCount++];
    *definition = candidate;
    definition->key = key;
    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

static bool8 RuntimeTrainerKeyExists(const char *key)
{
    u16 i;

    if (key == NULL)
        return FALSE;

    for (i = 0; i < sProfile->trainerCount; i++)
    {
        if (strcmp(sProfile->trainerKeys[i], key) == 0)
            return TRUE;
    }

    return FALSE;
}

static u8 ParseTrainerRecord(const u8 *payload, u16 size)
{
    struct ModRuntimeProfileTrainerRecord record;
    struct ModTrainerDefinition candidate;
    struct ModTrainerDefinition *definition;
    char *key;

    if (size != sizeof(record) || sProfile->trainerCount >= MOD_RUNTIME_PROFILE_MAX_TRAINERS)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memcpy(&record, payload, sizeof(record));
    record.key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    if (record.key[0] == '\0')
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    if (RuntimeTrainerKeyExists(record.key))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memset(&candidate, 0, sizeof(candidate));
    candidate.key = record.key;
    candidate.trainerId = record.trainerId;
    candidate.priority = record.priority;
    candidate.flags = record.flags;
    candidate.trainerClass = record.trainerClass;
    candidate.encounterMusicGender = record.encounterMusicGender;
    candidate.trainerPic = record.trainerPic;
    memcpy(candidate.items, record.items, sizeof(candidate.items));
    candidate.doubleBattle = record.doubleBattle;
    candidate.aiFlags = record.aiFlags;
    candidate.partySize = record.partySize;
    memcpy(candidate.party, record.party, sizeof(candidate.party));
    if (!TrainerApi_IsDefinitionValid(&candidate, FALSE))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    key = sProfile->trainerKeys[sProfile->trainerCount];
    CopyBoundedString(key, record.key, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);
    definition = &sProfile->trainers[sProfile->trainerCount++];
    *definition = candidate;
    definition->key = key;
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
    case MOD_RUNTIME_PROFILE_RECORD_BADGE_EFFECT:
        return ParseBadgeEffectRecord(payload, size);
    case MOD_RUNTIME_PROFILE_RECORD_FISHING_ACTION:
        return ParseFishingActionRecord(payload, size);
    case MOD_RUNTIME_PROFILE_RECORD_ENCOUNTER:
        return ParseEncounterRecord(payload, size);
    case MOD_RUNTIME_PROFILE_RECORD_SHOP:
        return ParseShopRecord(payload, size);
    case MOD_RUNTIME_PROFILE_RECORD_ITEM:
        return ParseItemRecord(payload, size);
    case MOD_RUNTIME_PROFILE_RECORD_REWARD:
        return ParseRewardRecord(payload, size);
    case MOD_RUNTIME_PROFILE_RECORD_POKEMON_DATA:
        return ParsePokemonDataRecord(payload, size);
    case MOD_RUNTIME_PROFILE_RECORD_BATTLE_MOVE:
        return ParseBattleMoveRecord(payload, size);
    case MOD_RUNTIME_PROFILE_RECORD_TRAINER:
        return ParseTrainerRecord(payload, size);
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
    case MOD_RUNTIME_PROFILE_RECORD_BADGE_EFFECT:
        return size == sizeof(struct ModRuntimeProfileBadgeEffectRecord) ? MOD_RUNTIME_PROFILE_RESULT_OK : MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    case MOD_RUNTIME_PROFILE_RECORD_FISHING_ACTION:
        return size == sizeof(struct ModRuntimeProfileFishingActionRecord) ? MOD_RUNTIME_PROFILE_RESULT_OK : MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    case MOD_RUNTIME_PROFILE_RECORD_ENCOUNTER:
        return size == sizeof(struct ModRuntimeProfileEncounterRecord) ? MOD_RUNTIME_PROFILE_RESULT_OK : MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    case MOD_RUNTIME_PROFILE_RECORD_SHOP:
        return size == sizeof(struct ModRuntimeProfileShopRecord) ? MOD_RUNTIME_PROFILE_RESULT_OK : MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    case MOD_RUNTIME_PROFILE_RECORD_ITEM:
        return size == sizeof(struct ModRuntimeProfileItemRecord) ? MOD_RUNTIME_PROFILE_RESULT_OK : MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    case MOD_RUNTIME_PROFILE_RECORD_REWARD:
        return size == sizeof(struct ModRuntimeProfileRewardRecord) ? MOD_RUNTIME_PROFILE_RESULT_OK : MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    case MOD_RUNTIME_PROFILE_RECORD_POKEMON_DATA:
        return size == sizeof(struct ModRuntimeProfilePokemonDataRecord) ? MOD_RUNTIME_PROFILE_RESULT_OK : MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    case MOD_RUNTIME_PROFILE_RECORD_BATTLE_MOVE:
        return size == sizeof(struct ModRuntimeProfileBattleMoveRecord) ? MOD_RUNTIME_PROFILE_RESULT_OK : MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    case MOD_RUNTIME_PROFILE_RECORD_TRAINER:
        return size == sizeof(struct ModRuntimeProfileTrainerRecord) ? MOD_RUNTIME_PROFILE_RESULT_OK : MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
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
    sProfile->badgeEffectCount = 0;
    sProfile->fishingActionCount = 0;
    sProfile->encounterCount = 0;
    sProfile->shopCount = 0;
    sProfile->itemCount = 0;
    sProfile->rewardCount = 0;
    sProfile->pokemonDataCount = 0;
    sProfile->battleMoveCount = 0;
    sProfile->trainerCount = 0;
    memset(sProfile->texts, 0, sizeof(sProfile->texts));
    memset(sProfile->npcs, 0, sizeof(sProfile->npcs));
    memset(sProfile->assets, 0, sizeof(sProfile->assets));
    memset(sProfile->badgeEffectKeys, 0, sizeof(sProfile->badgeEffectKeys));
    memset(sProfile->badgeEffects, 0, sizeof(sProfile->badgeEffects));
    memset(sProfile->fishingActionKeys, 0, sizeof(sProfile->fishingActionKeys));
    memset(sProfile->fishingActionHookKeys, 0, sizeof(sProfile->fishingActionHookKeys));
    memset(sProfile->fishingActionPromptKeys, 0, sizeof(sProfile->fishingActionPromptKeys));
    memset(sProfile->fishingActions, 0, sizeof(sProfile->fishingActions));
    memset(sProfile->encounterKeys, 0, sizeof(sProfile->encounterKeys));
    memset(sProfile->encounterHookKeys, 0, sizeof(sProfile->encounterHookKeys));
    memset(sProfile->encounters, 0, sizeof(sProfile->encounters));
    memset(sProfile->shopKeys, 0, sizeof(sProfile->shopKeys));
    memset(sProfile->shops, 0, sizeof(sProfile->shops));
    memset(sProfile->itemKeys, 0, sizeof(sProfile->itemKeys));
    memset(sProfile->itemNameKeys, 0, sizeof(sProfile->itemNameKeys));
    memset(sProfile->itemDescriptionKeys, 0, sizeof(sProfile->itemDescriptionKeys));
    memset(sProfile->itemFieldHookKeys, 0, sizeof(sProfile->itemFieldHookKeys));
    memset(sProfile->itemBattleHookKeys, 0, sizeof(sProfile->itemBattleHookKeys));
    memset(sProfile->items, 0, sizeof(sProfile->items));
    memset(sProfile->rewardKeys, 0, sizeof(sProfile->rewardKeys));
    memset(sProfile->rewardHookKeys, 0, sizeof(sProfile->rewardHookKeys));
    memset(sProfile->rewards, 0, sizeof(sProfile->rewards));
    memset(sProfile->pokemonDataKeys, 0, sizeof(sProfile->pokemonDataKeys));
    memset(sProfile->pokemonData, 0, sizeof(sProfile->pokemonData));
    memset(sProfile->battleMoveKeys, 0, sizeof(sProfile->battleMoveKeys));
    memset(sProfile->battleMoves, 0, sizeof(sProfile->battleMoves));
    memset(sProfile->trainerKeys, 0, sizeof(sProfile->trainerKeys));
    memset(sProfile->trainers, 0, sizeof(sProfile->trainers));
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

const struct ModBadgeEffectDefinition *ModRuntimeProfile_GetBadgeEffects(u16 *count)
{
    if (count != NULL)
        *count = 0;
    if (!ModRuntimeProfile_IsActive())
        return NULL;

    if (count != NULL)
        *count = sProfile->badgeEffectCount;
    return sProfile->badgeEffects;
}

const struct FishingActionDefinition *ModRuntimeProfile_GetFishingActions(u16 *count)
{
    if (count != NULL)
        *count = 0;
    if (!ModRuntimeProfile_IsActive())
        return NULL;

    if (count != NULL)
        *count = sProfile->fishingActionCount;
    return sProfile->fishingActions;
}

const struct ModEncounterDefinition *ModRuntimeProfile_GetEncounters(u16 *count)
{
    if (count != NULL)
        *count = 0;
    if (!ModRuntimeProfile_IsActive())
        return NULL;

    if (count != NULL)
        *count = sProfile->encounterCount;
    return sProfile->encounters;
}

const struct ModShopDefinition *ModRuntimeProfile_GetShops(u16 *count)
{
    if (count != NULL)
        *count = 0;
    if (!ModRuntimeProfile_IsActive())
        return NULL;

    if (count != NULL)
        *count = sProfile->shopCount;
    return sProfile->shops;
}

const struct ModItemDefinition *ModRuntimeProfile_GetItems(u16 *count)
{
    if (count != NULL)
        *count = 0;
    if (!ModRuntimeProfile_IsActive())
        return NULL;

    if (count != NULL)
        *count = sProfile->itemCount;
    return sProfile->items;
}

const struct ModRewardDefinition *ModRuntimeProfile_GetRewards(u16 *count)
{
    if (count != NULL)
        *count = 0;
    if (!ModRuntimeProfile_IsActive())
        return NULL;

    if (count != NULL)
        *count = sProfile->rewardCount;
    return sProfile->rewards;
}

const struct ModPokemonDataDefinition *ModRuntimeProfile_GetPokemonData(u16 *count)
{
    if (count != NULL)
        *count = 0;
    if (!ModRuntimeProfile_IsActive())
        return NULL;

    if (count != NULL)
        *count = sProfile->pokemonDataCount;
    return sProfile->pokemonData;
}

const struct ModBattleMoveDefinition *ModRuntimeProfile_GetBattleMoves(u16 *count)
{
    if (count != NULL)
        *count = 0;
    if (!ModRuntimeProfile_IsActive())
        return NULL;

    if (count != NULL)
        *count = sProfile->battleMoveCount;
    return sProfile->battleMoves;
}

const struct ModTrainerDefinition *ModRuntimeProfile_GetTrainers(u16 *count)
{
    if (count != NULL)
        *count = 0;
    if (!ModRuntimeProfile_IsActive())
        return NULL;

    if (count != NULL)
        *count = sProfile->trainerCount;
    return sProfile->trainers;
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
