#ifndef GUARD_MOD_OVERWORLD_SPRITE_H
#define GUARD_MOD_OVERWORLD_SPRITE_H

#include "mod/base.h"

#define OVERWORLD_SPRITE_API_INVALID_GFX 0xFF
#define OVERWORLD_SPRITE_API_VIRTUAL_ID_BASE 224
#define OVERWORLD_SPRITE_API_VIRTUAL_ID_END  247
#define OVERWORLD_SPRITE_API_MAX_VIRTUAL_AVATARS (OVERWORLD_SPRITE_API_VIRTUAL_ID_END - OVERWORLD_SPRITE_API_VIRTUAL_ID_BASE + 1)
#define OVERWORLD_SPRITE_API_OWNER_KEY_LENGTH 31

struct ModOverworldSpriteDefinition
{
    const char *key;
    const char *asset_key;
    u8 graphics_id;
    u16 graphics_revision;
};

struct ModFollowerSpriteDefinition
{
    const char *key;
    u16 species;
    u8 form;
    bool8 shiny;
    u8 graphics_id;
};

u8 OverworldSpriteApi_GetGraphicsId(const char *key);
bool8 OverworldSpriteApi_SetObjectGraphics(u8 object_event_id, const char *key);
bool8 OverworldSpriteApi_SetPlayerOutfit(const char *key);
u8 OverworldSpriteApi_GetFollowerSprite(u16 species, u8 form, bool8 shiny);
u8 OverworldSpriteApi_CreateOrUpdateVirtualAvatar(const char *owner_key, const char *sprite_key, s16 x, s16 y);
void OverworldSpriteApi_DestroyVirtualAvatar(const char *owner_key);

#endif // GUARD_MOD_OVERWORLD_SPRITE_H
