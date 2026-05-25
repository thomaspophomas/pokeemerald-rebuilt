#include "global.h"
#include "decompress.h"
#include "generated/mod_registry.h"
#include "mod/runtime_profile.h"
#include "mod/sprite_asset.h"

const struct ModSpriteAssetDefinition *SpriteAssetApi_Find(const char *key)
{
    const struct ModSpriteAssetDefinition *runtimeAsset;
    u16 sprite_asset_index;

    if (key == NULL)
        return NULL;

    runtimeAsset = ModRuntimeProfile_FindAsset(key);
    if (runtimeAsset != NULL)
        return runtimeAsset;

    for (sprite_asset_index = 0; sprite_asset_index < gModSpriteAssetCount; sprite_asset_index++)
    {
        if (strcmp(gModSpriteAssets[sprite_asset_index].key, key) == 0)
            return &gModSpriteAssets[sprite_asset_index];
    }

    return NULL;
}

bool8 SpriteAssetApi_LoadSheet(const char *key)
{
    const struct ModSpriteAssetDefinition *asset = SpriteAssetApi_Find(key);

    if (asset == NULL)
        return FALSE;
    if (asset->compressed_sheet != NULL)
    {
        LoadCompressedSpriteSheet(asset->compressed_sheet);
        return TRUE;
    }
    if (asset->sheet != NULL)
    {
        LoadSpriteSheet(asset->sheet);
        return TRUE;
    }

    return FALSE;
}

bool8 SpriteAssetApi_LoadPalette(const char *key)
{
    const struct ModSpriteAssetDefinition *asset = SpriteAssetApi_Find(key);

    if (asset == NULL)
        return FALSE;
    if (asset->compressed_palette != NULL)
    {
        LoadCompressedSpritePalette(asset->compressed_palette);
        return TRUE;
    }
    if (asset->palette != NULL)
    {
        LoadSpritePalette(asset->palette);
        return TRUE;
    }

    return FALSE;
}

void SpriteAssetApi_Release(const char *key)
{
    const struct ModSpriteAssetDefinition *asset = SpriteAssetApi_Find(key);

    if (asset == NULL)
        return;
    if (asset->tile_tag != TAG_NONE)
        FreeSpriteTilesByTag(asset->tile_tag);
    if (asset->palette_tag != TAG_NONE)
        FreeSpritePaletteByTag(asset->palette_tag);
}
