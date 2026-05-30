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


def test_mod_check_sprite_tag_conflict() -> None:
    root = linked_root()
    try:
        for mod_id in ("alpha", "beta"):
            write_json(root / "mods" / mod_id / "mod.json", base_manifest(mod_id))
            write_json(
                root / "mods" / mod_id / "sprites" / "assets" / "assets.json",
                {
                    "assets": [
                        {
                            "id": "shared_palette",
                            "paletteSymbol": f"{mod_id.capitalize()}Palette",
                            "paletteTag": 0xF123,
                        }
                    ]
                },
            )

        result = run([sys.executable, str(REPO / "scripts" / "mod_check.py"), "--root", str(root), "--no-examples"], root, expect_ok=False)
        assert "sprite asset tag conflict" in result.stdout
        assert "paletteTag:61731" in result.stdout
    finally:
        shutil.rmtree(root)


def test_mod_check_follower_mapping_conflict() -> None:
    root = linked_root()
    try:
        for mod_id in ("alpha", "beta"):
            write_json(root / "mods" / mod_id / "mod.json", base_manifest(mod_id))
            write_json(
                root / "mods" / mod_id / "followers" / "followers.json",
                {
                    "followers": [
                        {
                            "id": "treecko",
                            "species": "SPECIES_TREECKO",
                            "form": 0,
                            "shiny": False,
                            "graphicsId": "OBJ_EVENT_GFX_BRENDAN_MACH_BIKE",
                        }
                    ]
                },
            )

        result = run([sys.executable, str(REPO / "scripts" / "mod_check.py"), "--root", str(root), "--no-examples"], root, expect_ok=False)
        assert "follower mapping conflict" in result.stdout
        assert "SPECIES_TREECKO/form=0/shiny=False" in result.stdout
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


def test_level_cap_domain() -> None:
    root = linked_root()
    try:
        write_json(root / "mods" / "caps" / "mod.json", base_manifest("caps"))
        write_json(
            root / "mods" / "caps" / "level_caps" / "caps.json",
            {
                "caps": [
                    {
                        "id": "badge_soft_cap",
                        "capType": "SOFT",
                        "capStages": [
                            {"level": 15},
                            {"flag": "FLAG_BADGE01_GET", "level": 19},
                            {"flag": "FLAG_BADGE02_GET", "level": 24},
                            {"flag": "FLAG_BADGE03_GET", "level": 29},
                            {"flag": "FLAG_BADGE04_GET", "level": 31},
                            {"flag": "FLAG_BADGE05_GET", "level": 33},
                            {"flag": "FLAG_BADGE06_GET", "level": 42},
                            {"flag": "FLAG_BADGE07_GET", "level": 46},
                            {"flag": "FLAG_BADGE08_GET", "level": 58},
                        ],
                        "softExpCurve": [
                            {"minDelta": -99, "maxDelta": -3, "percent": 0},
                            {"delta": -2, "percent": 5},
                            {"delta": -1, "percent": 10},
                            {"delta": 0, "percent": 15},
                            {"delta": 1, "percent": 30},
                            {"delta": 2, "percent": 60},
                            {"delta": 3, "percent": 90},
                            {"minDelta": 4, "maxDelta": 99, "percent": 100},
                        ],
                        "rareCandy": "BLOCK_AT_CAP",
                        "priority": 50,
                    }
                ]
            },
        )

        run([sys.executable, str(REPO / "scripts" / "modgen.py"), "--root", str(root), "--out-header", "include/generated/mod_registry.h", "--out-source", "src/generated/mod_registry.c", "--out-make", "build/generated/mod_sources.mk"], root)

        source = (root / "src" / "generated" / "mod_registry.c").read_text(encoding="utf-8")
        assert "gModLevelCaps" in source
        assert '"caps:badge_soft_cap"' in source
        assert "MOD_LEVEL_CAP_MODE_SOFT" in source
        assert "MOD_LEVEL_CAP_RARE_CANDY_BLOCK_AT_CAP" in source
        assert "MOD_FLAG_TO_VANILLA(FLAG_BADGE08_GET), 58" in source
        assert "5, 10, 15, 30, 60, 90, 100" in source
        assert "MOD_CATALOG_ENTRY_LEVEL_CAP" in source
    finally:
        shutil.rmtree(root)


