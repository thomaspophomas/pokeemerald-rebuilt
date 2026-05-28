# Modding Docs

<!-- last_updated: 2026-05-26 -->

Use `docs/modding_sdk.md` as the public SDK overview. This folder contains the
machine-readable schemas and cookbook examples that back that SDK contract.

## Schemas

JSON schemas live under `docs/modding/schemas`. They describe the manifest,
enabled-modpack file, and all supported domain data folders. `make mod-check`
uses matching validators so errors point to the concrete mod file before C
registries are generated.

## Cookbook

Task-focused examples live under `docs/modding/cookbook`:

- `shop.md`: claim a shop inventory.
- `npc_script.md`: register an NPC script symbol.
- `weather.md`: add a weather provider.
- `state.md`: reserve and migrate persistent mod state.
- `map_script.md`: keep a map-local script include with `map.json`.
