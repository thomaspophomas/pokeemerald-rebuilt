# Modding SDK

<!-- last_updated: 2026-05-26 -->

Drop-in mods should treat `include/mod_sdk.h` as the public C SDK boundary. It
defines `MOD_SDK_VERSION`, pulls in the required base types, and aggregates the
stable mod-facing ports for flags, events, weather, time, sprites, language
text, Pokeballs, items, rewards, shops, encounters, Pokemon data, battle data,
trainers, NPCs, maps, engine rulesets, and state.

## C Entrypoints

Use this include from `mods/<modId>/src/*.c`:

```c
#include "mod_sdk.h"

s8 MyMod_OnGameInit(const struct ModEvent *event)
{
    (void)event;
    return MOD_EVENT_RESULT_CONTINUE;
}
```

Do not include `global.h`, `generated/mod_registry.h`, `src/mod/*.c`, or legacy
Emerald engine headers directly from mod code unless the relevant
`include/mod/*.h` port explicitly asks for that type. If a port is missing, add
or extend the adapter instead of writing around it.

## Manifest Checks

Run this before a ROM build:

```bash
make mod-check
```

`mod-check` validates `mods/<modId>/mod.json`, enabled-modpack selection,
dependency/conflict/load-order metadata, declared state byte budgets,
entrypoint paths and includes, hook ownership conflicts, unknown top-level
fields, SDK version requirements, core domain JSON files, and then runs the
normal generator collectors without writing generated files. Use `--json` for
tooling output:

```bash
python3 scripts/mod_check.py --root . --json
```

## Manifest Fields

Required:

- `id`
- `name`
- `version`

Recommended:

- `priority`: deterministic load order. Lower values load first.
- `requiresSdk`: SDK requirement such as `>=1`.
- `dependencies`: mod IDs that must exist and load first.
- `loadAfter`: soft ordering constraints for optional integrations.
- `conflictsWith`: mod IDs that must not be enabled together.
- `requiresFeatures`: compile-time feature gates the mod expects.
- `stateVersion`: integer version for mod-owned persistent data.
- `stateBytes`: declared persistent-state byte budget.
- `entrypoints`: optional C files under the mod folder.

The active loader still sorts by `(priority, id)` after validating metadata, so
prefer explicit `priority` values until a richer resolver is needed.

## Enabled Modpacks

By default every installed `mods/<modId>/mod.json` is active. Add
`mods/enabled.json` when you want a reproducible modpack:

```json
{
  "enabled": ["demo", "weather_plus"]
}
```

Use the helper targets instead of editing the file by hand during local work:

```bash
make mod-list
make mod-enable ID=demo
make mod-disable ID=demo
```

`make mod-check` reports installed, enabled, and disabled mods. The generator
uses only the enabled set, while `mods/examples` remains validation-only sample
content.

## Scaffolding

Create a starter mod with:

```bash
make mod-new ID=my_mod TYPE=event
```

Supported starter types are `basic`, `event`, `weather`, `npc`, and `item`.
The generated folders are valid inputs for `make mod-check`.

## Schemas

The manifest schema lives at `docs/modding/schemas/mod.schema.json`. Domain
schemas cover badges, battle moves, battle sprites, encounters, engine
rulesets, events, fishing actions, flags, followers, items, language strings,
maps, NPCs, overworld sprites, Pokeballs, Pokemon data, rewards, shops, sprite
assets, time segments, trainers, and weather. `make mod-check` enforces the
same core shape with file-specific errors before C registries are generated.

## Hook Modes

Domain records that provide hook-like behavior may declare:

```json
{ "mode": "observe" }
```

Accepted values are `observe`, `modify`, `replace`, and `claim`. `observe` and
`modify` may stack by priority. `replace` and `claim` are exclusive for the
same domain resource; `mod-check` fails if two enabled mods both claim the same
shop, item, encounter table, map, weather key, event type, or reward key.

## Persistent State

Mods reserve persistent bytes with `stateVersion` and `stateBytes` in
`mod.json`. At runtime, access only the assigned block:

```c
struct DemoState *state = ModState_GetBlock("demo", sizeof(*state), 1);
```

`ModState_IsBlockFresh`, `ModState_NeedsMigration`, and
`ModState_MarkMigrated` let a mod initialize or migrate its own block without
peeking into the save layout. The save registry stores per-mod versions for the
first 16 enabled mods that declare state.

## Map Scripts

Map folders may include a local script file:

```json
{
  "id": "demo_town",
  "name": "DemoTown",
  "script": "scripts.inc"
}
```

The generator validates that the path stays inside the map folder, records it
in the map registry, and emits it into `MOD_MAP_SCRIPT_INCS` for build plumbing.
Runtime code can query the registered path with `MapApi_GetScriptPath(map_id)`.

## Generator Layout

`scripts/modgen.py` is the CLI and compatibility facade. The implementation is
split under `scripts/modgen_lib`:

- `common.py`: parser helpers, enum normalization, C literal helpers.
- `manifest.py`: enabled-modpack handling, manifest loading, dependency
  presence checks, load ordering, and state offset assignment.
- `domains/`: per-domain collectors for flags, events, weather, items, NPCs,
  maps, battle data, catalog entries, and mod C sources.
- `emit_c.py`: generated header/source and Makefile fragment emission.
- `pipeline.py`: the end-to-end generation sequence used by the CLI.
