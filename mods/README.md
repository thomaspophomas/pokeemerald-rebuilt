# Mods Directory

<!-- last_updated: 2026-05-26 -->

Build-time mods live under `mods/<modId>`. The generator reads
`mods/<modId>/mod.json`, scans known domain folders, and writes deterministic C
registries for the mod-facing APIs.

## Build Modes

`FEATURE_MODS=0` is the default. In that mode the build ignores `mods/`, skips
generated mod registries, links vanilla-compatible stubs for `ModApi_*` and the
domain APIs, and can still build the base game plus `FEATURE_MULTIPLAYER=1`.

Use `FEATURE_MODS=1` when you want drop-in mod folders compiled into the ROM:

```bash
make -j"$(nproc)" modern FEATURE_MODS=1
```

`src/mod` and `include/mod` are SDK/runtime adapter code. Concrete gameplay
changes belong under `mods/<modId>`, including optional C entrypoints in
`mods/<modId>/src/*.c`.

Create a starter mod with:

```bash
make mod-new ID=my_mod TYPE=event
```

Supported starter types are `basic`, `event`, `weather`, `npc`, and `item`.

Run either command after changing manifests:

```bash
make mod-check
make mod-ci
make generated FEATURE_MODS=1
python3 scripts/modgen.py --root .
```

Generated files live under `include/generated`, `src/generated`, and
`build/generated`; do not edit those outputs by hand.

The JSON side still uses short keys such as `"id"`. Generated C registries use
explicit field names such as `.mod_id`, `.mod_flag_id`, `.weather_provider_id`,
`.ruleset_id`, `.npc_definition_id`, and `.map_id` so compiler errors point at
the mod-facing concept that changed.

By default every installed mod is active. Add `mods/enabled.json` to pin a
modpack:

```json
{
  "enabled": ["demo", "weather_plus"]
}
```

Local helpers keep that file deterministic:

```bash
make mod-list
make mod-enable ID=demo
make mod-disable ID=demo
```

## Manifest

Minimum `mods/demo/mod.json`:

```json
{
  "id": "demo",
  "name": "Demo Mod",
  "version": "1.0.0",
  "requiresSdk": ">=1",
  "priority": 1000,
  "dependencies": []
}
```

Optional fields:

```json
{
  "id": "demo",
  "name": "Demo Mod",
  "version": "1.0.0",
  "requiresSdk": ">=1",
  "priority": 10,
  "featureFlags": 0,
  "dependencies": ["base_mod"],
  "loadAfter": ["optional_integration"],
  "conflictsWith": ["other_weather_stack"],
  "requiresFeatures": ["FEATURE_MODS"],
  "stateVersion": 1,
  "stateBytes": 16,
  "entrypoints": ["src/demo.c"]
}
```

`id` must match `[a-z0-9][a-z0-9_-]*`. `requiresSdk` accepts simple
requirements such as `>=1`. Mods are sorted by `(priority, id)`. Dependencies
are checked for existence, not for version ranges. `make mod-check` also
rejects enabled conflicts, invalid optional `loadAfter` ordering, missing
entrypoint files, unknown top-level manifest fields, domain JSON shape errors,
and declared `stateBytes` that exceed the current mod save-state budget.

C files in `mods/<modId>/src/*.c` should include the public SDK aggregate:

```c
#include "mod_sdk.h"
```

Mod entrypoints may not include `global.h`, generated registries, `src/*`, or
individual `mod/*.h` headers directly. If the aggregate SDK is missing a type
or function, extend the adapter API instead of coupling the mod to engine
internals.

Copyable examples live under `mods/examples`. They are nested there so the
normal `mods/*/mod.json` build scan does not compile them automatically.

## Key Rules

Most domain entries accept `key`, `id`, or `name`. If the value does not
contain `:`, `scripts/modgen.py` prefixes the active mod id. For example,
`"badge_1"` in mod `demo` becomes `demo:badge_1`.

