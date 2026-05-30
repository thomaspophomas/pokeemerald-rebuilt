#!/usr/bin/env python3
"""Preview generated hourly day/night tint values."""

from __future__ import annotations

import argparse
import json
from pathlib import Path
from typing import Any


MOD_ROOT = Path(__file__).resolve().parents[1]
CONFIG = MOD_ROOT / "tools" / "config" / "tints.json"
SCALE = 256


def load_json(path: Path) -> dict[str, Any]:
    data = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(data, dict):
        raise SystemExit(f"{path} must contain an object")
    return data


def load_hours(config: dict[str, Any]) -> list[dict[str, int]]:
    hours = config.get("hours")
    if not isinstance(hours, list) or len(hours) != 24:
        raise SystemExit("config hours must contain 24 entries")
    return sorted(hours, key=lambda item: int(item["hour"]))


def override_for(config: dict[str, Any], map_section: str | None) -> dict[str, int] | None:
    if map_section is None:
        return None
    for item in config.get("mapOverrides", []):
        if isinstance(item, dict) and item.get("mapSection") == map_section:
            return {"r": int(item["r"]), "g": int(item["g"]), "b": int(item["b"])}
    return None


def combine(base: dict[str, int], overlay: dict[str, int] | None) -> dict[str, int]:
    if overlay is None:
        return {key: int(base[key]) for key in ("r", "g", "b")}
    return {
        key: (int(base[key]) * int(overlay[key]) + (SCALE // 2)) // SCALE
        for key in ("r", "g", "b")
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--config", type=Path, default=CONFIG)
    parser.add_argument("--map-section")
    parser.add_argument("--hour", type=int)
    args = parser.parse_args()

    config = load_json(args.config)
    hours = load_hours(config)
    override = override_for(config, args.map_section)
    selected = [hour for hour in hours if args.hour is None or int(hour["hour"]) == args.hour]
    if args.hour is not None and not selected:
        raise SystemExit(f"hour {args.hour} not found")

    print("hour,baseR,baseG,baseB,finalR,finalG,finalB,mapOverride")
    for item in selected:
        final = combine(item, override)
        print(
            f"{item['hour']},{item['r']},{item['g']},{item['b']},"
            f"{final['r']},{final['g']},{final['b']},{args.map_section or 'none'}"
        )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
