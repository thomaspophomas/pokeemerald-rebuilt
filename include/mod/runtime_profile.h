#ifndef GUARD_MOD_RUNTIME_PROFILE_H
#define GUARD_MOD_RUNTIME_PROFILE_H

#include "mod/base.h"
#include "mod/badge.h"
#include "mod/battle_data.h"
#include "mod/encounter.h"
#include "mod/fishing.h"
#include "mod/item.h"
#include "mod/npc.h"
#include "mod/pokemon_data.h"
#include "mod/reward.h"
#include "mod/shop.h"
#include "mod/sprite_asset.h"
#include "mod/trainer.h"
#include "mod/weather.h"

#define MOD_RUNTIME_PROFILE_PROTOCOL_VERSION 1
#define MOD_RUNTIME_PROFILE_CAPABILITY_HASH 0x00000006

#define MOD_RUNTIME_PROFILE_CAP_TEXT          (1 << 0)
#define MOD_RUNTIME_PROFILE_CAP_WEATHER       (1 << 1)
#define MOD_RUNTIME_PROFILE_CAP_ENGINE        (1 << 2)
#define MOD_RUNTIME_PROFILE_CAP_NPC           (1 << 3)
#define MOD_RUNTIME_PROFILE_CAP_ASSET_REF     (1 << 4)
#define MOD_RUNTIME_PROFILE_CAP_INLINE_ASSET  (1 << 5)
#define MOD_RUNTIME_PROFILE_CAP_BADGE_EFFECTS (1 << 6)
#define MOD_RUNTIME_PROFILE_CAP_FISHING_ACTIONS (1 << 7)
#define MOD_RUNTIME_PROFILE_CAP_ENCOUNTERS    (1 << 8)
#define MOD_RUNTIME_PROFILE_CAP_SHOPS         (1 << 9)
#define MOD_RUNTIME_PROFILE_CAP_ITEMS         (1 << 10)
#define MOD_RUNTIME_PROFILE_CAP_REWARDS       (1 << 11)
#define MOD_RUNTIME_PROFILE_CAP_POKEMON_DATA  (1 << 12)
#define MOD_RUNTIME_PROFILE_CAP_BATTLE_DATA   (1 << 13)
#define MOD_RUNTIME_PROFILE_CAP_TRAINERS      (1 << 14)
#define MOD_RUNTIME_PROFILE_CAPABILITIES (MOD_RUNTIME_PROFILE_CAP_TEXT | MOD_RUNTIME_PROFILE_CAP_WEATHER | MOD_RUNTIME_PROFILE_CAP_ENGINE | MOD_RUNTIME_PROFILE_CAP_NPC | MOD_RUNTIME_PROFILE_CAP_ASSET_REF | MOD_RUNTIME_PROFILE_CAP_INLINE_ASSET | MOD_RUNTIME_PROFILE_CAP_BADGE_EFFECTS | MOD_RUNTIME_PROFILE_CAP_FISHING_ACTIONS | MOD_RUNTIME_PROFILE_CAP_ENCOUNTERS | MOD_RUNTIME_PROFILE_CAP_SHOPS | MOD_RUNTIME_PROFILE_CAP_ITEMS | MOD_RUNTIME_PROFILE_CAP_REWARDS | MOD_RUNTIME_PROFILE_CAP_POKEMON_DATA | MOD_RUNTIME_PROFILE_CAP_BATTLE_DATA | MOD_RUNTIME_PROFILE_CAP_TRAINERS)

#define MOD_RUNTIME_PROFILE_MAX_BLOB_SIZE 8192
#define MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH 31
#define MOD_RUNTIME_PROFILE_MAX_TEXTS 32
#define MOD_RUNTIME_PROFILE_MAX_TEXT_BYTES 2048
#define MOD_RUNTIME_PROFILE_MAX_NPCS 16
#define MOD_RUNTIME_PROFILE_MAX_ASSETS 8
#define MOD_RUNTIME_PROFILE_MAX_ASSET_BYTES 4096
#define MOD_RUNTIME_PROFILE_MAX_BADGE_EFFECTS 32
#define MOD_RUNTIME_PROFILE_MAX_FISHING_ACTIONS 16
#define MOD_RUNTIME_PROFILE_MAX_ENCOUNTERS 16
#define MOD_RUNTIME_PROFILE_MAX_SHOPS 16
#define MOD_RUNTIME_PROFILE_MAX_ITEMS 32
#define MOD_RUNTIME_PROFILE_MAX_REWARDS 32
#define MOD_RUNTIME_PROFILE_MAX_POKEMON_DATA 32
#define MOD_RUNTIME_PROFILE_MAX_BATTLE_MOVES 64
#define MOD_RUNTIME_PROFILE_MAX_TRAINERS 32