Use fully qualified keys only when intentionally referencing a shared namespace:

```json
{ "id": "shared:night_music" }
```

Hook-like records may declare `"mode": "observe"`, `"modify"`, `"replace"`, or
`"claim"`. `observe` and `modify` can stack. `replace` and `claim` are
exclusive for the same resource, so two enabled mods cannot both own the same
shop, item, encounter table, map, weather key, event type, or reward key.

## Supported Folders

- `flags/*.json`
- `events/*.json`
- `weather/*.json`
- `time/*.json`
- `badges/*.json`
- `sprites/assets/*.json`
- `sprites/overworld/*.json`
- `sprites/battle/*.json`
- `outfits/*.json`
- `followers/*.json`
- `lang/*.json`
- `pokeballs/*.json`
- `fishing/*.json`
- `items/*.json`
- `rewards/*.json`
- `pokemon/*.json`
- `battle/moves/*.json`
- `trainers/*.json`
- `engines/*.json`
- `npcs/*.json`
- `maps/<MapName>/map.json`
- `maps/<MapName>/scripts.inc`
- `src/*.c`

`maps/<MapName>/scripts.inc` is optional. When `map.json` references it with
`"script": "scripts.inc"`, the generator validates the path, records it in the
map registry, and adds it to `MOD_MAP_SCRIPT_INCS`.

## Domain Examples

`mods/demo/flags/story.json`:

```json
{
  "flags": [
    { "id": "met_guide", "initial": false },
    "opened_gate"
  ]
}
```

`mods/demo/events/hooks.json`:

```json
{
  "subscriptions": [
    { "type": "GAME_INIT", "handler": "Demo_OnGameInit", "priority": 10 },
    { "type": "FLAG_CHANGED", "handler": "Demo_OnFlagChanged", "priority": 20 },
    { "type": "CUSTOM", "handler": "Demo_OnCustomEvent", "priority": 1000 }
  ]
}
```

`type` may be a generated enum token such as `MOD_EVENT_PLAYER_STEP` or one of
the short names accepted by `scripts/modgen.py`: `GAME_INIT`, `SAVE_LOADED`,
`MAP_LOAD`, `MAP_ENTER`, `PLAYER_STEP`, `FLAG_CHANGED`,
`TIME_SEGMENT_CHANGED`, `DAY_CHANGED`, `WEATHER_CHANGED`, `NPC_INTERACT`,
`BATTLE_STARTED`, `BATTLE_ENDED`, or `CUSTOM`.

`mods/demo/weather/providers.json`:

```json
{
  "providers": [
    { "id": "dark_rain", "handler": "Demo_ResolveWeather", "priority": 5 }
  ]
}
```

Lower priority values resolve first. A provider returns `TRUE` after filling a
`struct ModWeatherDisplay`.

`mods/demo/time/segments.json`:

```json
{
  "segments": [
    { "id": "morning", "startMinute": 300, "endMinute": 659, "segment": "MORNING" },
    { "id": "late_night", "startMinute": 1320, "endMinute": 299, "segment": "NIGHT" }
  ]
}
```

`startMinute` and `endMinute` are inclusive minute-of-day values in
`[0, 1439]`. Wrapping ranges such as 22:00 through 04:59 are valid.

`mods/demo/badges/effects.json`:

```json
{
  "effects": [
    {
      "id": "stone_ground_resist",
      "badge": "STONE",
      "effect": "RESISTANCE_PERCENT",
      "target": "TYPE_GROUND",
      "percentPerLevel": 1,
      "maxLevel": 10,
      "flags": 0
    }
  ]
}
```

