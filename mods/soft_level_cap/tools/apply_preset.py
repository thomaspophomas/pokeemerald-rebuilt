#!/usr/bin/env python3
"""Copy a soft level cap preset into level_caps/default.json."""

from __future__ import annotations

import argparse
import shutil
from pathlib import Path


MOD_ROOT = Path(__file__).resolve().parents[1]
PRESET_ROOT = MOD_ROOT / "tools" / "presets"
DEFAULT_CONFIG = MOD_ROOT / "level_caps" / "default.json"


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("preset", help="Preset name, with or without .json")
    parser.add_argument("--dry-run", action="store_true")
    args = parser.parse_args()

    preset_name = args.preset if args.preset.endswith(".json") else f"{args.preset}.json"
    source = PRESET_ROOT / preset_name
    if not source.exists():
        choices = ", ".join(path.stem for path in sorted(PRESET_ROOT.glob("*.json")))
        raise SystemExit(f"unknown preset {args.preset!r}; available: {choices}")

    if args.dry_run:
        print(f"would copy {source} -> {DEFAULT_CONFIG}")
        return 0

    shutil.copyfile(source, DEFAULT_CONFIG)
    print(f"copied {source} -> {DEFAULT_CONFIG}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
