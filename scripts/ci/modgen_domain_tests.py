#!/usr/bin/env python3
"""Small golden-style tests for modgen contracts that are easy to regress."""

from __future__ import annotations

import json
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path


REPO = Path(__file__).resolve().parents[2]


def write_json(path: Path, data: object) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(data, indent=2) + "\n", encoding="utf-8")


def write_text(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")


def base_manifest(mod_id: str, **extra):
    data = {
        "id": mod_id,
        "name": mod_id,
        "version": "1.0.0",
        "requiresSdk": ">=1",
        "priority": 1000,
        "dependencies": [],
        "entrypoints": [],
    }
    data.update(extra)
    return data


def run(cmd: list[str], root: Path, expect_ok: bool = True) -> subprocess.CompletedProcess:
    result = subprocess.run(cmd, cwd=root, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    if expect_ok and result.returncode != 0:
        raise AssertionError(result.stdout)
    if not expect_ok and result.returncode == 0:
        raise AssertionError("command unexpectedly succeeded:\n" + result.stdout)
    return result


def linked_root() -> Path:
    root = Path(tempfile.mkdtemp())
    (root / "mods").mkdir()
    (root / "scripts").symlink_to(REPO / "scripts", target_is_directory=True)
    (root / "include").symlink_to(REPO / "include", target_is_directory=True)
    return root


def test_enabled_state_and_map_script() -> None:
    root = linked_root()
    try:
        write_json(root / "mods" / "alpha" / "mod.json", base_manifest("alpha"))
        write_json(root / "mods" / "beta" / "mod.json", base_manifest("beta", stateVersion=2, stateBytes=12))
        write_json(root / "mods" / "enabled.json", {"enabled": ["beta"]})
        write_json(root / "mods" / "beta" / "maps" / "Route101" / "map.json", {"id": "route101", "map_group": 1, "map_num": 1, "script": "scripts.inc"})
        write_text(root / "mods" / "beta" / "maps" / "Route101" / "scripts.inc", "@ test script\n")

        run([sys.executable, str(REPO / "scripts" / "modgen.py"), "--root", str(root), "--out-header", "include/generated/mod_registry.h", "--out-source", "src/generated/mod_registry.c", "--out-make", "build/generated/mod_sources.mk"], root)

        source = (root / "src" / "generated" / "mod_registry.c").read_text(encoding="utf-8")
        make_fragment = (root / "build" / "generated" / "mod_sources.mk").read_text(encoding="utf-8")
        assert '"beta"' in source
        assert '"alpha"' not in source
        assert ".state_version = 2" in source
        assert ".state_bytes = 12" in source
        assert ".state_offset = 0" in source
        assert ".script_path = \"mods/beta/maps/Route101/scripts.inc\"" in source
        assert "MOD_MAP_SCRIPT_INCS += mods/beta/maps/Route101/scripts.inc" in make_fragment
    finally:
        shutil.rmtree(root)


def test_mod_check_include_ban() -> None:
    root = linked_root()
    try:
        write_json(root / "mods" / "bad_include" / "mod.json", base_manifest("bad_include", entrypoints=["src/bad_include.c"]))
        write_text(root / "mods" / "bad_include" / "src" / "bad_include.c", '#include "global.h"\n')
        result = run([sys.executable, str(REPO / "scripts" / "mod_check.py"), "--root", str(root), "--no-examples"], root, expect_ok=False)
        assert "global.h" in result.stdout
        assert "mod_sdk.h" in result.stdout
    finally:
        shutil.rmtree(root)


def test_mod_check_claim_conflict() -> None:
    root = linked_root()
    try:
        for mod_id in ("alpha", "beta"):
            write_json(root / "mods" / mod_id / "mod.json", base_manifest(mod_id))
            write_json(
                root / "mods" / mod_id / "shops" / "marts.json",
                {
                    "shops": [
                        {
                            "id": "oldale_mart",
                            "mapGroup": 0,
                            "mapNum": 0,
                            "items": ["ITEM_POTION"],
                            "mode": "claim",
                        }
                    ]
                },
            )

        result = run([sys.executable, str(REPO / "scripts" / "mod_check.py"), "--root", str(root), "--no-examples"], root, expect_ok=False)
        assert "hook conflict" in result.stdout
        assert "shops:oldale_mart" in result.stdout
    finally:
        shutil.rmtree(root)


def test_modgen_state_budget_guard() -> None:
    root = linked_root()
    try:
        write_json(root / "mods" / "large_state" / "mod.json", base_manifest("large_state", stateVersion=1, stateBytes=999))
        result = run([sys.executable, str(REPO / "scripts" / "modgen.py"), "--root", str(root)], root, expect_ok=False)
        assert "stateBytes" in result.stdout
        assert "exceeds budget" in result.stdout
    finally:
        shutil.rmtree(root)


def main() -> int:
    test_enabled_state_and_map_script()
    test_mod_check_include_ban()
    test_mod_check_claim_conflict()
    test_modgen_state_budget_guard()
    print("modgen domain tests OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
