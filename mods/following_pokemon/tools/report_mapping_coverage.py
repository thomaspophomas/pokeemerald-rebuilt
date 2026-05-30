#!/usr/bin/env python3
"""Report follower mapping coverage by shiny/form tuple."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any


MOD_ROOT = Path(__file__).resolve().parents[1]


def follower_items() -> list[dict[str, Any]]:
    items: list[dict[str, Any]] = []
    for path in sorted((MOD_ROOT / "followers").glob("*.json")):
        data = json.loads(path.read_text(encoding="utf-8"))
        for item in data.get("followers", []):
            if isinstance(item, dict):
                items.append(item)
    return items


def main() -> int:
    items = follower_items()
    normal = {(item.get("species"), item.get("form", 0)) for item in items if item.get("shiny") is not True}
    shiny = {(item.get("species"), item.get("form", 0)) for item in items if item.get("shiny") is True}
    forms = sorted({form for _, form in normal | shiny})
    missing_shiny = sorted(normal - shiny)
    print(f"totalMappings={len(items)}")
    print(f"normalTuples={len(normal)}")
    print(f"shinyTuples={len(shiny)}")
    print(f"formValues={','.join(str(form) for form in forms)}")
    print(f"normalTuplesWithoutShinyMapping={len(missing_shiny)}")
    if missing_shiny:
        print("firstMissing=" + ",".join(f"{species}:{form}" for species, form in missing_shiny[:20]))
    return 0 if not missing_shiny else 1


if __name__ == "__main__":
    raise SystemExit(main())
