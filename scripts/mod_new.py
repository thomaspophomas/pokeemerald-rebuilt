#!/usr/bin/env python3
"""Create a validated starter mod folder."""

from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path


MOD_ID_RE = re.compile(r"^[a-z0-9][a-z0-9_-]*$")


TYPES = {"basic", "event", "weather", "npc", "item"}


def c_prefix(mod_id: str) -> str:
    return "".join(part.capitalize() for part in re.split(r"[_-]+", mod_id) if part)


def write_json(path: Path, data: object) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(data, indent=2) + "\n", encoding="utf-8")


def write_text(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")


def create_mod(root: Path, mod_id: str, kind: str) -> None:
    if not MOD_ID_RE.match(mod_id):
        raise ValueError(f"ID must match {MOD_ID_RE.pattern}")
    if kind not in TYPES:
        raise ValueError(f"TYPE must be one of: {', '.join(sorted(TYPES))}")

    mod_root = root / "mods" / mod_id
    if mod_root.exists():
        raise FileExistsError(f"{mod_root} already exists")

    prefix = c_prefix(mod_id)
    manifest = {
        "id": mod_id,
        "name": mod_id.replace("_", " ").replace("-", " ").title(),
        "version": "1.0.0",
        "requiresSdk": ">=1",
        "priority": 1000,
        "requiresFeatures": ["FEATURE_MODS"],
        "stateVersion": 0,
        "stateBytes": 0,
        "dependencies": [],
        "loadAfter": [],
        "conflictsWith": [],
        "entrypoints": [],
    }

    if kind in {"event", "weather"}:
        manifest["entrypoints"] = [f"src/{mod_id}.c"]

    write_json(mod_root / "mod.json", manifest)

    if kind == "event":
        write_json(
            mod_root / "events" / "hooks.json",
            {"subscriptions": [{"type": "GAME_INIT", "handler": f"{prefix}_OnGameInit", "priority": 1000}]},
        )
        write_text(
            mod_root / "src" / f"{mod_id}.c",
            f'#include "mod_sdk.h"\n\n'
            f"s8 {prefix}_OnGameInit(const struct ModEvent *event)\n"
            "{\n"
            "    (void)event;\n"
            "    return MOD_EVENT_RESULT_CONTINUE;\n"
            "}\n",
        )
    elif kind == "weather":
        write_json(
            mod_root / "events" / "hooks.json",
            {"subscriptions": [{"type": "GAME_INIT", "handler": f"{prefix}_OnGameInit", "priority": 1000}]},
        )
        write_json(
            mod_root / "weather" / "providers.json",
            {"providers": [{"id": "default_weather", "handler": f"{prefix}_ResolveWeather", "priority": 1000}]},
        )
        write_text(
            mod_root / "src" / f"{mod_id}.c",
            f'#include "mod_sdk.h"\n\n'
            f"s8 {prefix}_OnGameInit(const struct ModEvent *event)\n"
            "{\n"
            "    (void)event;\n"
            "    return MOD_EVENT_RESULT_CONTINUE;\n"
            "}\n\n"
            f"bool8 {prefix}_ResolveWeather(struct ModWeatherDisplay *display)\n"
            "{\n"
            "    display->vanilla_weather = WEATHER_NONE;\n"
            "    display->layers = MOD_WEATHER_LAYER_NONE;\n"
            "    display->source = MOD_WEATHER_SOURCE_DEFAULT;\n"
            "    display->priority = 100;\n"
            "    display->battle_weather_mask = 0;\n"
            "    return FALSE;\n"
            "}\n",
        )
    elif kind == "npc":
        write_json(
            mod_root / "npcs" / "npc.json",
            {
                "npcs": [
                    {
                        "id": "guide",
                        "graphicsId": "OBJ_EVENT_GFX_BOY_1",
                        "movementType": "MOVEMENT_TYPE_FACE_DOWN",
                        "localId": 200,
                        "elevation": 3,
                        "flagId": 0,
                    }
                ]
            },
        )
    elif kind == "item":
        write_json(mod_root / "lang" / "en.json", {"language": "en", "strings": {"sample_name": "SAMPLE", "sample_desc": "A starter mod item."}})
        write_json(
            mod_root / "items" / "sample.json",
            {
                "items": [
                    {
                        "id": "sample_item",
                        "itemId": "ITEM_POTION",
                        "nameKey": "sample_name",
                        "descriptionKey": "sample_desc",
                        "price": 100,
                        "priority": 1000,
                    }
                ]
            },
        )


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", default=".")
    parser.add_argument("--id", required=True)
    parser.add_argument("--type", default="basic", choices=sorted(TYPES))
    args = parser.parse_args()

    try:
        create_mod(Path(args.root).resolve(), args.id, args.type)
    except Exception as exc:
        print(f"mod-new failed: {exc}", file=sys.stderr)
        return 1

    print(f"Created mods/{args.id}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