def test_level_cap_requires_stages() -> None:
    root = linked_root()
    try:
        write_json(root / "mods" / "caps" / "mod.json", base_manifest("caps"))
        write_json(
            root / "mods" / "caps" / "level_caps" / "caps.json",
            {"caps": [{"id": "bad", "capStages": []}]},
        )

        result = run([sys.executable, str(REPO / "scripts" / "modgen.py"), "--root", str(root)], root, expect_ok=False)
        assert "stages" in result.stdout
        assert "1..16" in result.stdout
    finally:
        shutil.rmtree(root)


def test_level_cap_curve_requires_full_delta_coverage() -> None:
    root = linked_root()
    try:
        write_json(root / "mods" / "caps" / "mod.json", base_manifest("caps"))
        write_json(
            root / "mods" / "caps" / "level_caps" / "caps.json",
            {"caps": [{"id": "bad", "capStages": [{"level": 15}], "softExpCurve": [{"delta": 0, "percent": 15}]}]},
        )

        result = run([sys.executable, str(REPO / "scripts" / "modgen.py"), "--root", str(root)], root, expect_ok=False)
        assert "softExpCurve" in result.stdout
        assert "cover every delta" in result.stdout
    finally:
        shutil.rmtree(root)


def test_followers_support_direct_graphics_info() -> None:
    root = linked_root()
    try:
        write_json(root / "mods" / "followers" / "mod.json", base_manifest("followers"))
        write_json(
            root / "mods" / "followers" / "sprites" / "assets" / "followers.json",
            {
                "_meta": {"source": "test"},
                "assets": [
                    {
                        "id": "treecko_asset",
                        "paletteSymbol": "Followers_TreeckoPalette",
                        "paletteTag": 0xF123,
                    }
                ],
            },
        )
        write_json(
            root / "mods" / "followers" / "followers" / "followers.json",
            {
                "_meta": {"source": "test"},
                "followers": [
                    {
                        "id": "treecko",
                        "species": "SPECIES_TREECKO",
                        "form": 0,
                        "shiny": False,
                        "graphicsId": "OBJ_EVENT_GFX_BRENDAN_MACH_BIKE",
                        "asset": "treecko_asset",
                        "graphicsRevision": 7,
                        "graphicsInfoSymbol": "Followers_TreeckoGraphicsInfo",
                    }
                ]
            },
        )

        run([sys.executable, str(REPO / "scripts" / "modgen.py"), "--root", str(root), "--out-header", "include/generated/mod_registry.h", "--out-source", "src/generated/mod_registry.c", "--out-make", "build/generated/mod_sources.mk"], root)

        source = (root / "src" / "generated" / "mod_registry.c").read_text(encoding="utf-8")
        assert "extern const struct SpritePalette Followers_TreeckoPalette;" in source
        assert '"followers:treecko_asset", NULL, NULL, &Followers_TreeckoPalette' in source
        assert "extern const struct ObjectEventGraphicsInfo Followers_TreeckoGraphicsInfo;" in source
        assert '"followers:treecko"' in source
        assert '"followers:treecko_asset"' in source
        assert "OBJ_EVENT_GFX_BRENDAN_MACH_BIKE" in source
        assert "7" in source
        assert "&Followers_TreeckoGraphicsInfo" in source
    finally:
        shutil.rmtree(root)


def main() -> int:
    test_enabled_state_and_map_script()
    test_mod_check_include_ban()
    test_mod_check_claim_conflict()
    test_mod_check_sprite_tag_conflict()
    test_mod_check_follower_mapping_conflict()
    test_modgen_state_budget_guard()
    test_level_cap_domain()
    test_level_cap_requires_stages()
    test_level_cap_curve_requires_full_delta_coverage()
    test_followers_support_direct_graphics_info()
    print("modgen domain tests OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
