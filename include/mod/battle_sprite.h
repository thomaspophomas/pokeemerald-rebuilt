#ifndef GUARD_MOD_BATTLE_SPRITE_H
#define GUARD_MOD_BATTLE_SPRITE_H

#include "global.h"

#define BATTLE_SPRITE_SIDE_FRONT 0
#define BATTLE_SPRITE_SIDE_BACK  1

struct ModBattleSpriteDefinition
{
    const char *key;
    const char *assetKey;
    u16 species;
    u8 form;
    u8 side;
    u16 flags;
};

const struct ModBattleSpriteDefinition *BattleSpriteApi_GetMonSprite(u16 species, u8 form, u8 side, u16 flags);
const struct ModBattleSpriteDefinition *BattleSpriteApi_GetTrainerSprite(u16 trainerId, u8 side);
u8 BattleSpriteApi_GetBallSprite(u16 ballItemId);
const struct ModBattleSpriteDefinition *BattleSpriteApi_GetMoveAnimSprite(const char *key);
bool8 BattleSpriteApi_RefreshBattlerPalette(u8 battlerId);

#endif // GUARD_MOD_BATTLE_SPRITE_H
