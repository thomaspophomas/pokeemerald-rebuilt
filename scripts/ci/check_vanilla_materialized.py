#!/usr/bin/env python3
"""Check that vanilla legacy mirrors match the current source tree."""

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
            "materialize",
            "--root",
            str(root),
            "--check",
        ]
    )


if __name__ == "__main__":
    raise SystemExit(main())
