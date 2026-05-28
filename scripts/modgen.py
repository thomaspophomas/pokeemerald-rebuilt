#!/usr/bin/env python3
"""Generate static mod registries from mods/<modId> manifests."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

SCRIPT_DIR = Path(__file__).resolve().parent
if str(SCRIPT_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPT_DIR))

from modgen_lib import *  # noqa: F401,F403 - compatibility for importlib users.
from modgen_lib.errors import ModgenError
from modgen_lib.pipeline import generate


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", default=".")
    parser.add_argument("--out-header", default="include/generated/mod_registry.h")
    parser.add_argument("--out-source", default="src/generated/mod_registry.c")
    parser.add_argument("--out-make", default="build/generated/mod_sources.mk")
    args = parser.parse_args()

    generate(Path(args.root).resolve(), args.out_header, args.out_source, args.out_make)
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except ModgenError as exc:
        print(f"modgen: {exc}")
        raise SystemExit(1)
