# Day Night Cycle

<!-- last_updated: 2026-05-24 -->

Source-only compile-time mod for outdoor day/night lighting and time segments.

## Status

This mod registers named time segments through rebuilt's `time/*.json` modgen
domain:

- `night_early`: 00:00-04:59
- `morning`: 05:00-10:59
- `day`: 11:00-16:59
- `evening`: 17:00-20:59
- `night_late`: 21:00-23:59

It also owns the palette tint behavior in `src/day_night_cycle.c`. Hourly tint
values live in `tools/config/tints.json`; `tools/generate_day_night_tints.py`
emits the checked-in `time/generated_tints.inc` table consumed by the C hook.
The tint is attached with generated `ModEvent_*` subscriptions instead of
central rebuilt patches. On map load, the mod creates one late-running overworld
task that:

- only runs while `CB2_Overworld` or `CB2_OverworldBasic` is active
- only tints outdoor map types
- avoids menu BG palettes 14 and 15
- stores the untinted palette ranges it owns
- restores only if its previous tint is still the owner
- suspends tinting when another system changes the palette under it

Debug helpers for emulator or scripted checks live in `src/day_night_cycle.h`:
`DayNightCycle_DebugIsTintApplied`, `DayNightCycle_DebugIsTintSuspended`,
`DayNightCycle_DebugGetAppliedTint`, `DayNightCycle_DebugResetTint`, and
`DayNightCycle_DebugSuspendTint`. Time debugging is available through
`DayNightCycle_DebugSetMinuteOfDay`, `DayNightCycle_DebugClearMinuteOverride`,
`DayNightCycle_DebugGetMinuteOverride`, and `DayNightCycle_DebugSetMinuteStep`.
The minute step helper advances the override time each tint task tick, which
makes emulator regressions fast without compiling a separate time-compression
build.

`tools/config/tints.json` also supports `mapOverrides`, which multiply the
hourly tint for specific `MAPSEC_*` sections such as ash, rainy, or graveyard
routes. `tools/map_override_presets.json` keeps cave, city, forest, water, and
indoor authoring presets outside the compiled config until selected.

`tools/time_windows.example.json` prepares a disabled data domain for future
time-gated evolutions and encounters. It is validated here, but no gameplay
behavior consumes it until rebuilt exposes the needed mod hooks.

## Validate

```bash
cd /home/thomas/pokeemerald-mods
python3 mods/day_night_cycle/tools/validate_day_night_cycle.py
python3 mods/day_night_cycle/tools/validate_map_override_presets.py
python3 mods/day_night_cycle/tools/validate_time_windows.py
python3 mods/day_night_cycle/tools/test_tint_composition.py
python3 mods/day_night_cycle/tools/preview_day_night_tints.py --hour 18
scripts/sync_mod_to_rebuilt.sh day_night_cycle

cd /home/thomas/pokeemerald-rebuilt
python3 scripts/modgen.py --root .
make -j"$(nproc)" modern
```

After changing `tools/config/tints.json`, regenerate the include:

```bash
python3 mods/day_night_cycle/tools/generate_day_night_tints.py
```