Badge effects are data records for `BadgeApi_GetEffectPercent`. They do not
change battle math until a battle formula explicitly queries and applies them.
`RESISTANCE_PERCENT` and `DAMAGE_PERCENT` targets must be real Pokemon types
`0..NUMBER_OF_MON_TYPES-1`; `TYPE_NONE` is rejected. `STAT_PERCENT` targets
must be battle stats `0..NUM_BATTLE_STATS-1`. `NONE` is reserved for empty
generated defaults and is not valid in mod JSON or runtime profiles. A vanilla
badge flag still counts as badge level `1`, so setting a modded badge level to
`0` does not suppress an already-earned vanilla badge.

`mods/demo/fishing/actions.json`:

```json
{
  "actions": [
    {
      "id": "extra_reel",
      "hook": "FishingApi_RequestConfiguredAction",
      "hookKey": "configured_button",
      "rods": ["GOOD", "SUPER"],
      "phases": ["INPUT_WINDOW"],
      "buttonMask": "B_BUTTON",
      "timeoutFrames": 24,
      "successOutcome": "CONTINUE",
      "failureOutcome": "GOT_AWAY",
      "promptKey": "press_b_to_reel",
      "params": [1, 2]
    }
  ]
}
```

Fishing actions are optional C hooks that run inside the vanilla fishing task.
With no registered actions, the fishing loop keeps the original bite odds,
dots, timing, inputs, and encounter flow. Hooks receive a `FishingContext` and a
`FishingActionRequest`; returning `CONTINUE` falls through to vanilla behavior,
`OVERRIDE` applies context changes, `REQUEST_ACTION` lets the fishing task wait
for the configured button/timeout, `MEMORY_GAME` starts the built-in up/down
sequence minigame, and `CANCEL` exits through the normal failure path. Valid
phases are `START`, `ROUND_START`, `DOT_CONFIG`, `BITE_CHECK`, `INPUT_WINDOW`,
`MORE_DOTS_CHECK`, `BEFORE_ENCOUNTER`, and `END`. Rods are `OLD`, `GOOD`, and
`SUPER`; `rodMask` may be used instead of `rods`. For `MEMORY_GAME`, params are
max earned level, sequence display frames, per-input timeout frames, and one
reserved slot.

Online profiles may only provide fishing-action data for hooks already compiled
into the ROM. A server record names the compiled source with `sourceKey` and
`hookKey`; it cannot provide function pointers, scripts, or new executable code.

`mods/demo/encounters/route101.json`:

```json
{
  "encounters": [
    {
      "id": "route101_land",
      "map": "MAP_ROUTE101",
      "area": "LAND",
      "encounterRate": 20,
      "priority": 10,
      "hook": "Demo_EncounterHook",
      "hookKey": "scale_level",
      "slots": [
        { "species": "SPECIES_POOCHYENA", "minLevel": 2, "maxLevel": 4, "weight": 100 },
        { "species": "SPECIES_ZIGZAGOON", "minLevel": 2, "maxLevel": 3, "weight": 60 }
      ]
    }
  ]
}
```

Encounter definitions override the current map/area when their map and area
match. Areas are `LAND`, `WATER`, `ROCK_SMASH`, and `FISHING`; fishing
definitions and slots may restrict `rods`/`rodMask` to `OLD`, `GOOD`, or
`SUPER`. Slots use weighted selection and fixed min/max levels. Optional hooks
receive `ModEncounterContext` during rate and species selection and can
continue, override, or cancel the generated result. With no matching
definition, all vanilla encounter rates, ability effects, repel checks, Feebas,
roamers, outbreaks, and battle setup remain on the original path.

Online profiles may provide bounded encounter data directly and may reference
only encounter hooks already advertised in the ROM catalog by `sourceKey` and
`hookKey`.

`mods/demo/shops/oldale.json`:

```json
{
  "shops": [
    {
      "id": "oldale_basic",
      "map": "MAP_OLDALE_TOWN_MART",
      "type": "NORMAL",
      "items": ["ITEM_POKE_BALL", "ITEM_POTION", "ITEM_ANTIDOTE"]
    }
  ]
}
```

