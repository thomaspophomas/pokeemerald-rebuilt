#!/usr/bin/env python3
"""Generate static mod registries from mods/<modId> manifests."""

from __future__ import annotations

import argparse
import json
import os
import re
from pathlib import Path
from typing import Any, Dict, Iterable, List, Optional


IDENT_RE = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")
MOD_ID_RE = re.compile(r"^[a-z0-9][a-z0-9_-]*$")

EVENT_TYPES = {
    "GAME_INIT": "MOD_EVENT_GAME_INIT",
    "SAVE_LOADED": "MOD_EVENT_SAVE_LOADED",
    "MAP_LOAD": "MOD_EVENT_MAP_LOAD",
    "MAP_ENTER": "MOD_EVENT_MAP_ENTER",
    "PLAYER_STEP": "MOD_EVENT_PLAYER_STEP",
    "FLAG_CHANGED": "MOD_EVENT_FLAG_CHANGED",
    "TIME_SEGMENT_CHANGED": "MOD_EVENT_TIME_SEGMENT_CHANGED",
    "DAY_CHANGED": "MOD_EVENT_DAY_CHANGED",
    "WEATHER_CHANGED": "MOD_EVENT_WEATHER_CHANGED",
    "NPC_INTERACT": "MOD_EVENT_NPC_INTERACT",
    "BATTLE_STARTED": "MOD_EVENT_BATTLE_STARTED",
    "BATTLE_ENDED": "MOD_EVENT_BATTLE_ENDED",
    "CUSTOM": "MOD_EVENT_CUSTOM",
}

TIME_SEGMENTS = {
    "MORNING": "MOD_TIME_MORNING",
    "DAY": "MOD_TIME_DAY",
    "EVENING": "MOD_TIME_EVENING",
    "NIGHT": "MOD_TIME_NIGHT",
}


class ModgenError(Exception):
    pass


def read_json(path: Path) -> Any:
    try:
        with path.open("r", encoding="utf-8") as handle:
            return json.load(handle)
    except json.JSONDecodeError as exc:
        raise ModgenError(f"{path}: invalid JSON: {exc}") from exc


def as_list(data: Any, key: str) -> List[Any]:
    if data is None:
        return []
    if isinstance(data, list):
        return data
    if isinstance(data, dict):
        value = data.get(key, [])
        if isinstance(value, list):
            return value
    raise ModgenError(f"Expected list or object with '{key}' list")


def c_string(value: Optional[str]) -> str:
    if value is None:
        return "NULL"
    return '"' + value.replace("\\", "\\\\").replace('"', '\\"') + '"'


def c_bool(value: Any) -> str:
    return "TRUE" if bool(value) else "FALSE"


def c_int_or_token(value: Any, default: str = "0") -> str:
    if value is None:
        return default
    if isinstance(value, bool):
        return "TRUE" if value else "FALSE"
    if isinstance(value, int):
        return str(value)
    if isinstance(value, str):
        value = value.strip()
        if value == "":
            return default
        if re.match(r"^-?[0-9]+$", value):
            return value
        if re.match(r"^0x[0-9A-Fa-f]+$", value):
            return value
        if IDENT_RE.match(value):
            return value
    raise ModgenError(f"Value {value!r} is not a C integer literal or token")


def c_func(value: Optional[str]) -> str:
    if value in (None, "", "NULL"):
        return "NULL"
    if not IDENT_RE.match(value):
        raise ModgenError(f"Function name {value!r} is not a C identifier")
    return value


def c_symbol(value: Optional[str]) -> str:
    if value in (None, "", "NULL"):
        return "NULL"
    if not IDENT_RE.match(value):
        raise ModgenError(f"Symbol name {value!r} is not a C identifier")
    return value


def c_u8_string(value: Optional[str]) -> str:
    if value is None:
        return "NULL"
    return "_(" + c_string(str(value)) + ")"


def normalize_event_type(value: Any) -> str:
    if isinstance(value, int):
        return str(value)
    if not isinstance(value, str):
        raise ModgenError(f"Event type {value!r} is invalid")
    value = value.strip()
    if value.startswith("MOD_EVENT_"):
        return value
    if value in EVENT_TYPES:
        return EVENT_TYPES[value]
    upper = value.upper()
    if upper in EVENT_TYPES:
        return EVENT_TYPES[upper]
    raise ModgenError(f"Unknown event type {value!r}")


