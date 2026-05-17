#!/usr/bin/env python3
"""Check that the linked ROM keeps a small static EWRAM safety margin."""

from __future__ import annotations

import argparse
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
DEFAULT_MAP = ROOT / "pokeemerald_modern.map"
EWRAM_SIZE = 256 * 1024
DEFAULT_MIN_SLACK = 3 * 1024


def parse_ewram_used(map_text: str) -> int:
    match = re.search(r"^ewram\s+0x[0-9a-fA-F]+\s+0x([0-9a-fA-F]+)\s*$", map_text, re.MULTILINE)
    if not match:
        raise SystemExit("Could not find linked ewram section in map file")
    return int(match.group(1), 16)


def parse_heap_size(map_text: str) -> int:
    match = re.search(r"ewram_data\s+0x[0-9a-fA-F]+\s+0x([0-9a-fA-F]+)\s+src/malloc\.o", map_text)
    if not match:
        raise SystemExit("Could not find gHeap allocation in map file")
    return int(match.group(1), 16)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--map", default=str(DEFAULT_MAP))
    parser.add_argument("--min-static-slack", type=int, default=DEFAULT_MIN_SLACK)
    args = parser.parse_args()

    map_path = Path(args.map)
    if not map_path.exists():
        raise SystemExit(f"Map file not found: {map_path}")

    text = map_path.read_text(encoding="utf-8", errors="replace")
    ewram_used = parse_ewram_used(text)
    heap_size = parse_heap_size(text)
    static_slack = EWRAM_SIZE - ewram_used
    if static_slack < args.min_static_slack:
        raise SystemExit(
            f"EWRAM static slack too low: {static_slack} bytes "
            f"(minimum {args.min_static_slack}, heap reservation {heap_size} bytes)"
        )

    print(
        f"EWRAM budget OK: {static_slack} bytes static slack, "
        f"{heap_size} bytes heap reservation"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