Shop definitions replace the item list for the current map and shop type when a
matching clerk opens a mart. Types are `NORMAL`, `DECOR`, and `DECOR2`; item
lists are bounded to `MOD_SHOP_MAX_ITEMS` and must contain real item IDs. With
no matching definition, the script-provided vanilla mart list is used unchanged.
Online profiles may provide shop data directly because it contains no executable
hook code.

`mods/demo/items/items.json`:

```json
{
  "items": [
    {
      "id": "cheap_potion",
      "itemId": "ITEM_POTION",
      "nameKey": "item_cheap_potion_name",
      "descriptionKey": "item_cheap_potion_desc",
      "price": 100,
      "pocket": "POCKET_ITEMS",
      "fieldUseHook": "ItemUseOutOfBattle_Medicine",
      "fieldUseHookKey": "vanilla_medicine"
    }
  ]
}
```

Item definitions can override item metadata and field/battle use functions for
existing item IDs. The generator infers override flags from supplied fields, or
`flags` may name explicit item override bits. Online item profiles may provide
metadata directly, but `fieldUseHookKey` and `battleUseHookKey` must reference
compiled hooks from the ROM catalog.

`mods/demo/rewards/pickup.json`:

```json
{
  "rewards": [
    {
      "id": "early_pickup_potion",
      "source": "PICKUP_COMMON",
      "minLevel": 1,
      "maxLevel": 20,
      "itemId": "ITEM_POTION",
      "quantity": 1
    }
  ]
}
```

Reward definitions currently cover Pickup, rare Pickup, and Battle Pyramid
Pickup sources. They can replace the selected item/quantity or delegate to a
compiled reward hook. Hook references in online profiles use `sourceKey` and
`hookKey`; server profiles cannot introduce new reward code.

`mods/demo/pokemon/poochyena.json`:

```json
{
  "pokemon": [
    {
      "id": "fast_poochyena",
      "species": "SPECIES_POOCHYENA",
      "info": {
        "baseStats": [35, 55, 35, 45, 30, 30],
        "types": ["TYPE_DARK", "TYPE_DARK"],
        "abilities": ["ABILITY_RUN_AWAY", "ABILITY_QUICK_FEET"],
        "growthRate": "GROWTH_MEDIUM_FAST",
        "items": ["ITEM_NONE", "ITEM_NONE"]
      },
      "levelUpMoves": [
        { "level": 1, "move": "MOVE_TACKLE" },
        { "level": 5, "move": "MOVE_HOWL" }
      ],
      "evolutions": [
        { "method": "EVO_LEVEL", "param": 18, "targetSpecies": "SPECIES_MIGHTYENA" }
      ]
    }
  ]
}
```

Pokemon data overrides cover base species info, level-up moves, and evolutions.
Arrays are bounded to six stats, two types, two abilities/egg groups,
`MOD_POKEMON_MAX_LEVEL_UP_MOVES`, and `EVOS_PER_MON`. With no matching
definition, all species reads use vanilla tables.

`mods/demo/battle/moves/moves.json`:

```json
{
  "moves": [
    {
      "id": "tackle_buff",
      "move": "MOVE_TACKLE",
      "power": 45,
      "pp": 35,
      "type": "TYPE_NORMAL"
    }
  ]
}
```

Battle move definitions override selected `struct BattleMove` fields such as
effect, power, type, accuracy, PP, secondary chance, target, priority, and move
flags. `overrideFlags` is inferred from present fields unless supplied
explicitly, and must be non-zero.

`mods/demo/trainers/trainers.json`:

```json
{
  "trainers": [
    {
      "id": "route101_youngster",
      "trainerId": "TRAINER_YOUNGSTER_CALVIN",
      "trainerClass": "TRAINER_CLASS_YOUNGSTER",
      "doubleBattle": false,
      "items": ["ITEM_POTION"],
      "party": [
        {
          "species": "SPECIES_POOCHYENA",
          "level": 5,
          "heldItem": "ITEM_NONE",
          "moves": ["MOVE_TACKLE", "MOVE_HOWL"]
        }
      ]
    }
  ]
}
```

