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
    api_domains = [
        "maps",
        "npcs",
        "trainers",
        "trainer_parties",
        "weather",
        "time",
        "flags",
        "events",
        "language",
        "sprite_assets",
        "overworld_sprites",
        "battle_sprites",
        "followers",
        "outfits",
        "pokeballs",
        "engine_rulesets",
        "state",
        "quests",
        "wild_encounters",
        "items",
        "pokemon",
        "moves",
        "shops",
    ]

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
            mod_root / "sprite_assets" / "assets" / "guide_asset.json",
            {
                "id": "guide_asset",
                "domain": "sprite_assets",
                "schemaVersion": 1,
                "compressedSheetSymbol": "Demo_GuideSheet",
                "compressedPaletteSymbol": "Demo_GuidePalette",
                "tileTag": "0xF100",
                "paletteTag": "0xF101",
            },
        )
        write_json(
            mod_root / "sprite_assets" / "index.json",
            {"domain": "sprite_assets", "schemaVersion": 1, "entries": [{"id": "guide_asset", "path": "assets/guide_asset.json"}]},
        )
        write_json(
            mod_root / "overworld_sprites" / "sprites" / "guide_ow.json",
            {"id": "guide_ow", "domain": "overworld_sprites", "schemaVersion": 1, "assetKey": "guide_asset", "graphicsId": "OBJ_EVENT_GFX_BOY_1"},
        )
        write_json(
            mod_root / "overworld_sprites" / "index.json",
            {"domain": "overworld_sprites", "schemaVersion": 1, "entries": [{"id": "guide_ow", "path": "sprites/guide_ow.json"}]},
        )
        write_json(
            mod_root / "battle_sprites" / "pokemon" / "SPECIES_TREECKO_front.json",
            {
                "id": "demo_ball_anim",
                "domain": "battle_sprites",
                "schemaVersion": 1,
                "assetKey": "guide_asset",
                "species": "SPECIES_TREECKO",
                "side": "BATTLE_SPRITE_SIDE_FRONT",
            },
        )
        write_json(
            mod_root / "battle_sprites" / "index.json",
            {"domain": "battle_sprites", "schemaVersion": 1, "entries": [{"id": "demo_ball_anim", "path": "pokemon/SPECIES_TREECKO_front.json"}]},
        )
        write_json(
            mod_root / "followers" / "followers" / "SPECIES_TREECKO.json",
            {"id": "treecko", "domain": "followers", "schemaVersion": 1, "species": "SPECIES_TREECKO", "form": 0, "graphicsId": "OBJ_EVENT_GFX_BOY_1"},
        )
        write_json(
            mod_root / "followers" / "index.json",
            {"domain": "followers", "schemaVersion": 1, "entries": [{"id": "treecko", "path": "followers/SPECIES_TREECKO.json"}]},
        )
        write_json(mod_root / "language" / "en" / "core_strings.json", {"id": "en:core_strings", "domain": "language", "schemaVersion": 1, "language": "en", "runtime": True, "strings": {"npc_intro": "Hello!"}})
        write_json(mod_root / "language" / "de" / "core_strings.json", {"id": "de:core_strings", "domain": "language", "schemaVersion": 1, "language": "de", "runtime": True, "strings": {"npc_intro": "Hallo!"}})
        write_json(
            mod_root / "language" / "index.json",
            {
                "domain": "language",
                "schemaVersion": 1,
                "entries": [
                    {"id": "en:core_strings", "path": "en/core_strings.json"},
                    {"id": "de:core_strings", "path": "de/core_strings.json"},
                ],
            },
        )
        write_json(
            mod_root / "pokeballs" / "balls" / "ITEM_POKE_BALL.json",
            {
                "id": "story_ball",
                "domain": "pokeballs",
                "schemaVersion": 1,
                "itemId": "ITEM_POKE_BALL",
                "ballId": "BALL_PREMIER",
                "catchModifier": 20,
                "catchModifierHook": "Demo_BallModifier",
            },
        )
        write_json(
            mod_root / "pokeballs" / "index.json",
            {"domain": "pokeballs", "schemaVersion": 1, "entries": [{"id": "story_ball", "path": "balls/ITEM_POKE_BALL.json"}]},
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
                        "interactionPolicy": "SHARED_READONLY",
                    }
                ]
            },
        )
        write_json(
            mod_root / "maps" / "DemoTown" / "map.json",
            {"id": "demo_town", "name": "DemoTown"},
        )
        write_json(
            mod_root / "maps" / "map_groups.json",
            {"group_order": ["gMapGroup_Demo"], "gMapGroup_Demo": ["DemoTown"]},
        )
        write_json(
            mod_root / "trainers" / "TRAINER_DEMO.json",
            {
                "id": "TRAINER_DEMO",
                "domain": "trainers",
                "schemaVersion": 1,
                "source": {"path": "src/data/trainers.h", "sha1": "demo"},
                "legacy": {"kind": "trainer_c_initializer"},
                "symbol": "TRAINER_DEMO",
            },
        )
        write_json(
            mod_root / "trainers" / "index.json",
            {
                "domain": "trainers",
                "schemaVersion": 1,
                "entries": [
                    {
                        "id": "TRAINER_DEMO",
                        "path": "TRAINER_DEMO.json",
                        "legacyKind": "trainer_c_initializer",
                    }
                ],
            },
        )
        for domain in api_domains:
            domain_root = mod_root / domain
            if domain_root.exists():
                continue
            write_json(
                domain_root / "manifest.json",
                {
                    "domain": domain,
                    "schemaVersion": 1,
                    "migrationMode": "source_file_manifest",
                    "sourceFiles": [],
                },
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
            "MOD_NPC_INTERACTION_SHARED_READONLY",
            "demo:demo_town",
            "demo:demo_engine",
            "demo:trainers:TRAINER_DEMO",
            "gModDomainFileCount",
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
