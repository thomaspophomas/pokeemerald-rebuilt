#!/usr/bin/env python3
"""Validate follower screenshot regression inputs."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path


MOD_ROOT = Path(__file__).resolve().parents[1]
GENERATOR = MOD_ROOT / "tools" / "generate_screenshot_manifest.py"


def main() -> int:
    result = subprocess.run([sys.executable, str(GENERATOR), "--check"], cwd=MOD_ROOT, check=False)
    return result.returncode


if __name__ == "__main__":
    raise SystemExit(main())
