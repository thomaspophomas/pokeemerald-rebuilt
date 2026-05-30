#ifndef GUARD_MOD_SDK_H
#define GUARD_MOD_SDK_H

/*
 * Stable public SDK entrypoint for drop-in mods.
 *
 * Gameplay mods should include this header directly instead of including
 * global.h, generated registries, or individual engine internals. The headers
 * below are mod-facing ports; implementation details and legacy adapters
 * remain under src/mod and src/engine.
 */

#include "global.h"
#include "mod/base.h"

#include "mod/badge.h"
#include "mod/battle_data.h"
#include "mod/battle_sprite.h"
#include "mod/catalog.h"
#include "mod/core.h"
#include "mod/encounter.h"
#include "mod/engine.h"
#include "mod/event.h"
#include "mod/fishing.h"
#include "mod/flags.h"
#include "mod/item.h"
#include "mod/language.h"
#include "mod/level_cap.h"
#include "mod/map.h"
#include "mod/npc.h"
#include "mod/overworld_sprite.h"
#include "mod/pokeball.h"
#include "mod/pokemon_data.h"
#include "mod/reward.h"
#include "mod/shop.h"
#include "mod/sprite_asset.h"
#include "mod/state.h"
#include "mod/time.h"
#include "mod/trainer.h"
#include "mod/types.h"
#include "mod/weather.h"

#endif // GUARD_MOD_SDK_H
