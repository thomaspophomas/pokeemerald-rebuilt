#!/usr/bin/env python3
"""Print deterministic screenshot targets from the follower runtime plan."""

from __future__ import annotations

import argparse
import json
from pathlib import Path


MOD_ROOT = Path(__file__).resolve().parents[1]
RUNTIME = MOD_ROOT / "tools" / "runtime_matrix.json"
PLAN = MOD_ROOT / "tools" / "screenshot_plan.json"


def load_json(path: Path) -> dict:
    data = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(data, dict):
        raise SystemExit(f"{path} must contain an object")
    return data


def manifest() -> list[str]:
    runtime = load_json(RUNTIME)
    plan = load_json(PLAN)
    scenarios = {scenario.get("id"): scenario for scenario in runtime.get("scenarios", []) if isinstance(scenario, dict)}
    output_dir = str(plan.get("outputDir", "tools/screenshots")).strip("/")
    targets: list[str] = []
    for scenario_id in plan.get("scenarios", []):
        if scenario_id not in scenarios:
            raise SystemExit(f"unknown screenshot scenario {scenario_id!r}")
        targets.append(f"{output_dir}/{scenario_id}.png")
    return targets


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true", help="Only validate that all planned scenarios exist")
    args = parser.parse_args()
    targets = manifest()
    if args.check:
        print(f"following_pokemon screenshot manifest OK: {len(targets)} targets")
        return 0
    for target in targets:
        print(target)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
