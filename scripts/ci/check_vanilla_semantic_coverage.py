#!/usr/bin/env python3
"""Ensure modder-facing vanilla domains do not regress to source-file entries."""

from __future__ import annotations

import json
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
VANILLA = ROOT / "mods" / "vanilla"
REQUIRED_SEMANTIC_DOMAINS = [
    "pokeballs",
    "sprite_assets",
    "overworld_sprites",
    "battle_sprites",
    "followers",
    "outfits",
    "language",
    "events",
    "state",
]


def main() -> int:
    errors = []
    for domain in REQUIRED_SEMANTIC_DOMAINS:
        index_path = VANILLA / domain / "index.json"
        if not index_path.exists():
            errors.append(f"missing index: {index_path.relative_to(ROOT).as_posix()}")
            continue
        data = json.loads(index_path.read_text(encoding="utf-8"))
        entries = data.get("entries", [])
        if not entries:
            errors.append(f"{domain}: index has no semantic entries")
        if data.get("semanticCoverage") == "source_audit_only":
            errors.append(f"{domain}: semanticCoverage is source_audit_only")
        for entry in entries:
            if entry.get("legacyKind") == "source_file":
                errors.append(f"{domain}: source_file entry remains in primary index: {entry.get('path')}")
    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print(f"OK: semantic coverage enforced for {len(REQUIRED_SEMANTIC_DOMAINS)} vanilla domains")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
