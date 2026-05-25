#include "global.h"
#include "battle.h"
#include "generated/mod_registry.h"
#include "mod/battle_sprite.h"
#include "mod/pokeball.h"
#include "palette.h"

const struct ModBattleSpriteDefinition *BattleSpriteApi_GetMonSprite(u16 species, u8 form, u8 side, u16 flags)
{
    u16 battle_sprite_index;

    for (battle_sprite_index = 0; battle_sprite_index < gModBattleSpriteCount; battle_sprite_index++)
    {
        if (gModBattleSprites[battle_sprite_index].species == species
         && gModBattleSprites[battle_sprite_index].form == form
         && gModBattleSprites[battle_sprite_index].side == side
         && (gModBattleSprites[battle_sprite_index].flags & flags) == flags)
            return &gModBattleSprites[battle_sprite_index];
    }

    return NULL;
}

const struct ModBattleSpriteDefinition *BattleSpriteApi_GetTrainerSprite(u16 trainer_id, u8 side)
{
    u16 battle_sprite_index;

    for (battle_sprite_index = 0; battle_sprite_index < gModBattleSpriteCount; battle_sprite_index++)
    {
        if (gModBattleSprites[battle_sprite_index].species == trainer_id && gModBattleSprites[battle_sprite_index].side == side)
            return &gModBattleSprites[battle_sprite_index];
    }

    return NULL;
}

u8 BattleSpriteApi_GetBallSprite(u16 ball_item_id)
{
    return PokeBallApi_GetBallSprite(ball_item_id);
}

const struct ModBattleSpriteDefinition *BattleSpriteApi_GetMoveAnimSprite(const char *key)
{
    u16 battle_sprite_index;

    if (key == NULL)
        return NULL;

    for (battle_sprite_index = 0; battle_sprite_index < gModBattleSpriteCount; battle_sprite_index++)
    {
        if (strcmp(gModBattleSprites[battle_sprite_index].key, key) == 0)
            return &gModBattleSprites[battle_sprite_index];
    }

    return NULL;
}

bool8 BattleSpriteApi_RefreshBattlerPalette(u8 battler_id)
{
    if (battler_id >= MAX_BATTLERS_COUNT)
        return FALSE;

    return TRUE;
}
