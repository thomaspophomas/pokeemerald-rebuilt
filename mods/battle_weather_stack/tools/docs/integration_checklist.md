# Battle Weather Stack Integration Checklist

The mod already owns data, normalization, overworld scheduling, debug helpers,
and battle formula helper functions. The remaining engine integration points
belong in rebuilt or battle-script glue code that calls the public helpers from
`src/battle_weather_stack.h`.

## Battle Start

- Read the active overworld layer mask with
  `BattleWeatherStack_GetEffectiveBattleLayers`.
- Merge scripted battle weather only through the stack normalization helpers.
- Call `BattleWeatherStack_GetBattleMessage` after the final mask is known.

## Battle Formulas

- Move power: multiply by `BattleWeatherStack_GetMovePowerPercentEx`.
- Stat modifiers: multiply by `BattleWeatherStack_GetStatPercentEx`.
- Damage taken: multiply by `BattleWeatherStack_GetDamageTakenPercentEx`.
- Weather Ball: use `BattleWeatherStack_GetWeatherBallTypeEx`.
- Suppression abilities should pass `weatherEffectsSuppressed = TRUE` to the
  `...Ex` helpers instead of clearing the stored layer mask.

## End Of Turn

- Read intent flags from `BattleWeatherStack_GetResidualFlagsEx`.
- Apply normal status legality checks before poison or toxic poison.
- Apply normal HP legality checks before healing, leech, or chip damage.
- Keep UI messages tied to actual applied effects, not just raw intent flags.

## Overworld Debug

- Use `BattleWeatherStack_DebugForceOverworldWeather` for deterministic route
  screenshots and emulator checks.
- Use `BattleWeatherStack_DebugForceOverworldLayers` when an admin/debug menu
  should force a compatible two-layer stack in one call.
- Use `BattleWeatherStack_DebugClearOverworldWeather` before save/load tests.
- Capture `BattleWeatherStack_GetOverworldDebugLastRollMinute` when comparing
  route probabilities between builds.
