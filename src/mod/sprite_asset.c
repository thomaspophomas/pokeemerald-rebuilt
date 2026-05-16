#include "global.h"
#include "decompress.h"
#include "generated/mod_registry.h"
#include "mod/sprite_asset.h"

const struct ModSpriteAssetDefinition *SpriteAssetApi_Find(const char *key)
{
    u16 i;

    if (key == NULL)
        return NULL;

    for (i = 0; i < gModSpriteAssetCount; i++)
    {
        if (strcmp(gModSpriteAssets[i].key, key) == 0)
            return &gModSpriteAssets[i];
    }

    return NULL;
}

bool8 SpriteAssetApi_LoadSheet(const char *key)
{
    const struct ModSpriteAssetDefinition *asset = SpriteAssetApi_Find(key);

    if (asset == NULL)
        return FALSE;
    if (asset->compressedSheet != NULL)
    {
        LoadCompressedSpriteSheet(asset->compressedSheet);
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
    if (asset->compressedPalette != NULL)
    {
        LoadCompressedSpritePalette(asset->compressedPalette);
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
    if (asset->tileTag != TAG_NONE)
        FreeSpriteTilesByTag(asset->tileTag);
    if (asset->paletteTag != TAG_NONE)
        FreeSpritePaletteByTag(asset->paletteTag);
}
