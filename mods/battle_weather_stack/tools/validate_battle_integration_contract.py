#!/usr/bin/env python3
"""Validate public battle integration functions and generated config bindings."""

from __future__ import annotations

import json
import re
from pathlib import Path


MOD_ROOT = Path(__file__).resolve().parents[1]
HEADER = MOD_ROOT / "src" / "battle_weather_stack.h"
SOURCE = MOD_ROOT / "src" / "battle_weather_stack.c"
CONFIG = MOD_ROOT / "src" / "tools" / "weather_config.json"
GENERATED = MOD_ROOT / "weather" / "generated_config.inc"

REQUIRED_FUNCTIONS = [
    "BattleWeatherStack_GetBattleMessage",
    "BattleWeatherStack_BattleWeather",
    "BattleWeatherStack_GetMovePowerPercentEx",
    "BattleWeatherStack_GetStatPercentEx",
    "BattleWeatherStack_GetDamageTakenPercentEx",
    "BattleWeatherStack_GetResidualFlagsEx",
    "BattleWeatherStack_GetWeatherBallTypeEx",
    "BattleWeatherStack_DebugForceOverworldLayers",
]


def fail(message: str) -> None:
    raise SystemExit(f"battle_weather_stack integration contract validation failed: {message}")


def main() -> int:
    header = HEADER.read_text(encoding="utf-8")
    source = SOURCE.read_text(encoding="utf-8")
    generated = GENERATED.read_text(encoding="utf-8")
    config = json.loads(CONFIG.read_text(encoding="utf-8"))

    for function in REQUIRED_FUNCTIONS:
        if function not in header:
            fail(f"{function} missing from header")
        if re.search(rf"\b{re.escape(function)}\s*\(", source) is None:
            fail(f"{function} missing from source")

    for array_name in (
        "sMessageText",
        "sLayerInfo",
        "sResidualEffects",
        "sWeatherBallOrder",
        "sOverworldVisualPriority",
        "sOverworldVisualCombos",
    ):
        if array_name not in generated:
            fail(f"{array_name} missing from generated_config.inc")

    for layer in config.get("layers", []):
        if not isinstance(layer, dict):
            continue
        if "messageKey" not in layer or "nameKey" not in layer:
            fail(f"layer {layer.get('id')} must define nameKey and messageKey")
        battle = layer.get("battle", {})
        if not isinstance(battle, dict) or "weatherBallType" not in battle:
            fail(f"layer {layer.get('id')} must define battle.weatherBallType")

    print(f"battle_weather_stack integration contract OK: {len(REQUIRED_FUNCTIONS)} functions")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
