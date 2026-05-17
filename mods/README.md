# Mods Directory

Place build-time mods in subdirectories named `mods/<modId>`.
`mods/vanilla` is reserved for the extracted vanilla source-of-truth mod; do not
hand-edit its generated JSON without rerunning the vanilla extraction checks.

Minimum manifest:

```json
{
  "id": "example",
  "name": "Example Mod",
  "version": "1.0.0",
  "priority": 1000,
  "dependencies": []
}
```

Supported folders:

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

Vanilla extraction domains also reserve these top-level folders under
`mods/vanilla`: `maps`, `npcs`, `trainers`, `trainer_parties`, `weather`,
`time`, `flags`, `events`, `language`, `sprite_assets`, `overworld_sprites`,
`battle_sprites`, `followers`, `outfits`, `pokeballs`, `engine_rulesets`,
`state`, `quests`, `wild_encounters`, `items`, `pokemon`, `moves`, and `shops`.

Vanilla-style domains should include an `index.json` with deterministic
`entries`. Each entry points at an entity JSON file with `id`, `domain`,
`schemaVersion`, `source`, and `legacy`. Mods may provide only the entries they
override or add.

Examples:

```text
mods/example/trainers/TRAINER_ALFONS.json
mods/example/trainer_parties/sParty_Alfons.json
mods/example/maps/AlfonsHouse/map.json
mods/example/maps/AlfonsHouse/scripts.inc
mods/example/npcs/AlfonsHouse/LOCALID_ALFONS.json
mods/example/items/items/ITEM_ALFONS_STONE.json
```

Run `python3 scripts/modgen.py --root .` or let `make generated` run it during
the normal build. Generated files live under `include/generated`,
`src/generated`, and `build/generated`; do not edit those outputs by hand.

NPC entries may include `interactionPolicy`: `EXCLUSIVE` (default),
`SHARED_READONLY`, or `DISABLED_ONLINE`. Only use `SHARED_READONLY` for dialog
that cannot mutate flags, inventory, money, party, warps, battles, shops, or
online-relevant state.
