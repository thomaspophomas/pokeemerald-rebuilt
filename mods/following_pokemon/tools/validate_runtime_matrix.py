#!/usr/bin/env python3
"""Validate the follower runtime matrix shape."""

from __future__ import annotations

import json
from pathlib import Path


MOD_ROOT = Path(__file__).resolve().parents[1]
MATRIX = MOD_ROOT / "tools" / "runtime_matrix.json"


def fail(message: str) -> None:
    raise SystemExit(f"following_pokemon runtime matrix validation failed: {message}")


def main() -> int:
    data = json.loads(MATRIX.read_text(encoding="utf-8"))
    scenarios = data.get("scenarios") if isinstance(data, dict) else None
    if not isinstance(scenarios, list) or len(scenarios) < 10:
        fail("runtime matrix needs at least 10 scenarios")
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
        if not isinstance(scenario.get("expected"), str) or not scenario["expected"]:
            fail(f"scenarios[{index}].expected is required")
    print(f"following_pokemon runtime matrix OK: {len(scenarios)} scenarios")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