enum ModRuntimeProfileRecordType
{
    MOD_RUNTIME_PROFILE_RECORD_TEXT = 1,
    MOD_RUNTIME_PROFILE_RECORD_WEATHER,
    MOD_RUNTIME_PROFILE_RECORD_ENGINE,
    MOD_RUNTIME_PROFILE_RECORD_NPC,
    MOD_RUNTIME_PROFILE_RECORD_ASSET_REF,
    MOD_RUNTIME_PROFILE_RECORD_ASSET_INLINE_SHEET,
    MOD_RUNTIME_PROFILE_RECORD_ASSET_INLINE_PALETTE,
    MOD_RUNTIME_PROFILE_RECORD_BADGE_EFFECT,
    MOD_RUNTIME_PROFILE_RECORD_FISHING_ACTION,
    MOD_RUNTIME_PROFILE_RECORD_ENCOUNTER,
    MOD_RUNTIME_PROFILE_RECORD_SHOP,
    MOD_RUNTIME_PROFILE_RECORD_ITEM,
    MOD_RUNTIME_PROFILE_RECORD_REWARD,
    MOD_RUNTIME_PROFILE_RECORD_POKEMON_DATA,
    MOD_RUNTIME_PROFILE_RECORD_BATTLE_MOVE,
    MOD_RUNTIME_PROFILE_RECORD_TRAINER,
};

enum ModRuntimeProfileResult
{
    MOD_RUNTIME_PROFILE_RESULT_OK,
    MOD_RUNTIME_PROFILE_RESULT_UNSUPPORTED_VERSION,
    MOD_RUNTIME_PROFILE_RESULT_UNSUPPORTED_CAPABILITY,
    MOD_RUNTIME_PROFILE_RESULT_BAD_HASH,
    MOD_RUNTIME_PROFILE_RESULT_BAD_SIZE,
    MOD_RUNTIME_PROFILE_RESULT_OUT_OF_MEMORY,
    MOD_RUNTIME_PROFILE_RESULT_BAD_RECORD,
};

struct ModRuntimeProfileRecordHeader
{
    u8 type;
    u8 flags;
    u16 size;
} __attribute__((packed));

struct ModRuntimeProfileTextRecord
{
    char language[8];
    char key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    u16 text_size;
} __attribute__((packed));

struct ModRuntimeProfileWeatherRecord
{
    u8 vanilla_weather;
    u8 priority;
    u16 layers;
    u16 battle_weather_mask;
} __attribute__((packed));