Trainer definitions can override trainer class, pic/music metadata, item list,
AI flags, double-battle status, party size, held items, and moves. Parties are
bounded to `PARTY_SIZE`, each mon can list at most four moves, and absent
definitions fall back to the generated vanilla trainer tables.

`mods/demo/sprites/assets/assets.json`:

```json
{
  "assets": [
    {
      "id": "guide_asset",
      "compressedSheetSymbol": "Demo_GuideSheet",
      "compressedPaletteSymbol": "Demo_GuidePalette",
      "templateSymbol": "Demo_GuideTemplate",
      "tileTag": "0xF100",
      "paletteTag": "0xF101"
    }
  ]
}
```

Sprite assets may use uncompressed `sheetSymbol`/`paletteSymbol`, compressed
`compressedSheetSymbol`/`compressedPaletteSymbol`, and an optional
`templateSymbol`. Symbols must be C identifiers.

`mods/demo/sprites/overworld/sprites.json`:

```json
{
  "sprites": [
    {
      "id": "guide_ow",
      "asset": "guide_asset",
      "graphicsId": "OBJ_EVENT_GFX_BOY_1",
      "graphicsRevision": 1
    }
  ]
}
```

`mods/demo/outfits/outfits.json`:

```json
{
  "outfits": [
    { "id": "raincoat", "asset": "guide_asset", "graphicsId": "OBJ_EVENT_GFX_BOY_1" }
  ]
}
```

`mods/demo/followers/followers.json`:

```json
{
  "followers": [
    { "id": "treecko", "species": "SPECIES_TREECKO", "form": 0, "shiny": false, "graphicsId": "OBJ_EVENT_GFX_BOY_1" }
  ]
}
```

`mods/demo/sprites/battle/sprites.json`:

```json
{
  "sprites": [
    { "id": "treecko_front", "asset": "guide_asset", "species": "SPECIES_TREECKO", "form": 0, "side": 0, "flags": 0 },
    { "id": "demo_trainer", "asset": "guide_asset", "trainerId": "TRAINER_NONE", "side": 0 }
  ]
}
```

`side` uses `BATTLE_SPRITE_SIDE_FRONT`/`0` or `BATTLE_SPRITE_SIDE_BACK`/`1`.

`mods/demo/lang/en.json`:

```json
{
  "language": "en",
  "strings": {
    "npc_intro": "Hello!",
    "shared:ok": "OK"
  }
}
```

The language defaults to the filename when `language` is omitted. Text keys are
unique per `(language, key)`.

`mods/demo/pokeballs/balls.json`:

```json
{
  "pokeballs": [
    {
      "id": "story_ball",
      "itemId": "ITEM_POKE_BALL",
      "ballId": "BALL_PREMIER",
      "catchModifier": 20,
      "flags": 0,
      "catchModifierHook": "Demo_BallModifier",
      "battleScript": "BattleScript_DemoBall",
      "commitHook": "Demo_OnBallCommit"
    }
  ]
}
```

Each Pokeball entry claims one `itemId`; duplicate item IDs fail generation.

`mods/demo/engines/rulesets.json`:

```json
{
  "engines": [
    {
      "id": "demo_engine",
      "name": "Demo Engine",
      "version": 1,
      "saveCompatible": true,
      "captureHook": "Demo_EngineCapture",
      "battleWeatherHook": "Demo_BattleWeather"
    }
  ]
}
```

The built-in default ruleset is `engine:gen3`.

`mods/demo/npcs/npcs.json`:

```json
{
  "npcs": [
    {
      "id": "guide",
      "graphicsId": "OBJ_EVENT_GFX_BOY_1",
      "movementType": "MOVEMENT_TYPE_FACE_DOWN",
      "localId": 208,
      "elevation": 3,
      "flagId": 0,
      "scriptSymbol": "Demo_GuideScript"
    }
  ]
}
```

