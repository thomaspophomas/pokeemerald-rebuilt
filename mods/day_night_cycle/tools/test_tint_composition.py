#!/usr/bin/env python3
"""Check day/night, map override, and battle weather tint composition bounds."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any


MOD_ROOT = Path(__file__).resolve().parents[1]
REPO_ROOT = MOD_ROOT.parents[1]
TINTS = MOD_ROOT / "tools" / "config" / "tints.json"
WEATHER_CONFIG = REPO_ROOT / "mods" / "battle_weather_stack" / "src" / "tools" / "weather_config.json"
SCALE = 256


def load_json(path: Path) -> dict[str, Any]:
    data = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(data, dict):
        raise SystemExit(f"{path} must contain an object")
    return data


def combine(left: dict[str, int], right: dict[str, int] | None) -> dict[str, int]:
    if right is None:
        return dict(left)
    return {
        key: (int(left[key]) * int(right[key]) + (SCALE // 2)) // SCALE
        for key in ("r", "g", "b")
    }


def tint_from(item: dict[str, Any]) -> dict[str, int]:
    return {key: int(item[key]) for key in ("r", "g", "b")}


def main() -> int:
    tints = load_json(TINTS)
    hours = [tint_from(item) for item in tints.get("hours", []) if isinstance(item, dict)]
    map_overrides = [tint_from(item) for item in tints.get("mapOverrides", []) if isinstance(item, dict)]

    weather_tints: list[dict[str, int]] = []
    if WEATHER_CONFIG.exists():
        weather = load_json(WEATHER_CONFIG)
        for layer in weather.get("layers", []):
            if not isinstance(layer, dict):
                continue
            overworld = layer.get("overworld", {})
            if isinstance(overworld, dict) and isinstance(overworld.get("tint"), dict):
                weather_tints.append(tint_from(overworld["tint"]))

    max_component = 0
    cases = 0
    for hour in hours:
        map_candidates = map_overrides or [None]
        weather_candidates = weather_tints or [None]
        for map_tint in map_candidates:
            mapped = combine(hour, map_tint)
            for weather_tint in weather_candidates:
                final = combine(mapped, weather_tint)
                max_component = max(max_component, final["r"], final["g"], final["b"])
                if any(component < 0 or component > 512 for component in final.values()):
                    raise SystemExit(f"tint composition out of bounds: {final}")
                cases += 1

    print(f"day_night_cycle tint composition OK: {cases} cases, maxComponent={max_component}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
