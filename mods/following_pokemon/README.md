# Following Pokemon

<!-- last_updated: 2026-05-24 -->

Source-only compile-time mod for first-party Pokemon follower sprites.

## Contents

- Imported follower sprite graphics and palettes from `pokeemerald-follow`.
- `followers/imported.json` registers imported species mappings with direct
  `ObjectEventGraphicsInfo` symbols.
- `src/follower_assets.c` defines the imported follower graphics info, animation
  tables, and palette/sheet symbols used by the JSON registry.
- `engine_requirements.json` documents the rebuilt-side API support this mod
  expects from `pokeemerald-rebuilt`.
- `PROVENANCE.md` records source and licensing status for the imported assets.
- `TEST_PLAN.md` lists emulator/runtime checks that cannot be proven by static
  asset validation alone.

The source import currently contains 411 imported follower sprite assets, 436
direct imported mappings, 31 generated form aliases, and 934 total follower
mappings after shiny fallbacks are generated.

## Coverage

- Normal follower sprites are provided for the imported species set.
- No shiny-specific sprite sheets are included yet. `followers/shiny_fallbacks.json`
  is generated from all normal mappings and explicitly reuses normal graphics
  for shiny Pokemon until dedicated shiny sheets exist. The replacement plan is
  tracked in `tools/shiny_asset_plan.json`.
- `followers/form_aliases.json` is generated for Castform, Deoxys, and Unown
  form lookup. These aliases reuse the best imported normal asset until real
  per-form sheets are added.
- Native fallback mappings were removed because all former native entries are now
  covered by imported sprites. This avoids relying on JSON file ordering to pick
  a winner for duplicate species/form/shiny tuples.

## Engine Requirements

This mod is a drop-in source mod once `pokeemerald-rebuilt` contains the
follower runtime support described in `engine_requirements.json`.

- follower registry records with `graphicsInfoSymbol`
- sprite asset palette-slot loading
- virtual object creation from direct `ObjectEventGraphicsInfo`
- a runtime follower controller that selects the first usable party Pokemon,
  handles asset release, and hides followers in unsupported overworld states

This repository owns only the source mod folder under
`mods/following_pokemon`; rebuilt owns the shared SDK/runtime support.

## Local Validation

Run the mod-local validator from this repo:

```bash
cd /home/thomas/pokeemerald-mods
python3 mods/following_pokemon/tools/validate_following_pokemon.py
python3 mods/following_pokemon/tools/audit_following_pokemon_assets.py
python3 mods/following_pokemon/tools/report_mapping_coverage.py
python3 mods/following_pokemon/tools/validate_runtime_matrix.py
python3 mods/following_pokemon/tools/validate_screenshot_plan.py
python3 mods/following_pokemon/tools/validate_shiny_asset_plan.py
python3 mods/following_pokemon/tools/estimate_asset_size.py
python3 mods/following_pokemon/tools/generate_screenshot_manifest.py
```

To reimport pinned PNG/PAL source assets from a local clone of the source repo:

```bash
python3 mods/following_pokemon/tools/import_from_pokeemerald_follow.py \
  --reference /home/thomas/.tmp/pokeemerald-follow-ref
```

## Validate

After syncing this mod into a rebuilt checkout:

```bash
cd /home/thomas/pokeemerald-mods
scripts/sync_mod_to_rebuilt.sh following_pokemon /path/to/rebuilt-branch

cd /path/to/rebuilt-branch
python3 scripts/mod_check.py --root .
python3 scripts/ci/modgen_smoke.py
make -j"$(nproc)" modern FEATURE_MODS=1
```
