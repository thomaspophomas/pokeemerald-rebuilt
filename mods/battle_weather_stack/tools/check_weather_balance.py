#!/usr/bin/env python3
"""Run lightweight route simulations and warn about dominant weather states."""

from __future__ import annotations

import argparse
import importlib.util
import re
import sys
from pathlib import Path


MOD_ROOT = Path(__file__).resolve().parents[1]
WEATHER_RULES = MOD_ROOT / "src" / "tools" / "weather_rules.py"
PERCENT_RE = re.compile(r"^\s+([^:]+):\s+([0-9.]+)%$")
sys.dont_write_bytecode = True


def load_weather_rules():
    spec = importlib.util.spec_from_file_location("weather_rules", WEATHER_RULES)
    if spec is None or spec.loader is None:
        raise RuntimeError("failed to load weather_rules.py")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def max_non_clear_state(simulation: str) -> tuple[str, float]:
    in_states = False
    best = ("none", 0.0)
    for line in simulation.splitlines():
        if line == "top combined states:":
            in_states = True
            continue
        if not in_states:
            continue
        match = PERCENT_RE.match(line)
        if match is None:
            continue
        state = match.group(1)
        percent = float(match.group(2))
        if state != "clear" and percent > best[1]:
            best = (state, percent)
    return best


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--days", type=int, default=30)
    parser.add_argument("--seed", type=int, default=1)
    parser.add_argument("--max-dominant-percent", type=float, default=95.0)
    args = parser.parse_args()

    weather_rules = load_weather_rules()
    config = weather_rules.checked_config(weather_rules.CONFIG_JSON)
    routes = sorted({rule["mapSection"] for rule in config["overworldRules"]})
    failures: list[str] = []
    print(f"routes={len(routes)} days={args.days} seed={args.seed}")
    for route in routes:
        state, percent = max_non_clear_state(weather_rules.simulate(config, route, args.days, args.seed))
        print(f"{route}: dominant={state} {percent:.2f}%")
        if percent > args.max_dominant_percent:
            failures.append(f"{route} dominant state {state} is {percent:.2f}%")
    if failures:
        for failure in failures:
            print(f"error: {failure}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
