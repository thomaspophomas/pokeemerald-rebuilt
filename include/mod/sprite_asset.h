#ifndef GUARD_MOD_SPRITE_ASSET_H
#define GUARD_MOD_SPRITE_ASSET_H

#include "global.h"
#include "sprite.h"

struct ModSpriteAssetDefinition
{
    const char *key;
    const struct SpriteSheet *sheet;
    const struct CompressedSpriteSheet *compressedSheet;
    const struct SpritePalette *palette;
    const struct CompressedSpritePalette *compressedPalette;
    const struct SpriteTemplate *template;
    u16 tileTag;
    u16 paletteTag;
};

const struct ModSpriteAssetDefinition *SpriteAssetApi_Find(const char *key);
bool8 SpriteAssetApi_LoadSheet(const char *key);
bool8 SpriteAssetApi_LoadPalette(const char *key);
void SpriteAssetApi_Release(const char *key);

#endif // GUARD_MOD_SPRITE_ASSET_H
