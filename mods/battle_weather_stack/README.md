# Battle Weather Stack

Source mod for stacked weather conditions in battle and on the overworld.

Requires the `day_night_cycle` mod. That mod owns the palette pass used for
Ascheregen and Pollenflug tints, so `battle_weather_stack` declares it as a hard
dependency in `mod.json`.

The mod defines up to three simultaneous weather layers, their conflict rules,
combo names, battle messages, battle effects, overworld route rules, visuals,
tints, and helper functions for battle formula hooks. All authored weather data
lives in `src/tools/weather_config.json`; `src/tools/weather_rules.py generate`
emits the C tables in `weather/generated_config.inc`.

## Layers

- `Hell`: weakens Dark and Ghost moves and makes Dark/Ghost Pokemon take more
  damage.
- `Dunkelheit`: strengthens Dark and Ghost moves and makes Dark/Ghost Pokemon
  more resilient.
- `Hitze`: vanilla sun identity, Fire up, Water down, Electric Pokemon Attack
  and Sp. Atk x1.25.
- `Regen`: vanilla rain identity, Water up, Fire down, Electric Pokemon Attack
  and Sp. Atk x1.25.
- `Schnee`: replaces hail; Ice Pokemon take x0.8 damage, with no weather
  chip from this mod.
- `Sandsturm`: vanilla sand identity, including Rock Sp. Def x1.5 and sand chip
  metadata for non-Rock/Ground/Steel Pokemon.
- `Wind`: Flying and Bug Pokemon Speed x1.5; Flying moves x1.2.
- `Absolute Stille`: Psychic, Fighting, and Normal moves x1.2; Pokemon of those
  types take x0.8 damage.
- `Ascheregen`: Poison Pokemon heal 1/16 each turn and take x0.8 damage; non-Poison
  Pokemon are poisoned, while non-Poison Grass Pokemon are badly poisoned; Bug
  Pokemon Attack and Sp. Atk x1.5.
- `Pollenflug`: Grass Pokemon heal 1/16 each turn and take x0.8 damage; Poison
  Pokemon Speed x0.5; non-Grass Pokemon receive the weather leech flag; Bug
  Pokemon Defense and Sp. Def x1.5.

## Conflicts

- Hell + Dunkelheit
- Hell + Regen
- Hell + Ascheregen
- Wind + Absolute Stille
- Ascheregen + Pollenflug
- Hitze + Schnee

When combining layers with `BattleWeatherStack_CombineLayers`, newly added
layers win conflicts. Normalization trims the result to at most three layers.

## Overworld

Overworld weather is route-driven through `BattleWeatherStack_ResolveOverworldWeather`.
Route probabilities are authored in `src/tools/weather_config.json` under
`overworldRules`. Each row defines:

- route or map section
- weather layer
- chance per real-time minute, in 1/10000 units
- min/max duration in minutes
- block time before another random layer may start
- best vanilla weather visual for the current engine

Each weather layer in the same JSON also defines its default overworld visual,
future sprite key, optional tint, battle Weather Ball type, vanilla battle
weather flags, move-power modifiers, stat modifiers, damage-taken modifiers, and
end-of-turn residual flags.

Random weather uses slot 1 and slot 2. A triggered slot stays occupied until its
duration expires, and `blockNewWeatherMinutes` prevents immediate stacking. Slot
3 is reserved for time lighting: `Hell` from 07:00 to 17:59 and `Dunkelheit`
from 20:00 to 04:59. Lighting applies to outdoor maps even when no random route
rules exist. If slot 3 conflicts with an active random layer, the random weather
wins for that interval.

When a minute rolls over, all compatible rules for the current map are evaluated
as one weighted pool. The total rule weight is the chance that any new weather
starts, capped at 100%, and the selected rule is weighted by its own
`chancePerMinute`. Table order does not bias the result.

Active random slots, their absolute end minutes, route visuals, and the block
timer are persisted in this mod's state block. Save/load should continue the
current random weather schedule instead of resetting it on every load.

