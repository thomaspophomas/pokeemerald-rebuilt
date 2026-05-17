#ifndef GUARD_MOD_RUNTIME_PROFILE_H
#define GUARD_MOD_RUNTIME_PROFILE_H

#include "global.h"
#include "mod/npc.h"
#include "mod/sprite_asset.h"
#include "mod/weather.h"

#define MOD_RUNTIME_PROFILE_PROTOCOL_VERSION 1
#define MOD_RUNTIME_PROFILE_CAPABILITY_HASH 0x00000001

#define MOD_RUNTIME_PROFILE_CAP_TEXT          (1 << 0)
#define MOD_RUNTIME_PROFILE_CAP_WEATHER       (1 << 1)
#define MOD_RUNTIME_PROFILE_CAP_ENGINE        (1 << 2)
#define MOD_RUNTIME_PROFILE_CAP_NPC           (1 << 3)
#define MOD_RUNTIME_PROFILE_CAP_ASSET_REF     (1 << 4)
#define MOD_RUNTIME_PROFILE_CAP_INLINE_ASSET  (1 << 5)
#define MOD_RUNTIME_PROFILE_CAPABILITIES (MOD_RUNTIME_PROFILE_CAP_TEXT | MOD_RUNTIME_PROFILE_CAP_WEATHER | MOD_RUNTIME_PROFILE_CAP_ENGINE | MOD_RUNTIME_PROFILE_CAP_NPC | MOD_RUNTIME_PROFILE_CAP_ASSET_REF | MOD_RUNTIME_PROFILE_CAP_INLINE_ASSET)

#define MOD_RUNTIME_PROFILE_MAX_BLOB_SIZE 8192
#define MOD_RUNTIME_PROFILE_MAX_KEY_LENGTH 31
#define MOD_RUNTIME_PROFILE_MAX_TEXTS 32
#define MOD_RUNTIME_PROFILE_MAX_TEXT_BYTES 2048
#define MOD_RUNTIME_PROFILE_MAX_NPCS 16
#define MOD_RUNTIME_PROFILE_MAX_ASSETS 8
#define MOD_RUNTIME_PROFILE_MAX_ASSET_BYTES 4096

enum ModRuntimeProfileRecordType
{
    MOD_RUNTIME_PROFILE_RECORD_TEXT = 1,
    MOD_RUNTIME_PROFILE_RECORD_WEATHER,
    MOD_RUNTIME_PROFILE_RECORD_ENGINE,
    MOD_RUNTIME_PROFILE_RECORD_NPC,
    MOD_RUNTIME_PROFILE_RECORD_ASSET_REF,
    MOD_RUNTIME_PROFILE_RECORD_ASSET_INLINE_SHEET,
    MOD_RUNTIME_PROFILE_RECORD_ASSET_INLINE_PALETTE,
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

void ModRuntimeProfile_Init(void);
void ModRuntimeProfile_Clear(void);
bool8 ModRuntimeProfile_IsActive(void);
u32 ModRuntimeProfile_GetActiveHash(void);
bool8 ModRuntimeProfile_BeginReceive(u32 profileHash, u16 profileSize, u16 chunkCount);
bool8 ModRuntimeProfile_ReceiveChunk(u32 profileHash, u16 chunkIndex, u16 offset, const void *data, u8 size);
u8 ModRuntimeProfile_CommitReceive(u32 profileHash);
const u8 *ModRuntimeProfile_GetText(const char *key, const char *language);
bool8 ModRuntimeProfile_GetWeather(struct ModWeatherDisplay *display);
const char *ModRuntimeProfile_GetEngineRulesetId(void);
const struct ModNpcDefinition *ModRuntimeProfile_FindNpc(const char *key);
const struct ModSpriteAssetDefinition *ModRuntimeProfile_FindAsset(const char *key);
void ModRuntimeProfile_OnMapLoad(void);

#endif // GUARD_MOD_RUNTIME_PROFILE_H
