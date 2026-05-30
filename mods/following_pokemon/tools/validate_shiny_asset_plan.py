#!/usr/bin/env python3
"""Validate the shiny asset migration plan."""

from __future__ import annotations

import json
from pathlib import Path


MOD_ROOT = Path(__file__).resolve().parents[1]
PLAN = MOD_ROOT / "tools" / "shiny_asset_plan.json"


def fail(message: str) -> None:
    raise SystemExit(f"following_pokemon shiny asset plan validation failed: {message}")


def main() -> int:
    data = json.loads(PLAN.read_text(encoding="utf-8"))
    if not isinstance(data, dict):
        fail("plan must be an object")
    if not isinstance(data.get("enabled"), bool):
        fail("enabled must be boolean")
    if data.get("status") not in {"pending_assets", "ready", "active"}:
        fail("status must be pending_assets, ready, or active")
    naming = data.get("expectedNaming")
    if not isinstance(naming, dict) or not naming.get("png") or not naming.get("pal"):
        fail("expectedNaming must define png and pal")
    steps = data.get("importStrategy")
    if not isinstance(steps, list) or len(steps) < 4:
        fail("importStrategy must contain at least four steps")
    print("following_pokemon shiny asset plan OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
