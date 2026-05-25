#ifndef GUARD_MOD_SPRITE_ASSET_H
#define GUARD_MOD_SPRITE_ASSET_H

#include "global.h"
#include "sprite.h"

struct ModSpriteAssetDefinition
{
    const char *key;
    const struct SpriteSheet *sheet;
    const struct CompressedSpriteSheet *compressed_sheet;
    const struct SpritePalette *palette;
    const struct CompressedSpritePalette *compressed_palette;
    const struct SpriteTemplate *template;
    u16 tile_tag;
    u16 palette_tag;
};

const struct ModSpriteAssetDefinition *SpriteAssetApi_Find(const char *key);
bool8 SpriteAssetApi_LoadSheet(const char *key);
bool8 SpriteAssetApi_LoadPalette(const char *key);
void SpriteAssetApi_Release(const char *key);

#endif // GUARD_MOD_SPRITE_ASSET_H
