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
    char source_key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    struct ModSpriteAssetDefinition definition;
    struct SpriteSheet sheet;
    struct SpritePalette palette;
};

struct ModRuntimeProfileState
{
    bool8 active;
    u32 profile_hash;
    u16 expected_size;
    u16 expected_chunks;
    u16 received_bytes;
    u8 *blob;
    u8 *text_bytes;
    u8 *asset_bytes;
    u16 text_byte_capacity;
    u16 asset_byte_capacity;
    u16 text_byte_count;
    u8 chunk_received[MOD_RUNTIME_PROFILE_MAX_CHUNKS];
    struct RuntimeProfileText texts[MOD_RUNTIME_PROFILE_MAX_TEXTS];
    u16 text_count;
    bool8 has_weather;
    struct ModWeatherDisplay weather;
    char engine_ruleset_id[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    struct RuntimeProfileNpc npcs[MOD_RUNTIME_PROFILE_MAX_NPCS];
    u16 npc_count;
    struct RuntimeProfileAsset assets[MOD_RUNTIME_PROFILE_MAX_ASSETS];
    u16 asset_count;
    u16 asset_byte_count;
    char badge_effect_keys[MOD_RUNTIME_PROFILE_MAX_BADGE_EFFECTS][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    struct ModBadgeEffectDefinition badge_effects[MOD_RUNTIME_PROFILE_MAX_BADGE_EFFECTS];
    u16 badge_effect_count;
    char fishing_action_keys[MOD_RUNTIME_PROFILE_MAX_FISHING_ACTIONS][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char fishing_action_hook_keys[MOD_RUNTIME_PROFILE_MAX_FISHING_ACTIONS][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char fishing_action_prompt_keys[MOD_RUNTIME_PROFILE_MAX_FISHING_ACTIONS][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    struct FishingActionDefinition fishing_actions[MOD_RUNTIME_PROFILE_MAX_FISHING_ACTIONS];
    u16 fishing_action_count;
    char encounter_keys[MOD_RUNTIME_PROFILE_MAX_ENCOUNTERS][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char encounter_hook_keys[MOD_RUNTIME_PROFILE_MAX_ENCOUNTERS][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    struct ModEncounterDefinition encounters[MOD_RUNTIME_PROFILE_MAX_ENCOUNTERS];
    u16 encounter_count;
    char shop_keys[MOD_RUNTIME_PROFILE_MAX_SHOPS][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    struct ModShopDefinition shops[MOD_RUNTIME_PROFILE_MAX_SHOPS];
    u16 shop_count;
    char item_keys[MOD_RUNTIME_PROFILE_MAX_ITEMS][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char item_name_keys[MOD_RUNTIME_PROFILE_MAX_ITEMS][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char item_description_keys[MOD_RUNTIME_PROFILE_MAX_ITEMS][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char item_field_hook_keys[MOD_RUNTIME_PROFILE_MAX_ITEMS][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char item_battle_hook_keys[MOD_RUNTIME_PROFILE_MAX_ITEMS][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    struct ModItemDefinition items[MOD_RUNTIME_PROFILE_MAX_ITEMS];
    u16 item_count;
    char reward_keys[MOD_RUNTIME_PROFILE_MAX_REWARDS][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char reward_hook_keys[MOD_RUNTIME_PROFILE_MAX_REWARDS][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    struct ModRewardDefinition rewards[MOD_RUNTIME_PROFILE_MAX_REWARDS];
    u16 reward_count;
    char pokemon_data_keys[MOD_RUNTIME_PROFILE_MAX_POKEMON_DATA][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    struct ModPokemonDataDefinition pokemon_data[MOD_RUNTIME_PROFILE_MAX_POKEMON_DATA];
    u16 pokemon_data_count;
    char battle_move_keys[MOD_RUNTIME_PROFILE_MAX_BATTLE_MOVES][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    struct ModBattleMoveDefinition battle_moves[MOD_RUNTIME_PROFILE_MAX_BATTLE_MOVES];
    u16 battle_move_count;
    char trainer_keys[MOD_RUNTIME_PROFILE_MAX_TRAINERS][MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    struct ModTrainerDefinition trainers[MOD_RUNTIME_PROFILE_MAX_TRAINERS];
    u16 trainer_count;
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
    u8 local_id;
    u8 map_group;
    u8 map_num;

    if (sProfile == NULL || gSaveBlock1Ptr == NULL)
        return;

    map_group = gSaveBlock1Ptr->location.mapGroup;
    map_num = gSaveBlock1Ptr->location.mapNum;
    for (npc_index = 0; npc_index < sProfile->npc_count; npc_index++)
    {
        if (sProfile->npcs[npc_index].record.map_group != map_group || sProfile->npcs[npc_index].record.map_num != map_num)
            continue;
        local_id = sProfile->npcs[npc_index].record.local_id;
        if (local_id == 0)
            local_id = MOD_NPC_DYNAMIC_LOCAL_ID_BASE + (npc_index % (MOD_NPC_DYNAMIC_LOCAL_ID_END - MOD_NPC_DYNAMIC_LOCAL_ID_BASE + 1));
        RemoveObjectEventByLocalIdAndMap(local_id, map_num, map_group);
    }
}

static struct RuntimeProfileAsset *FindRuntimeAssetSlot(const char *key)
{
    u16 asset_index;

    if (sProfile == NULL || key == NULL)
        return NULL;

    for (asset_index = 0; asset_index < sProfile->asset_count; asset_index++)
    {
        if (strcmp(sProfile->assets[asset_index].key, key) == 0)
            return &sProfile->assets[asset_index];
    }

    if (sProfile->asset_count >= MOD_RUNTIME_PROFILE_MAX_ASSETS)
        return NULL;

    asset_index = sProfile->asset_count++;
    memset(&sProfile->assets[asset_index], 0, sizeof(sProfile->assets[asset_index]));
    CopyBoundedString(sProfile->assets[asset_index].key, key, sizeof(sProfile->assets[asset_index].key));
    sProfile->assets[asset_index].definition.key = sProfile->assets[asset_index].key;
    sProfile->assets[asset_index].definition.tile_tag = TAG_NONE;
    sProfile->assets[asset_index].definition.palette_tag = TAG_NONE;
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
    const u8 *text_bytes;

    if (size < sizeof(record) || sProfile->text_count >= MOD_RUNTIME_PROFILE_MAX_TEXTS)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memcpy(&record, payload, sizeof(record));
    if ((u16)(sizeof(record) + record.text_size) > size)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    if (sProfile->text_byte_count + record.text_size + 1 > sProfile->text_byte_capacity)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_SIZE;

    text_bytes = payload + sizeof(record);
    text = &sProfile->texts[sProfile->text_count++];
    CopyBoundedString(text->language, record.language, sizeof(text->language));
    CopyBoundedString(text->key, record.key, sizeof(text->key));
    text->text = &sProfile->text_bytes[sProfile->text_byte_count];
    memcpy(text->text, text_bytes, record.text_size);
    text->text[record.text_size] = EOS;
    sProfile->text_byte_count += record.text_size + 1;
    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

static u8 ParseWeatherRecord(const u8 *payload, u16 size)
{
    struct ModRuntimeProfileWeatherRecord record;

    if (size != sizeof(record))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memcpy(&record, payload, sizeof(record));
    sProfile->has_weather = TRUE;
    sProfile->weather.vanilla_weather = record.vanilla_weather;
    sProfile->weather.priority = record.priority;
    sProfile->weather.layers = record.layers;
    sProfile->weather.battle_weather_mask = record.battle_weather_mask;
    sProfile->weather.source = MOD_WEATHER_SOURCE_SERVER;
    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

static u8 ParseEngineRecord(const u8 *payload, u16 size)
{
    struct ModRuntimeProfileEngineRecord record;

    if (size != sizeof(record))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memcpy(&record, payload, sizeof(record));
    CopyBoundedString(sProfile->engine_ruleset_id, record.ruleset_id, sizeof(sProfile->engine_ruleset_id));
    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

static u8 ParseNpcRecord(const u8 *payload, u16 size)
{
    if (size != sizeof(struct ModRuntimeProfileNpcRecord) || sProfile->npc_count >= MOD_RUNTIME_PROFILE_MAX_NPCS)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memcpy(&sProfile->npcs[sProfile->npc_count].record, payload, sizeof(struct ModRuntimeProfileNpcRecord));
    sProfile->npcs[sProfile->npc_count].record.key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    sProfile->npc_count++;
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
    record.source_key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    if (FindGeneratedAsset(record.source_key) == NULL)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    asset = FindRuntimeAssetSlot(record.key);
    if (asset == NULL)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_SIZE;

    CopyBoundedString(asset->source_key, record.source_key, sizeof(asset->source_key));
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
    assetOffset = AlignAssetOffset(sProfile->asset_byte_count);
    if (assetOffset + record.size > sProfile->asset_byte_capacity)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_SIZE;

    record.key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    asset = FindRuntimeAssetSlot(record.key);
    if (asset == NULL)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_SIZE;

    sheet_bytes_from_profile = payload + sizeof(record);
    sheet_storage_in_profile = &sProfile->asset_bytes[assetOffset];
    memcpy(sheet_storage_in_profile, sheet_bytes_from_profile, record.size);
    asset->sheet.data = sheet_storage_in_profile;
    asset->sheet.size = record.size;
    asset->sheet.tag = record.tile_tag;
    asset->definition.sheet = &asset->sheet;
    asset->definition.tile_tag = record.tile_tag;
    sProfile->asset_byte_count = assetOffset + record.size;
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
    palette_byte_count_from_profile = record.color_count * sizeof(u16);
    if (record.color_count == 0 || record.color_count > 16)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    if ((u16)(sizeof(record) + palette_byte_count_from_profile) > size)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    assetOffset = AlignAssetOffset(sProfile->asset_byte_count);
    if (assetOffset + PLTT_SIZE_4BPP > sProfile->asset_byte_capacity)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_SIZE;

    record.key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    asset = FindRuntimeAssetSlot(record.key);
    if (asset == NULL)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_SIZE;

    palette_bytes_from_profile = payload + sizeof(record);
    palette_storage_in_profile = &sProfile->asset_bytes[assetOffset];
    memset(palette_storage_in_profile, 0, PLTT_SIZE_4BPP);
    memcpy(palette_storage_in_profile, palette_bytes_from_profile, palette_byte_count_from_profile);
    asset->palette.data = (const u16 *)palette_storage_in_profile;
    asset->palette.tag = record.palette_tag;
    asset->definition.palette = &asset->palette;
    asset->definition.palette_tag = record.palette_tag;
    sProfile->asset_byte_count = assetOffset + PLTT_SIZE_4BPP;
    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

static bool8 RuntimeBadgeEffectKeyExists(const char *key)
{
    u16 badge_effect_index;

    if (key == NULL)
        return FALSE;

    for (badge_effect_index = 0; badge_effect_index < sProfile->badge_effect_count; badge_effect_index++)
    {
        if (strcmp(sProfile->badge_effect_keys[badge_effect_index], key) == 0)
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

    if (size != sizeof(record) || sProfile->badge_effect_count >= MOD_RUNTIME_PROFILE_MAX_BADGE_EFFECTS)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memcpy(&record, payload, sizeof(record));
    record.key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    if (record.key[0] == '\0')
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    if (RuntimeBadgeEffectKeyExists(record.key))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    candidate.key = record.key;
    candidate.badge_id = record.badge_id;
    candidate.effect_kind = record.effect_kind;
    candidate.target = record.target;
    candidate.percent_per_level = record.percent_per_level;
    candidate.max_level = record.max_level;
    candidate.flags = record.flags;
    if (!BadgeApi_IsEffectDefinitionValid(&candidate, FALSE))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    key = sProfile->badge_effect_keys[sProfile->badge_effect_count];
    CopyBoundedString(key, record.key, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);
    definition = &sProfile->badge_effects[sProfile->badge_effect_count++];
    definition->key = key;
    definition->badge_id = record.badge_id;
    definition->effect_kind = record.effect_kind;
    definition->target = record.target;
    definition->percent_per_level = record.percent_per_level;
    definition->max_level = record.max_level;
    definition->flags = record.flags;
    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

static bool8 RuntimeFishingActionKeyExists(const char *key)
{
    u16 fishing_action_index;

    if (key == NULL)
        return FALSE;

    for (fishing_action_index = 0; fishing_action_index < sProfile->fishing_action_count; fishing_action_index++)
    {
        if (strcmp(sProfile->fishing_action_keys[fishing_action_index], key) == 0)
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
    char *hook_key;
    char *prompt_key;

    if (size != sizeof(record) || sProfile->fishing_action_count >= MOD_RUNTIME_PROFILE_MAX_FISHING_ACTIONS)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memcpy(&record, payload, sizeof(record));
    record.key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    record.source_key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    record.hook_key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    record.prompt_key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    if (record.key[0] == '\0' || record.source_key[0] == '\0' || record.hook_key[0] == '\0')
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    if (RuntimeFishingActionKeyExists(record.key))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    hook = FishingApi_FindCompiledHook(record.source_key, record.hook_key);
    if (hook == NULL)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memset(&candidate, 0, sizeof(candidate));
    candidate.key = record.key;
    candidate.hook_key = record.hook_key;
    candidate.rod_mask = record.rod_mask;
    candidate.phase_mask = record.phase_mask;
    candidate.priority = record.priority;
    candidate.flags = record.flags;
    candidate.hook = hook;
    candidate.prompt_key = record.prompt_key[0] == '\0' ? NULL : record.prompt_key;
    candidate.button_mask = record.button_mask;
    candidate.timeout_frames = record.timeout_frames;
    candidate.success_outcome = record.success_outcome;
    candidate.failure_outcome = record.failure_outcome;
    for (action_param_index = 0; action_param_index < FISHING_ACTION_PARAM_COUNT; action_param_index++)
        candidate.params[action_param_index] = record.params[action_param_index];

    if (!FishingApi_IsDefinitionValid(&candidate, FALSE))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    key = sProfile->fishing_action_keys[sProfile->fishing_action_count];
    hook_key = sProfile->fishing_action_hook_keys[sProfile->fishing_action_count];
    prompt_key = sProfile->fishing_action_prompt_keys[sProfile->fishing_action_count];
    CopyBoundedString(key, record.key, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);
    CopyBoundedString(hook_key, record.hook_key, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);
    CopyBoundedString(prompt_key, record.prompt_key, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);

    definition = &sProfile->fishing_actions[sProfile->fishing_action_count++];
    *definition = candidate;
    definition->key = key;
    definition->hook_key = hook_key;
    definition->prompt_key = prompt_key[0] == '\0' ? NULL : prompt_key;
    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

static bool8 RuntimeEncounterKeyExists(const char *key)
{
    u16 encounter_index;

    if (key == NULL)
        return FALSE;

    for (encounter_index = 0; encounter_index < sProfile->encounter_count; encounter_index++)
    {
        if (strcmp(sProfile->encounter_keys[encounter_index], key) == 0)
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
    char *hook_key;
    u8 encounter_slot_index;

    if (size != sizeof(record) || sProfile->encounter_count >= MOD_RUNTIME_PROFILE_MAX_ENCOUNTERS)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memcpy(&record, payload, sizeof(record));
    record.key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    record.source_key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    record.hook_key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    if (record.key[0] == '\0')
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    if (RuntimeEncounterKeyExists(record.key))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    if ((record.source_key[0] == '\0') != (record.hook_key[0] == '\0'))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    if (record.source_key[0] != '\0')
    {
        hook = EncounterApi_FindCompiledHook(record.source_key, record.hook_key);
        if (hook == NULL)
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    }

    memset(&candidate, 0, sizeof(candidate));
    candidate.key = record.key;
    candidate.hook_key = record.hook_key[0] == '\0' ? NULL : record.hook_key;
    candidate.map_group = record.map_group;
    candidate.map_num = record.map_num;
    candidate.area = record.area;
    candidate.rod_mask = record.rod_mask;
    candidate.encounter_rate = record.encounter_rate;
    candidate.slot_count = record.slot_count;
    candidate.priority = record.priority;
    candidate.flags = record.flags;
    candidate.hook = hook;
    for (encounter_slot_index = 0; encounter_slot_index < MOD_ENCOUNTER_MAX_SLOTS; encounter_slot_index++)
        candidate.slots[encounter_slot_index] = record.slots[encounter_slot_index];

    if (!EncounterApi_IsDefinitionValid(&candidate, FALSE))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    key = sProfile->encounter_keys[sProfile->encounter_count];
    hook_key = sProfile->encounter_hook_keys[sProfile->encounter_count];
    CopyBoundedString(key, record.key, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);
    CopyBoundedString(hook_key, record.hook_key, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);

    definition = &sProfile->encounters[sProfile->encounter_count++];
    *definition = candidate;
    definition->key = key;
    definition->hook_key = hook_key[0] == '\0' ? NULL : hook_key;
    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

static bool8 RuntimeShopKeyExists(const char *key)
{
    u16 shop_index;

    if (key == NULL)
        return FALSE;

    for (shop_index = 0; shop_index < sProfile->shop_count; shop_index++)
    {
        if (strcmp(sProfile->shop_keys[shop_index], key) == 0)
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

    if (size != sizeof(record) || sProfile->shop_count >= MOD_RUNTIME_PROFILE_MAX_SHOPS)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memcpy(&record, payload, sizeof(record));
    record.key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    if (record.key[0] == '\0')
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    if (RuntimeShopKeyExists(record.key))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memset(&candidate, 0, sizeof(candidate));
    candidate.key = record.key;
    candidate.map_group = record.map_group;
    candidate.map_num = record.map_num;
    candidate.mart_type = record.mart_type;
    candidate.item_count = record.item_count;
    candidate.priority = record.priority;
    candidate.flags = record.flags;
    for (shop_item_index = 0; shop_item_index < MOD_SHOP_MAX_ITEMS; shop_item_index++)
        candidate.items[shop_item_index] = record.items[shop_item_index];

    if (!ShopApi_IsDefinitionValid(&candidate, FALSE))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    key = sProfile->shop_keys[sProfile->shop_count];
    CopyBoundedString(key, record.key, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);
    definition = &sProfile->shops[sProfile->shop_count++];
    *definition = candidate;
    definition->key = key;
    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

static bool8 RuntimeItemKeyExists(const char *key)
{
    u16 item_index;

    if (key == NULL)
        return FALSE;

    for (item_index = 0; item_index < sProfile->item_count; item_index++)
    {
        if (strcmp(sProfile->item_keys[item_index], key) == 0)
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
    char *name_key;
    char *description_key;
    char *field_hook_key;
    char *battle_hook_key;
    ItemUseFunc field_use_func = NULL;
    ItemUseFunc battle_use_func = NULL;

    if (size != sizeof(record) || sProfile->item_count >= MOD_RUNTIME_PROFILE_MAX_ITEMS)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memcpy(&record, payload, sizeof(record));
    record.key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    record.name_key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    record.description_key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    record.field_use_source_key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    record.field_use_hook_key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    record.battle_use_source_key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    record.battle_use_hook_key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    if (record.key[0] == '\0')
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    if (RuntimeItemKeyExists(record.key))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    if ((record.field_use_source_key[0] == '\0') != (record.field_use_hook_key[0] == '\0'))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    if ((record.battle_use_source_key[0] == '\0') != (record.battle_use_hook_key[0] == '\0'))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    if (record.field_use_source_key[0] != '\0')
    {
        field_use_func = ItemApi_FindCompiledFieldUseHook(record.field_use_source_key, record.field_use_hook_key);
        if (field_use_func == NULL)
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    }
    if (record.battle_use_source_key[0] != '\0')
    {
        battle_use_func = ItemApi_FindCompiledBattleUseHook(record.battle_use_source_key, record.battle_use_hook_key);
        if (battle_use_func == NULL)
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    }

    memset(&candidate, 0, sizeof(candidate));
    candidate.key = record.key;
    candidate.name_key = record.name_key[0] == '\0' ? NULL : record.name_key;
    candidate.description_key = record.description_key[0] == '\0' ? NULL : record.description_key;
    candidate.field_use_hook_key = record.field_use_hook_key[0] == '\0' ? NULL : record.field_use_hook_key;
    candidate.battle_use_hook_key = record.battle_use_hook_key[0] == '\0' ? NULL : record.battle_use_hook_key;
    candidate.item_id = record.item_id;
    candidate.price = record.price;
    candidate.flags = record.flags;
    candidate.hold_effect = record.hold_effect;
    candidate.hold_effect_param = record.hold_effect_param;
    candidate.importance = record.importance;
    candidate.pocket = record.pocket;
    candidate.type = record.type;
    candidate.battle_usage = record.battle_usage;
    candidate.secondary_id = record.secondary_id;
    candidate.priority = record.priority;
    candidate.field_use_func = field_use_func;
    candidate.battle_use_func = battle_use_func;
    if (!ItemApi_IsDefinitionValid(&candidate, FALSE))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    key = sProfile->item_keys[sProfile->item_count];
    name_key = sProfile->item_name_keys[sProfile->item_count];
    description_key = sProfile->item_description_keys[sProfile->item_count];
    field_hook_key = sProfile->item_field_hook_keys[sProfile->item_count];
    battle_hook_key = sProfile->item_battle_hook_keys[sProfile->item_count];
    CopyBoundedString(key, record.key, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);
    CopyBoundedString(name_key, record.name_key, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);
    CopyBoundedString(description_key, record.description_key, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);
    CopyBoundedString(field_hook_key, record.field_use_hook_key, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);
    CopyBoundedString(battle_hook_key, record.battle_use_hook_key, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);

    definition = &sProfile->items[sProfile->item_count++];
    *definition = candidate;
    definition->key = key;
    definition->name_key = name_key[0] == '\0' ? NULL : name_key;
    definition->description_key = description_key[0] == '\0' ? NULL : description_key;
    definition->field_use_hook_key = field_hook_key[0] == '\0' ? NULL : field_hook_key;
    definition->battle_use_hook_key = battle_hook_key[0] == '\0' ? NULL : battle_hook_key;
    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

static bool8 RuntimeRewardKeyExists(const char *key)
{
    u16 reward_index;

    if (key == NULL)
        return FALSE;

    for (reward_index = 0; reward_index < sProfile->reward_count; reward_index++)
    {
        if (strcmp(sProfile->reward_keys[reward_index], key) == 0)
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
    char *hook_key;

    if (size != sizeof(record) || sProfile->reward_count >= MOD_RUNTIME_PROFILE_MAX_REWARDS)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memcpy(&record, payload, sizeof(record));
    record.key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    record.source_key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    record.hook_key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    if (record.key[0] == '\0')
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    if (RuntimeRewardKeyExists(record.key))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    if ((record.source_key[0] == '\0') != (record.hook_key[0] == '\0'))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    if (record.source_key[0] != '\0')
    {
        hook = RewardApi_FindCompiledHook(record.source_key, record.hook_key);
        if (hook == NULL)
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    }

    memset(&candidate, 0, sizeof(candidate));
    candidate.key = record.key;
    candidate.hook_key = record.hook_key[0] == '\0' ? NULL : record.hook_key;
    candidate.source = record.source;
    candidate.min_level = record.min_level;
    candidate.max_level = record.max_level;
    candidate.priority = record.priority;
    candidate.flags = record.flags;
    candidate.item_id = record.item_id;
    candidate.quantity = record.quantity;
    candidate.hook = hook;
    if (!RewardApi_IsDefinitionValid(&candidate, FALSE))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    key = sProfile->reward_keys[sProfile->reward_count];
    hook_key = sProfile->reward_hook_keys[sProfile->reward_count];
    CopyBoundedString(key, record.key, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);
    CopyBoundedString(hook_key, record.hook_key, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);
    definition = &sProfile->rewards[sProfile->reward_count++];
    *definition = candidate;
    definition->key = key;
    definition->hook_key = hook_key[0] == '\0' ? NULL : hook_key;
    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

static bool8 RuntimePokemonDataKeyExists(const char *key)
{
    u16 pokemon_data_index;

    if (key == NULL)
        return FALSE;

    for (pokemon_data_index = 0; pokemon_data_index < sProfile->pokemon_data_count; pokemon_data_index++)
    {
        if (strcmp(sProfile->pokemon_data_keys[pokemon_data_index], key) == 0)
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

    if (size != sizeof(record) || sProfile->pokemon_data_count >= MOD_RUNTIME_PROFILE_MAX_POKEMON_DATA)
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
    candidate.level_up_move_count = record.level_up_move_count;
    memcpy(candidate.level_up_moves, record.level_up_moves, sizeof(candidate.level_up_moves));
    candidate.evolution_count = record.evolution_count;
    memcpy(candidate.evolutions, record.evolutions, sizeof(candidate.evolutions));
    if (!PokemonDataApi_IsDefinitionValid(&candidate, FALSE))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    key = sProfile->pokemon_data_keys[sProfile->pokemon_data_count];
    CopyBoundedString(key, record.key, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);
    definition = &sProfile->pokemon_data[sProfile->pokemon_data_count++];
    *definition = candidate;
    definition->key = key;
    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

static bool8 RuntimeBattleMoveKeyExists(const char *key)
{
    u16 battle_move_index;

    if (key == NULL)
        return FALSE;

    for (battle_move_index = 0; battle_move_index < sProfile->battle_move_count; battle_move_index++)
    {
        if (strcmp(sProfile->battle_move_keys[battle_move_index], key) == 0)
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

    if (size != sizeof(record) || sProfile->battle_move_count >= MOD_RUNTIME_PROFILE_MAX_BATTLE_MOVES)
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
    candidate.override_flags = record.override_flags;
    candidate.data = record.data;
    if (!BattleDataApi_IsMoveDefinitionValid(&candidate, FALSE))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    key = sProfile->battle_move_keys[sProfile->battle_move_count];
    CopyBoundedString(key, record.key, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);
    definition = &sProfile->battle_moves[sProfile->battle_move_count++];
    *definition = candidate;
    definition->key = key;
    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

static bool8 RuntimeTrainerKeyExists(const char *key)
{
    u16 trainer_index;

    if (key == NULL)
        return FALSE;

    for (trainer_index = 0; trainer_index < sProfile->trainer_count; trainer_index++)
    {
        if (strcmp(sProfile->trainer_keys[trainer_index], key) == 0)
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

    if (size != sizeof(record) || sProfile->trainer_count >= MOD_RUNTIME_PROFILE_MAX_TRAINERS)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memcpy(&record, payload, sizeof(record));
    record.key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH] = '\0';
    if (record.key[0] == '\0')
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
    if (RuntimeTrainerKeyExists(record.key))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    memset(&candidate, 0, sizeof(candidate));
    candidate.key = record.key;
    candidate.trainer_id = record.trainer_id;
    candidate.priority = record.priority;
    candidate.flags = record.flags;
    candidate.trainer_class = record.trainer_class;
    candidate.encounter_music_gender = record.encounter_music_gender;
    candidate.trainer_pic = record.trainer_pic;
    memcpy(candidate.items, record.items, sizeof(candidate.items));
    candidate.double_battle = record.double_battle;
    candidate.ai_flags = record.ai_flags;
    candidate.party_size = record.party_size;
    memcpy(candidate.party, record.party, sizeof(candidate.party));
    if (!TrainerApi_IsDefinitionValid(&candidate, FALSE))
        return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

    key = sProfile->trainer_keys[sProfile->trainer_count];
    CopyBoundedString(key, record.key, MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1);
    definition = &sProfile->trainers[sProfile->trainer_count++];
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
        if ((u16)(sizeof(text_record) + text_record.text_size) > record_payload_size)
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;
        if (*required_text_bytes + text_record.text_size + 1 > MOD_RUNTIME_PROFILE_MAX_TEXT_BYTES)
            return MOD_RUNTIME_PROFILE_RESULT_BAD_SIZE;
        *required_text_bytes += text_record.text_size + 1;
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
        palette_byte_count = palette_record.color_count * sizeof(u16);
        if (palette_record.color_count == 0 || palette_record.color_count > 16)
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

    while (profile_offset < sProfile->expected_size)
    {
        if (sProfile->expected_size - profile_offset < sizeof(record_header))
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

        memcpy(&record_header, &sProfile->blob[profile_offset], sizeof(record_header));
        profile_offset += sizeof(record_header);
        if (record_header.size > sProfile->expected_size - profile_offset)
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

    sProfile->text_byte_capacity = required_text_bytes;
    sProfile->asset_byte_capacity = required_asset_bytes;
    if (required_text_bytes != 0)
    {
        sProfile->text_bytes = AllocZeroed(required_text_bytes);
        if (sProfile->text_bytes == NULL)
            return MOD_RUNTIME_PROFILE_RESULT_OUT_OF_MEMORY;
    }
    if (required_asset_bytes != 0)
    {
        sProfile->asset_bytes = AllocZeroed(required_asset_bytes);
        if (sProfile->asset_bytes == NULL)
            return MOD_RUNTIME_PROFILE_RESULT_OUT_OF_MEMORY;
    }

    return MOD_RUNTIME_PROFILE_RESULT_OK;
}

static u8 ParseProfile(void)
{
    u16 profile_offset = 0;
    u8 record_parse_result;
    struct ModRuntimeProfileRecordHeader record_header;

    sProfile->text_count = 0;
    sProfile->text_byte_count = 0;
    sProfile->has_weather = FALSE;
    sProfile->engine_ruleset_id[0] = '\0';
    sProfile->npc_count = 0;
    sProfile->asset_count = 0;
    sProfile->asset_byte_count = 0;
    sProfile->badge_effect_count = 0;
    sProfile->fishing_action_count = 0;
    sProfile->encounter_count = 0;
    sProfile->shop_count = 0;
    sProfile->item_count = 0;
    sProfile->reward_count = 0;
    sProfile->pokemon_data_count = 0;
    sProfile->battle_move_count = 0;
    sProfile->trainer_count = 0;
    memset(sProfile->texts, 0, sizeof(sProfile->texts));
    memset(sProfile->npcs, 0, sizeof(sProfile->npcs));
    memset(sProfile->assets, 0, sizeof(sProfile->assets));
    memset(sProfile->badge_effect_keys, 0, sizeof(sProfile->badge_effect_keys));
    memset(sProfile->badge_effects, 0, sizeof(sProfile->badge_effects));
    memset(sProfile->fishing_action_keys, 0, sizeof(sProfile->fishing_action_keys));
    memset(sProfile->fishing_action_hook_keys, 0, sizeof(sProfile->fishing_action_hook_keys));
    memset(sProfile->fishing_action_prompt_keys, 0, sizeof(sProfile->fishing_action_prompt_keys));
    memset(sProfile->fishing_actions, 0, sizeof(sProfile->fishing_actions));
    memset(sProfile->encounter_keys, 0, sizeof(sProfile->encounter_keys));
    memset(sProfile->encounter_hook_keys, 0, sizeof(sProfile->encounter_hook_keys));
    memset(sProfile->encounters, 0, sizeof(sProfile->encounters));
    memset(sProfile->shop_keys, 0, sizeof(sProfile->shop_keys));
    memset(sProfile->shops, 0, sizeof(sProfile->shops));
    memset(sProfile->item_keys, 0, sizeof(sProfile->item_keys));
    memset(sProfile->item_name_keys, 0, sizeof(sProfile->item_name_keys));
    memset(sProfile->item_description_keys, 0, sizeof(sProfile->item_description_keys));
    memset(sProfile->item_field_hook_keys, 0, sizeof(sProfile->item_field_hook_keys));
    memset(sProfile->item_battle_hook_keys, 0, sizeof(sProfile->item_battle_hook_keys));
    memset(sProfile->items, 0, sizeof(sProfile->items));
    memset(sProfile->reward_keys, 0, sizeof(sProfile->reward_keys));
    memset(sProfile->reward_hook_keys, 0, sizeof(sProfile->reward_hook_keys));
    memset(sProfile->rewards, 0, sizeof(sProfile->rewards));
    memset(sProfile->pokemon_data_keys, 0, sizeof(sProfile->pokemon_data_keys));
    memset(sProfile->pokemon_data, 0, sizeof(sProfile->pokemon_data));
    memset(sProfile->battle_move_keys, 0, sizeof(sProfile->battle_move_keys));
    memset(sProfile->battle_moves, 0, sizeof(sProfile->battle_moves));
    memset(sProfile->trainer_keys, 0, sizeof(sProfile->trainer_keys));
    memset(sProfile->trainers, 0, sizeof(sProfile->trainers));
    if (sProfile->text_bytes != NULL)
        memset(sProfile->text_bytes, 0, sProfile->text_byte_capacity);
    if (sProfile->asset_bytes != NULL)
        memset(sProfile->asset_bytes, 0, sProfile->asset_byte_capacity);

    while (profile_offset < sProfile->expected_size)
    {
        if (sProfile->expected_size - profile_offset < sizeof(record_header))
            return MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD;

        memcpy(&record_header, &sProfile->blob[profile_offset], sizeof(record_header));
        profile_offset += sizeof(record_header);
        if (record_header.size > sProfile->expected_size - profile_offset)
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
    TRY_FREE_AND_SET_NULL(profile->text_bytes);
    TRY_FREE_AND_SET_NULL(profile->asset_bytes);
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

    return sProfile->profile_hash;
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

    sProfile->profile_hash = profile_hash;
    sProfile->expected_size = profile_size;
    sProfile->expected_chunks = chunk_count;
    return TRUE;
}

bool8 ModRuntimeProfile_ReceiveChunk(u32 profile_hash, u16 chunk_index, u16 chunk_offset, const void *runtime_profile_chunk_data, u8 chunk_size)
{
    if (sProfile == NULL || runtime_profile_chunk_data == NULL)
        return FALSE;
    if (profile_hash != sProfile->profile_hash)
        return FALSE;
    if (sProfile->blob == NULL)
        return FALSE;
    if (chunk_index >= sProfile->expected_chunks || chunk_index >= MOD_RUNTIME_PROFILE_MAX_CHUNKS)
        return FALSE;
    if (chunk_size == 0 || chunk_size > NET_PROFILE_CHUNK_DATA_SIZE)
        return FALSE;
    if (chunk_offset > sProfile->expected_size || chunk_size > sProfile->expected_size - chunk_offset)
        return FALSE;

    memcpy(&sProfile->blob[chunk_offset], runtime_profile_chunk_data, chunk_size);
    if (!sProfile->chunk_received[chunk_index])
    {
        sProfile->chunk_received[chunk_index] = TRUE;
        sProfile->received_bytes += chunk_size;
    }
    return TRUE;
}

u8 ModRuntimeProfile_CommitReceive(u32 profile_hash)
{
    u16 chunk_index;
    u8 profile_commit_result;

    if (sProfile == NULL || profile_hash != sProfile->profile_hash)
        return MOD_RUNTIME_PROFILE_RESULT_BAD_HASH;
    if (sProfile->active && sProfile->blob == NULL)
        return MOD_RUNTIME_PROFILE_RESULT_OK;
    for (chunk_index = 0; chunk_index < sProfile->expected_chunks; chunk_index++)
    {
        if (!sProfile->chunk_received[chunk_index])
            return MOD_RUNTIME_PROFILE_RESULT_BAD_SIZE;
    }
    if (CalcProfileHash(sProfile->blob, sProfile->expected_size) != profile_hash)
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

    for (text_index = 0; text_index < sProfile->text_count; text_index++)
    {
        if (strcmp(sProfile->texts[text_index].key, key) == 0
         && strcmp(sProfile->texts[text_index].language, language) == 0)
            return sProfile->texts[text_index].text;
    }

    return NULL;
}

bool8 ModRuntimeProfile_GetWeather(struct ModWeatherDisplay *display)
{
    if (!ModRuntimeProfile_IsActive() || display == NULL || !sProfile->has_weather)
        return FALSE;

    *display = sProfile->weather;
    return TRUE;
}

const char *ModRuntimeProfile_GetEngineRulesetId(void)
{
    if (!ModRuntimeProfile_IsActive() || sProfile->engine_ruleset_id[0] == '\0')
        return NULL;

    return sProfile->engine_ruleset_id;
}

const struct ModNpcDefinition *ModRuntimeProfile_FindNpc(const char *key)
{
    u16 npc_index;
    static struct ModNpcDefinition definition;

    if (!ModRuntimeProfile_IsActive() || key == NULL)
        return NULL;

    for (npc_index = 0; npc_index < sProfile->npc_count; npc_index++)
    {
        if (strcmp(sProfile->npcs[npc_index].record.key, key) == 0)
        {
            memset(&definition, 0, sizeof(definition));
            definition.key = sProfile->npcs[npc_index].record.key;
            definition.npc_definition_id = npc_index;
            definition.graphics_id = sProfile->npcs[npc_index].record.graphics_id;
            definition.movement_type = sProfile->npcs[npc_index].record.movement_type;
            definition.local_id = sProfile->npcs[npc_index].record.local_id;
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

    for (asset_index = 0; asset_index < sProfile->asset_count; asset_index++)
    {
        if (strcmp(sProfile->assets[asset_index].key, key) != 0)
            continue;
        if (sProfile->assets[asset_index].source_key[0] != '\0')
            return FindGeneratedAsset(sProfile->assets[asset_index].source_key);
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
        *count = sProfile->badge_effect_count;
    return sProfile->badge_effects;
}

const struct FishingActionDefinition *ModRuntimeProfile_GetFishingActions(u16 *count)
{
    if (count != NULL)
        *count = 0;
    if (!ModRuntimeProfile_IsActive())
        return NULL;

    if (count != NULL)
        *count = sProfile->fishing_action_count;
    return sProfile->fishing_actions;
}

const struct ModEncounterDefinition *ModRuntimeProfile_GetEncounters(u16 *count)
{
    if (count != NULL)
        *count = 0;
    if (!ModRuntimeProfile_IsActive())
        return NULL;

    if (count != NULL)
        *count = sProfile->encounter_count;
    return sProfile->encounters;
}

const struct ModShopDefinition *ModRuntimeProfile_GetShops(u16 *count)
{
    if (count != NULL)
        *count = 0;
    if (!ModRuntimeProfile_IsActive())
        return NULL;

    if (count != NULL)
        *count = sProfile->shop_count;
    return sProfile->shops;
}

const struct ModItemDefinition *ModRuntimeProfile_GetItems(u16 *count)
{
    if (count != NULL)
        *count = 0;
    if (!ModRuntimeProfile_IsActive())
        return NULL;

    if (count != NULL)
        *count = sProfile->item_count;
    return sProfile->items;
}

const struct ModRewardDefinition *ModRuntimeProfile_GetRewards(u16 *count)
{
    if (count != NULL)
        *count = 0;
    if (!ModRuntimeProfile_IsActive())
        return NULL;

    if (count != NULL)
        *count = sProfile->reward_count;
    return sProfile->rewards;
}

const struct ModPokemonDataDefinition *ModRuntimeProfile_GetPokemonData(u16 *count)
{
    if (count != NULL)
        *count = 0;
    if (!ModRuntimeProfile_IsActive())
        return NULL;

    if (count != NULL)
        *count = sProfile->pokemon_data_count;
    return sProfile->pokemon_data;
}

const struct ModBattleMoveDefinition *ModRuntimeProfile_GetBattleMoves(u16 *count)
{
    if (count != NULL)
        *count = 0;
    if (!ModRuntimeProfile_IsActive())
        return NULL;

    if (count != NULL)
        *count = sProfile->battle_move_count;
    return sProfile->battle_moves;
}

const struct ModTrainerDefinition *ModRuntimeProfile_GetTrainers(u16 *count)
{
    if (count != NULL)
        *count = 0;
    if (!ModRuntimeProfile_IsActive())
        return NULL;

    if (count != NULL)
        *count = sProfile->trainer_count;
    return sProfile->trainers;
}

void ModRuntimeProfile_OnMapLoad(void)
{
    u16 npc_index;
    u8 local_id;
    u8 objectEventId;
    const struct ModRuntimeProfileNpcRecord *record;

    if (!ModRuntimeProfile_IsActive() || gSaveBlock1Ptr == NULL)
        return;

    for (npc_index = 0; npc_index < sProfile->npc_count; npc_index++)
    {
        record = &sProfile->npcs[npc_index].record;
        if (record->map_group != gSaveBlock1Ptr->location.mapGroup || record->map_num != gSaveBlock1Ptr->location.mapNum)
            continue;
        local_id = record->local_id;
        if (local_id == 0)
            local_id = MOD_NPC_DYNAMIC_LOCAL_ID_BASE + (npc_index % (MOD_NPC_DYNAMIC_LOCAL_ID_END - MOD_NPC_DYNAMIC_LOCAL_ID_BASE + 1));
        if (TryGetObjectEventIdByLocalIdAndMap(local_id, record->map_num, record->map_group, &objectEventId))
            continue;
        SpawnSpecialObjectEventParameterized(record->graphics_id, record->movement_type, local_id, record->x, record->y, record->elevation);
    }
}
