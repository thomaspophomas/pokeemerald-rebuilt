#ifndef GUARD_MOD_OVERWORLD_SPRITE_H
#define GUARD_MOD_OVERWORLD_SPRITE_H

#include "global.h"

struct ObjectEventGraphicsInfo;

#define OVERWORLD_SPRITE_API_INVALID_GFX 0xFF
#define OVERWORLD_SPRITE_API_VIRTUAL_ID_BASE 224
#define OVERWORLD_SPRITE_API_VIRTUAL_ID_END  246
#define OVERWORLD_SPRITE_API_FOLLOWER_VIRTUAL_ID 247
#define OVERWORLD_SPRITE_API_MAX_VIRTUAL_AVATARS (OVERWORLD_SPRITE_API_VIRTUAL_ID_END - OVERWORLD_SPRITE_API_VIRTUAL_ID_BASE + 1)
#define OVERWORLD_SPRITE_API_OWNER_KEY_LENGTH 31

struct ModOverworldSpriteDefinition
{
    const char *key;
    const char *assetKey;
    u8 graphicsId;
    u16 graphicsRevision;
};

struct ModFollowerSpriteDefinition
{
    const char *key;
    u16 species;
    u8 form;
    bool8 shiny;
    u8 graphicsId;
    const char *assetKey;
    u16 graphicsRevision;
    const struct ObjectEventGraphicsInfo *graphicsInfo;
};

u8 OverworldSpriteApi_GetGraphicsId(const char *key);
bool8 OverworldSpriteApi_SetObjectGraphics(u8 objectEventId, const char *key);
bool8 OverworldSpriteApi_SetPlayerOutfit(const char *key);
const struct ModFollowerSpriteDefinition *OverworldSpriteApi_FindFollowerSprite(u16 species, u8 form, bool8 shiny);
u8 OverworldSpriteApi_GetFollowerSprite(u16 species, u8 form, bool8 shiny);
u8 OverworldSpriteApi_CreateOrUpdateVirtualAvatar(const char *ownerKey, const char *spriteKey, s16 x, s16 y);
void OverworldSpriteApi_DestroyVirtualAvatar(const char *ownerKey);

#endif // GUARD_MOD_OVERWORLD_SPRITE_H
