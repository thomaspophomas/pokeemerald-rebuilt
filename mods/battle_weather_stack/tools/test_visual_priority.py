#!/usr/bin/env python3
"""Validate overworld visual priority and debug scenario resolution."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any


MOD_ROOT = Path(__file__).resolve().parents[1]
CONFIG = MOD_ROOT / "src" / "tools" / "weather_config.json"
SCENARIOS = MOD_ROOT / "tools" / "debug_scenarios.json"


def fail(message: str) -> None:
    raise SystemExit(f"battle_weather_stack visual priority test failed: {message}")


def load_json(path: Path) -> dict[str, Any]:
    data = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(data, dict):
        fail(f"{path.name} must contain an object")
    return data


def normalize(config: dict[str, Any], layers: set[str]) -> set[str]:
    layers = set(layers)
    for first, second in config["conflicts"]:
        if first in layers and second in layers:
            layers.remove(second)
    result: set[str] = set()
    for layer in config["trimOrder"]:
        if layer in layers and len(result) < config["maxLayers"]:
            result.add(layer)
    return result


def visual_for(config: dict[str, Any], layers: set[str]) -> str:
    for combo in config.get("overworldVisualCombos", []):
        combo_layers = set(combo.get("layers", []))
        if combo_layers and combo_layers <= layers:
            return str(combo["vanillaWeather"])
    layer_info = {layer["id"]: layer for layer in config["layers"]}
    for layer_id in config["overworldVisualPriority"]:
        if layer_id in layers:
            return str(layer_info[layer_id]["overworld"]["defaultVisual"])
    return "WEATHER_NONE"


def main() -> int:
    config = load_json(CONFIG)
    scenarios = load_json(SCENARIOS)
    known_layers = {layer["id"] for layer in config["layers"]}
    priority = config.get("overworldVisualPriority")
    if not isinstance(priority, list) or set(priority) != known_layers:
        fail("overworldVisualPriority must include every layer exactly once")
    for combo in config.get("overworldVisualCombos", []):
        combo_layers = set(combo.get("layers", []))
        if not combo_layers or not combo_layers <= known_layers:
            fail(f"combo references unknown layers: {combo}")
        if not str(combo.get("vanillaWeather", "")).startswith("WEATHER_"):
            fail(f"combo has invalid vanillaWeather: {combo}")

    resolved = 0
    for scenario in scenarios.get("scenarios", []):
        if not isinstance(scenario, dict):
            continue
        layers = set(scenario.get("layers", []))
        if not layers <= known_layers:
            fail(f"scenario {scenario.get('id')} references unknown layers")
        normalized = normalize(config, layers)
        visual = visual_for(config, normalized)
        if not visual.startswith("WEATHER_"):
            fail(f"scenario {scenario.get('id')} resolved invalid visual {visual}")
        resolved += 1
    print(f"battle_weather_stack visual priority OK: {resolved} scenarios")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
