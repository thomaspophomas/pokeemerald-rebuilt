# Vanilla Mod Extraction

`mods/vanilla` is the source of truth for the built-in vanilla content. Its
layout is intentionally the same shape that later mods should use: every domain
has a stable `index.json`, entity JSON files, optional scripts, and an audit-only
`_source_manifest.json`.

## Rules

- All gameplay and mod data must be JSON. GitHub Actions workflow YAML is the
  only YAML exception.
- Every mod API domain has `mods/vanilla/<domain>/index.json`.
- Every entity JSON contains `id`, `domain`, `schemaVersion`, `source`, and
  `legacy`.
- `_source_manifest.json` is only for drift auditing; it is not the modding API.
- Legacy C/INC data can remain while the build consumes it, but it must be
  referenced or reproducible from the entity JSON structure.
- Multiplayer v7 protocol, bridge, manifest, resource locks, and
  `OverworldInteraction_*` contracts must not be rolled back.

## Layout

The extraction covers `maps`, `npcs`, `trainers`, `trainer_parties`, `weather`,
`time`, `flags`, `events`, `language`, `sprite_assets`, `overworld_sprites`,
`battle_sprites`, `followers`, `outfits`, `pokeballs`, `engine_rulesets`,
`state`, `quests`, `wild_encounters`, `items`, `pokemon`, `moves`, and `shops`.

Important entity layouts:

- `maps/<MapName>/map.json` plus optional `maps/<MapName>/scripts.inc`.
- `npcs/<MapName>/<localId>.json`.
- `trainers/<TRAINER_SYMBOL>.json`.
- `trainer_parties/<sPartySymbol>.json`.
- `flags/flags/<FLAG>.json` and `flags/vars/<VAR>.json`.
- `items/items/<ITEM>.json`, `pokemon/species/<SPECIES>.json`,
  `moves/moves/<MOVE>.json`, and `shops/<ShopSymbol>.json`.
- `pokeballs/balls/<ITEM>.json` describes capture behavior, battle script,
  item icon references, throw sprite references, and opening particle metadata.
- `sprite_assets/assets/<asset>.json`,
  `overworld_sprites/sprites/<OBJ_EVENT_GFX>.json`,
  `battle_sprites/pokemon/<SPECIES>_<side>.json`,
  `battle_sprites/trainers/<TRAINER_PIC>_<side>.json`,
  `followers/followers/<SPECIES>.json`, and `outfits/outfits/<state>.json`
  describe sprite-facing mod concepts rather than raw source files.
- `language/en/<group>.json` groups text keys by natural source group. Runtime
  C strings are marked `runtime: true`; assembly text labels remain editable
  mod data but are not registered as runtime `LanguageApi` strings yet.
- `events/scripts/<ScriptLabel>.json` stores script labels with
  `legacy.rawScript` and detected references to flags, vars, items, trainers,
  and maps.
- `state/*.json` describes save, runtime, and multiplayer state contracts.
- Domains should only fall back to source-file entities when no semantic
  gameplay concept exists yet. For modder-facing domains, `_source_manifest.json`
  carries source-file audit data instead.

## Tooling

Run the full extraction with:

```sh
python scripts/vanilla/vanilla_migration.py extract --root .
```

Run drift checks with:

```sh
python scripts/vanilla/vanilla_migration.py check --root .
```

Per-domain wrappers exist as `scripts/vanilla/extract_<domain>.py`,
`scripts/vanilla/materialize_<domain>.py`, and
`scripts/ci/check_vanilla_<domain>_materialized.py`.

## CI Checks

- `python scripts/ci/check_mod_json.py`
- `python scripts/ci/check_no_game_yaml.py`
- `python scripts/ci/check_vanilla_expectations.py`
- `python scripts/ci/check_vanilla_semantic_coverage.py`
- `python scripts/ci/check_vanilla_materialized.py`

`make generated` runs vanilla extraction before `scripts/modgen.py`, so the
generated mod registry is built from the current `mods/vanilla` indices.