def normalize_time_segment(value: Any) -> str:
    if isinstance(value, int):
        return str(value)
    if not isinstance(value, str):
        raise ModgenError(f"Time segment {value!r} is invalid")
    value = value.strip()
    if value.startswith("MOD_TIME_"):
        return value
    upper = value.upper()
    if upper in TIME_SEGMENTS:
        return TIME_SEGMENTS[upper]
    raise ModgenError(f"Unknown time segment {value!r}")


def require_mod_id(value: Any, path: Path) -> str:
    if not isinstance(value, str) or not MOD_ID_RE.match(value):
        raise ModgenError(f"{path}: mod id must match {MOD_ID_RE.pattern}")
    return value


def iter_json_files(path: Path) -> Iterable[Path]:
    if not path.exists():
        return []
    return sorted(path.rglob("*.json"))


def load_mods(root: Path) -> List[Dict[str, Any]]:
    mods_dir = root / "mods"
    mods: List[Dict[str, Any]] = []
    seen = set()

    if not mods_dir.exists():
        return mods

    for manifest_path in sorted(mods_dir.glob("*/mod.json")):
        manifest = read_json(manifest_path)
        mod_id = require_mod_id(manifest.get("id"), manifest_path)
        if mod_id in seen:
            raise ModgenError(f"Duplicate mod id {mod_id!r}")
        seen.add(mod_id)
        mod_root = manifest_path.parent
        mods.append(
            {
                "id": mod_id,
                "name": str(manifest.get("name", mod_id)),
                "version": str(manifest.get("version", "0.0.0")),
                "priority": int(manifest.get("priority", 1000)),
                "features": int(manifest.get("featureFlags", manifest.get("features", 0)) or 0),
                "dependencies": list(manifest.get("dependencies", [])),
                "root": mod_root,
                "repo_root": root,
                "manifest": manifest,
            }
        )

    known = {mod["id"] for mod in mods}
    for mod in mods:
        for dep in mod["dependencies"]:
            if dep not in known:
                raise ModgenError(f"{mod['id']}: missing dependency {dep!r}")

    mods.sort(key=lambda mod: (mod["priority"], mod["id"]))
    return mods


def key_for(mod_id: str, item: Dict[str, Any], fallback: str) -> str:
    raw = item.get("key") or item.get("id") or item.get("name") or fallback
    raw = str(raw)
    if ":" in raw:
        return raw
    return f"{mod_id}:{raw}"


