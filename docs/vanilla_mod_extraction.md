# Vanilla Mod Extraction

`mods/vanilla` is the source of truth for the built-in vanilla content. Legacy
C, INC, and data files remain in place only as materialized mirrors while the
existing build pipeline still consumes them.

## Rules

- All gameplay and mod data must be JSON. GitHub Actions workflow YAML is the
  only YAML exception.
- Every mod API domain has a folder under `mods/vanilla/<domain>`.
- Every mod API domain has a schema under `docs/mod_api_schemas/<domain>.schema.json`.
- Every mod API domain has an expectation file under
  `mods/vanilla/expectations/<domain>/baseline.expected.json`.
- Ambiguous domains use deterministic source-file manifests until their legacy
  C/INC pipeline is replaced by typed materializers.
- Multiplayer v7 protocol, bridge, manifest, resource locks, and
  `OverworldInteraction_*` contracts are not part of this migration and must not
  be rolled back.

## Domains

The vanilla extraction currently covers:

- `maps`
- `npcs`
- `trainers`
- `trainer_parties`
- `weather`
- `time`
- `flags`
- `events`
- `language`
- `sprite_assets`
- `overworld_sprites`
- `battle_sprites`
- `followers`
- `outfits`
- `pokeballs`
- `engine_rulesets`
- `state`
- `quests`
- `wild_encounters`
- `items`
- `pokemon`
- `moves`
- `shops`

Maps are extracted fully from `data/maps`, including all 518 map JSON files,
`map_groups.json`, and each map `scripts.inc` mirror. Trainers are extracted
from `src/data/trainers.h`, with all 855 trainer records represented as JSON
and the legacy C mirror retained. Trainer parties are extracted from
`src/data/trainer_parties.h`, with all 854 party blocks represented as JSON.

## Tooling

Run the full extraction with:

```sh
python scripts/vanilla/vanilla_migration.py extract --root .
```

Run materialized drift checks with:

```sh
python scripts/vanilla/vanilla_migration.py materialize --root . --check
```

Run expectation checks with:

```sh
python scripts/vanilla/vanilla_migration.py check --root .
```

Per-domain wrappers exist as `scripts/vanilla/extract_<domain>.py` and
`scripts/vanilla/materialize_<domain>.py` so future CI jobs can narrow drift
checks to a single API.

## CI Checks

The migration adds:

- `python scripts/ci/check_mod_json.py`
- `python scripts/ci/check_no_game_yaml.py`
- `python scripts/ci/check_vanilla_expectations.py`
- `python scripts/ci/check_vanilla_materialized.py`

`make generated` runs vanilla extraction before `scripts/modgen.py`, so the
generated mod registry sees `mods/vanilla` before it emits C registry files.
