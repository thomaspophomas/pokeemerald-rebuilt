#!/usr/bin/env python3
"""Validate day/night tint and segment authoring files."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path


MOD_ROOT = Path(__file__).resolve().parents[1]
SEGMENTS = MOD_ROOT / "time" / "segments.json"
TINT_GENERATOR = MOD_ROOT / "tools" / "generate_day_night_tints.py"
PRESET_ROOT = MOD_ROOT / "tools" / "presets"
MINUTES_PER_DAY = 24 * 60


def fail(message: str) -> None:
    raise SystemExit(f"day_night_cycle validation failed: {message}")


def minute(value: object, label: str) -> int:
    if isinstance(value, bool) or not isinstance(value, int):
        fail(f"{label} must be an integer")
    if value < 0 or value >= MINUTES_PER_DAY:
        fail(f"{label} must be in [0, {MINUTES_PER_DAY - 1}]")
    return value


def validate_segments() -> None:
    data = json.loads(SEGMENTS.read_text(encoding="utf-8"))
    segments = data.get("segments") if isinstance(data, dict) else None
    if not isinstance(segments, list) or not segments:
        fail("time/segments.json must contain a non-empty segments list")

    covered = [False] * MINUTES_PER_DAY
    seen_ids: set[str] = set()
    for index, segment in enumerate(segments):
        label = f"segments[{index}]"
        if not isinstance(segment, dict):
            fail(f"{label} must be an object")
        segment_id = segment.get("id")
        if not isinstance(segment_id, str) or not segment_id:
            fail(f"{label}.id must be a non-empty string")
        if segment_id in seen_ids:
            fail(f"{label}.id duplicates {segment_id}")
        seen_ids.add(segment_id)

        start = minute(segment.get("startMinute"), f"{label}.startMinute")
        end = minute(segment.get("endMinute"), f"{label}.endMinute")
        if start > end:
            fail(f"{label}.startMinute must be <= endMinute")
        for current in range(start, end + 1):
            if covered[current]:
                fail(f"{label} overlaps minute {current}")
            covered[current] = True

    for current, is_covered in enumerate(covered):
        if not is_covered:
            fail(f"time segments miss minute {current}")


def main() -> int:
    validate_segments()
    result = subprocess.run(
        [sys.executable, str(TINT_GENERATOR), "--check"],
        cwd=MOD_ROOT,
        text=True,
        check=False,
    )
    if result.returncode != 0:
        return result.returncode
    for preset in sorted(PRESET_ROOT.glob("*.json")):
        result = subprocess.run(
            [sys.executable, str(TINT_GENERATOR), "--config", str(preset), "--validate-only"],
            cwd=MOD_ROOT,
            text=True,
            check=False,
        )
        if result.returncode != 0:
            return result.returncode
    print("day_night_cycle validation OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
