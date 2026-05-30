#!/usr/bin/env python3
"""Validate prepared time-window domain config."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any


MOD_ROOT = Path(__file__).resolve().parents[1]
WINDOWS = MOD_ROOT / "tools" / "time_windows.example.json"
SEGMENTS = MOD_ROOT / "time" / "segments.json"


def fail(message: str) -> None:
    raise SystemExit(f"day_night_cycle time window validation failed: {message}")


def load_json(path: Path) -> dict[str, Any]:
    data = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(data, dict):
        fail(f"{path.name} must contain an object")
    return data


def validate_segment_refs(refs: Any, known_groups: set[str], label: str) -> None:
    if not isinstance(refs, list) or not refs:
        fail(f"{label} must be a non-empty list")
    for ref in refs:
        if not isinstance(ref, str) or ref not in known_groups:
            fail(f"{label} references unknown segment group {ref!r}")


def main() -> int:
    segment_data = load_json(SEGMENTS)
    known_segment_ids = {
        item.get("id")
        for item in segment_data.get("segments", [])
        if isinstance(item, dict) and isinstance(item.get("id"), str)
    }
    data = load_json(WINDOWS)
    if not isinstance(data.get("enabled"), bool):
        fail("enabled must be boolean")
    groups = data.get("segments")
    if not isinstance(groups, dict) or not groups:
        fail("segments must be an object")
    for group, refs in groups.items():
        if not isinstance(group, str) or not group:
            fail("segment group names must be non-empty strings")
        if not isinstance(refs, list) or not refs:
            fail(f"segments.{group} must be a non-empty list")
        for ref in refs:
            if ref not in known_segment_ids:
                fail(f"segments.{group} references unknown time segment {ref!r}")

    known_groups = set(groups)
    for domain in ("evolutions", "encounters"):
        entries = data.get(domain)
        if not isinstance(entries, list):
            fail(f"{domain} must be a list")
        seen: set[str] = set()
        for index, entry in enumerate(entries):
            label = f"{domain}[{index}]"
            if not isinstance(entry, dict):
                fail(f"{label} must be an object")
            entry_id = entry.get("id")
            if not isinstance(entry_id, str) or not entry_id:
                fail(f"{label}.id must be a non-empty string")
            if entry_id in seen:
                fail(f"{label}.id duplicates {entry_id}")
            seen.add(entry_id)
            validate_segment_refs(entry.get("segments"), known_groups, f"{label}.segments")
            if domain == "encounters":
                weight = entry.get("weight")
                if isinstance(weight, bool) or not isinstance(weight, int) or weight <= 0:
                    fail(f"{label}.weight must be a positive integer")
    print("day_night_cycle time windows OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