`localId` values `0xD0` through `0xEF` are reserved for dynamic mod NPCs by
`NpcApi_*`.

`mods/demo/maps/DemoTown/map.json`:

```json
{
  "id": "demo_town",
  "name": "DemoTown",
  "map_group": 0,
  "map_num": 0,
  "script": "scripts.inc"
}
```

Runtime code can call `MapApi_GetScriptPath(map_id)` to inspect the registered
script include path.

`mods/demo/src/demo.c` is picked up automatically. Extra `.c` files listed in
`entrypoints` are also included when they exist.

## Persistent State

Mods reserve save-backed bytes with `stateVersion` and `stateBytes` in
`mod.json`. Runtime code should use the SDK state port:

```c
struct DemoState *state = ModState_GetBlock("demo", sizeof(*state), 1);
```

Use `ModState_IsBlockFresh` for first-run initialization,
`ModState_NeedsMigration` when the saved block version is older than the
manifest version, and `ModState_MarkMigrated` after successful migration. The
registry stores per-mod block versions for the first 16 enabled stateful mods.

## Hook Symbols

Generated registries emit `extern` declarations for hook symbols. Implement
them in `mods/<modId>/src/*.c` or existing source files:

```c
#include "mod_sdk.h"

s8 Demo_OnFlagChanged(const struct ModEvent *event);
bool8 Demo_ResolveWeather(struct ModWeatherDisplay *display);
u8 Demo_BallModifier(const struct PokeBallCatchContext *context);
void Demo_OnBallCommit(const struct PokeBallCatchContext *context, const struct PokeBallThrowResult *result);
u8 Demo_EngineCapture(const struct EngineRuleset *ruleset, u16 ball_item_id);
u32 Demo_BattleWeather(const struct EngineRuleset *ruleset, u16 weather_layers);
extern const u8 Demo_GuideScript[];
extern const u8 BattleScript_DemoBall[];
```

Expected signatures:

- Event handler: `s8 Handler(const struct ModEvent *event)`
- Weather provider: `bool8 Handler(struct ModWeatherDisplay *display)`
- Pokeball catch modifier: `u8 Handler(const struct PokeBallCatchContext *context)`
- Pokeball commit hook: `void Handler(const struct PokeBallCatchContext *context, const struct PokeBallThrowResult *result)`
- Engine capture hook: `u8 Handler(const struct EngineRuleset *ruleset, u16 ball_item_id)`
- Engine battle-weather hook: `u32 Handler(const struct EngineRuleset *ruleset, u16 weather_layers)`
- Reward hook: `u8 Handler(const struct ModRewardDefinition *definition, struct ModRewardContext *context)`
- Item field/battle use hook: `void Handler(u8 task_id)`
- NPC and battle script symbols: `extern const u8 Symbol[]`

## Conflict Rules

Generation fails on:

- duplicate mod IDs
- missing dependencies
- duplicate keys for flags, weather providers, time segments, sprite assets,
  overworld sprites, battle sprites, followers, engine rulesets, NPCs, maps,
  fishing actions, encounters, shops, items, rewards, Pokemon data, battle
  moves, or trainers
- duplicate language keys for the same language
- duplicate Pokeball keys or duplicate Pokeball `itemId` values
- invalid C identifiers for function/script/sprite symbols
- invalid C integer literals/tokens for numeric fields
- time segment bounds outside `[0, 1439]`
- forbidden C entrypoint includes such as `global.h`, generated registries, or
  direct `mod/*.h` headers
- duplicate `replace`/`claim` ownership for the same hook resource

Mod C sources are de-duplicated by path. If two mods need to share code, put the
shared behavior behind a normal source file or a shared mod dependency instead
of relying on duplicate paths.

Cookbook examples live under `docs/modding/cookbook`.
