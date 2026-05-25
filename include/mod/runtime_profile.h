#ifndef GUARD_MOD_RUNTIME_PROFILE_H
#define GUARD_MOD_RUNTIME_PROFILE_H

#include "global.h"
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
    u16 textSize;
} __attribute__((packed));

struct ModRuntimeProfileWeatherRecord
{
    u8 vanillaWeather;
    u8 priority;
    u16 layers;
    u16 battleWeatherMask;
} __attribute__((packed));

struct ModRuntimeProfileEngineRecord
{
    char rulesetId[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
} __attribute__((packed));

struct ModRuntimeProfileNpcRecord
{
    char key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    u8 graphicsId;
    u8 movementType;
    u8 localId;
    u8 elevation;
    u8 mapGroup;
    u8 mapNum;
    s16 x;
    s16 y;
} __attribute__((packed));

struct ModRuntimeProfileAssetRefRecord
{
    char key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char sourceKey[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
} __attribute__((packed));

struct ModRuntimeProfileInlineSheetRecord
{
    char key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    u16 tileTag;
    u16 size;
} __attribute__((packed));

struct ModRuntimeProfileInlinePaletteRecord
{
    char key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    u16 paletteTag;
    u16 colorCount;
} __attribute__((packed));

struct ModRuntimeProfileBadgeEffectRecord
{
    char key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    u8 badgeId;
    u8 effectKind;
    u8 target;
    s8 percentPerLevel;
    u8 maxLevel;
    u16 flags;
} __attribute__((packed));

struct ModRuntimeProfileFishingActionRecord
{
    char key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char sourceKey[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char hookKey[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char promptKey[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    u8 rodMask;
    u16 phaseMask;
    s16 priority;
    u16 flags;
    u16 buttonMask;
    u16 timeoutFrames;
    u8 successOutcome;
    u8 failureOutcome;
    s16 params[FISHING_ACTION_PARAM_COUNT];
} __attribute__((packed));

struct ModRuntimeProfileEncounterRecord
{
    char key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char sourceKey[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char hookKey[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    u8 mapGroup;
    u8 mapNum;
    u8 area;
    u8 rodMask;
    u8 encounterRate;
    u8 slotCount;
    s16 priority;
    u16 flags;
    struct ModEncounterSlot slots[MOD_ENCOUNTER_MAX_SLOTS];
} __attribute__((packed));

struct ModRuntimeProfileShopRecord
{
    char key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    u8 mapGroup;
    u8 mapNum;
    u8 martType;
    u8 itemCount;
    s16 priority;
    u16 flags;
    u16 items[MOD_SHOP_MAX_ITEMS];
} __attribute__((packed));

struct ModRuntimeProfileItemRecord
{
    char key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char nameKey[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char descriptionKey[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char fieldUseSourceKey[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char fieldUseHookKey[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char battleUseSourceKey[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char battleUseHookKey[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    u16 itemId;
    u16 price;
    u16 flags;
    u8 holdEffect;
    u8 holdEffectParam;
    u8 importance;
    u8 pocket;
    u8 type;
    u8 battleUsage;
    u8 secondaryId;
    s16 priority;
} __attribute__((packed));

struct ModRuntimeProfileRewardRecord
{
    char key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char sourceKey[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    char hookKey[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    u8 source;
    u8 minLevel;
    u8 maxLevel;
    s16 priority;
    u16 flags;
    u16 itemId;
    u16 quantity;
} __attribute__((packed));

struct ModRuntimeProfilePokemonDataRecord
{
    char key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    u16 species;
    s16 priority;
    u16 flags;
    struct ModSpeciesInfoOverride info;
    u8 levelUpMoveCount;
    struct ModLevelUpMoveDefinition levelUpMoves[MOD_POKEMON_MAX_LEVEL_UP_MOVES];
    u8 evolutionCount;
    struct Evolution evolutions[MOD_POKEMON_MAX_EVOLUTIONS];
} __attribute__((packed));

struct ModRuntimeProfileBattleMoveRecord
{
    char key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    u16 move;
    s16 priority;
    u16 overrideFlags;
    struct BattleMove data;
} __attribute__((packed));

struct ModRuntimeProfileTrainerRecord
{
    char key[MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH + 1];
    u16 trainerId;
    s16 priority;
    u16 flags;
    u8 trainerClass;
    u8 encounterMusicGender;
    u8 trainerPic;
    u16 items[MAX_TRAINER_ITEMS];
    bool8 doubleBattle;
    u32 aiFlags;
    u8 partySize;
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
