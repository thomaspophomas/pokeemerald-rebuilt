# Mods Directory

Place build-time mods in subdirectories named `mods/<modId>`.

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

Run `python3 scripts/modgen.py --root .` or let `make generated` run it during
the normal build. Generated files live under `include/generated`,
`src/generated`, and `build/generated`; do not edit those outputs by hand.

NPC entries may include `interactionPolicy`: `EXCLUSIVE` (default),
`SHARED_READONLY`, or `DISABLED_ONLINE`. Only use `SHARED_READONLY` for dialog
that cannot mutate flags, inventory, money, party, warps, battles, shops, or
online-relevant state.
