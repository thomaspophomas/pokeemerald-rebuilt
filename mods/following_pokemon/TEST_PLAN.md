# Following Pokemon Test Plan

Use this plan after the mod is synced into a rebuilt checkout and a ROM is
built. The local validator proves registry and asset shape; this plan covers the
runtime behavior that still needs emulator or device observation.

## Local Static Checks

```bash
cd /home/thomas/pokeemerald-mods
python3 mods/following_pokemon/tools/validate_following_pokemon.py
python3 mods/following_pokemon/tools/audit_following_pokemon_assets.py
```

## Runtime Checks

The machine-readable scenario list lives in `tools/runtime_matrix.json`; keep it
in sync with this checklist.

- Start a new save and confirm no follower is shown before a party Pokemon is
  available.
- Add Bulbasaur, Magikarp, Unown A, a flying Pokemon, and a large Pokemon to the
  party; confirm the first usable party member follows the player.
- Walk, run, turn in place, jump ledges, surf, enter doors, change maps, and
  open/close menus. The follower should hide only in unsupported states and
  return without palette corruption.
- Swap party order and confirm follower graphics update after the next supported
  refresh point.
- Test a shiny Pokemon for fallback behavior. With no shiny-specific imported
  sheet, the normal follower mapping should be used.
- Save, reload, and confirm no duplicate virtual object remains.
- Check one indoor map, one outdoor map, one cave, one route with weather, and
  one battle transition.

## Failure Triage

- Missing sprite: check `followers/imported.json` for the species/form/shiny
  tuple and `src/follower_assets.c` for the referenced graphics info symbol.
- Palette issue: compare the PAL filename with the sheet basename and run the
  asset audit for duplicated or missing pairs.
- Follower persists in invalid state: update rebuilt runtime state gating, not
  the asset registry.
