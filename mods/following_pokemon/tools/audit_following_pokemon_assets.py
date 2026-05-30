#!/usr/bin/env python3
"""Print a non-failing audit summary for imported follower assets."""

from __future__ import annotations

import hashlib
import json
from collections import defaultdict
from pathlib import Path


MOD_ROOT = Path(__file__).resolve().parents[1]
GRAPHICS = MOD_ROOT / "graphics" / "followers"
ALLOWLIST = MOD_ROOT / "tools" / "asset_dedupe_allowlist.json"


def digest(path: Path) -> str:
    return hashlib.sha1(path.read_bytes()).hexdigest()


def duplicate_groups(paths: list[Path]) -> list[list[Path]]:
    buckets: dict[str, list[Path]] = defaultdict(list)
    for path in paths:
        buckets[digest(path)].append(path)
    return sorted(
        (group for group in buckets.values() if len(group) > 1),
        key=lambda group: (-len(group), group[0].name),
    )


def print_groups(label: str, groups: list[list[Path]]) -> None:
    print(f"{label} duplicate byte groups: {len(groups)}")
    for group in groups[:20]:
        names = ", ".join(path.name for path in group[:8])
        suffix = "" if len(group) <= 8 else f", ... +{len(group) - 8}"
        print(f"  {len(group)} files: {names}{suffix}")
    if len(groups) > 20:
        print(f"  ... {len(groups) - 20} more groups")


def allowlisted(label: str, group: list[Path]) -> bool:
    data = json.loads(ALLOWLIST.read_text(encoding="utf-8"))
    allowed = data.get(label.lower(), [])
    names = sorted(path.name for path in group)
    return any(sorted(entry) == names for entry in allowed if isinstance(entry, list))


def main() -> int:
    pngs = sorted(GRAPHICS.glob("*.png"))
    pals = sorted(GRAPHICS.glob("*.pal"))
    missing_pal = [png.with_suffix(".pal").name for png in pngs if not png.with_suffix(".pal").exists()]
    missing_png = [pal.with_suffix(".png").name for pal in pals if not pal.with_suffix(".png").exists()]

    print(f"PNG assets: {len(pngs)}")
    print(f"PAL assets: {len(pals)}")
    print(f"PNG without PAL: {len(missing_pal)}")
    print(f"PAL without PNG: {len(missing_png)}")
    if missing_pal:
        print("  " + ", ".join(missing_pal[:20]))
    if missing_png:
        print("  " + ", ".join(missing_png[:20]))
    png_groups = duplicate_groups(pngs)
    pal_groups = duplicate_groups(pals)
    print_groups("PNG", png_groups)
    print_groups("PAL", pal_groups)
    unexpected = [
        *(group for group in png_groups if not allowlisted("png", group)),
        *(group for group in pal_groups if not allowlisted("pal", group)),
    ]
    print(f"unexpected duplicate groups: {len(unexpected)}")
    return 0 if not unexpected else 1


if __name__ == "__main__":
    raise SystemExit(main())