The current vanilla overworld engine can only render one visible weather at a
time. The provider still exposes the full layer mask for battles/refactor code;
the field task maps that stack to the closest vanilla visual, e.g. rain + wind
as thunderstorm, `Hell` as the Groudon/drought field effect, Ascheregen as ash
particles, and Schnee as the same falling particle idea in white.

Ascheregen and pollen add their screen tint through the required
`day_night_cycle` palette pass: Ascheregen desaturates toward gray, pollen
pushes the map green. No external art assets are bundled.

Validate, regenerate, and simulate route rules with:

```bash
python3 mods/battle_weather_stack/src/tools/weather_rules.py validate
python3 mods/battle_weather_stack/src/tools/weather_rules.py generate --check
python3 mods/battle_weather_stack/tools/test_weather_rules.py
python3 mods/battle_weather_stack/tools/simulate_residual_effects.py --self-test
python3 mods/battle_weather_stack/tools/check_weather_balance.py --days 30
python3 mods/battle_weather_stack/tools/generate_balance_report.py --check
python3 mods/battle_weather_stack/tools/test_visual_priority.py
python3 mods/battle_weather_stack/tools/validate_debug_scenarios.py
python3 mods/battle_weather_stack/tools/validate_debug_command_examples.py
python3 mods/battle_weather_stack/tools/validate_battle_integration_contract.py
python3 mods/battle_weather_stack/src/tools/weather_rules.py simulate --map-section MAPSEC_ROUTE_119 --days 1000 --seed 1
python3 mods/battle_weather_stack/tools/generate_weather_report.py
```

Debug/tuning helpers expose and control the active random slots without
requiring a UI yet:
`BattleWeatherStack_GetOverworldDebugSlotCount`,
`BattleWeatherStack_GetOverworldDebugMapSection`,
`BattleWeatherStack_GetOverworldDebugLastRollMinute`,
`BattleWeatherStack_GetOverworldDebugBlockUntilMinute`,
`BattleWeatherStack_GetOverworldDebugSlotLayer`,
`BattleWeatherStack_GetOverworldDebugSlotEndMinute`, and
`BattleWeatherStack_GetOverworldDebugSlotVanillaWeather`. For scripted testing,
`BattleWeatherStack_DebugClearOverworldWeather`,
`BattleWeatherStack_DebugForceOverworldSlot`, and
`BattleWeatherStack_DebugForceOverworldWeather` can clear or force random slot
weather. Pass `BWS_OVERWORLD_VISUAL_AUTO` to let the mod choose the default
visual for the forced layer. `BattleWeatherStack_DebugForceOverworldLayers`
forces a compatible two-layer stack for admin/debug menus. Deterministic
scenario inputs live in `tools/debug_scenarios.json`; UI command examples live
in `tools/debug_command_examples.json`.

## Battle integration

The public integration prototypes and constants live in
`src/battle_weather_stack.h`. The remaining battle-script/UI glue is tracked in
`tools/docs/integration_checklist.md` so the source mod and rebuilt-side
integration stay clearly separated.

The plain battle helpers apply active weather effects directly. The `...Ex`
variants take a `weatherEffectsSuppressed` flag for Cloud Nine, Air Lock, or
similar suppression checks:

- `BattleWeatherStack_GetEffectiveBattleLayers`
- `BattleWeatherStack_GetMovePowerPercentEx`
- `BattleWeatherStack_GetStatPercentEx`
- `BattleWeatherStack_GetDamageTakenPercentEx`
- `BattleWeatherStack_GetResidualFlagsEx`
- `BattleWeatherStack_GetWeatherBallTypeEx`

Residual flags describe the weather intent. Battle script/application code must
still run normal status and HP-drain legality checks before applying poison,
toxic poison, leech, healing, or chip damage.
`tools/simulate_residual_effects.py` mirrors those legality gates for local
checks with `--status`, `--leech-seeded`, and `--hp-full`.

## Messages

Every named layer/combination has a `name.*` key, a `message.*` key, and message
text in `src/tools/weather_config.json`. Engine or battle-script code should
call `BattleWeatherStack_GetBattleMessage(layers)` after normalizing the active
layer mask.
