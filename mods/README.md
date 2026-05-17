# Mods Directory

<!-- last_updated: 2026-05-17 -->

Build-time mods live under `mods/<modId>`. The generator reads
`mods/<modId>/mod.json`, scans known domain folders, and writes deterministic C
registries for the mod-facing APIs.

Run either command after changing manifests:

```bash
make generated
python3 scripts/modgen.py --root .
```

Generated files live under `include/generated`, `src/generated`, and
`build/generated`; do not edit those outputs by hand.

## Manifest

Minimum `mods/demo/mod.json`:

```json
{
  "id": "demo",
  "name": "Demo Mod",
  "version": "1.0.0",
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
  "priority": 10,
  "featureFlags": 0,
  "dependencies": ["base_mod"],
  "entrypoints": ["src/demo.c"]
}
```

`id` must match `[a-z0-9][a-z0-9_-]*`. Mods are sorted by
`(priority, id)`. Dependencies are checked for existence, not for version
ranges.

## Key Rules

Most domain entries accept `key`, `id`, or `name`. If the value does not
contain `:`, `scripts/modgen.py` prefixes the active mod id. For example,
`"badge_1"` in mod `demo` becomes `demo:badge_1`.

Use fully qualified keys only when intentionally referencing a shared namespace:

```json
{ "id": "shared:night_music" }
```

## Supported Folders

- `flags/*.json`
- `events/*.json`
- `weather/*.json`
- `time/*.json`
- `sprites/assets/*.json`
- `sprites/overworld/*.json`
- `sprites/battle/*.json`
- `outfits/*.json`
- `followers/*.json`
- `lang/*.json`
- `pokeballs/*.json`
- `engines/*.json`
- `npcs/*.json`
- `maps/<MapName>/map.json`
- `maps/<MapName>/scripts.inc`
- `src/*.c`

`maps/<MapName>/scripts.inc` is kept with the map folder for source layout, but
the current generator indexes `map.json`; script inclusion still needs the
normal map/script build plumbing.

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
  "map_num": 0
}
```

`mods/demo/src/demo.c` is picked up automatically. Extra `.c` files listed in
`entrypoints` are also included when they exist.

## Hook Symbols

Generated registries emit `extern` declarations for hook symbols. Implement
them in `mods/<modId>/src/*.c` or existing source files:

```c
#include "global.h"
#include "mod/event.h"
#include "mod/engine.h"
#include "mod/pokeball.h"
#include "mod/weather.h"

s8 Demo_OnFlagChanged(const struct ModEvent *event);
bool8 Demo_ResolveWeather(struct ModWeatherDisplay *display);
u8 Demo_BallModifier(const struct PokeBallCatchContext *context);
void Demo_OnBallCommit(const struct PokeBallCatchContext *context, const struct PokeBallThrowResult *result);
u8 Demo_EngineCapture(const struct EngineRuleset *ruleset, u16 ballItemId);
u32 Demo_BattleWeather(const struct EngineRuleset *ruleset, u16 weatherLayers);
extern const u8 Demo_GuideScript[];
extern const u8 BattleScript_DemoBall[];
```

Expected signatures:

- Event handler: `s8 Handler(const struct ModEvent *event)`
- Weather provider: `bool8 Handler(struct ModWeatherDisplay *display)`
- Pokeball catch modifier: `u8 Handler(const struct PokeBallCatchContext *context)`
- Pokeball commit hook: `void Handler(const struct PokeBallCatchContext *context, const struct PokeBallThrowResult *result)`
- Engine capture hook: `u8 Handler(const struct EngineRuleset *ruleset, u16 ballItemId)`
- Engine battle-weather hook: `u32 Handler(const struct EngineRuleset *ruleset, u16 weatherLayers)`
- NPC and battle script symbols: `extern const u8 Symbol[]`

## Conflict Rules

Generation fails on:

- duplicate mod IDs
- missing dependencies
- duplicate keys for flags, weather providers, time segments, sprite assets,
  overworld sprites, battle sprites, followers, engine rulesets, NPCs, or maps
- duplicate language keys for the same language
- duplicate Pokeball keys or duplicate Pokeball `itemId` values
- invalid C identifiers for function/script/sprite symbols
- invalid C integer literals/tokens for numeric fields
- time segment bounds outside `[0, 1439]`

Mod C sources are de-duplicated by path. If two mods need to share code, put the
shared behavior behind a normal source file or a shared mod dependency instead
of relying on duplicate paths.
