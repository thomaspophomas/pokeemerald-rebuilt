#!/usr/bin/env python3
"""Check vanilla mod JSON expectations and materialized mirrors."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path


def main() -> int:
    root = Path(__file__).resolve().parents[2]
    return subprocess.call(
        [
            sys.executable,
            str(root / "scripts" / "vanilla" / "vanilla_migration.py"),
            "check",
            "--root",
            str(root),
        ]
    )


if __name__ == "__main__":
    raise SystemExit(main())
