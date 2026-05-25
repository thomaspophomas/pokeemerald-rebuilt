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

static void CopyBoundedString(char *destination_text, const char *source_text, u16 destination_capacity)
{
    u16 char_index;

    if (destination_capacity == 0)
        return;
    if (source_text == NULL)
    {
        destination_text[0] = '\0';
        return;
    }

    for (char_index = 0; char_index < destination_capacity - 1 && source_text[char_index] != '\0'; char_index++)
        destination_text[char_index] = source_text[char_index];
    destination_text[char_index] = '\0';
}

static u32 CalcProfileHash(const u8 *profile_bytes, u16 profile_size)
{
    u16 byte_index;
    u32 hash = 2166136261U;

    for (byte_index = 0; byte_index < profile_size; byte_index++)
    {
        hash ^= profile_bytes[byte_index];
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
    u16 npc_index;
    u8 localId;
    u8 mapGroup;
    u8 mapNum;

    if (sProfile == NULL || gSaveBlock1Ptr == NULL)
        return;

    mapGroup = gSaveBlock1Ptr->location.mapGroup;
    mapNum = gSaveBlock1Ptr->location.mapNum;
    for (npc_index = 0; npc_index < sProfile->npcCount; npc_index++)
    {
        if (sProfile->npcs[npc_index].record.mapGroup != mapGroup || sProfile->npcs[npc_index].record.mapNum != mapNum)
            continue;
        localId = sProfile->npcs[npc_index].record.localId;
        if (localId == 0)
            localId = MOD_NPC_DYNAMIC_LOCAL_ID_BASE + (npc_index % (MOD_NPC_DYNAMIC_LOCAL_ID_END - MOD_NPC_DYNAMIC_LOCAL_ID_BASE + 1));
        RemoveObjectEventByLocalIdAndMap(localId, mapNum, mapGroup);
    }
}

static struct RuntimeProfileAsset *FindRuntimeAssetSlot(const char *key)
{
    u16 asset_index;

    if (sProfile == NULL || key == NULL)
        return NULL;

    for (asset_index = 0; asset_index < sProfile->assetCount; asset_index++)
    {
        if (strcmp(sProfile->assets[asset_index].key, key) == 0)
            return &sProfile->assets[asset_index];
    }

    if (sProfile->assetCount >= MOD_RUNTIME_PROFILE_MAX_ASSETS)
        return NULL;

    asset_index = sProfile->assetCount++;
    memset(&sProfile->assets[asset_index], 0, sizeof(sProfile->assets[asset_index]));
    CopyBoundedString(sProfile->assets[asset_index].key, key, sizeof(sProfile->assets[asset_index].key));
    sProfile->assets[asset_index].definition.key = sProfile->assets[asset_index].key;
    sProfile->assets[asset_index].definition.tileTag = TAG_NONE;
    sProfile->assets[asset_index].definition.paletteTag = TAG_NONE;
    return &sProfile->assets[asset_index];
}

static const struct ModSpriteAssetDefinition *FindGeneratedAsset(const char *key)
{
    u16 generated_asset_index;

    if (key == NULL)
        return NULL;

    for (generated_asset_index = 0; generated_asset_index < gModSpriteAssetCount; generated_asset_index++)
    {
        if (strcmp(gModSpriteAssets[generated_asset_index].key, key) == 0)
            return &gModSpriteAssets[generated_asset_index];
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
    const u8 *sheet_bytes_from_profile;
    u8 *sheet_storage_in_profile;
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

    sheet_bytes_from_profile = payload + sizeof(record);
    sheet_storage_in_profile = &sProfile->assetBytes[assetOffset];
    memcpy(sheet_storage_in_profile, sheet_bytes_from_profile, record.size);
    asset->sheet.data = sheet_storage_in_profile;
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
    const u8 *palette_bytes_from_profile;
    u8 *palette_storage_in_profile;
    u16 palette_byte_count_from_profile;
    u16 assetOffset;

    if (size < sizeof(record))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memcpy(&record, payload, sizeof(record));
    palette_byte_count_from_profile = record.colorCount * sizeof(u16);
    if (record.colorCount == 0 || record.colorCount > 16)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    if ((u16)(sizeof(record) + palette_byte_count_from_profile) > size)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    assetOffset = AlignAssetOffset(sProfile->assetByteCount);
    if (assetOffset + PLTT_SIZE_4BPP > sProfile->assetByteCapacity)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_SIZE;

    record.key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    asset = FindRuntimeAssetSlot(record.key);
    if (asset == NULL)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_SIZE;

    palette_bytes_from_profile = payload + sizeof(record);
    palette_storage_in_profile = &sProfile->assetBytes[assetOffset];
    memset(palette_storage_in_profile, 0, PLTT_SIZE_4BPP);
    memcpy(palette_storage_in_profile, palette_bytes_from_profile, palette_byte_count_from_profile);
    asset->palette.data = (const u16 *)palette_storage_in_profile;
    asset->palette.tag = record.paletteTag;
    asset->definition.palette = &asset->palette;
    asset->definition.paletteTag = record.paletteTag;
    sProfile->assetByteCount = assetOffset + PLTT_SIZE_4BPP;
    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

static bool8 RuntimeBadgeEffectKeyExists(const char *key)
{
    u16 badge_effect_index;

    if (key == NULL)
        return FALSE;

    for (badge_effect_index = 0; badge_effect_index < sProfile->badgeEffectCount; badge_effect_index++)
    {
        if (strcmp(sProfile->badgeEffectKeys[badge_effect_index], key) == 0)
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
    u16 fishing_action_index;

    if (key == NULL)
        return FALSE;

    for (fishing_action_index = 0; fishing_action_index < sProfile->fishingActionCount; fishing_action_index++)
    {
        if (strcmp(sProfile->fishingActionKeys[fishing_action_index], key) == 0)
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
    u16 action_param_index;
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
    for (action_param_index = 0; action_param_index < FISHING_ACTION_PARAM_COUNT; action_param_index++)
        candidate.params[action_param_index] = record.params[action_param_index];

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
    u16 encounter_index;

    if (key == NULL)
        return FALSE;

    for (encounter_index = 0; encounter_index < sProfile->encounterCount; encounter_index++)
    {
        if (strcmp(sProfile->encounterKeys[encounter_index], key) == 0)
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
    u8 encounter_slot_index;

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
    for (encounter_slot_index = 0; encounter_slot_index < MOD_ENCOUNTER_MAX_SLOTS; encounter_slot_index++)
        candidate.slots[encounter_slot_index] = record.slots[encounter_slot_index];

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
    u16 shop_index;

    if (key == NULL)
        return FALSE;

    for (shop_index = 0; shop_index < sProfile->shopCount; shop_index++)
    {
        if (strcmp(sProfile->shopKeys[shop_index], key) == 0)
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
    u8 shop_item_index;

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
    for (shop_item_index = 0; shop_item_index < MOD_SHOP_MAX_ITEMS; shop_item_index++)
        candidate.items[shop_item_index] = record.items[shop_item_index];

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
    u16 item_index;

    if (key == NULL)
        return FALSE;

    for (item_index = 0; item_index < sProfile->itemCount; item_index++)
    {
        if (strcmp(sProfile->itemKeys[item_index], key) == 0)
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
    u16 reward_index;

    if (key == NULL)
        return FALSE;

    for (reward_index = 0; reward_index < sProfile->rewardCount; reward_index++)
    {
        if (strcmp(sProfile->rewardKeys[reward_index], key) == 0)
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
    u16 pokemon_data_index;

    if (key == NULL)
        return FALSE;

    for (pokemon_data_index = 0; pokemon_data_index < sProfile->pokemonDataCount; pokemon_data_index++)
    {
        if (strcmp(sProfile->pokemonDataKeys[pokemon_data_index], key) == 0)
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
    u16 battle_move_index;

    if (key == NULL)
        return FALSE;

    for (battle_move_index = 0; battle_move_index < sProfile->battleMoveCount; battle_move_index++)
    {
        if (strcmp(sProfile->battleMoveKeys[battle_move_index], key) == 0)
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
    u16 trainer_index;

    if (key == NULL)
        return FALSE;

    for (trainer_index = 0; trainer_index < sProfile->trainerCount; trainer_index++)
    {
        if (strcmp(sProfile->trainerKeys[trainer_index], key) == 0)
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

static u8 ParseRecord(u8 runtime_record_type, const u8 *record_payload, u16 record_payload_size)
{
    switch (runtime_record_type)
    {
    case MOD_RUNTIME_PROFILE_RECORD_TEXT:
        return ParseTextRecord(record_payload, record_payload_size);
    case MOD_RUNTIME_PROFILE_RECORD_WEATHER:
        return ParseWeatherRecord(record_payload, record_payload_size);
    case MOD_RUNTIME_PROFILE_RECORD_ENGINE:
        return ParseEngineRecord(record_payload, record_payload_size);
    case MOD_RUNTIME_PROFILE_RECORD_NPC:
        return ParseNpcRecord(record_payload, record_payload_size);
    case MOD_RUNTIME_PROFILE_RECORD_ASSET_REF:
        return ParseAssetRefRecord(record_payload, record_payload_size);
    case MOD_RUNTIME_PROFILE_RECORD_ASSET_INLINE_SHEET:
        return ParseInlineSheetRecord(record_payload, record_payload_size);
    case MOD_RUNTIME_PROFILE_RECORD_ASSET_INLINE_PALETTE:
        return ParseInlinePaletteRecord(record_payload, record_payload_size);
    case MOD_RUNTIME_PROFILE_RECORD_BADGE_EFFECT:
        return ParseBadgeEffectRecord(record_payload, record_payload_size);
    case MOD_RUNTIME_PROFILE_RECORD_FISHING_ACTION:
        return ParseFishingActionRecord(record_payload, record_payload_size);
    case MOD_RUNTIME_PROFILE_RECORD_ENCOUNTER:
        return ParseEncounterRecord(record_payload, record_payload_size);
    case MOD_RUNTIME_PROFILE_RECORD_SHOP:
        return ParseShopRecord(record_payload, record_payload_size);
    case MOD_RUNTIME_PROFILE_RECORD_ITEM:
        return ParseItemRecord(record_payload, record_payload_size);
    case MOD_RUNTIME_PROFILE_RECORD_REWARD:
        return ParseRewardRecord(record_payload, record_payload_size);
    case MOD_RUNTIME_PROFILE_RECORD_POKEMON_DATA:
        return ParsePokemonDataRecord(record_payload, record_payload_size);
    case MOD_RUNTIME_PROFILE_RECORD_BATTLE_MOVE:
        return ParseBattleMoveRecord(record_payload, record_payload_size);
    case MOD_RUNTIME_PROFILE_RECORD_TRAINER:
        return ParseTrainerRecord(record_payload, record_payload_size);
    default:
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    }
}

static u8 MeasureRecordStorage(u8 runtime_record_type, const u8 *record_payload, u16 record_payload_size, u16 *required_text_bytes, u16 *required_asset_bytes)
{
    struct ModRuntimeProfileTextRecord text_record;
    struct ModRuntimeProfileInlineSheetRecord sheet_record;
    struct ModRuntimeProfileInlinePaletteRecord palette_record;
    u16 palette_byte_count;
    u16 next_asset_offset;

    switch (runtime_record_type)
    {
    case MOD_RUNTIME_PROFILE_RECORD_TEXT:
        if (record_payload_size < sizeof(text_record))
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
        memcpy(&text_record, record_payload, sizeof(text_record));
        if ((u16)(sizeof(text_record) + text_record.textSize) > record_payload_size)
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
        if (*required_text_bytes + text_record.textSize + 1 > MOD_RUNTIME_PROFILE_MAX_TEXT_BYTES)
            return MOD_RUNTIME_PROFILE_RESULT_BAD_SIZE;
        *required_text_bytes += text_record.textSize + 1;
        return MOD_RUNTIME_PROFILE_RESULT_OK;
    case MOD_RUNTIME_PROFILE_RECORD_WEATHER:
        return record_payload_size == sizeof(struct ModRuntimeProfileWeatherRecord) ? MOD_RUNTIME_PROFILE_RESULT_OK : MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    case MOD_RUNTIME_PROFILE_RECORD_ENGINE:
        return record_payload_size == sizeof(struct ModRuntimeProfileEngineRecord) ? MOD_RUNTIME_PROFILE_RESULT_OK : MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    case MOD_RUNTIME_PROFILE_RECORD_NPC:
        return record_payload_size == sizeof(struct ModRuntimeProfileNpcRecord) ? MOD_RUNTIME_PROFILE_RESULT_OK : MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    case MOD_RUNTIME_PROFILE_RECORD_ASSET_REF:
        return record_payload_size == sizeof(struct ModRuntimeProfileAssetRefRecord) ? MOD_RUNTIME_PROFILE_RESULT_OK : MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    case MOD_RUNTIME_PROFILE_RECORD_ASSET_INLINE_SHEET:
        if (record_payload_size < sizeof(sheet_record))
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
        memcpy(&sheet_record, record_payload, sizeof(sheet_record));
        if (sheet_record.size == 0 || sheet_record.size % TILE_SIZE_4BPP != 0)
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
        if ((u16)(sizeof(sheet_record) + sheet_record.size) > record_payload_size)
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
        next_asset_offset = AlignAssetOffset(*required_asset_bytes);
        if (next_asset_offset + sheet_record.size > MOD_RUNTIME_PROFILE_MAX_ASSET_BYTES)
            return MOD_RUNTIME_PROFILE_RESULT_BAD_SIZE;
        *required_asset_bytes = next_asset_offset + sheet_record.size;
        return MOD_RUNTIME_PROFILE_RESULT_OK;
    case MOD_RUNTIME_PROFILE_RECORD_ASSET_INLINE_PALETTE:
        if (record_payload_size < sizeof(palette_record))
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
        memcpy(&palette_record, record_payload, sizeof(palette_record));
        palette_byte_count = palette_record.colorCount * sizeof(u16);
        if (palette_record.colorCount == 0 || palette_record.colorCount > 16)
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
        if ((u16)(sizeof(palette_record) + palette_byte_count) > record_payload_size)
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
        next_asset_offset = AlignAssetOffset(*required_asset_bytes);
        if (next_asset_offset + PLTT_SIZE_4BPP > MOD_RUNTIME_PROFILE_MAX_ASSET_BYTES)
            return MOD_RUNTIME_PROFILE_RESULT_BAD_SIZE;
        *required_asset_bytes = next_asset_offset + PLTT_SIZE_4BPP;
        return MOD_RUNTIME_PROFILE_RESULT_OK;
    case MOD_RUNTIME_PROFILE_RECORD_BADGE_EFFECT:
        return record_payload_size == sizeof(struct ModRuntimeProfileBadgeEffectRecord) ? MOD_RUNTIME_PROFILE_RESULT_OK : MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    case MOD_RUNTIME_PROFILE_RECORD_FISHING_ACTION:
        return record_payload_size == sizeof(struct ModRuntimeProfileFishingActionRecord) ? MOD_RUNTIME_PROFILE_RESULT_OK : MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    case MOD_RUNTIME_PROFILE_RECORD_ENCOUNTER:
        return record_payload_size == sizeof(struct ModRuntimeProfileEncounterRecord) ? MOD_RUNTIME_PROFILE_RESULT_OK : MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    case MOD_RUNTIME_PROFILE_RECORD_SHOP:
        return record_payload_size == sizeof(struct ModRuntimeProfileShopRecord) ? MOD_RUNTIME_PROFILE_RESULT_OK : MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    case MOD_RUNTIME_PROFILE_RECORD_ITEM:
        return record_payload_size == sizeof(struct ModRuntimeProfileItemRecord) ? MOD_RUNTIME_PROFILE_RESULT_OK : MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    case MOD_RUNTIME_PROFILE_RECORD_REWARD:
        return record_payload_size == sizeof(struct ModRuntimeProfileRewardRecord) ? MOD_RUNTIME_PROFILE_RESULT_OK : MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    case MOD_RUNTIME_PROFILE_RECORD_POKEMON_DATA:
        return record_payload_size == sizeof(struct ModRuntimeProfilePokemonDataRecord) ? MOD_RUNTIME_PROFILE_RESULT_OK : MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    case MOD_RUNTIME_PROFILE_RECORD_BATTLE_MOVE:
        return record_payload_size == sizeof(struct ModRuntimeProfileBattleMoveRecord) ? MOD_RUNTIME_PROFILE_RESULT_OK : MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    case MOD_RUNTIME_PROFILE_RECORD_TRAINER:
        return record_payload_size == sizeof(struct ModRuntimeProfileTrainerRecord) ? MOD_RUNTIME_PROFILE_RESULT_OK : MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    default:
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    }
}

static u8 PrepareProfileStorage(void)
{
    u16 profile_offset = 0;
    u16 required_text_bytes = 0;
    u16 required_asset_bytes = 0;
    u8 storage_measure_result;
    struct ModRuntimeProfileRecordHeader record_header;

    while (profile_offset < sProfile->expectedSize)
    {
        if (sProfile->expectedSize - profile_offset < sizeof(record_header))
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

        memcpy(&record_header, &sProfile->blob[profile_offset], sizeof(record_header));
        profile_offset += sizeof(record_header);
        if (record_header.size > sProfile->expectedSize - profile_offset)
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

        storage_measure_result = MeasureRecordStorage(
            record_header.type,
            &sProfile->blob[profile_offset],
            record_header.size,
            &required_text_bytes,
            &required_asset_bytes);
        if (storage_measure_result != MOD_RUNTIME_PROFILE_RESULT_OK)
            return storage_measure_result;

        profile_offset += record_header.size;
    }

    sProfile->textByteCapacity = required_text_bytes;
    sProfile->assetByteCapacity = required_asset_bytes;
    if (required_text_bytes != 0)
    {
        sProfile->textBytes = AllocZeroed(required_text_bytes);
        if (sProfile->textBytes == NULL)
            return MOD_RUNTIME_PROFILE_RESULT_OUT_OF_MEMORY;
    }
    if (required_asset_bytes != 0)
    {
        sProfile->assetBytes = AllocZeroed(required_asset_bytes);
        if (sProfile->assetBytes == NULL)
            return MOD_RUNTIME_PROFILE_RESULT_OUT_OF_MEMORY;
    }

    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

static u8 ParseProfile(void)
{
    u16 profile_offset = 0;
    u8 record_parse_result;
    struct ModRuntimeProfileRecordHeader record_header;

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

    while (profile_offset < sProfile->expectedSize)
    {
        if (sProfile->expectedSize - profile_offset < sizeof(record_header))
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

        memcpy(&record_header, &sProfile->blob[profile_offset], sizeof(record_header));
        profile_offset += sizeof(record_header);
        if (record_header.size > sProfile->expectedSize - profile_offset)
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

        record_parse_result = ParseRecord(record_header.type, &sProfile->blob[profile_offset], record_header.size);
        if (record_parse_result != MOD_RUNTIME_PROFILE_RESULT_OK)
            return record_parse_result;

        profile_offset += record_header.size;
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

bool8 ModRuntimeProfile_BeginReceive(u32 profile_hash, u16 profile_size, u16 chunk_count)
{
    ModRuntimeProfile_Clear();

    if (profile_size == 0 || profile_size > MOD_RUNTIME_PROFILE_MAX_BLOB_SIZE)
        return FALSE;
    if (chunk_count == 0 || chunk_count > MOD_RUNTIME_PROFILE_MAX_CHUNKS)
        return FALSE;

    sProfile = AllocZeroed(sizeof(*sProfile));
    if (sProfile == NULL)
        return FALSE;

    sProfile->blob = AllocZeroed(profile_size);
    if (sProfile->blob == NULL)
    {
        ModRuntimeProfile_Clear();
        return FALSE;
    }

    sProfile->profileHash = profile_hash;
    sProfile->expectedSize = profile_size;
    sProfile->expectedChunks = chunk_count;
    return TRUE;
}

bool8 ModRuntimeProfile_ReceiveChunk(u32 profile_hash, u16 chunk_index, u16 chunk_offset, const void *runtime_profile_chunk_data, u8 chunk_size)
{
    if (sProfile == NULL || runtime_profile_chunk_data == NULL)
        return FALSE;
    if (profile_hash != sProfile->profileHash)
        return FALSE;
    if (sProfile->blob == NULL)
        return FALSE;
    if (chunk_index >= sProfile->expectedChunks || chunk_index >= MOD_RUNTIME_PROFILE_MAX_CHUNKS)
        return FALSE;
    if (chunk_size == 0 || chunk_size > NET_PROFILE_CHUNK_DATA_SIZE)
        return FALSE;
    if (chunk_offset > sProfile->expectedSize || chunk_size > sProfile->expectedSize - chunk_offset)
        return FALSE;

    memcpy(&sProfile->blob[chunk_offset], runtime_profile_chunk_data, chunk_size);
    if (!sProfile->chunkReceived[chunk_index])
    {
        sProfile->chunkReceived[chunk_index] = TRUE;
        sProfile->receivedBytes += chunk_size;
    }
    return TRUE;
}

u8 ModRuntimeProfile_CommitReceive(u32 profile_hash)
{
    u16 chunk_index;
    u8 profile_commit_result;

    if (sProfile == NULL || profile_hash != sProfile->profileHash)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_HASH;
    if (sProfile->active && sProfile->blob == NULL)
        return MOD_RUNTIME_PROFILE_RESULT_OK;
    for (chunk_index = 0; chunk_index < sProfile->expectedChunks; chunk_index++)
    {
        if (!sProfile->chunkReceived[chunk_index])
            return MOD_RUNTIME_PROFILE_RESULT_BAD_SIZE;
    }
    if (CalcProfileHash(sProfile->blob, sProfile->expectedSize) != profile_hash)
    {
        ModRuntimeProfile_Clear();
        return MOD_RUNTIME_PROFILE_RESULT_BAD_HASH;
    }

    profile_commit_result = PrepareProfileStorage();
    if (profile_commit_result != MOD_RUNTIME_PROFILE_RESULT_OK)
    {
        ModRuntimeProfile_Clear();
        return profile_commit_result;
    }

    profile_commit_result = ParseProfile();
    if (profile_commit_result != MOD_RUNTIME_PROFILE_RESULT_OK)
    {
        ModRuntimeProfile_Clear();
        return profile_commit_result;
    }

    TRY_FREE_AND_SET_NULL(sProfile->blob);
    sProfile->active = TRUE;
    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

const u8 *ModRuntimeProfile_GetText(const char *key, const char *language)
{
    u16 text_index;

    if (!ModRuntimeProfile_IsActive() || key == NULL || language == NULL)
        return NULL;

    for (text_index = 0; text_index < sProfile->textCount; text_index++)
    {
        if (strcmp(sProfile->texts[text_index].key, key) == 0
         && strcmp(sProfile->texts[text_index].language, language) == 0)
            return sProfile->texts[text_index].text;
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
    u16 npc_index;
    static struct ModNpcDefinition definition;

    if (!ModRuntimeProfile_IsActive() || key == NULL)
        return NULL;

    for (npc_index = 0; npc_index < sProfile->npcCount; npc_index++)
    {
        if (strcmp(sProfile->npcs[npc_index].record.key, key) == 0)
        {
            memset(&definition, 0, sizeof(definition));
            definition.key = sProfile->npcs[npc_index].record.key;
            definition.npc_definition_id = npc_index;
            definition.graphicsId = sProfile->npcs[npc_index].record.graphicsId;
            definition.movementType = sProfile->npcs[npc_index].record.movementType;
            definition.localId = sProfile->npcs[npc_index].record.localId;
            definition.elevation = sProfile->npcs[npc_index].record.elevation;
            return &definition;
        }
    }

    return NULL;
}

const struct ModSpriteAssetDefinition *ModRuntimeProfile_FindAsset(const char *key)
{
    u16 asset_index;

    if (!ModRuntimeProfile_IsActive() || key == NULL)
        return NULL;

    for (asset_index = 0; asset_index < sProfile->assetCount; asset_index++)
    {
        if (strcmp(sProfile->assets[asset_index].key, key) != 0)
            continue;
        if (sProfile->assets[asset_index].sourceKey[0] != '\0')
            return FindGeneratedAsset(sProfile->assets[asset_index].sourceKey);
        return &sProfile->assets[asset_index].definition;
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
    u16 npc_index;
    u8 localId;
    u8 objectEventId;
    const struct ModRuntimeProfileNpcRecord *record;

    if (!ModRuntimeProfile_IsActive() || gSaveBlock1Ptr == NULL)
        return;

    for (npc_index = 0; npc_index < sProfile->npcCount; npc_index++)
    {
        record = &sProfile->npcs[npc_index].record;
        if (record->mapGroup != gSaveBlock1Ptr->location.mapGroup || record->mapNum != gSaveBlock1Ptr->location.mapNum)
            continue;
        localId = record->localId;
        if (localId == 0)
            localId = MOD_NPC_DYNAMIC_LOCAL_ID_BASE + (npc_index % (MOD_NPC_DYNAMIC_LOCAL_ID_END - MOD_NPC_DYNAMIC_LOCAL_ID_BASE + 1));
        if (TryGetObjectEventIdByLocalIdAndMap(localId, record->mapNum, record->mapGroup, &objectEventId))
            continue;
        SpawnSpecialObjectEventParameterized(record->graphicsId, record->movementType, localId, record->x, record->y, record->elevation);
    }
}