struct ModRuntimeProfileEngineRecord
{
    char ruleset_id[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
} __attribute__((packed));

struct ModRuntimeProfileNpcRecord
{
    char key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    u8 graphics_id;
    u8 movement_type;
    u8 local_id;
    u8 elevation;
    u8 map_group;
    u8 map_num;
    s16 x;
    s16 y;
} __attribute__((packed));

struct ModRuntimeProfileAssetRefRecord
{
    char key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char source_key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
} __attribute__((packed));

struct ModRuntimeProfileInlineSheetRecord
{
    char key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    u16 tile_tag;
    u16 size;
} __attribute__((packed));

struct ModRuntimeProfileInlinePaletteRecord
{
    char key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    u16 palette_tag;
    u16 color_count;
} __attribute__((packed));

struct ModRuntimeProfileBadgeEffectRecord
{
    char key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    u8 badge_id;
    u8 effect_kind;
    u8 target;
    s8 percent_per_level;
    u8 max_level;
    u16 flags;
} __attribute__((packed));

struct ModRuntimeProfileFishingActionRecord
{
    char key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char source_key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char hook_key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char prompt_key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    u8 rod_mask;
    u16 phase_mask;
    s16 priority;
    u16 flags;
    u16 button_mask;
    u16 timeout_frames;
    u8 success_outcome;
    u8 failure_outcome;
    s16 params[FISHING_ACTION_PARAM_COUNT];
} __attribute__((packed));

struct ModRuntimeProfileEncounterRecord
{
    char key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char source_key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char hook_key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    u8 map_group;
    u8 map_num;
    u8 area;
    u8 rod_mask;
    u8 encounter_rate;
    u8 slot_count;
    s16 priority;
    u16 flags;
    struct ModEncounterSlot slots[MOD_ENCOUNTER_MAX_SLOTS];
} __attribute__((packed));

struct ModRuntimeProfileShopRecord
{
    char key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    u8 map_group;
    u8 map_num;
    u8 mart_type;
    u8 item_count;
    s16 priority;
    u16 flags;
    u16 items[MOD_SHOP_MAX_ITEMS];
} __attribute__((packed));

struct ModRuntimeProfileItemRecord
{
    char key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char name_key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char description_key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char field_use_source_key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char field_use_hook_key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char battle_use_source_key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char battle_use_hook_key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    u16 item_id;
    u16 price;
    u16 flags;
    u8 hold_effect;
    u8 hold_effect_param;
    u8 importance;
    u8 pocket;
    u8 type;
    u8 battle_usage;
    u8 secondary_id;
    s16 priority;
} __attribute__((packed));

struct ModRuntimeProfileRewardRecord
{
    char key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char source_key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char hook_key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    u8 source;
    u8 min_level;
    u8 max_level;
    s16 priority;
    u16 flags;
    u16 item_id;
    u16 quantity;
} __attribute__((packed));

struct ModRuntimeProfilePokemonDataRecord
{
    char key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    u16 species;
    s16 priority;
    u16 flags;
    struct ModSpeciesInfoOverride info;
    u8 level_up_move_count;
    struct ModLevelUpMoveDefinition level_up_moves[MOD_POKEMON_MAX_LEVEL_UP_MOVES];
    u8 evolution_count;
    struct Evolution evolutions[MOD_POKEMON_MAX_EVOLUTIONS];
} __attribute__((packed));

struct ModRuntimeProfileBattleMoveRecord
{
    char key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    u16 move;
    s16 priority;
    u16 override_flags;
    struct BattleMove data;
} __attribute__((packed));

struct ModRuntimeProfileTrainerRecord
{
    char key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    u16 trainer_id;
    s16 priority;
    u16 flags;
    u8 trainer_class;
    u8 encounter_music_gender;
    u8 trainer_pic;
    u16 items[MAX_TRAINER_ITEMS];
    bool8 double_battle;
    u32 ai_flags;
    u8 party_size;
    struct ModTrainerMon party[MOD_TRAINER_MAX_PARTY_SIZE];
} __attribute__((packed));

void ModRuntimeProfile_Init(void);
void ModRuntimeProfile_Clear(void);
bool8 ModRuntimeProfile_IsActive(void);
u32 ModRuntimeProfile_GetActiveHash(void);
bool8 ModRuntimeProfile_BeginReceive(u32 profile_hash, u16 profile_size, u16 chunk_count);
bool8 ModRuntimeProfile_ReceiveChunk(u32 profile_hash, u16 chunk_index, u16 chunk_offset, const void *runtime_profile_chunk_data, u8 chunk_size);
u8 ModRuntimeProfile_CommitReceive(u32 profile_hash);
const u8 *ModRuntimeProfile_GetText(const char *key, const char *language);
bool8 ModRuntimeProfile_GetWeather(struct ModWeatherDisplay *display);
const char *ModRuntimeProfile_GetEngineRulesetId(void);
const struct ModNpcDefinition *ModRuntimeProfile_FindNpc(const char *key);
const struct ModSpriteAssetDefinition *ModRuntimeProfile_FindAsset(const char *key);
const struct ModBadgeEffectDefinition *ModRuntimeProfile_GetBadgeEffects(u16 *count);
const struct FishingActionDefinition *ModRuntimeProfile_GetFishingActions(u16 *count);
const struct ModEncounterDefinition *ModRuntimeProfile_GetEncounters(u16 *count);
const struct ModShopDefinition *ModRuntimeProfile_GetShops(u16 *count);
const struct ModItemDefinition *ModRuntimeProfile_GetItems(u16 *count);
const struct ModRewardDefinition *ModRuntimeProfile_GetRewards(u16 *count);
const struct ModPokemonDataDefinition *ModRuntimeProfile_GetPokemonData(u16 *count);
const struct ModBattleMoveDefinition *ModRuntimeProfile_GetBattleMoves(u16 *count);
const struct ModTrainerDefinition *ModRuntimeProfile_GetTrainers(u16 *count);
void ModRuntimeProfile_OnMapLoad(void);

#endif // GUARD_MOD_RUNTIME_PROFILE_H
