#!/usr/bin/env python3
"""Smoke-test the mod generator with a synthetic mod tree."""

from __future__ import annotations

import json
import subprocess
import sys
import tempfile
from pathlib import Path


def write_json(path: Path, data) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(data, indent=2), encoding="utf-8")


def main() -> int:
    repo = Path(__file__).resolve().parents[2]
    modgen = repo / "scripts" / "modgen.py"

    with tempfile.TemporaryDirectory() as temp_dir:
        root = Path(temp_dir)
        mod_root = root / "mods" / "demo"
        write_json(
            mod_root / "mod.json",
            {
                "id": "demo",
                "name": "Demo Mod",
                "version": "1.0.0",
                "priority": 10,
                "dependencies": [],
            },
        )
        write_json(mod_root / "flags" / "story.json", {"flags": [{"id": "badge_1", "initial": False}]})
        write_json(
            mod_root / "events" / "hooks.json",
            {"subscriptions": [{"type": "FLAG_CHANGED", "handler": "Demo_OnFlagChanged", "priority": 10}]},
        )
        write_json(
            mod_root / "weather" / "providers.json",
            {"providers": [{"id": "dark_rain", "handler": "Demo_Weather", "priority": 5}]},
        )
        write_json(
            mod_root / "sprites" / "assets" / "assets.json",
            {
                "assets": [
                    {
                        "id": "guide_asset",
                        "compressedSheetSymbol": "Demo_GuideSheet",
                        "compressedPaletteSymbol": "Demo_GuidePalette",
                        "tileTag": "0xF100",
                        "paletteTag": "0xF101",
                    }
                ]
            },
        )
        write_json(
            mod_root / "sprites" / "overworld" / "sprites.json",
            {"sprites": [{"id": "guide_ow", "asset": "guide_asset", "graphicsId": "OBJ_EVENT_GFX_BOY_1"}]},
        )
        write_json(
            mod_root / "sprites" / "battle" / "sprites.json",
            {"sprites": [{"id": "demo_ball_anim", "asset": "guide_asset", "species": 1, "side": 0}]},
        )
        write_json(
            mod_root / "followers" / "followers.json",
            {"followers": [{"id": "treecko", "species": "SPECIES_TREECKO", "form": 0, "graphicsId": "OBJ_EVENT_GFX_BOY_1"}]},
        )
        write_json(mod_root / "lang" / "en.json", {"strings": {"npc_intro": "Hello!"}})
        write_json(mod_root / "lang" / "de.json", {"strings": {"npc_intro": "Hallo!"}})
        write_json(
            mod_root / "pokeballs" / "balls.json",
            {
                "pokeballs": [
                    {
                        "id": "story_ball",
                        "itemId": "ITEM_POKE_BALL",
                        "ballId": "BALL_PREMIER",
                        "catchModifier": 20,
                        "catchModifierHook": "Demo_BallModifier",
                    }
                ]
            },
        )
        write_json(
            mod_root / "time" / "segments.json",
            {"segments": [{"id": "deep_night", "startMinute": 1320, "endMinute": 299, "segment": "NIGHT"}]},
        )
        write_json(
            mod_root / "engines" / "rulesets.json",
            {"engines": [{"id": "demo_engine", "name": "Demo Engine", "version": 1, "saveCompatible": True}]},
        )
        write_json(
            mod_root / "npcs" / "npcs.json",
            {
                "npcs": [
                    {
                        "id": "guide",
                        "graphicsId": "OBJ_EVENT_GFX_BOY_1",
                        "movementType": "MOVEMENT_TYPE_FACE_DOWN",
                        "localId": 208,
                        "elevation": 3,
                    }
                ]
            },
        )
        write_json(
            mod_root / "maps" / "DemoTown" / "map.json",
            {"id": "demo_town", "name": "DemoTown", "map_group": 0, "map_num": 0},
        )
        (mod_root / "src").mkdir(parents=True, exist_ok=True)
        (mod_root / "src" / "demo.c").write_text("/* demo */\n", encoding="utf-8")

        subprocess.check_call(
            [
                sys.executable,
                str(modgen),
                "--root",
                str(root),
                "--out-header",
                "include/generated/mod_registry.h",
                "--out-source",
                "src/generated/mod_registry.c",
                "--out-make",
                "build/generated/mod_sources.mk",
            ]
        )

        header = (root / "include" / "generated" / "mod_registry.h").read_text(encoding="utf-8")
        source = (root / "src" / "generated" / "mod_registry.c").read_text(encoding="utf-8")
        make = (root / "build" / "generated" / "mod_sources.mk").read_text(encoding="utf-8")

        required = [
            "gModFlagDefinitionCount = 1",
            "Demo_OnFlagChanged",
            "Demo_Weather",
            "demo:deep_night",
            "demo:guide_asset",
            "demo:guide_ow",
            "demo:demo_ball_anim",
            "demo:npc_intro",
            "demo:story_ball",
            "Demo_BallModifier",
            "demo:guide",
            "demo:demo_town",
            "demo:demo_engine",
        ]
        for needle in required:
            if needle not in source:
                raise AssertionError(f"generated source missing {needle}")
        if "extern const struct ModManifest gModManifests[]" not in header:
            raise AssertionError("generated header missing manifest declaration")
        if "MOD_C_SRCS += mods/demo/src/demo.c" not in make:
            raise AssertionError("generated make fragment missing mod C source")

    print("modgen smoke OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