def collect_flags(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    flags = []
    seen = set()
    next_id = 0
    for mod in mods:
        for path in iter_json_files(mod["root"] / "flags"):
            for index, item in enumerate(as_list(read_json(path), "flags")):
                if isinstance(item, str):
                    item = {"id": item}
                key = key_for(mod["id"], item, f"flag_{index}")
                if key in seen:
                    raise ModgenError(f"Duplicate flag key {key!r}")
                seen.add(key)
                flags.append({"key": key, "id": next_id, "initial": bool(item.get("initial", False))})
                next_id += 1
    return flags


def collect_events(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    events = []
    for mod in mods:
        for path in iter_json_files(mod["root"] / "events"):
            for item in as_list(read_json(path), "subscriptions"):
                handler = c_func(item.get("handler"))
                events.append(
                    {
                        "type": normalize_event_type(item.get("type", "CUSTOM")),
                        "priority": int(item.get("priority", 1000)),
                        "handler": handler,
                        "source": key_for(mod["id"], item, path.stem),
                    }
                )
    events.sort(key=lambda event: (event["type"], event["priority"], event["source"]))
    return events


def collect_weather(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    providers = []
    seen = set()
    for mod in mods:
        for path in iter_json_files(mod["root"] / "weather"):
            for item in as_list(read_json(path), "providers"):
                key = key_for(mod["id"], item, path.stem)
                if key in seen:
                    raise ModgenError(f"Duplicate weather provider key {key!r}")
                seen.add(key)
                providers.append(
                    {
                        "id": key,
                        "priority": int(item.get("priority", 1000)),
                        "handler": c_func(item.get("handler")),
                    }
                )
    providers.sort(key=lambda provider: (provider["priority"], provider["id"]))
    return providers


def collect_time_segments(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    segments = []
    seen = set()
    for mod in mods:
        for path in iter_json_files(mod["root"] / "time"):
            for item in as_list(read_json(path), "segments"):
                key = key_for(mod["id"], item, path.stem)
                if key in seen:
                    raise ModgenError(f"Duplicate time segment key {key!r}")
                seen.add(key)
                start = int(item.get("startMinute", item.get("start_minute", 0)))
                end = int(item.get("endMinute", item.get("end_minute", 0)))
                if start < 0 or start >= 24 * 60 or end < 0 or end >= 24 * 60:
                    raise ModgenError(f"{key}: time segment minutes must be in [0, 1439]")
                segments.append(
                    {
                        "key": key,
                        "start": start,
                        "end": end,
                        "segment": normalize_time_segment(item.get("segment", "DAY")),
                    }
                )
    segments.sort(key=lambda segment: (segment["start"], segment["key"]))
    return segments


def collect_sprite_assets(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    assets = []
    seen = set()
    for mod in mods:
        for path in iter_json_files(mod["root"] / "sprites" / "assets"):
            for item in as_list(read_json(path), "assets"):
                key = key_for(mod["id"], item, path.stem)
                if key in seen:
                    raise ModgenError(f"Duplicate sprite asset key {key!r}")
                seen.add(key)
                assets.append(
                    {
                        "key": key,
                        "sheet": c_symbol(item.get("sheetSymbol", item.get("sheet"))),
                        "compressed_sheet": c_symbol(item.get("compressedSheetSymbol", item.get("compressed_sheet"))),
                        "palette": c_symbol(item.get("paletteSymbol", item.get("palette"))),
                        "compressed_palette": c_symbol(item.get("compressedPaletteSymbol", item.get("compressed_palette"))),
                        "template": c_symbol(item.get("templateSymbol", item.get("template"))),
                        "tile_tag": c_int_or_token(item.get("tileTag", item.get("tile_tag")), "TAG_NONE"),
                        "palette_tag": c_int_or_token(item.get("paletteTag", item.get("palette_tag")), "TAG_NONE"),
                    }
                )
    return assets


def collect_overworld_sprites(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    sprites = []
    seen = set()
    for mod in mods:
        roots = [
            (mod["root"] / "sprites" / "overworld", "sprites"),
            (mod["root"] / "outfits", "outfits"),
        ]
        for root, list_key in roots:
            for path in iter_json_files(root):
                for item in as_list(read_json(path), list_key):
                    key = key_for(mod["id"], item, path.stem)
                    if key in seen:
                        raise ModgenError(f"Duplicate overworld sprite key {key!r}")
                    seen.add(key)
                    asset_key = item.get("assetKey", item.get("asset"))
                    if asset_key is not None and ":" not in str(asset_key):
                        asset_key = f"{mod['id']}:{asset_key}"
                    sprites.append(
                        {
                            "key": key,
                            "asset_key": asset_key,
                            "graphics": c_int_or_token(item.get("graphicsId", item.get("graphics_id")), "0"),
                            "revision": c_int_or_token(item.get("graphicsRevision", item.get("graphics_revision")), "1"),
                        }
                    )
    return sprites


def collect_battle_sprites(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    sprites = []
    seen = set()
    for mod in mods:
        for path in iter_json_files(mod["root"] / "sprites" / "battle"):
            for item in as_list(read_json(path), "sprites"):
                key = key_for(mod["id"], item, path.stem)
                if key in seen:
                    raise ModgenError(f"Duplicate battle sprite key {key!r}")
                seen.add(key)
                asset_key = item.get("assetKey", item.get("asset"))
                if asset_key is not None and ":" not in str(asset_key):
                    asset_key = f"{mod['id']}:{asset_key}"
                sprites.append(
                    {
                        "key": key,
                        "asset_key": asset_key,
                        "species": c_int_or_token(item.get("species", item.get("trainerId")), "0"),
                        "form": c_int_or_token(item.get("form"), "0"),
                        "side": c_int_or_token(item.get("side"), "0"),
                        "flags": c_int_or_token(item.get("flags"), "0"),
                    }
                )
    return sprites


def collect_followers(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    followers = []
    seen = set()
    for mod in mods:
        for path in iter_json_files(mod["root"] / "followers"):
            for item in as_list(read_json(path), "followers"):
                key = key_for(mod["id"], item, path.stem)
                if key in seen:
                    raise ModgenError(f"Duplicate follower sprite key {key!r}")
                seen.add(key)
                followers.append(
                    {
                        "key": key,
                        "species": c_int_or_token(item.get("species"), "0"),
                        "form": c_int_or_token(item.get("form"), "0"),
                        "shiny": c_bool(item.get("shiny", False)),
                        "graphics": c_int_or_token(item.get("graphicsId", item.get("graphics_id")), "0"),
                    }
                )
    return followers


def collect_language_texts(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    texts = []
    seen = set()
    for mod in mods:
        lang_root = mod["root"] / "lang"
        for path in iter_json_files(lang_root):
            data = read_json(path)
            language = str(data.get("language", path.stem)) if isinstance(data, dict) else path.stem
            entries = data.get("strings", data) if isinstance(data, dict) else data
            if not isinstance(entries, dict):
                raise ModgenError(f"{path}: language file must be an object or contain a 'strings' object")
            for raw_key, value in sorted(entries.items()):
                key = raw_key if ":" in raw_key else f"{mod['id']}:{raw_key}"
                seen_key = (language, key)
                if seen_key in seen:
                    raise ModgenError(f"Duplicate language key {language}:{key}")
                seen.add(seen_key)
                texts.append({"language": language, "key": key, "text": str(value)})
    texts.sort(key=lambda text: (text["key"], text["language"]))
    return texts


def collect_pokeballs(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    balls = []
    seen_items = set()
    seen_keys = set()
    for mod in mods:
        for path in iter_json_files(mod["root"] / "pokeballs"):
            for item in as_list(read_json(path), "pokeballs"):
                key = key_for(mod["id"], item, path.stem)
                if key in seen_keys:
                    raise ModgenError(f"Duplicate pokeball key {key!r}")
                seen_keys.add(key)
                item_id = c_int_or_token(item.get("itemId", item.get("item_id")), "ITEM_POKE_BALL")
                if item_id in seen_items:
                    raise ModgenError(f"Duplicate pokeball item id {item_id!r}")
                seen_items.add(item_id)
                balls.append(
                    {
                        "key": key,
                        "item": item_id,
                        "ball": c_int_or_token(item.get("ballId", item.get("ball_id")), "BALL_POKE"),
                        "modifier": c_int_or_token(item.get("catchModifier", item.get("catch_modifier")), "10"),
                        "flags": c_int_or_token(item.get("flags"), "0"),
                        "modifier_hook": c_func(item.get("catchModifierHook", item.get("catch_modifier_hook"))),
                        "battle_script": c_symbol(item.get("battleScript", item.get("battle_script"))),
                        "commit_hook": c_func(item.get("commitHook", item.get("commit_hook"))),
                    }
                )
    return balls


def collect_engines(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    engines = [
        {
            "id": "engine:gen3",
            "name": "Generation III",
            "version": 1,
            "flags": "ENGINE_RULESET_FLAG_SAVE_COMPATIBLE",
            "capture": "NULL",
            "battle_weather": "NULL",
        }
    ]
    seen = {"engine:gen3"}
    for mod in mods:
        for path in iter_json_files(mod["root"] / "engines"):
            for item in as_list(read_json(path), "engines"):
                key = key_for(mod["id"], item, path.stem)
                if key in seen:
                    raise ModgenError(f"Duplicate engine key {key!r}")
                seen.add(key)
                flags = "ENGINE_RULESET_FLAG_SAVE_COMPATIBLE" if item.get("saveCompatible", False) else "0"
                engines.append(
                    {
                        "id": key,
                        "name": str(item.get("name", key)),
                        "version": int(item.get("version", 1)),
                        "flags": flags,
                        "capture": c_func(item.get("captureHook")),
                        "battle_weather": c_func(item.get("battleWeatherHook")),
                    }
                )
    return engines


def collect_npcs(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    npcs = []
    seen = set()
    next_id = 0
    for mod in mods:
        for path in iter_json_files(mod["root"] / "npcs"):
            for index, item in enumerate(as_list(read_json(path), "npcs")):
                key = key_for(mod["id"], item, f"npc_{index}")
                if key in seen:
                    raise ModgenError(f"Duplicate NPC key {key!r}")
                seen.add(key)
                npcs.append(
                    {
                        "key": key,
                        "id": next_id,
                        "graphics": c_int_or_token(item.get("graphicsId", item.get("graphics_id")), "OBJ_EVENT_GFX_BOY_1"),
                        "movement": c_int_or_token(item.get("movementType", item.get("movement_type")), "MOVEMENT_TYPE_FACE_DOWN"),
                        "local": c_int_or_token(item.get("localId", item.get("local_id")), "0"),
                        "elevation": c_int_or_token(item.get("elevation"), "3"),
                        "flag": c_int_or_token(item.get("flagId", item.get("flag")), "0"),
                        "script": c_func(item.get("scriptSymbol", item.get("script"))),
                    }
                )
                next_id += 1
    return npcs


def collect_maps(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    maps = []
    seen = set()
    next_id = 0
    for mod in mods:
        maps_root = mod["root"] / "maps"
        if not maps_root.exists():
            continue
        for path in sorted(maps_root.glob("*/map.json")):
            data = read_json(path)
            key = key_for(mod["id"], data, path.parent.name)
            if key in seen:
                raise ModgenError(f"Duplicate map key {key!r}")
            seen.add(key)
            maps.append(
                {
                    "key": key,
                    "id": next_id,
                    "group": c_int_or_token(data.get("map_group"), "0"),
                    "num": c_int_or_token(data.get("map_num"), "0"),
                    "path": path.relative_to(mod["repo_root"]).as_posix(),
                }
            )
            next_id += 1
    return maps


def collect_mod_sources(mods: List[Dict[str, Any]]) -> List[str]:
    sources = []
    seen = set()
    for mod in mods:
        for path in sorted((mod["root"] / "src").rglob("*.c")) if (mod["root"] / "src").exists() else []:
            rel = path.relative_to(mod["repo_root"]).as_posix()
            if rel not in seen:
                sources.append(rel)
                seen.add(rel)
        for entry in mod["manifest"].get("entrypoints", []):
            path = mod["root"] / str(entry)
            rel = path.relative_to(mod["repo_root"]).as_posix()
            if path.suffix == ".c" and path.exists() and rel not in seen:
                sources.append(rel)
                seen.add(rel)
    return sources


def write_header(path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(
        """#ifndef GUARD_GENERATED_MOD_REGISTRY_H
#define GUARD_GENERATED_MOD_REGISTRY_H

#include "global.h"
#include "mod/battle_sprite.h"
#include "mod/engine.h"
#include "mod/event.h"
#include "mod/flags.h"
#include "mod/language.h"
#include "mod/map.h"
#include "mod/npc.h"
#include "mod/overworld_sprite.h"
#include "mod/pokeball.h"
#include "mod/sprite_asset.h"
#include "mod/time.h"
#include "mod/types.h"
#include "mod/weather.h"

extern const struct ModManifest gModManifests[];
extern const u16 gModManifestCount;
extern const struct ModFlagDefinition gModFlagDefinitions[];
extern const u16 gModFlagDefinitionCount;
extern const struct ModEventSubscription gModEventSubscriptions[];
extern const u16 gModEventSubscriptionCount;
extern const struct ModWeatherProvider gModWeatherProviders[];
extern const u16 gModWeatherProviderCount;
extern const struct ModTimeSegmentDefinition gModTimeSegments[];
extern const u16 gModTimeSegmentCount;
extern const struct ModSpriteAssetDefinition gModSpriteAssets[];
extern const u16 gModSpriteAssetCount;
extern const struct ModOverworldSpriteDefinition gModOverworldSprites[];
extern const u16 gModOverworldSpriteCount;
extern const struct ModBattleSpriteDefinition gModBattleSprites[];
extern const u16 gModBattleSpriteCount;
extern const struct ModFollowerSpriteDefinition gModFollowerSprites[];
extern const u16 gModFollowerSpriteCount;
extern const struct ModLanguageText gModLanguageTexts[];
extern const u16 gModLanguageTextCount;
extern const struct ModPokeBallDefinition gModPokeBallDefinitions[];
extern const u16 gModPokeBallDefinitionCount;
extern const struct EngineRuleset gModEngineRulesets[];
extern const u16 gModEngineRulesetCount;
extern const struct ModNpcDefinition gModNpcDefinitions[];
extern const u16 gModNpcDefinitionCount;
extern const struct ModMapDefinition gModMapDefinitions[];
extern const u16 gModMapDefinitionCount;

#endif // GUARD_GENERATED_MOD_REGISTRY_H
""",
        encoding="utf-8",
    )


def write_source(path: Path, mods: List[Dict[str, Any]], flags: List[Dict[str, Any]], events: List[Dict[str, Any]], weather: List[Dict[str, Any]], time_segments: List[Dict[str, Any]], sprite_assets: List[Dict[str, Any]], overworld_sprites: List[Dict[str, Any]], battle_sprites: List[Dict[str, Any]], followers: List[Dict[str, Any]], language_texts: List[Dict[str, Any]], pokeballs: List[Dict[str, Any]], engines: List[Dict[str, Any]], npcs: List[Dict[str, Any]], maps: List[Dict[str, Any]]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    event_handlers = sorted({event["handler"] for event in events if event["handler"] != "NULL"})
    weather_handlers = sorted({provider["handler"] for provider in weather if provider["handler"] != "NULL"})
    capture_hooks = sorted({engine["capture"] for engine in engines if engine["capture"] != "NULL"})
    battle_weather_hooks = sorted({engine["battle_weather"] for engine in engines if engine["battle_weather"] != "NULL"})
    npc_scripts = sorted({npc["script"] for npc in npcs if npc["script"] != "NULL"})
    sheet_symbols = sorted({asset["sheet"] for asset in sprite_assets if asset["sheet"] != "NULL"})
    compressed_sheet_symbols = sorted({asset["compressed_sheet"] for asset in sprite_assets if asset["compressed_sheet"] != "NULL"})
    palette_symbols = sorted({asset["palette"] for asset in sprite_assets if asset["palette"] != "NULL"})
    compressed_palette_symbols = sorted({asset["compressed_palette"] for asset in sprite_assets if asset["compressed_palette"] != "NULL"})
    template_symbols = sorted({asset["template"] for asset in sprite_assets if asset["template"] != "NULL"})
    pokeball_modifier_hooks = sorted({ball["modifier_hook"] for ball in pokeballs if ball["modifier_hook"] != "NULL"})
    pokeball_commit_hooks = sorted({ball["commit_hook"] for ball in pokeballs if ball["commit_hook"] != "NULL"})
    pokeball_scripts = sorted({ball["battle_script"] for ball in pokeballs if ball["battle_script"] != "NULL"})

    lines = [
        "/* Auto-generated by scripts/modgen.py. */",
        '#include "global.h"',
        '#include "constants/items.h"',
        '#include "constants/event_object_movement.h"',
        '#include "constants/event_objects.h"',
        '#include "generated/mod_registry.h"',
        '#include "pokeball.h"',
        "",
    ]

    for handler in event_handlers:
        lines.append(f"extern s8 {handler}(const struct ModEvent *event);")
    for handler in weather_handlers:
        lines.append(f"extern bool8 {handler}(struct ModWeatherDisplay *display);")
    for hook in capture_hooks:
        lines.append(f"extern u8 {hook}(const struct EngineRuleset *ruleset, u16 ballItemId);")
    for hook in battle_weather_hooks:
        lines.append(f"extern u32 {hook}(const struct EngineRuleset *ruleset, u16 weatherLayers);")
    for script in npc_scripts:
        lines.append(f"extern const u8 {script}[];")
    for symbol in sheet_symbols:
        lines.append(f"extern const struct SpriteSheet {symbol};")
    for symbol in compressed_sheet_symbols:
        lines.append(f"extern const struct CompressedSpriteSheet {symbol};")
    for symbol in palette_symbols:
        lines.append(f"extern const struct SpritePalette {symbol};")
    for symbol in compressed_palette_symbols:
        lines.append(f"extern const struct CompressedSpritePalette {symbol};")
    for symbol in template_symbols:
        lines.append(f"extern const struct SpriteTemplate {symbol};")
    for hook in pokeball_modifier_hooks:
        lines.append(f"extern u8 {hook}(const struct PokeBallCatchContext *context);")
    for hook in pokeball_commit_hooks:
        lines.append(f"extern void {hook}(const struct PokeBallCatchContext *context, const struct PokeBallThrowResult *result);")
    for script in pokeball_scripts:
        lines.append(f"extern const u8 {script}[];")
    if event_handlers or weather_handlers or capture_hooks or battle_weather_hooks or npc_scripts or sheet_symbols or compressed_sheet_symbols or palette_symbols or compressed_palette_symbols or template_symbols or pokeball_modifier_hooks or pokeball_commit_hooks or pokeball_scripts:
        lines.append("")

    lines.append("const struct ModManifest gModManifests[] =")
    lines.append("{")
    if mods:
        for mod in mods:
            lines.append(f"    {{ {c_string(mod['id'])}, {c_string(mod['name'])}, {c_string(mod['version'])}, {mod['priority']}, {mod['features']} }},")
    else:
        lines.append("    { NULL, NULL, NULL, 0, 0 },")
    lines.append("};")
    lines.append(f"const u16 gModManifestCount = {len(mods)};")
    lines.append("")

    lines.append("const struct ModFlagDefinition gModFlagDefinitions[] =")
    lines.append("{")
    if flags:
        for flag in flags:
            lines.append(f"    {{ {c_string(flag['key'])}, {flag['id']}, {c_bool(flag['initial'])} }},")
    else:
        lines.append("    { NULL, 0, FALSE },")
    lines.append("};")
    lines.append(f"const u16 gModFlagDefinitionCount = {len(flags)};")
    lines.append("")

    lines.append("const struct ModEventSubscription gModEventSubscriptions[] =")
    lines.append("{")
    if events:
        for event in events:
            lines.append(f"    {{ {event['type']}, {event['priority']}, {event['handler']}, {c_string(event['source'])} }},")
    else:
        lines.append("    { 0, 0, NULL, NULL },")
    lines.append("};")
    lines.append(f"const u16 gModEventSubscriptionCount = {len(events)};")
    lines.append("")

    lines.append("const struct ModWeatherProvider gModWeatherProviders[] =")
    lines.append("{")
    if weather:
        for provider in weather:
            lines.append(f"    {{ {c_string(provider['id'])}, {provider['priority']}, {provider['handler']} }},")
    else:
        lines.append("    { NULL, 0, NULL },")
    lines.append("};")
    lines.append(f"const u16 gModWeatherProviderCount = {len(weather)};")
    lines.append("")

    lines.append("const struct ModTimeSegmentDefinition gModTimeSegments[] =")
    lines.append("{")
    if time_segments:
        for segment in time_segments:
            lines.append(f"    {{ {c_string(segment['key'])}, {segment['start']}, {segment['end']}, {segment['segment']} }},")
    else:
        lines.append("    { NULL, 0, 0, 0 },")
    lines.append("};")
    lines.append(f"const u16 gModTimeSegmentCount = {len(time_segments)};")
    lines.append("")

    lines.append("const struct ModSpriteAssetDefinition gModSpriteAssets[] =")
    lines.append("{")
    if sprite_assets:
        for asset in sprite_assets:
            lines.append(f"    {{ {c_string(asset['key'])}, {asset['sheet']}, {asset['compressed_sheet']}, {asset['palette']}, {asset['compressed_palette']}, {asset['template']}, {asset['tile_tag']}, {asset['palette_tag']} }},")
    else:
        lines.append("    { NULL, NULL, NULL, NULL, NULL, NULL, TAG_NONE, TAG_NONE },")
    lines.append("};")
    lines.append(f"const u16 gModSpriteAssetCount = {len(sprite_assets)};")
    lines.append("")

    lines.append("const struct ModOverworldSpriteDefinition gModOverworldSprites[] =")
    lines.append("{")
    if overworld_sprites:
        for sprite in overworld_sprites:
            lines.append(f"    {{ {c_string(sprite['key'])}, {c_string(sprite['asset_key'])}, {sprite['graphics']}, {sprite['revision']} }},")
    else:
        lines.append("    { NULL, NULL, 0, 0 },")
    lines.append("};")
    lines.append(f"const u16 gModOverworldSpriteCount = {len(overworld_sprites)};")
    lines.append("")

    lines.append("const struct ModBattleSpriteDefinition gModBattleSprites[] =")
    lines.append("{")
    if battle_sprites:
        for sprite in battle_sprites:
            lines.append(f"    {{ {c_string(sprite['key'])}, {c_string(sprite['asset_key'])}, {sprite['species']}, {sprite['form']}, {sprite['side']}, {sprite['flags']} }},")
    else:
        lines.append("    { NULL, NULL, 0, 0, 0, 0 },")
    lines.append("};")
    lines.append(f"const u16 gModBattleSpriteCount = {len(battle_sprites)};")
    lines.append("")

    lines.append("const struct ModFollowerSpriteDefinition gModFollowerSprites[] =")
    lines.append("{")
    if followers:
        for follower in followers:
            lines.append(f"    {{ {c_string(follower['key'])}, {follower['species']}, {follower['form']}, {follower['shiny']}, {follower['graphics']} }},")
    else:
        lines.append("    { NULL, 0, 0, FALSE, 0 },")
    lines.append("};")
    lines.append(f"const u16 gModFollowerSpriteCount = {len(followers)};")
    lines.append("")

    lines.append("const struct ModLanguageText gModLanguageTexts[] =")
    lines.append("{")
    if language_texts:
        for text in language_texts:
            lines.append(f"    {{ {c_string(text['language'])}, {c_string(text['key'])}, {c_u8_string(text['text'])} }},")
    else:
        lines.append("    { NULL, NULL, NULL },")
    lines.append("};")
    lines.append(f"const u16 gModLanguageTextCount = {len(language_texts)};")
    lines.append("")

    lines.append("const struct ModPokeBallDefinition gModPokeBallDefinitions[] =")
    lines.append("{")
    if pokeballs:
        for ball in pokeballs:
            lines.append(f"    {{ {c_string(ball['key'])}, {ball['item']}, {ball['ball']}, {ball['modifier']}, {ball['flags']}, {ball['modifier_hook']}, {ball['battle_script']}, {ball['commit_hook']} }},")
    else:
        lines.append("    { NULL, 0, BALL_POKE, 10, 0, NULL, NULL, NULL },")
    lines.append("};")
    lines.append(f"const u16 gModPokeBallDefinitionCount = {len(pokeballs)};")
    lines.append("")

    lines.append("const struct EngineRuleset gModEngineRulesets[] =")
    lines.append("{")
    for engine in engines:
        lines.append(f"    {{ {c_string(engine['id'])}, {c_string(engine['name'])}, {engine['version']}, {engine['flags']}, {engine['capture']}, {engine['battle_weather']} }},")
    lines.append("};")
    lines.append(f"const u16 gModEngineRulesetCount = {len(engines)};")
    lines.append("")

    lines.append("const struct ModNpcDefinition gModNpcDefinitions[] =")
    lines.append("{")
    if npcs:
        for npc in npcs:
            lines.append(f"    {{ {c_string(npc['key'])}, {npc['id']}, {npc['graphics']}, {npc['movement']}, {npc['local']}, {npc['elevation']}, {npc['flag']}, {npc['script']} }},")
    else:
        lines.append("    { NULL, 0, 0, 0, 0, 0, 0, NULL },")
    lines.append("};")
    lines.append(f"const u16 gModNpcDefinitionCount = {len(npcs)};")
    lines.append("")

    lines.append("const struct ModMapDefinition gModMapDefinitions[] =")
    lines.append("{")
    if maps:
        for map_def in maps:
            lines.append(f"    {{ {c_string(map_def['key'])}, {map_def['id']}, {map_def['group']}, {map_def['num']}, {c_string(map_def['path'])} }},")
    else:
        lines.append("    { NULL, 0, 0, 0, NULL },")
    lines.append("};")
    lines.append(f"const u16 gModMapDefinitionCount = {len(maps)};")
    lines.append("")

    path.write_text("\n".join(lines), encoding="utf-8")


def write_make_fragment(path: Path, sources: List[str]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    lines = ["# Auto-generated by scripts/modgen.py.", "MOD_C_SRCS :="]
    for source in sources:
        lines.append(f"MOD_C_SRCS += {source}")
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", default=".")
    parser.add_argument("--out-header", default="include/generated/mod_registry.h")
    parser.add_argument("--out-source", default="src/generated/mod_registry.c")
    parser.add_argument("--out-make", default="build/generated/mod_sources.mk")
    args = parser.parse_args()

    root = Path(args.root).resolve()
    mods = load_mods(root)
    flags = collect_flags(mods)
    events = collect_events(mods)
    weather = collect_weather(mods)
    time_segments = collect_time_segments(mods)
    sprite_assets = collect_sprite_assets(mods)
    overworld_sprites = collect_overworld_sprites(mods)
    battle_sprites = collect_battle_sprites(mods)
    followers = collect_followers(mods)
    language_texts = collect_language_texts(mods)
    pokeballs = collect_pokeballs(mods)
    engines = collect_engines(mods)
    npcs = collect_npcs(mods)
    maps = collect_maps(mods)
    sources = collect_mod_sources(mods)

    write_header(root / args.out_header)
    write_source(
        root / args.out_source,
        mods,
        flags,
        events,
        weather,
        time_segments,
        sprite_assets,
        overworld_sprites,
        battle_sprites,
        followers,
        language_texts,
        pokeballs,
        engines,
        npcs,
        maps,
    )
    write_make_fragment(root / args.out_make, sources)
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except ModgenError as exc:
        print(f"modgen: {exc}")
        raise SystemExit(1)
