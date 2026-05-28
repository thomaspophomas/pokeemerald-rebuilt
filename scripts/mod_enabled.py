#!/usr/bin/env python3
"""List and edit mods/enabled.json."""

from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path


MOD_ID_RE = re.compile(r"^[a-z0-9][a-z0-9_-]*$")


def installed_mods(root: Path) -> list[str]:
    mods_dir = root / "mods"
    if not mods_dir.exists():
        return []
    return sorted(path.parent.name for path in mods_dir.glob("*/mod.json") if path.parent.name != "examples")


def enabled_path(root: Path) -> Path:
    return root / "mods" / "enabled.json"


def read_enabled(root: Path) -> list[str] | None:
    path = enabled_path(root)
    if not path.exists():
        return None
    data = json.loads(path.read_text(encoding="utf-8"))
    enabled = data.get("enabled") if isinstance(data, dict) else data
    if not isinstance(enabled, list):
        raise ValueError(f"{path}: expected list or object with 'enabled' list")
    return [str(item) for item in enabled]


def write_enabled(root: Path, enabled: list[str]) -> None:
    path = enabled_path(root)
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps({"enabled": enabled}, indent=2) + "\n", encoding="utf-8")


def checked_id(mod_id: str) -> str:
    if not MOD_ID_RE.match(mod_id):
        raise ValueError(f"ID must match {MOD_ID_RE.pattern}")
    return mod_id


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("action", choices=["list", "enable", "disable"])
    parser.add_argument("--root", default=".")
    parser.add_argument("--id")
    args = parser.parse_args()

    try:
        root = Path(args.root).resolve()
        installed = installed_mods(root)
        enabled = read_enabled(root)
        active = installed[:] if enabled is None else [mod_id for mod_id in enabled if mod_id in installed]

        if args.action == "list":
            for mod_id in installed:
                marker = "enabled" if mod_id in active else "disabled"
                print(f"{marker}\t{mod_id}")
            if enabled is None:
                print("# mods/enabled.json absent; all installed mods are active")
            return 0

        if args.id is None:
            raise ValueError(f"{args.action} requires --id")
        mod_id = checked_id(args.id)
        if mod_id not in installed:
            raise ValueError(f"{mod_id!r} is not installed under mods/")

        if enabled is None:
            active = installed[:]

        if args.action == "enable" and mod_id not in active:
            active.append(mod_id)
        elif args.action == "disable":
            active = [item for item in active if item != mod_id]

        write_enabled(root, sorted(active))
        return 0
    except Exception as exc:
        print(f"mod-enabled failed: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
