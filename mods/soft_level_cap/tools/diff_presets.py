#!/usr/bin/env python3
"""Compare two soft level cap presets or config files."""

from __future__ import annotations

import argparse
import json
from pathlib import Path
from typing import Any


MOD_ROOT = Path(__file__).resolve().parents[1]
PRESET_ROOT = MOD_ROOT / "tools" / "presets"


def load_json(path: Path) -> dict[str, Any]:
    data = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(data, dict):
        raise SystemExit(f"{path} must contain an object")
    return data


def resolve_config(value: str) -> Path:
    candidate = Path(value)
    if candidate.exists():
        return candidate
    preset_name = value if value.endswith(".json") else f"{value}.json"
    preset = PRESET_ROOT / preset_name
    if preset.exists():
        return preset
    choices = ", ".join(path.stem for path in sorted(PRESET_ROOT.glob("*.json")))
    raise SystemExit(f"unknown preset or file {value!r}; available presets: {choices}")


def first_cap(path: Path) -> dict[str, Any]:
    data = load_json(path)
    caps = data.get("caps")
    if not isinstance(caps, list) or not caps or not isinstance(caps[0], dict):
        raise SystemExit(f"{path} must contain at least one cap object")
    return caps[0]


def curve_map(curve: object) -> dict[int, int]:
    if curve is None:
        return {delta: 100 for delta in range(-100, 100)}
    if isinstance(curve, list) and all(isinstance(value, int) and not isinstance(value, bool) for value in curve):
        return {delta: int(curve[delta + 100]) for delta in range(-100, 100)}
    result: dict[int, int] = {}
    if isinstance(curve, dict):
        entries = [{"delta": delta, "percent": percent} for delta, percent in curve.items()]
    elif isinstance(curve, list):
        entries = curve
    else:
        return result
    for entry in entries:
        if not isinstance(entry, dict):
            continue
        percent = int(entry.get("percent", 100))
        if "delta" in entry:
            result[int(entry["delta"])] = percent
            continue
        minimum = int(entry.get("minDelta", entry.get("min_delta", entry.get("from", -100))))
        maximum = int(entry.get("maxDelta", entry.get("max_delta", entry.get("to", 99))))
        for delta in range(max(-100, minimum), min(99, maximum) + 1):
            result[delta] = percent
    return result


def stage_map(cap: dict[str, Any]) -> dict[str, int]:
    result: dict[str, int] = {}
    for index, stage in enumerate(cap.get("capStages", [])):
        if isinstance(stage, int):
            result[f"base:{index}"] = stage
        elif isinstance(stage, dict):
            key = str(stage.get("flag") or stage.get("vanillaFlag") or stage.get("modFlag") or f"base:{index}")
            result[key] = int(stage.get("level", stage.get("cap", stage.get("maxLevel", 100))))
    return result


def print_field_diff(label: str, left: object, right: object) -> int:
    if left == right:
        return 0
    print(f"{label}: {left!r} -> {right!r}")
    return 1


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("left")
    parser.add_argument("right")
    args = parser.parse_args()

    left_path = resolve_config(args.left)
    right_path = resolve_config(args.right)
    left_cap = first_cap(left_path)
    right_cap = first_cap(right_path)

    print(f"left={left_path}")
    print(f"right={right_path}")
    differences = 0
    for field in ("id", "capType", "rareCandy", "priority", "flags"):
        differences += print_field_diff(field, left_cap.get(field), right_cap.get(field))

    left_stages = stage_map(left_cap)
    right_stages = stage_map(right_cap)
    for key in sorted(set(left_stages) | set(right_stages)):
        differences += print_field_diff(f"capStage.{key}", left_stages.get(key), right_stages.get(key))

    left_curve = curve_map(left_cap.get("softExpCurve"))
    right_curve = curve_map(right_cap.get("softExpCurve"))
    changed_deltas = [delta for delta in range(-100, 100) if left_curve.get(delta) != right_curve.get(delta)]
    if changed_deltas:
        differences += len(changed_deltas)
        print(f"softExpCurve.changedDeltas={len(changed_deltas)}")
        for delta in changed_deltas[:12]:
            print(f"softExpCurve.{delta}: {left_curve.get(delta)} -> {right_curve.get(delta)}")
        if len(changed_deltas) > 12:
            print(f"softExpCurve.more={len(changed_deltas) - 12}")

    print(f"differences={differences}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
