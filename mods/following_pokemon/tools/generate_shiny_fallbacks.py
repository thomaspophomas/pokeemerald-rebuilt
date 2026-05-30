#!/usr/bin/env python3
"""Generate explicit shiny fallback mappings using normal follower graphics."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any


MOD_ROOT = Path(__file__).resolve().parents[1]
OUTPUT = MOD_ROOT / "followers" / "shiny_fallbacks.json"


def load_normal_followers() -> list[dict[str, Any]]:
    followers: list[dict[str, Any]] = []
    for path in sorted((MOD_ROOT / "followers").glob("*.json")):
        if path == OUTPUT:
            continue
        data = json.loads(path.read_text(encoding="utf-8"))
        raw_items = data.get("followers")
        if not isinstance(raw_items, list):
            raise ValueError(f"{path.relative_to(MOD_ROOT)} has no followers list")
        for item in raw_items:
            if isinstance(item, dict) and item.get("shiny") is not True:
                followers.append(item)
    return followers


def render() -> str:
    fallbacks: list[dict[str, Any]] = []
    for item in load_normal_followers():
        fallback = dict(item)
        fallback["id"] = f"{item['id']}_shiny_fallback"
        fallback["shiny"] = True
        fallback["mode"] = "observe"
        fallbacks.append(fallback)
    return json.dumps(
        {
            "_meta": {
                "generatedBy": "tools/generate_shiny_fallbacks.py",
                "source": "all non-shiny followers/*.json except shiny_fallbacks.json",
                "note": "These mappings intentionally reuse normal graphics until shiny sheets exist, including form aliases.",
            },
            "followers": fallbacks,
        },
        indent=2,
        ensure_ascii=True,
    ) + "\n"


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    try:
        content = render()
    except ValueError as exc:
        print(f"following_pokemon shiny fallback generation failed: {exc}", file=sys.stderr)
        return 1
    if args.check:
        existing = OUTPUT.read_text(encoding="utf-8") if OUTPUT.exists() else ""
        if existing != content:
            print(f"error: {OUTPUT} is out of date", file=sys.stderr)
            return 1
        print(f"{OUTPUT} is up to date")
        return 0
    OUTPUT.write_text(content, encoding="utf-8")
    print(f"wrote {OUTPUT}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
