#!/usr/bin/env python3
"""Ensure gameplay/mod data is not stored as YAML."""

from __future__ import annotations

import sys
from pathlib import Path


ALLOWED_PREFIX = ".github/workflows/"


def main() -> int:
    root = Path(__file__).resolve().parents[2]
    errors = []
    for path in sorted(root.rglob("*")):
        if not path.is_file() or path.suffix.lower() not in (".yml", ".yaml"):
            continue
        relative = path.relative_to(root).as_posix()
        if not relative.startswith(ALLOWED_PREFIX):
            errors.append(relative)

    if errors:
        print("game/mod YAML files are forbidden; only GitHub workflow YAML is allowed:", file=sys.stderr)
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("OK: no game/mod YAML files found")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
