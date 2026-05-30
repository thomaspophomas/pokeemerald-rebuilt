#!/usr/bin/env python3
"""Focused tests for Battle Weather Stack rule helpers."""

from __future__ import annotations

import importlib.util
import sys
from pathlib import Path


MOD_ROOT = Path(__file__).resolve().parents[1]
WEATHER_RULES = MOD_ROOT / "src" / "tools" / "weather_rules.py"
sys.dont_write_bytecode = True


def load_weather_rules():
    spec = importlib.util.spec_from_file_location("weather_rules", WEATHER_RULES)
    if spec is None or spec.loader is None:
        raise RuntimeError("failed to load weather_rules.py")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def main() -> int:
    weather_rules = load_weather_rules()
    config = weather_rules.load_config()
    errors = weather_rules.validate_config(config)
    if errors:
        raise SystemExit("\n".join(errors))

    assert weather_rules.compatible(config, {"rain"}, "wind")
    assert not weather_rules.compatible(config, {"rain"}, "light")
    assert not weather_rules.compatible(config, {"ash_rain"}, "pollen")
    assert weather_rules.scheduled_lighting(config, "MAPSEC_ROUTE_119", 7 * 60) == "light"
    assert weather_rules.scheduled_lighting(config, "MAPSEC_ROUTE_119", 20 * 60) == "darkness"
    assert weather_rules.scheduled_lighting(config, "MAPSEC_ROUTE_119", 18 * 60) is None

    generated = weather_rules.generate_config_inc(config) + "\n"
    existing = weather_rules.GENERATED_INC.read_text(encoding="utf-8")
    if generated != existing:
        raise SystemExit(f"{weather_rules.GENERATED_INC} is out of date")

    first_route = sorted({rule["mapSection"] for rule in config["overworldRules"]})[0]
    simulation = weather_rules.simulate(config, first_route, days=3, seed=1)
    if f"route: {first_route}" not in simulation or "top combined states:" not in simulation:
        raise SystemExit("simulation output is missing expected sections")

    print("battle_weather_stack rule tests OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
