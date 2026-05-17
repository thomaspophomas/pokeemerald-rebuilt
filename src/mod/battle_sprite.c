#include "global.h"
#include "battle.h"
#include "generated/mod_registry.h"
#include "mod/battle_sprite.h"
#include "mod/pokeball.h"
#include "palette.h"

const struct ModBattleSpriteDefinition *BattleSpriteApi_GetMonSprite(u16 species, u8 form, u8 side, u16 flags)
{
    u16 i;

    for (i = 0; i < gModBattleSpriteCount; i++)
    {
        if (gModBattleSprites[i].species == species
         && gModBattleSprites[i].form == form
         && gModBattleSprites[i].side == side
         && (gModBattleSprites[i].flags & flags) == flags)
            return &gModBattleSprites[i];
    }

    return NULL;
}

const struct ModBattleSpriteDefinition *BattleSpriteApi_GetTrainerSprite(u16 trainerId, u8 side)
{
    u16 i;

    for (i = 0; i < gModBattleSpriteCount; i++)
    {
        if (gModBattleSprites[i].species == trainerId && gModBattleSprites[i].side == side)
            return &gModBattleSprites[i];
    }

    return NULL;
}

u8 BattleSpriteApi_GetBallSprite(u16 ballItemId)
{
    return PokeBallApi_GetBallSprite(ballItemId);
}

const struct ModBattleSpriteDefinition *BattleSpriteApi_GetMoveAnimSprite(const char *key)
{
    u16 i;

    if (key == NULL)
        return NULL;

    for (i = 0; i < gModBattleSpriteCount; i++)
    {
        if (strcmp(gModBattleSprites[i].key, key) == 0)
            return &gModBattleSprites[i];
    }

    return NULL;
}

bool8 BattleSpriteApi_RefreshBattlerPalette(u8 battlerId)
{
    if (battlerId >= MAX_BATTLERS_COUNT)
        return FALSE;

    return TRUE;
}
