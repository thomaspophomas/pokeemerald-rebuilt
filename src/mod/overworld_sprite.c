#include "global.h"
#include "event_object_movement.h"
#include "generated/mod_registry.h"
#include "global.fieldmap.h"
#include "mod/overworld_sprite.h"
#include "mod/sprite_asset.h"

struct VirtualAvatarSlot
{
    bool8 active;
    char ownerKey[OVERWORLD_SPRITE_API_OWNER_KEY_LENGTH + 1];
};

static EWRAM_DATA struct VirtualAvatarSlot sVirtualAvatarSlots[OVERWORLD_SPRITE_API_MAX_VIRTUAL_AVATARS] = {};

static const struct ModOverworldSpriteDefinition *FindOverworldSprite(const char *key)
{
    u16 i;

    if (key == NULL)
        return NULL;

    for (i = 0; i < gModOverworldSpriteCount; i++)
    {
        if (strcmp(gModOverworldSprites[i].key, key) == 0)
            return &gModOverworldSprites[i];
    }

    return NULL;
}

static void CopyOwnerKey(char *dest, const char *src)
{
    u8 i;

    for (i = 0; i < OVERWORLD_SPRITE_API_OWNER_KEY_LENGTH && src[i] != '\0'; i++)
        dest[i] = src[i];
    dest[i] = '\0';
}

static u8 GetVirtualSlotForOwner(const char *ownerKey)
{
    char normalizedOwnerKey[OVERWORLD_SPRITE_API_OWNER_KEY_LENGTH + 1];
    u8 i;

    if (ownerKey == NULL)
        return OVERWORLD_SPRITE_API_MAX_VIRTUAL_AVATARS;

    CopyOwnerKey(normalizedOwnerKey, ownerKey);

    for (i = 0; i < OVERWORLD_SPRITE_API_MAX_VIRTUAL_AVATARS; i++)
    {
        if (sVirtualAvatarSlots[i].active && strcmp(sVirtualAvatarSlots[i].ownerKey, normalizedOwnerKey) == 0)
            return i;
    }

    for (i = 0; i < OVERWORLD_SPRITE_API_MAX_VIRTUAL_AVATARS; i++)
    {
        if (!sVirtualAvatarSlots[i].active)
        {
            sVirtualAvatarSlots[i].active = TRUE;
            strcpy(sVirtualAvatarSlots[i].ownerKey, normalizedOwnerKey);
            return i;
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

bool8 OverworldSpriteApi_SetObjectGraphics(u8 objectEventId, const char *key)
{
    u8 graphicsId = OverworldSpriteApi_GetGraphicsId(key);

    if (graphicsId == OVERWORLD_SPRITE_API_INVALID_GFX)
        return FALSE;
    if (objectEventId >= OBJECT_EVENTS_COUNT || !gObjectEvents[objectEventId].active)
        return FALSE;

    ObjectEventSetGraphicsId(&gObjectEvents[objectEventId], graphicsId);
    return TRUE;
}

bool8 OverworldSpriteApi_SetPlayerOutfit(const char *key)
{
    if (gPlayerAvatar.objectEventId >= OBJECT_EVENTS_COUNT)
        return FALSE;

    return OverworldSpriteApi_SetObjectGraphics(gPlayerAvatar.objectEventId, key);
}

const struct ModFollowerSpriteDefinition *OverworldSpriteApi_FindFollowerSprite(u16 species, u8 form, bool8 shiny)
{
    u16 i;

    for (i = 0; i < gModFollowerSpriteCount; i++)
    {
        if (gModFollowerSprites[i].species == species
         && gModFollowerSprites[i].form == form
         && gModFollowerSprites[i].shiny == shiny)
            return &gModFollowerSprites[i];
    }

    return NULL;
}

u8 OverworldSpriteApi_GetFollowerSprite(u16 species, u8 form, bool8 shiny)
{
    const struct ModFollowerSpriteDefinition *sprite = OverworldSpriteApi_FindFollowerSprite(species, form, shiny);

    if (sprite == NULL)
        return OVERWORLD_SPRITE_API_INVALID_GFX;

    return sprite->graphicsId;
}

u8 OverworldSpriteApi_CreateOrUpdateVirtualAvatar(const char *ownerKey, const char *spriteKey, s16 x, s16 y)
{
    const struct ModOverworldSpriteDefinition *sprite = FindOverworldSprite(spriteKey);
    u8 virtualSlot;

    if (sprite == NULL)
        return SPRITE_NONE;
    if (sprite->assetKey != NULL)
    {
        SpriteAssetApi_LoadSheet(sprite->assetKey);
        SpriteAssetApi_LoadPalette(sprite->assetKey);
    }

    virtualSlot = GetVirtualSlotForOwner(ownerKey);
    if (virtualSlot >= OVERWORLD_SPRITE_API_MAX_VIRTUAL_AVATARS)
        return SPRITE_NONE;

    return CreateOrUpdateVirtualObject(
        sprite->graphicsId,
        OVERWORLD_SPRITE_API_VIRTUAL_ID_BASE + virtualSlot,
        x,
        y,
        ELEVATION_DEFAULT,
        DIR_SOUTH,
        sprite->graphicsRevision);
}

void OverworldSpriteApi_DestroyVirtualAvatar(const char *ownerKey)
{
    char normalizedOwnerKey[OVERWORLD_SPRITE_API_OWNER_KEY_LENGTH + 1];
    u8 i;

    if (ownerKey == NULL)
        return;

    CopyOwnerKey(normalizedOwnerKey, ownerKey);

    for (i = 0; i < OVERWORLD_SPRITE_API_MAX_VIRTUAL_AVATARS; i++)
    {
        if (sVirtualAvatarSlots[i].active && strcmp(sVirtualAvatarSlots[i].ownerKey, normalizedOwnerKey) == 0)
        {
            sVirtualAvatarSlots[i].active = FALSE;
            sVirtualAvatarSlots[i].ownerKey[0] = '\0';
            DestroyVirtualObject(OVERWORLD_SPRITE_API_VIRTUAL_ID_BASE + i);
            return;
        }
    }
}
