#!/usr/bin/env python3
"""Validate day/night map override preset authoring data."""

from __future__ import annotations

import json
import re
from pathlib import Path
from typing import Any


MOD_ROOT = Path(__file__).resolve().parents[1]
PRESETS = MOD_ROOT / "tools" / "map_override_presets.json"
REGION_SECTIONS = Path("/home/thomas/pokeemerald-rebuilt/include/constants/region_map_sections.h")
REQUIRED_CATEGORIES = {"caves", "cities", "forests", "water", "indoors"}
MAPSEC_RE = re.compile(r"\b(MAPSEC_[A-Z0-9_]+)\b")


def fail(message: str) -> None:
    raise SystemExit(f"day_night_cycle map override preset validation failed: {message}")


def known_map_sections() -> set[str]:
    if not REGION_SECTIONS.exists():
        return set()
    return set(MAPSEC_RE.findall(REGION_SECTIONS.read_text(encoding="utf-8", errors="ignore")))


def tint(value: Any, label: str) -> int:
    if isinstance(value, bool) or not isinstance(value, int):
        fail(f"{label} must be an integer")
    if value < 0 or value > 512:
        fail(f"{label} must be in [0, 512]")
    return value


def main() -> int:
    data = json.loads(PRESETS.read_text(encoding="utf-8"))
    categories = data.get("categories") if isinstance(data, dict) else None
    if not isinstance(categories, dict):
        fail("categories must be an object")
    missing = sorted(REQUIRED_CATEGORIES - set(categories))
    if missing:
        fail("missing categories: " + ", ".join(missing))

    known = known_map_sections()
    count = 0
    seen: set[str] = set()
    for category, items in categories.items():
        if not isinstance(items, list) or not items:
            fail(f"{category} must contain at least one override")
        for index, item in enumerate(items):
            label = f"{category}[{index}]"
            if not isinstance(item, dict):
                fail(f"{label} must be an object")
            map_section = item.get("mapSection")
            if not isinstance(map_section, str) or not map_section.startswith("MAPSEC_"):
                fail(f"{label}.mapSection must be a MAPSEC_* token")
            if known and map_section not in known:
                fail(f"{label}.mapSection is unknown: {map_section}")
            if map_section in seen:
                fail(f"duplicate preset mapSection {map_section}")
            seen.add(map_section)
            tint(item.get("r"), f"{label}.r")
            tint(item.get("g"), f"{label}.g")
            tint(item.get("b"), f"{label}.b")
            count += 1
    print(f"day_night_cycle map override presets OK: {count} entries")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
