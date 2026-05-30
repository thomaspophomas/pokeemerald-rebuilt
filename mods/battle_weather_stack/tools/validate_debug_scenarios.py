#!/usr/bin/env python3
"""Validate Battle Weather Stack debug scenarios."""

from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path


MOD_ROOT = Path(__file__).resolve().parents[1]
SCENARIOS = MOD_ROOT / "tools" / "debug_scenarios.json"
WEATHER_RULES = MOD_ROOT / "src" / "tools" / "weather_rules.py"
sys.dont_write_bytecode = True


def fail(message: str) -> None:
    raise SystemExit(f"battle_weather_stack debug scenario validation failed: {message}")


def load_weather_rules():
    spec = importlib.util.spec_from_file_location("weather_rules", WEATHER_RULES)
    if spec is None or spec.loader is None:
        raise RuntimeError("failed to load weather_rules.py")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def main() -> int:
    weather_rules = load_weather_rules()
    config = weather_rules.checked_config(weather_rules.CONFIG_JSON)
    valid_layers = weather_rules.layer_ids(config)
    data = json.loads(SCENARIOS.read_text(encoding="utf-8"))
    scenarios = data.get("scenarios") if isinstance(data, dict) else None
    if not isinstance(scenarios, list) or not scenarios:
        fail("scenarios must be a non-empty list")
    seen: set[str] = set()
    for index, scenario in enumerate(scenarios):
        if not isinstance(scenario, dict):
            fail(f"scenarios[{index}] must be an object")
        scenario_id = scenario.get("id")
        if not isinstance(scenario_id, str) or not scenario_id:
            fail(f"scenarios[{index}].id is required")
        if scenario_id in seen:
            fail(f"duplicate scenario id {scenario_id}")
        seen.add(scenario_id)
        layers = scenario.get("layers")
        if not isinstance(layers, list):
            fail(f"{scenario_id}.layers must be a list")
        for layer in layers:
            if layer not in valid_layers:
                fail(f"{scenario_id}.layers references unknown layer {layer!r}")
    print(f"battle_weather_stack debug scenarios OK: {len(scenarios)} scenarios")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
