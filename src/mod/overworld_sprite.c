#include "global.h"
#include "event_object_movement.h"
#include "generated/mod_registry.h"
#include "global.fieldmap.h"
#include "mod/overworld_sprite.h"
#include "mod/sprite_asset.h"

struct VirtualAvatarSlot
{
    bool8 active;
    char owner_key[OVERWORLD_SPRITE_API_OWNER_KEY_LENGTH + 1];
};

static EWRAM_DATA struct VirtualAvatarSlot sVirtualAvatarSlots[OVERWORLD_SPRITE_API_MAX_VIRTUAL_AVATARS] = {};

static const struct ModOverworldSpriteDefinition *FindOverworldSprite(const char *key)
{
    u16 overworld_sprite_index;

    if (key == NULL)
        return NULL;

    for (overworld_sprite_index = 0; overworld_sprite_index < gModOverworldSpriteCount; overworld_sprite_index++)
    {
        if (strcmp(gModOverworldSprites[overworld_sprite_index].key, key) == 0)
            return &gModOverworldSprites[overworld_sprite_index];
    }

    return NULL;
}

static void CopyOwnerKey(char *destination_owner_key, const char *source_owner_key)
{
    u8 owner_key_char_index;

    for (owner_key_char_index = 0; owner_key_char_index < OVERWORLD_SPRITE_API_OWNER_KEY_LENGTH && source_owner_key[owner_key_char_index] != '\0'; owner_key_char_index++)
        destination_owner_key[owner_key_char_index] = source_owner_key[owner_key_char_index];
    destination_owner_key[owner_key_char_index] = '\0';
}

static u8 GetVirtualSlotForOwner(const char *owner_key)
{
    char normalized_owner_key[OVERWORLD_SPRITE_API_OWNER_KEY_LENGTH + 1];
    u8 virtual_avatar_index;

    if (owner_key == NULL)
        return OVERWORLD_SPRITE_API_MAX_VIRTUAL_AVATARS;

    CopyOwnerKey(normalized_owner_key, owner_key);

    for (virtual_avatar_index = 0; virtual_avatar_index < OVERWORLD_SPRITE_API_MAX_VIRTUAL_AVATARS; virtual_avatar_index++)
    {
        if (sVirtualAvatarSlots[virtual_avatar_index].active && strcmp(sVirtualAvatarSlots[virtual_avatar_index].owner_key, normalized_owner_key) == 0)
            return virtual_avatar_index;
    }

    for (virtual_avatar_index = 0; virtual_avatar_index < OVERWORLD_SPRITE_API_MAX_VIRTUAL_AVATARS; virtual_avatar_index++)
    {
        if (!sVirtualAvatarSlots[virtual_avatar_index].active)
        {
            sVirtualAvatarSlots[virtual_avatar_index].active = TRUE;
            strcpy(sVirtualAvatarSlots[virtual_avatar_index].owner_key, normalized_owner_key);
            return virtual_avatar_index;
        }
    }

    return OVERWORLD_SPRITE_API_MAX_VIRTUAL_AVATARS;
}

u8 OverworldSpriteApi_GetGraphicsId(const char *key)
{
    const struct ModOverworldSpriteDefinition *sprite = FindOverworldSprite(key);

    if (sprite == NULL)
        return OVERWORLD_SPRITE_API_INVALID_GFX;

    return sprite->graphicsId;
}

bool8 OverworldSpriteApi_SetObjectGraphics(u8 object_event_id, const char *key)
{
    u8 graphicsId = OverworldSpriteApi_GetGraphicsId(key);

    if (graphicsId == OVERWORLD_SPRITE_API_INVALID_GFX)
        return FALSE;
    if (object_event_id >= OBJECT_EVENTS_COUNT || !gObjectEvents[object_event_id].active)
        return FALSE;

    ObjectEventSetGraphicsId(&gObjectEvents[object_event_id], graphicsId);
    return TRUE;
}

bool8 OverworldSpriteApi_SetPlayerOutfit(const char *key)
{
    if (gPlayerAvatar.objectEventId >= OBJECT_EVENTS_COUNT)
        return FALSE;

    return OverworldSpriteApi_SetObjectGraphics(gPlayerAvatar.objectEventId, key);
}

u8 OverworldSpriteApi_GetFollowerSprite(u16 species, u8 form, bool8 shiny)
{
    u16 follower_sprite_index;

    for (follower_sprite_index = 0; follower_sprite_index < gModFollowerSpriteCount; follower_sprite_index++)
    {
        if (gModFollowerSprites[follower_sprite_index].species == species
         && gModFollowerSprites[follower_sprite_index].form == form
         && gModFollowerSprites[follower_sprite_index].shiny == shiny)
            return gModFollowerSprites[follower_sprite_index].graphicsId;
    }

    return OVERWORLD_SPRITE_API_INVALID_GFX;
}

u8 OverworldSpriteApi_CreateOrUpdateVirtualAvatar(const char *owner_key, const char *sprite_key, s16 x, s16 y)
{
    const struct ModOverworldSpriteDefinition *sprite = FindOverworldSprite(sprite_key);
    u8 virtual_avatar_slot;

    if (sprite == NULL)
        return SPRITE_NONE;
    if (sprite->assetKey != NULL)
    {
        SpriteAssetApi_LoadSheet(sprite->assetKey);
        SpriteAssetApi_LoadPalette(sprite->assetKey);
    }

    virtual_avatar_slot = GetVirtualSlotForOwner(owner_key);
    if (virtual_avatar_slot >= OVERWORLD_SPRITE_API_MAX_VIRTUAL_AVATARS)
        return SPRITE_NONE;

    return CreateOrUpdateVirtualObject(
        sprite->graphicsId,
        OVERWORLD_SPRITE_API_VIRTUAL_ID_BASE + virtual_avatar_slot,
        x,
        y,
        ELEVATION_DEFAULT,
        DIR_SOUTH,
        sprite->graphicsRevision);
}

void OverworldSpriteApi_DestroyVirtualAvatar(const char *owner_key)
{
    char normalized_owner_key[OVERWORLD_SPRITE_API_OWNER_KEY_LENGTH + 1];
    u8 virtual_avatar_index;

    if (owner_key == NULL)
        return;

    CopyOwnerKey(normalized_owner_key, owner_key);

    for (virtual_avatar_index = 0; virtual_avatar_index < OVERWORLD_SPRITE_API_MAX_VIRTUAL_AVATARS; virtual_avatar_index++)
    {
        if (sVirtualAvatarSlots[virtual_avatar_index].active && strcmp(sVirtualAvatarSlots[virtual_avatar_index].owner_key, normalized_owner_key) == 0)
        {
            sVirtualAvatarSlots[virtual_avatar_index].active = FALSE;
            sVirtualAvatarSlots[virtual_avatar_index].owner_key[0] = '\0';
            DestroyVirtualObject(OVERWORLD_SPRITE_API_VIRTUAL_ID_BASE + virtual_avatar_index);
            return;
        }
    }
}
