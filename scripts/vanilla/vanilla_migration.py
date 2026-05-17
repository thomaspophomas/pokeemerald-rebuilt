#!/usr/bin/env python3
"""Extract and verify vanilla data as the built-in vanilla mod.

This is intentionally conservative: when a domain cannot yet be represented as
typed gameplay records, it is still migrated as deterministic JSON metadata over
the authoritative vanilla source files. That gives every API domain a JSON
source of truth while the C/INC mirrors can continue to be materialized for the
existing build pipeline.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import re
import shutil
import sys
import time
from pathlib import Path
from typing import Any, Dict, Iterable, List, Optional, Sequence, Tuple


API_DOMAINS = [
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

MOD_ID = "vanilla"
SCHEMA_VERSION = 1


DOMAIN_SOURCE_SPECS: Dict[str, Sequence[str]] = {
    "flags": [
        "include/constants/flags.h",
        "include/constants/vars.h",
        "include/event_data.h",
        "src/event_data.c",
    ],
    "events": [
        "data/scripts",
        "data/maps",
        "include/script.h",
        "include/event_scripts.h",
        "src/script.c",
        "src/scrcmd.c",
        "src/event_object_movement.c",
    ],
    "language": [
        "data/text",
        "src/data/text",
        "include/strings.h",
        "src/strings.c",
    ],
    "sprite_assets": [
        "graphics/object_events",
        "graphics/trainers",
        "graphics/pokemon",
        "graphics/items",
        "src/data/graphics",
    ],
    "overworld_sprites": [
        "graphics/object_events",
        "src/data/object_events",
        "include/constants/event_objects.h",
    ],
    "battle_sprites": [
        "graphics/trainers",
        "graphics/pokemon",
        "src/data/pokemon_graphics",
        "src/trainer_pokemon_sprites.c",
        "src/data/graphics/pokemon.h",
    ],
    "followers": [
        "graphics/object_events",
        "src/data/object_events",
    ],
    "outfits": [
        "graphics/object_events/pics/people",
        "src/player_pc.c",
        "src/overworld.c",
        "include/constants/event_objects.h",
    ],
    "pokeballs": [
        "include/constants/items.h",
        "include/pokeball.h",
        "src/pokeball.c",
        "src/mod/pokeball.c",
        "src/battle_anim_throw.c",
        "data/battle_scripts_2.s",
        "graphics/items",
    ],
    "items": [
        "include/constants/items.h",
        "src/data/items.h",
        "src/data/text/item_descriptions.h",
        "src/data/graphics/items.h",
        "graphics/items",
    ],
    "pokemon": [
        "include/constants/species.h",
        "include/constants/pokemon.h",
        "include/pokemon.h",
        "src/data/pokemon",
        "src/data/pokemon_graphics",
        "src/data/graphics/pokemon.h",
        "graphics/pokemon",
    ],
    "moves": [
        "include/constants/moves.h",
        "src/data/battle_moves.h",
        "src/data/contest_moves.h",
        "src/data/text/move_descriptions.h",
        "data/battle_anim_scripts.s",
        "src/data/bard_music/moves.h",
    ],
    "shops": [
        "include/shop.h",
        "src/shop.c",
        "data/text/mart_clerk.inc",
        "data/maps",
    ],
    "state": [
        "include/save.h",
        "include/global.h",
        "src/save.c",
        "src/mod/state.c",
        "include/mod/state.h",
    ],
}


def rel(root: Path, path: Path) -> str:
    return path.resolve().relative_to(root.resolve()).as_posix()


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8", errors="replace")


def read_json(path: Path) -> Any:
    with path.open("r", encoding="utf-8") as handle:
        return json.load(handle)


def write_json(path: Path, data: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8", newline="\n") as handle:
        json.dump(data, handle, indent=2, ensure_ascii=True)
        handle.write("\n")


def sha1_bytes(data: bytes) -> str:
    return hashlib.sha1(data).hexdigest()


def sha1_file(path: Path) -> str:
    with path.open("rb") as handle:
        digest = hashlib.sha1()
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
        return digest.hexdigest()


def copy_file_exact(src: Path, dst: Path) -> None:
    dst.parent.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(src, dst)


def collect_files(root: Path, specs: Sequence[str]) -> List[Path]:
    files: List[Path] = []
    seen = set()
    for spec in specs:
        candidates: Iterable[Path]
        if any(token in spec for token in ["*", "?", "["]):
            candidates = root.glob(spec)
        else:
            path = root / spec
            if path.is_dir():
                candidates = path.rglob("*")
            elif path.exists():
                candidates = [path]
            else:
                candidates = []

        for candidate in candidates:
            if candidate.is_file():
                key = candidate.resolve()
                if key not in seen:
                    files.append(candidate)
                    seen.add(key)
    return sorted(files, key=lambda path: rel(root, path))


def source_manifest(root: Path, files: Sequence[Path]) -> List[Dict[str, Any]]:
    entries = []
    for path in files:
        stat = path.stat()
        entries.append(
            {
                "path": rel(root, path),
                "sha1": sha1_file(path),
                "bytes": stat.st_size,
            }
        )
    return entries


def combined_file_digest(root: Path, files: Sequence[Path]) -> str:
    digest = hashlib.sha1()
    for path in sorted(files, key=lambda candidate: rel(root, candidate)):
        relative = rel(root, path).encode("utf-8")
        digest.update(relative)
        digest.update(b"\0")
        digest.update(sha1_file(path).encode("ascii"))
        digest.update(b"\0")
    return digest.hexdigest()


def ensure_mod_skeleton(root: Path) -> None:
    vanilla = root / "mods" / MOD_ID
    for domain in API_DOMAINS:
        (vanilla / domain).mkdir(parents=True, exist_ok=True)
        (vanilla / "src" / domain).mkdir(parents=True, exist_ok=True)
        (vanilla / "expectations" / domain).mkdir(parents=True, exist_ok=True)
    (root / "docs" / "mod_api_schemas").mkdir(parents=True, exist_ok=True)
    write_json(
        vanilla / "mod.json",
        {
            "id": MOD_ID,
            "name": "Pokemon Emerald Vanilla",
            "version": "1.0.0",
            "priority": 0,
            "featureFlags": 0,
            "dependencies": [],
            "sourceOfTruth": "mods/vanilla",
            "schemaVersion": SCHEMA_VERSION,
            "apiDomains": API_DOMAINS,
        },
    )


def c_prefix(domain: str) -> str:
    return "".join(part.title() for part in domain.split("_"))


def write_c_adapters(root: Path, domains: Sequence[str]) -> None:
    for domain in domains:
        path = root / "mods" / MOD_ID / "src" / domain / f"vanilla_{domain}.c"
        body = f"""#include \"global.h\"

/* Adapter anchor for the vanilla {domain} JSON domain. */
const char *Vanilla{c_prefix(domain)}Api_GetDomain(void)
{{
    return \"{domain}\";
}}
"""
        path.write_text(body, encoding="utf-8", newline="\n")


def write_schema(root: Path, domain: str) -> None:
    title = "Vanilla " + domain.replace("_", " ").title() + " API"
    write_json(
        root / "docs" / "mod_api_schemas" / f"{domain}.schema.json",
        {
            "$schema": "https://json-schema.org/draft/2020-12/schema",
            "$id": f"https://pokeemerald.local/mod_api_schemas/{domain}.schema.json",
            "title": title,
            "description": (
                "Modder-facing entity JSON representation for the vanilla "
                f"{domain} mod API domain. Each domain has an index.json, "
                "one or more entity JSON files, and _source_manifest.json "
                "only for audit and drift checks."
            ),
            "type": "object",
            "required": ["domain", "schemaVersion"],
            "properties": {
                "id": {"type": "string"},
                "domain": {"type": "string"},
                "schemaVersion": {"type": "integer"},
                "entityKind": {"type": "string"},
                "semanticCoverage": {"type": "string"},
                "source": {"type": "object"},
                "legacy": {"type": "object"},
                "entries": {"type": "array"},
            },
            "examples": [
                {
                    "domain": domain,
                    "schemaVersion": SCHEMA_VERSION,
                    "entityKind": f"{domain}_entity",
                    "semanticCoverage": "full",
                    "entries": [{"id": "EXAMPLE", "path": "EXAMPLE.json"}],
                }
            ],
            "additionalProperties": True,
        },
    )


def write_all_schemas(root: Path, domains: Sequence[str]) -> None:
    for domain in domains:
        write_schema(root, domain)


def expectation_for_domain(root: Path, domain: str) -> Dict[str, Any]:
    domain_root = root / "mods" / MOD_ID / domain
    files = sorted([path for path in domain_root.rglob("*") if path.is_file()], key=lambda path: rel(root, path))
    json_files = [path for path in files if path.suffix == ".json"]
    return {
        "domain": domain,
        "schemaVersion": SCHEMA_VERSION,
        "fileCount": len(files),
        "jsonFileCount": len(json_files),
        "bytes": sum(path.stat().st_size for path in files),
        "sha1": combined_file_digest(root, files),
        "files": source_manifest(root, files),
    }


def write_expectation(root: Path, domain: str, extra: Optional[Dict[str, Any]] = None) -> None:
    expectation = expectation_for_domain(root, domain)
    if extra:
        expectation["counts"] = extra
    write_json(
        root / "mods" / MOD_ID / "expectations" / domain / "baseline.expected.json",
        expectation,
    )


def discover_map_group_lookup(root: Path) -> Dict[str, Dict[str, Any]]:
    map_groups_path = root / "data" / "maps" / "map_groups.json"
    groups = read_json(map_groups_path)
    lookup: Dict[str, Dict[str, Any]] = {}
    for group_index, group_name in enumerate(groups.get("group_order", [])):
        for map_index, map_name in enumerate(groups.get(group_name, [])):
            lookup[map_name] = {
                "mapGroup": group_index,
                "mapNum": map_index,
                "mapGroupSymbol": group_name,
            }
    return lookup


def list_len(value: Any) -> int:
    return len(value) if isinstance(value, list) else 0


def extract_maps(root: Path) -> Dict[str, Any]:
    maps_root = root / "data" / "maps"
    out_root = root / "mods" / MOD_ID / "maps"
    map_dirs = sorted([path for path in maps_root.iterdir() if path.is_dir() and (path / "map.json").exists()])
    group_lookup = discover_map_group_lookup(root)
    index = []

    copy_file_exact(maps_root / "map_groups.json", out_root / "map_groups.json")
    for map_dir in map_dirs:
        map_name = map_dir.name
        copy_file_exact(map_dir / "map.json", out_root / map_name / "map.json")
        scripts_path = map_dir / "scripts.inc"
        if scripts_path.exists():
            copy_file_exact(scripts_path, out_root / map_name / "scripts.inc")
        data = read_json(map_dir / "map.json")
        entry = {
            "name": map_name,
            "id": data.get("id"),
            "sourcePath": rel(root, map_dir / "map.json"),
            "jsonPath": rel(root, out_root / map_name / "map.json"),
            "scriptsPath": rel(root, out_root / map_name / "scripts.inc") if scripts_path.exists() else None,
            "mapGroup": group_lookup.get(map_name, {}).get("mapGroup"),
            "mapNum": group_lookup.get(map_name, {}).get("mapNum"),
            "mapGroupSymbol": group_lookup.get(map_name, {}).get("mapGroupSymbol"),
            "objectEventCount": list_len(data.get("object_events")),
            "warpCount": list_len(data.get("warp_events")),
            "coordEventCount": list_len(data.get("coord_events")),
            "bgEventCount": list_len(data.get("bg_events")),
            "connectionCount": list_len(data.get("connections")),
            "interactionPolicy": "EXCLUSIVE",
            "resourceLocks": ["MAP_SCRIPT", "OBJECT_EVENT", "WARP"],
        }
        index.append(entry)
    write_json(
        out_root / "index.json",
        {
            "domain": "maps",
            "schemaVersion": SCHEMA_VERSION,
            "mapCount": len(map_dirs),
            "maps": index,
        },
    )
    return {"maps": len(map_dirs)}


def extract_npcs(root: Path) -> Dict[str, Any]:
    maps_root = root / "data" / "maps"
    out = root / "mods" / MOD_ID / "npcs" / "by_map.json"
    entries = []
    total = 0
    for map_json in sorted(maps_root.glob("*/map.json")):
        map_data = read_json(map_json)
        map_name = map_json.parent.name
        object_events = map_data.get("object_events", [])
        for index, obj in enumerate(object_events):
            local_id = obj.get("local_id", index + 1)
            script = obj.get("script")
            trainer_type = obj.get("trainer_type", "TRAINER_TYPE_NONE")
            entries.append(
                {
                    "key": f"{map_name}:{local_id}",
                    "map": map_name,
                    "mapId": map_data.get("id"),
                    "localId": local_id,
                    "graphicsId": obj.get("graphics_id"),
                    "x": obj.get("x"),
                    "y": obj.get("y"),
                    "elevation": obj.get("elevation"),
                    "movementType": obj.get("movement_type"),
                    "movementRangeX": obj.get("movement_range_x"),
                    "movementRangeY": obj.get("movement_range_y"),
                    "trainerType": trainer_type,
                    "trainerSightOrBerryTreeId": obj.get("trainer_sight_or_berry_tree_id"),
                    "script": script,
                    "flag": obj.get("flag"),
                    "interactionPolicy": "EXCLUSIVE" if script not in (None, "0", "0x0") else "DISABLED_ONLINE",
                    "resourceLocks": ["OBJECT_EVENT", "SCRIPT"] if script not in (None, "0", "0x0") else [],
                    "sourcePath": rel(root, map_json),
                }
            )
            total += 1
    write_json(
        out,
        {
            "domain": "npcs",
            "schemaVersion": SCHEMA_VERSION,
            "vanillaNpcs": entries,
        },
    )
    return {"npcs": total}


def extract_weather(root: Path) -> Dict[str, Any]:
    entries = []
    by_weather: Dict[str, int] = {}
    for map_json in sorted((root / "data" / "maps").glob("*/map.json")):
        data = read_json(map_json)
        weather = data.get("weather")
        entries.append({"map": map_json.parent.name, "mapId": data.get("id"), "weather": weather})
        by_weather[str(weather)] = by_weather.get(str(weather), 0) + 1
    write_json(
        root / "mods" / MOD_ID / "weather" / "map_weather.json",
        {
            "domain": "weather",
            "schemaVersion": SCHEMA_VERSION,
            "mapWeather": entries,
            "weatherCounts": by_weather,
        },
    )
    return {"mapsWithWeather": len(entries), "weatherKinds": len(by_weather)}


def find_matching_brace(text: str, brace_start: int) -> int:
    depth = 0
    in_string = False
    in_char = False
    in_line_comment = False
    in_block_comment = False
    escape = False
    i = brace_start
    while i < len(text):
        ch = text[i]
        nxt = text[i + 1] if i + 1 < len(text) else ""
        if in_line_comment:
            if ch == "\n":
                in_line_comment = False
        elif in_block_comment:
            if ch == "*" and nxt == "/":
                in_block_comment = False
                i += 1
        elif in_string:
            if escape:
                escape = False
            elif ch == "\\":
                escape = True
            elif ch == '"':
                in_string = False
        elif in_char:
            if escape:
                escape = False
            elif ch == "\\":
                escape = True
            elif ch == "'":
                in_char = False
        else:
            if ch == "/" and nxt == "/":
                in_line_comment = True
                i += 1
            elif ch == "/" and nxt == "*":
                in_block_comment = True
                i += 1
            elif ch == '"':
                in_string = True
            elif ch == "'":
                in_char = True
            elif ch == "{":
                depth += 1
            elif ch == "}":
                depth -= 1
                if depth == 0:
                    return i
        i += 1
    raise ValueError("unterminated C initializer")


def extract_c_field(block: str, field: str) -> Optional[str]:
    match = re.search(rf"\.{re.escape(field)}\s*=\s*([^,\n]+)", block)
    return match.group(1).strip() if match else None


def extract_trainer_name(block: str) -> Optional[str]:
    match = re.search(r"\.trainerName\s*=\s*_\(\"((?:\\.|[^\"])*)\"\)", block)
    if not match:
        return None
    return match.group(1)


def parse_trainer_blocks(root: Path) -> List[Dict[str, Any]]:
    text = read_text(root / "src" / "data" / "trainers.h")
    opponent_ids = parse_opponent_ids(root)
    entry_re = re.compile(r"(?m)^\s*\[(TRAINER_[A-Z0-9_]+)\]\s*=\s*\{")
    trainers = []
    for match in entry_re.finditer(text):
        symbol = match.group(1)
        brace_start = text.find("{", match.start(), match.end())
        brace_end = find_matching_brace(text, brace_start)
        line_end = text.find("\n", brace_end)
        raw_block = text[match.start() : line_end if line_end != -1 else brace_end + 1].rstrip()
        party_match = re.search(r"sParty_[A-Za-z0-9_]+", raw_block)
        trainers.append(
            {
                "symbol": symbol,
                "numericId": opponent_ids.get(symbol),
                "trainerName": extract_trainer_name(raw_block),
                "trainerClass": extract_c_field(raw_block, "trainerClass"),
                "trainerPic": extract_c_field(raw_block, "trainerPic"),
                "encounterMusicGender": extract_c_field(raw_block, "encounterMusic_gender"),
                "doubleBattle": extract_c_field(raw_block, "doubleBattle"),
                "aiFlags": extract_c_field(raw_block, "aiFlags"),
                "partyRef": party_match.group(0) if party_match else None,
                "rawC": raw_block,
                "sourcePath": "src/data/trainers.h",
            }
        )
    return trainers


def parse_opponent_ids(root: Path) -> Dict[str, int]:
    text = read_text(root / "include" / "constants" / "opponents.h")
    ids: Dict[str, int] = {}
    for match in re.finditer(r"^#define\s+(TRAINER_[A-Z0-9_]+)\s+([0-9]+)\b", text, re.MULTILINE):
        ids[match.group(1)] = int(match.group(2))
    return ids


def parse_party_blocks(root: Path) -> List[Dict[str, Any]]:
    text = read_text(root / "src" / "data" / "trainer_parties.h")
    entry_re = re.compile(r"(?m)^static const struct\s+(TrainerMon[A-Za-z0-9_]+)\s+(sParty_[A-Za-z0-9_]+)\[\]\s*=\s*\{")
    parties = []
    for match in entry_re.finditer(text):
        party_type = match.group(1)
        symbol = match.group(2)
        brace_start = text.find("{", match.start(), match.end())
        brace_end = find_matching_brace(text, brace_start)
        semicolon = text.find(";", brace_end)
        raw_block = text[match.start() : semicolon + 1].rstrip()
        pokemon_count = len(re.findall(r"\.species\s*=", raw_block))
        parties.append(
            {
                "symbol": symbol,
                "partyType": party_type,
                "pokemonCount": pokemon_count,
                "rawC": raw_block,
                "sourcePath": "src/data/trainer_parties.h",
            }
        )
    return parties


def extract_trainers(root: Path) -> Tuple[Dict[str, Any], Dict[str, Any]]:
    trainers = parse_trainer_blocks(root)
    parties = parse_party_blocks(root)
    copy_file_exact(
        root / "src" / "data" / "trainers.h",
        root / "mods" / MOD_ID / "trainers" / "legacy" / "trainers.h",
    )
    copy_file_exact(
        root / "include" / "constants" / "opponents.h",
        root / "mods" / MOD_ID / "trainers" / "legacy" / "opponents.h",
    )
    copy_file_exact(
        root / "src" / "data" / "trainer_parties.h",
        root / "mods" / MOD_ID / "trainer_parties" / "legacy" / "trainer_parties.h",
    )
    write_json(
        root / "mods" / MOD_ID / "trainers" / "trainers.json",
        {
            "domain": "trainers",
            "schemaVersion": SCHEMA_VERSION,
            "trainers": trainers,
            "interactionPolicy": "EXCLUSIVE",
            "resourceLocks": ["TRAINER_BATTLE", "SCRIPT"],
        },
    )
    write_json(
        root / "mods" / MOD_ID / "trainer_parties" / "parties.json",
        {
            "domain": "trainer_parties",
            "schemaVersion": SCHEMA_VERSION,
            "trainerParties": parties,
        },
    )
    return {"trainers": len(trainers)}, {"trainerParties": len(parties)}


def extract_wild_encounters(root: Path) -> Dict[str, Any]:
    src = root / "src" / "data" / "wild_encounters.json"
    data = read_json(src)
    write_json(
        root / "mods" / MOD_ID / "wild_encounters" / "wild_encounters.json",
        {
            "domain": "wild_encounters",
            "schemaVersion": SCHEMA_VERSION,
            "sourcePath": rel(root, src),
            "wildEncounters": data,
        },
    )
    copy_file_exact(src, root / "mods" / MOD_ID / "wild_encounters" / "legacy" / "wild_encounters.json")
    return {"files": 1}


def extract_time(root: Path) -> Dict[str, Any]:
    segments = [
        {"id": "morning", "segment": "MORNING", "startMinute": 4 * 60, "endMinute": 10 * 60 - 1},
        {"id": "day", "segment": "DAY", "startMinute": 10 * 60, "endMinute": 17 * 60 - 1},
        {"id": "evening", "segment": "EVENING", "startMinute": 17 * 60, "endMinute": 20 * 60 - 1},
        {"id": "night", "segment": "NIGHT", "startMinute": 20 * 60, "endMinute": 4 * 60 - 1},
    ]
    write_json(
        root / "mods" / MOD_ID / "time" / "segments.json",
        {
            "domain": "time",
            "schemaVersion": SCHEMA_VERSION,
            "segments": segments,
        },
    )
    return {"segments": len(segments)}


def extract_engine_rulesets(root: Path) -> Dict[str, Any]:
    rulesets = [
        {
            "id": "engine:gen3",
            "name": "Generation III",
            "version": 1,
            "saveCompatible": True,
            "captureHook": None,
            "battleWeatherHook": None,
        }
    ]
    write_json(
        root / "mods" / MOD_ID / "engine_rulesets" / "gen3.json",
        {
            "domain": "engine_rulesets",
            "schemaVersion": SCHEMA_VERSION,
            "engineRulesets": rulesets,
        },
    )
    return {"rulesets": len(rulesets)}


def extract_quests(root: Path) -> Dict[str, Any]:
    quest_events = []
    for map_json in sorted((root / "data" / "maps").glob("*/map.json")):
        data = read_json(map_json)
        for key in ["coord_events", "bg_events", "warp_events"]:
            for index, event in enumerate(data.get(key, [])):
                quest_events.append(
                    {
                        "map": map_json.parent.name,
                        "mapId": data.get("id"),
                        "eventType": key,
                        "index": index,
                        "script": event.get("script"),
                        "raw": event,
                        "interactionPolicy": "EXCLUSIVE" if event.get("script") else "SHARED_READONLY",
                        "resourceLocks": ["SCRIPT"] if event.get("script") else [],
                    }
                )
    write_json(
        root / "mods" / MOD_ID / "quests" / "map_event_hooks.json",
        {
            "domain": "quests",
            "schemaVersion": SCHEMA_VERSION,
            "questHooks": quest_events,
        },
    )
    return {"questHooks": len(quest_events)}


def extract_source_manifest_domain(root: Path, domain: str) -> Dict[str, Any]:
    files = collect_files(root, DOMAIN_SOURCE_SPECS.get(domain, []))
    write_json(
        root / "mods" / MOD_ID / domain / "manifest.json",
        {
            "domain": domain,
            "schemaVersion": SCHEMA_VERSION,
            "migrationMode": "source_file_manifest",
            "sourceAuthority": "vanilla",
            "sourceFiles": source_manifest(root, files),
        },
    )
    return {"sourceFiles": len(files)}


def extract_domain(root: Path, domain: str) -> Dict[str, Any]:
    if domain == "maps":
        return extract_maps(root)
    if domain == "npcs":
        return extract_npcs(root)
    if domain in ("trainers", "trainer_parties"):
        trainer_counts, party_counts = extract_trainers(root)
        return trainer_counts if domain == "trainers" else party_counts
    if domain == "weather":
        return extract_weather(root)
    if domain == "wild_encounters":
        return extract_wild_encounters(root)
    if domain == "time":
        return extract_time(root)
    if domain == "engine_rulesets":
        return extract_engine_rulesets(root)
    if domain == "quests":
        return extract_quests(root)
    return extract_source_manifest_domain(root, domain)


def write_wrapper_scripts(root: Path) -> None:
    wrapper_root = root / "scripts" / "vanilla"
    ci_root = root / "scripts" / "ci"
    extract_all = """#!/usr/bin/env python3
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
raise SystemExit(subprocess.call([sys.executable, str(ROOT / "scripts" / "vanilla" / "vanilla_migration.py"), "extract", "--root", str(ROOT)]))
"""
    materialize_all = """#!/usr/bin/env python3
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
raise SystemExit(subprocess.call([sys.executable, str(ROOT / "scripts" / "vanilla" / "vanilla_migration.py"), "materialize", "--root", str(ROOT)]))
"""
    (wrapper_root / "extract_all.py").write_text(extract_all, encoding="utf-8", newline="\n")
    (wrapper_root / "materialize_all.py").write_text(materialize_all, encoding="utf-8", newline="\n")
    for domain in API_DOMAINS:
        extract = f"""#!/usr/bin/env python3
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
raise SystemExit(subprocess.call([sys.executable, str(ROOT / "scripts" / "vanilla" / "vanilla_migration.py"), "extract", "--root", str(ROOT), "--domain", "{domain}"]))
"""
        materialize = f"""#!/usr/bin/env python3
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
raise SystemExit(subprocess.call([sys.executable, str(ROOT / "scripts" / "vanilla" / "vanilla_migration.py"), "materialize", "--root", str(ROOT), "--domain", "{domain}"]))
"""
        check_materialized = f"""#!/usr/bin/env python3
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
raise SystemExit(subprocess.call([sys.executable, str(ROOT / "scripts" / "vanilla" / "vanilla_migration.py"), "materialize", "--root", str(ROOT), "--domain", "{domain}", "--check"]))
"""
        (wrapper_root / f"extract_{domain}.py").write_text(extract, encoding="utf-8", newline="\n")
        (wrapper_root / f"materialize_{domain}.py").write_text(materialize, encoding="utf-8", newline="\n")
        (ci_root / f"check_vanilla_{domain}_materialized.py").write_text(check_materialized, encoding="utf-8", newline="\n")


def extract(root: Path, domains: Sequence[str]) -> None:
    ensure_mod_skeleton(root)
    write_c_adapters(root, domains)
    write_wrapper_scripts(root)
    write_all_schemas(root, domains)
    counts: Dict[str, Dict[str, Any]] = {}
    completed_trainers = False
    for domain in domains:
        if domain in ("trainers", "trainer_parties"):
            if not completed_trainers:
                trainer_counts, party_counts = extract_trainers(root)
                counts["trainers"] = trainer_counts
                counts["trainer_parties"] = party_counts
                completed_trainers = True
            continue
        counts[domain] = extract_domain(root, domain)
    for domain in domains:
        write_expectation(root, domain, counts.get(domain))
    write_json(
        root / "mods" / MOD_ID / "extraction_summary.json",
        {
            "mod": MOD_ID,
            "schemaVersion": SCHEMA_VERSION,
            "domains": API_DOMAINS,
            "counts": counts,
        },
    )


def compare_file_bytes(src: Path, dst: Path) -> Optional[str]:
    if not src.exists():
        return f"missing source {src}"
    if not dst.exists():
        return f"missing mirror {dst}"
    if src.read_bytes() != dst.read_bytes():
        return f"{dst} differs from {src}"
    return None


def materialize(root: Path, domains: Sequence[str], check_only: bool) -> None:
    errors = []
    if "maps" in domains:
        for map_json in sorted((root / "data" / "maps").glob("*/map.json")):
            dst = root / "mods" / MOD_ID / "maps" / map_json.parent.name / "map.json"
            if check_only:
                error = compare_file_bytes(map_json, dst)
                if error:
                    errors.append(error)
            else:
                copy_file_exact(map_json, dst)
            scripts = map_json.parent / "scripts.inc"
            if scripts.exists():
                dst_scripts = root / "mods" / MOD_ID / "maps" / map_json.parent.name / "scripts.inc"
                if check_only:
                    error = compare_file_bytes(scripts, dst_scripts)
                    if error:
                        errors.append(error)
                else:
                    copy_file_exact(scripts, dst_scripts)
    if "trainers" in domains:
        pairs = [
            (root / "src" / "data" / "trainers.h", root / "mods" / MOD_ID / "trainers" / "legacy" / "trainers.h"),
            (root / "include" / "constants" / "opponents.h", root / "mods" / MOD_ID / "trainers" / "legacy" / "opponents.h"),
        ]
        for src, dst in pairs:
            if check_only:
                error = compare_file_bytes(src, dst)
                if error:
                    errors.append(error)
            else:
                copy_file_exact(src, dst)
    if "trainer_parties" in domains:
        src = root / "src" / "data" / "trainer_parties.h"
        dst = root / "mods" / MOD_ID / "trainer_parties" / "legacy" / "trainer_parties.h"
        if check_only:
            error = compare_file_bytes(src, dst)
            if error:
                errors.append(error)
        else:
            copy_file_exact(src, dst)
    if "wild_encounters" in domains:
        src = root / "src" / "data" / "wild_encounters.json"
        dst = root / "mods" / MOD_ID / "wild_encounters" / "legacy" / "wild_encounters.json"
        if check_only:
            error = compare_file_bytes(src, dst)
            if error:
                errors.append(error)
        else:
            copy_file_exact(src, dst)
    if errors:
        raise SystemExit("\n".join(errors))


def check_expectations(root: Path, domains: Sequence[str]) -> None:
    errors = []
    for domain in domains:
        expected_path = root / "mods" / MOD_ID / "expectations" / domain / "baseline.expected.json"
        schema_path = root / "docs" / "mod_api_schemas" / f"{domain}.schema.json"
        domain_root = root / "mods" / MOD_ID / domain
        if not domain_root.exists():
            errors.append(f"missing domain folder: {domain_root}")
            continue
        if not schema_path.exists():
            errors.append(f"missing schema: {schema_path}")
        if not expected_path.exists():
            errors.append(f"missing expectation: {expected_path}")
            continue
        expected = read_json(expected_path)
        current = expectation_for_domain(root, domain)
        for key in ["fileCount", "jsonFileCount", "bytes", "sha1"]:
            if expected.get(key) != current.get(key):
                errors.append(f"{domain}: expectation mismatch for {key}: expected {expected.get(key)!r}, got {current.get(key)!r}")
    if errors:
        raise SystemExit("\n".join(errors))


def parse_domains(value: Optional[str]) -> Sequence[str]:
    if value is None or value == "all":
        return API_DOMAINS
    domains = [part.strip() for part in value.split(",") if part.strip()]
    unknown = [domain for domain in domains if domain not in API_DOMAINS]
    if unknown:
        raise SystemExit(f"unknown domain(s): {', '.join(unknown)}")
    return domains


METADATA_KEYS = {"domain", "schemaVersion", "source", "legacy", "mapGroup", "mapNum", "mapGroupSymbol", "interactionPolicy", "resourceLocks"}


def safe_name(value: Any) -> str:
    text = str(value)
    text = re.sub(r"[^A-Za-z0-9_.-]+", "_", text)
    return text.strip("_") or "entry"


def clear_domain(root: Path, domain: str) -> None:
    path = root / "mods" / MOD_ID / domain
    if path.exists():
        last_error = None
        for _ in range(5):
            try:
                shutil.rmtree(path)
                last_error = None
                break
            except OSError as exc:
                last_error = exc
                time.sleep(0.25)
        if last_error is not None:
            raise last_error
    path.mkdir(parents=True, exist_ok=True)


def entity_source(root: Path, source_path: Path) -> Dict[str, Any]:
    return {"path": rel(root, source_path), "sha1": sha1_file(source_path)}


def entity_base(root: Path, domain: str, entity_id: str, source_path: Path, legacy_kind: str, legacy: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
    legacy_data = {"kind": legacy_kind, "sourcePath": rel(root, source_path)}
    if legacy:
        legacy_data.update(legacy)
    return {
        "id": entity_id,
        "domain": domain,
        "schemaVersion": SCHEMA_VERSION,
        "source": entity_source(root, source_path),
        "legacy": legacy_data,
    }


def write_domain_index(root: Path, domain: str, entries: List[Dict[str, Any]], extra: Optional[Dict[str, Any]] = None) -> None:
    data: Dict[str, Any] = {
        "domain": domain,
        "schemaVersion": SCHEMA_VERSION,
        "entityKind": f"{domain}_entity",
        "semanticCoverage": "full",
        "entries": sorted(entries, key=lambda entry: (str(entry.get("id")), str(entry.get("path")))),
    }
    if extra:
        data.update(extra)
    write_json(root / "mods" / MOD_ID / domain / "index.json", data)


def write_domain_source_manifest(root: Path, domain: str, specs: Sequence[str]) -> None:
    files = collect_files(root, specs)
    write_json(
        root / "mods" / MOD_ID / domain / "_source_manifest.json",
        {
            "id": f"{domain}:source_manifest",
            "domain": domain,
            "schemaVersion": SCHEMA_VERSION,
            "migrationMode": "source_file_manifest",
            "sourceAuthority": "vanilla",
            "sourceFiles": source_manifest(root, files),
        },
    )


def write_entity(root: Path, domain: str, relative_path: str, data: Dict[str, Any], entries: List[Dict[str, Any]], entry: Optional[Dict[str, Any]] = None) -> None:
    path = root / "mods" / MOD_ID / domain / relative_path
    write_json(path, data)
    index_entry = {
        "id": data["id"],
        "path": relative_path.replace("\\", "/"),
        "sourcePath": data.get("source", {}).get("path"),
        "legacyKind": data.get("legacy", {}).get("kind"),
    }
    if entry:
        index_entry.update(entry)
    entries.append(index_entry)


def parse_define_tokens(root: Path, relative_path: str, prefix: str) -> List[Dict[str, Any]]:
    path = root / relative_path
    tokens = []
    seen = set()
    for match in re.finditer(rf"^#define\s+({re.escape(prefix)}[A-Z0-9_]+)\s+([^\s/]+)", read_text(path), re.MULTILINE):
        symbol = match.group(1)
        if symbol in seen:
            continue
        seen.add(symbol)
        value = match.group(2)
        tokens.append({"symbol": symbol, "value": int(value) if value.isdigit() else value, "source": path})
    return tokens


def extract_entity_maps(root: Path) -> Dict[str, Any]:
    domain = "maps"
    clear_domain(root, domain)
    maps_root = root / "data" / "maps"
    out_root = root / "mods" / MOD_ID / domain
    map_dirs = sorted([path for path in maps_root.iterdir() if path.is_dir() and (path / "map.json").exists()])
    group_lookup = discover_map_group_lookup(root)
    entries: List[Dict[str, Any]] = []

    map_groups_path = maps_root / "map_groups.json"
    map_groups_entity = entity_base(root, domain, "map_groups", map_groups_path, "map_groups_json", {"rawJson": read_json(map_groups_path)})
    write_json(out_root / "map_groups.json", map_groups_entity)
    for map_dir in map_dirs:
        map_name = map_dir.name
        source_path = map_dir / "map.json"
        data = read_json(source_path)
        scripts_path = map_dir / "scripts.inc"
        info = group_lookup.get(map_name, {})
        entity = dict(data)
        entity.update(
            entity_base(
                root,
                domain,
                str(data.get("id", map_name)),
                source_path,
                "map_json",
                {
                    "mapName": map_name,
                    "scriptsSourcePath": rel(root, scripts_path) if scripts_path.exists() else None,
                },
            )
        )
        entity["name"] = map_name
        entity["mapGroup"] = info.get("mapGroup")
        entity["mapNum"] = info.get("mapNum")
        entity["mapGroupSymbol"] = info.get("mapGroupSymbol")
        entity["interactionPolicy"] = "EXCLUSIVE"
        entity["resourceLocks"] = ["MAP_SCRIPT", "OBJECT_EVENT", "WARP"]
        write_entity(
            root,
            domain,
            f"{map_name}/map.json",
            entity,
            entries,
            {
                "symbol": data.get("id"),
                "mapGroup": info.get("mapGroup"),
                "mapNum": info.get("mapNum"),
                "mapGroupSymbol": info.get("mapGroupSymbol"),
            },
        )
        if scripts_path.exists():
            copy_file_exact(scripts_path, out_root / map_name / "scripts.inc")
    write_domain_index(root, domain, entries, {"entityCount": len(entries)})
    write_domain_source_manifest(root, domain, ["data/maps"])
    return {"maps": len(entries)}


def extract_entity_npcs(root: Path) -> Dict[str, Any]:
    domain = "npcs"
    clear_domain(root, domain)
    entries: List[Dict[str, Any]] = []
    total = 0
    for map_json in sorted((root / "data" / "maps").glob("*/map.json")):
        map_data = read_json(map_json)
        map_name = map_json.parent.name
        for index, obj in enumerate(map_data.get("object_events") or []):
            local_id = obj.get("local_id", index + 1)
            script = obj.get("script")
            entity_id = f"{map_name}:{local_id}"
            entity = entity_base(root, domain, entity_id, map_json, "map_object_event", {"raw": obj})
            entity.update(
                {
                    "map": map_name,
                    "mapId": map_data.get("id"),
                    "localId": local_id,
                    "graphicsId": obj.get("graphics_id"),
                    "x": obj.get("x"),
                    "y": obj.get("y"),
                    "elevation": obj.get("elevation"),
                    "movementType": obj.get("movement_type"),
                    "movementRangeX": obj.get("movement_range_x"),
                    "movementRangeY": obj.get("movement_range_y"),
                    "trainerType": obj.get("trainer_type", "TRAINER_TYPE_NONE"),
                    "trainerSightOrBerryTreeId": obj.get("trainer_sight_or_berry_tree_id"),
                    "script": script,
                    "flag": obj.get("flag"),
                    "interactionPolicy": "EXCLUSIVE" if script not in (None, "0", "0x0") else "DISABLED_ONLINE",
                    "resourceLocks": ["OBJECT_EVENT", "SCRIPT"] if script not in (None, "0", "0x0") else [],
                }
            )
            write_entity(root, domain, f"{map_name}/{safe_name(local_id)}.json", entity, entries, {"map": map_name, "localId": local_id})
            total += 1
    write_domain_index(root, domain, entries, {"entityCount": total})
    write_domain_source_manifest(root, domain, ["data/maps"])
    return {"npcs": total}


def extract_entity_trainers(root: Path) -> Dict[str, Any]:
    domain = "trainers"
    clear_domain(root, domain)
    entries: List[Dict[str, Any]] = []
    for trainer in parse_trainer_blocks(root):
        raw_c = trainer.pop("rawC")
        source_path = root / trainer.pop("sourcePath")
        symbol = trainer["symbol"]
        entity = entity_base(root, domain, symbol, source_path, "trainer_c_initializer", {"rawC": raw_c})
        entity.update({"symbol": symbol, **trainer, "interactionPolicy": "EXCLUSIVE", "resourceLocks": ["TRAINER_BATTLE", "SCRIPT"]})
        write_entity(root, domain, f"{symbol}.json", entity, entries, {"symbol": symbol, "numericId": trainer.get("numericId")})
    write_domain_index(root, domain, entries, {"entityCount": len(entries)})
    write_domain_source_manifest(root, domain, ["src/data/trainers.h", "include/constants/opponents.h"])
    return {"trainers": len(entries)}


def extract_entity_trainer_parties(root: Path) -> Dict[str, Any]:
    domain = "trainer_parties"
    clear_domain(root, domain)
    entries: List[Dict[str, Any]] = []
    for party in parse_party_blocks(root):
        raw_c = party.pop("rawC")
        source_path = root / party.pop("sourcePath")
        symbol = party["symbol"]
        entity = entity_base(root, domain, symbol, source_path, "trainer_party_c_initializer", {"rawC": raw_c})
        entity.update({"symbol": symbol, **party})
        write_entity(root, domain, f"{symbol}.json", entity, entries, {"symbol": symbol})
    write_domain_index(root, domain, entries, {"entityCount": len(entries)})
    write_domain_source_manifest(root, domain, ["src/data/trainer_parties.h"])
    return {"trainerParties": len(entries)}


def extract_entity_weather(root: Path) -> Dict[str, Any]:
    domain = "weather"
    clear_domain(root, domain)
    entries: List[Dict[str, Any]] = []
    counts: Dict[str, int] = {}
    for map_json in sorted((root / "data" / "maps").glob("*/map.json")):
        data = read_json(map_json)
        weather = data.get("weather")
        map_name = map_json.parent.name
        counts[str(weather)] = counts.get(str(weather), 0) + 1
        entity_id = f"{map_name}:weather"
        entity = entity_base(root, domain, entity_id, map_json, "map_weather")
        entity.update({"map": map_name, "mapId": data.get("id"), "weather": weather})
        write_entity(root, domain, f"maps/{map_name}.json", entity, entries, {"map": map_name})
    write_domain_index(root, domain, entries, {"entityCount": len(entries), "weatherCounts": counts})
    write_domain_source_manifest(root, domain, ["data/maps"])
    return {"mapsWithWeather": len(entries), "weatherKinds": len(counts)}


def extract_entity_time(root: Path) -> Dict[str, Any]:
    domain = "time"
    clear_domain(root, domain)
    entries: List[Dict[str, Any]] = []
    segments = [
        {"id": "morning", "segment": "MORNING", "startMinute": 4 * 60, "endMinute": 10 * 60 - 1},
        {"id": "day", "segment": "DAY", "startMinute": 10 * 60, "endMinute": 17 * 60 - 1},
        {"id": "evening", "segment": "EVENING", "startMinute": 17 * 60, "endMinute": 20 * 60 - 1},
        {"id": "night", "segment": "NIGHT", "startMinute": 20 * 60, "endMinute": 4 * 60 - 1},
    ]
    source_path = root / "src" / "mod" / "time.c"
    for segment in segments:
        entity = entity_base(root, domain, segment["id"], source_path, "default_time_segment")
        entity.update(segment)
        write_entity(root, domain, f"{segment['id']}.json", entity, entries, {"symbol": segment["segment"]})
    write_domain_index(root, domain, entries, {"entityCount": len(entries)})
    write_domain_source_manifest(root, domain, ["src/mod/time.c", "include/mod/time.h"])
    return {"segments": len(entries)}


def extract_entity_engine_rulesets(root: Path) -> Dict[str, Any]:
    domain = "engine_rulesets"
    clear_domain(root, domain)
    entries: List[Dict[str, Any]] = []
    source_path = root / "src" / "mod" / "engine.c"
    entity = entity_base(root, domain, "engine:gen3", source_path, "default_engine_ruleset")
    entity.update({"name": "Generation III", "version": 1, "saveCompatible": True, "captureHook": None, "battleWeatherHook": None})
    write_entity(root, domain, "gen3.json", entity, entries, {"symbol": "engine:gen3"})
    write_domain_index(root, domain, entries, {"entityCount": len(entries)})
    write_domain_source_manifest(root, domain, ["src/mod/engine.c", "include/mod/engine.h"])
    return {"rulesets": len(entries)}


def extract_entity_quests(root: Path) -> Dict[str, Any]:
    domain = "quests"
    clear_domain(root, domain)
    entries: List[Dict[str, Any]] = []
    for map_json in sorted((root / "data" / "maps").glob("*/map.json")):
        data = read_json(map_json)
        map_name = map_json.parent.name
        for key in ["coord_events", "bg_events", "warp_events"]:
            for index, event in enumerate(data.get(key) or []):
                entity_id = f"{map_name}:{key}:{index}"
                entity = entity_base(root, domain, entity_id, map_json, "map_event_hook", {"raw": event})
                entity.update(
                    {
                        "map": map_name,
                        "mapId": data.get("id"),
                        "eventType": key,
                        "index": index,
                        "script": event.get("script"),
                        "interactionPolicy": "EXCLUSIVE" if event.get("script") else "SHARED_READONLY",
                        "resourceLocks": ["SCRIPT"] if event.get("script") else [],
                    }
                )
                write_entity(root, domain, f"{map_name}/{key}_{index}.json", entity, entries, {"map": map_name, "eventType": key})
    write_domain_index(root, domain, entries, {"entityCount": len(entries)})
    write_domain_source_manifest(root, domain, ["data/maps"])
    return {"questHooks": len(entries)}


def extract_entity_wild_encounters(root: Path) -> Dict[str, Any]:
    domain = "wild_encounters"
    clear_domain(root, domain)
    source_path = root / "src" / "data" / "wild_encounters.json"
    data = read_json(source_path)
    entries: List[Dict[str, Any]] = []
    for group in data.get("wild_encounter_groups", []):
        label = group.get("label", "wild_encounters")
        for encounter in group.get("encounters", []):
            map_id = encounter.get("map", safe_name(encounter.get("base_label", len(entries))))
            entity_id = f"{label}:{map_id}:{len(entries)}"
            entity = entity_base(root, domain, entity_id, source_path, "wild_encounter_group", {"groupLabel": label})
            entity.update({"groupLabel": label, "forMaps": group.get("for_maps"), "fields": group.get("fields"), "encounter": encounter})
            write_entity(root, domain, f"{safe_name(label)}/{safe_name(map_id)}_{len(entries)}.json", entity, entries, {"symbol": map_id})
    write_domain_index(root, domain, entries, {"entityCount": len(entries)})
    write_domain_source_manifest(root, domain, ["src/data/wild_encounters.json"])
    return {"encounters": len(entries)}


def extract_entity_flags(root: Path) -> Dict[str, Any]:
    domain = "flags"
    clear_domain(root, domain)
    entries: List[Dict[str, Any]] = []
    for token in parse_define_tokens(root, "include/constants/flags.h", "FLAG_"):
        entity = entity_base(root, domain, token["symbol"], token["source"], "flag_define")
        entity.update({"symbol": token["symbol"], "value": token["value"], "kind": "flag"})
        write_entity(root, domain, f"flags/{token['symbol']}.json", entity, entries, {"symbol": token["symbol"]})
    for token in parse_define_tokens(root, "include/constants/vars.h", "VAR_"):
        entity = entity_base(root, domain, token["symbol"], token["source"], "var_define")
        entity.update({"symbol": token["symbol"], "value": token["value"], "kind": "var"})
        write_entity(root, domain, f"vars/{token['symbol']}.json", entity, entries, {"symbol": token["symbol"]})
    write_domain_index(root, domain, entries, {"entityCount": len(entries)})
    write_domain_source_manifest(root, domain, DOMAIN_SOURCE_SPECS[domain])
    return {"entries": len(entries)}


def extract_token_domain(root: Path, domain: str, relative_path: str, prefix: str, subdir: str, legacy_kind: str) -> Dict[str, Any]:
    clear_domain(root, domain)
    entries: List[Dict[str, Any]] = []
    for token in parse_define_tokens(root, relative_path, prefix):
        entity = entity_base(root, domain, token["symbol"], token["source"], legacy_kind)
        entity.update({"symbol": token["symbol"], "value": token["value"]})
        write_entity(root, domain, f"{subdir}/{token['symbol']}.json", entity, entries, {"symbol": token["symbol"], "numericId": token["value"] if isinstance(token["value"], int) else None})
    write_domain_index(root, domain, entries, {"entityCount": len(entries)})
    write_domain_source_manifest(root, domain, DOMAIN_SOURCE_SPECS.get(domain, [relative_path]))
    return {"entries": len(entries)}


def parse_shop_blocks(root: Path) -> List[Dict[str, Any]]:
    shops = []
    label_re = re.compile(r"^([A-Za-z0-9_]+):\s*$")
    item_re = re.compile(r"\.2byte\s+([A-Z0-9_]+)")
    for scripts in sorted((root / "data" / "maps").glob("*/scripts.inc")):
        lines = read_text(scripts).splitlines()
        i = 0
        while i < len(lines):
            label_match = label_re.match(lines[i].strip())
            if not label_match:
                i += 1
                continue
            label = label_match.group(1)
            items = []
            j = i + 1
            while j < len(lines):
                stripped = lines[j].strip()
                if stripped == "pokemartlistend":
                    if items:
                        shops.append({"symbol": label, "map": scripts.parent.name, "items": items, "source": scripts})
                    break
                if label_re.match(stripped) and j != i + 1:
                    break
                item_match = item_re.search(stripped)
                if item_match:
                    items.append(item_match.group(1))
                j += 1
            i += 1
    return shops


def extract_entity_shops(root: Path) -> Dict[str, Any]:
    domain = "shops"
    clear_domain(root, domain)
    entries: List[Dict[str, Any]] = []
    for shop in parse_shop_blocks(root):
        entity = entity_base(root, domain, shop["symbol"], shop["source"], "pokemart_script_list")
        entity.update({"symbol": shop["symbol"], "map": shop["map"], "items": shop["items"]})
        write_entity(root, domain, f"{shop['symbol']}.json", entity, entries, {"symbol": shop["symbol"], "map": shop["map"]})
    write_domain_index(root, domain, entries, {"entityCount": len(entries)})
    write_domain_source_manifest(root, domain, DOMAIN_SOURCE_SPECS[domain])
    return {"shops": len(entries)}


def extract_source_file_entities(root: Path, domain: str, specs: Sequence[str]) -> Dict[str, Any]:
    clear_domain(root, domain)
    entries: List[Dict[str, Any]] = []
    files = collect_files(root, specs)
    for path in files:
        entity_id = rel(root, path)
        entity = entity_base(root, domain, entity_id, path, "source_file")
        entity.update({"path": rel(root, path), "bytes": path.stat().st_size})
        write_entity(root, domain, f"source_files/{safe_name(entity_id)}.json", entity, entries)
    write_domain_index(root, domain, entries, {"entityCount": len(entries), "semanticCoverage": "source_audit_only"})
    write_domain_source_manifest(root, domain, specs)
    return {"sourceEntities": len(entries)}


def parse_define_map(root: Path, relative_path: str, prefix: str) -> Dict[str, Any]:
    return {token["symbol"]: token["value"] for token in parse_define_tokens(root, relative_path, prefix)}


def parse_indexed_pointer_table(text: str) -> Dict[str, str]:
    entries: Dict[str, str] = {}
    for match in re.finditer(r"\[([A-Z0-9_]+)\]\s*=\s*&?([A-Za-z0-9_]+)", text):
        entries[match.group(1)] = match.group(2)
    return entries


def parse_c_array_designators(text: str, fields: Sequence[str]) -> Dict[str, Dict[str, str]]:
    entries: Dict[str, Dict[str, str]] = {}
    field_pattern = r"\{\s*" + r"\s*,\s*".join([r"([^,{}]+)" for _ in fields]) + r"\s*\}"
    for match in re.finditer(r"\[([A-Z0-9_]+)\]\s*=\s*" + field_pattern, text):
        entries[match.group(1)] = {field: match.group(index + 2).strip() for index, field in enumerate(fields)}
    return entries


def parse_c_designated_blocks(root: Path, relative_path: str, symbol_prefix: str, legacy_kind: str) -> List[Dict[str, Any]]:
    path = root / relative_path
    text = read_text(path)
    blocks: List[Dict[str, Any]] = []
    entry_re = re.compile(rf"(?m)^(?:static\s+)?(?:const\s+)?struct\s+[A-Za-z0-9_]+\s+({re.escape(symbol_prefix)}[A-Za-z0-9_]+)\s*=\s*\{{")
    for match in entry_re.finditer(text):
        symbol = match.group(1)
        brace_start = text.find("{", match.start(), match.end())
        brace_end = find_matching_brace(text, brace_start)
        raw_block = text[match.start() : brace_end + 2].rstrip()
        fields = {field: value for field, value in re.findall(r"\.([A-Za-z0-9_]+)\s*=\s*([^,\n]+)", raw_block)}
        blocks.append(
            {
                "symbol": symbol,
                "fields": fields,
                "rawC": raw_block,
                "source": path,
                "legacyKind": legacy_kind,
            }
        )
    return blocks


def parse_object_event_pointer_rows(root: Path) -> List[Dict[str, Any]]:
    constants = parse_define_map(root, "include/constants/event_objects.h", "OBJ_EVENT_GFX_")
    pointer_path = root / "src" / "data" / "object_events" / "object_event_graphics_info_pointers.h"
    pointer_text = read_text(pointer_path)
    info_blocks = {
        block["symbol"]: block
        for block in parse_c_designated_blocks(
            root,
            "src/data/object_events/object_event_graphics_info.h",
            "gObjectEventGraphicsInfo_",
            "object_event_graphics_info",
        )
    }
    rows: List[Dict[str, Any]] = []
    for graphics_id, info_symbol in sorted(parse_indexed_pointer_table(pointer_text).items(), key=lambda item: constants.get(item[0], 99999)):
        if not graphics_id.startswith("OBJ_EVENT_GFX_"):
            continue
        block = info_blocks.get(info_symbol, {})
        rows.append(
            {
                "graphicsId": graphics_id,
                "numericId": constants.get(graphics_id),
                "graphicsInfoSymbol": info_symbol,
                "fields": block.get("fields", {}),
                "rawC": block.get("rawC"),
                "source": pointer_path,
            }
        )
    return rows


def parse_item_icon_table(root: Path) -> Dict[str, Dict[str, str]]:
    path = root / "src" / "data" / "item_icon_table.h"
    icons: Dict[str, Dict[str, str]] = {}
    for match in re.finditer(r"\[(ITEM_[A-Z0-9_]+)\]\s*=\s*\{\s*([A-Za-z0-9_]+)\s*,\s*([A-Za-z0-9_]+)\s*\}", read_text(path)):
        icons[match.group(1)] = {"icon": match.group(2), "palette": match.group(3), "sourcePath": rel(root, path)}
    return icons


def extract_entity_pokeballs(root: Path) -> Dict[str, Any]:
    domain = "pokeballs"
    clear_domain(root, domain)
    source_path = root / "src" / "mod" / "pokeball.c"
    text = read_text(source_path)
    icon_lookup = parse_item_icon_table(root)
    ball_sheets = parse_c_array_designators(read_text(root / "src" / "pokeball.c"), ["gfx", "size", "tag"])
    ball_palettes = parse_c_array_designators(read_text(root / "src" / "pokeball.c"), ["palette", "tag"])
    particle_sheets = parse_c_array_designators(read_text(root / "src" / "battle_anim_throw.c"), ["gfx", "size", "tag"])
    particle_palettes = parse_c_array_designators(read_text(root / "src" / "battle_anim_throw.c"), ["palette", "tag"])
    particle_anims = {match.group(1): match.group(2).strip() for match in re.finditer(r"\[([A-Z0-9_]+)\]\s*=\s*([0-9]+)\s*,", read_text(root / "src" / "battle_anim_throw.c"))}
    particle_funcs = {match.group(1): match.group(2).strip() for match in re.finditer(r"\[([A-Z0-9_]+)\]\s*=\s*([A-Za-z0-9_]+)\s*,", read_text(root / "src" / "battle_anim_throw.c"))}
    hook_by_item = {
        "ITEM_NET_BALL": "PokeBallApi_NetBallCatchModifier",
        "ITEM_DIVE_BALL": "PokeBallApi_DiveBallCatchModifier",
        "ITEM_NEST_BALL": "PokeBallApi_NestBallCatchModifier",
        "ITEM_REPEAT_BALL": "PokeBallApi_RepeatBallCatchModifier",
        "ITEM_TIMER_BALL": "PokeBallApi_TimerBallCatchModifier",
    }
    entries: List[Dict[str, Any]] = []
    pattern = re.compile(
        r"\{\s*\"([^\"]+)\"\s*,\s*(ITEM_[A-Z0-9_]+)\s*,\s*(BALL_[A-Z0-9_]+)\s*,\s*([^,]+)\s*,\s*([^,]+)\s*,\s*([^,]+)\s*,\s*([^,]+)\s*,\s*([^}]+)\}",
        re.MULTILINE,
    )
    for match in pattern.finditer(text):
        key, item_id, ball_id, modifier, flags, modifier_hook, battle_script, commit_hook = [part.strip() for part in match.groups()]
        modifier_hook = hook_by_item.get(item_id, None if modifier_hook == "NULL" else modifier_hook)
        commit_hook = None if commit_hook == "NULL" else commit_hook
        icon = icon_lookup.get(item_id, {})
        entity = entity_base(
            root,
            domain,
            item_id,
            source_path,
            "pokeball_definition",
            {"rawC": match.group(0).strip(), "generatedKey": key},
        )
        entity.update(
            {
                "key": key,
                "itemId": item_id,
                "ballId": ball_id,
                "catchModifier": modifier,
                "catchModifierHook": modifier_hook,
                "battleScript": None if battle_script == "NULL" else battle_script,
                "commitHook": commit_hook,
                "flags": flags,
                "itemIcon": icon.get("icon"),
                "itemPalette": icon.get("palette"),
                "throwSprite": {
                    "sheet": ball_sheets.get(ball_id, {}).get("gfx"),
                    "palette": ball_palettes.get(ball_id, {}).get("palette"),
                    "tileTag": ball_sheets.get(ball_id, {}).get("tag"),
                    "paletteTag": ball_palettes.get(ball_id, {}).get("tag"),
                    "templateTable": "gBallSpriteTemplates",
                },
                "openAnimation": {
                    "particleSheet": particle_sheets.get(ball_id, {}).get("gfx"),
                    "particlePalette": particle_palettes.get(ball_id, {}).get("palette"),
                    "particleTag": particle_sheets.get(ball_id, {}).get("tag"),
                    "animNum": particle_anims.get(ball_id),
                    "taskFunc": particle_funcs.get(ball_id),
                },
            }
        )
        write_entity(root, domain, f"balls/{item_id}.json", entity, entries, {"symbol": item_id, "legacyKind": "pokeball_definition"})
    write_domain_index(root, domain, entries, {"entityCount": len(entries), "entityKind": "pokeball"})
    write_domain_source_manifest(root, domain, DOMAIN_SOURCE_SPECS[domain])
    return {"pokeballs": len(entries)}


def extract_entity_overworld_sprites(root: Path) -> Dict[str, Any]:
    domain = "overworld_sprites"
    clear_domain(root, domain)
    entries: List[Dict[str, Any]] = []
    source_path = root / "src" / "data" / "object_events" / "object_event_graphics_info_pointers.h"
    for row in parse_object_event_pointer_rows(root):
        entity = entity_base(root, domain, row["graphicsId"], source_path, "object_event_graphics")
        entity.update(
            {
                "key": row["graphicsId"],
                "graphicsId": row["graphicsId"],
                "numericId": row["numericId"],
                "graphicsRevision": 1,
                "assetKey": row["graphicsInfoSymbol"],
                "graphicsInfoSymbol": row["graphicsInfoSymbol"],
                "width": row["fields"].get("width"),
                "height": row["fields"].get("height"),
                "paletteTag": row["fields"].get("paletteTag"),
                "reflectionPaletteTag": row["fields"].get("reflectionPaletteTag"),
                "shadowSize": row["fields"].get("shadowSize"),
                "tracks": row["fields"].get("tracks"),
            }
        )
        if row.get("rawC"):
            entity["legacy"]["rawC"] = row["rawC"]
        write_entity(root, domain, f"sprites/{row['graphicsId']}.json", entity, entries, {"symbol": row["graphicsId"], "numericId": row["numericId"]})
    write_domain_index(root, domain, entries, {"entityCount": len(entries), "entityKind": "overworld_sprite"})
    write_domain_source_manifest(root, domain, DOMAIN_SOURCE_SPECS[domain])
    return {"overworldSprites": len(entries)}


def extract_entity_sprite_assets(root: Path) -> Dict[str, Any]:
    domain = "sprite_assets"
    clear_domain(root, domain)
    entries: List[Dict[str, Any]] = []
    source_path = root / "src" / "data" / "object_events" / "object_event_graphics_info.h"
    for row in parse_object_event_pointer_rows(root):
        asset_id = row["graphicsInfoSymbol"]
        fields = row["fields"]
        entity = entity_base(root, domain, asset_id, source_path, "object_event_sprite_asset", {"rawC": row.get("rawC")})
        entity.update(
            {
                "key": asset_id,
                "assetKind": "object_event",
                "graphicsId": row["graphicsId"],
                "graphicsInfoSymbol": asset_id,
                "sheetSymbol": None,
                "compressedSheetSymbol": None,
                "paletteSymbol": None,
                "compressedPaletteSymbol": None,
                "templateSymbol": None,
                "tileTag": fields.get("tileTag", "TAG_NONE"),
                "paletteTag": fields.get("paletteTag", "TAG_NONE"),
                "dimensions": {"width": fields.get("width"), "height": fields.get("height"), "size": fields.get("size")},
                "oam": fields.get("oam"),
                "images": fields.get("images"),
                "anims": fields.get("anims"),
                "subspriteTables": fields.get("subspriteTables"),
                "affineAnims": fields.get("affineAnims"),
            }
        )
        write_entity(root, domain, f"assets/{asset_id}.json", entity, entries, {"symbol": asset_id, "assetKind": "object_event"})
    for item_id, icon in parse_item_icon_table(root).items():
        entity = entity_base(root, domain, f"{item_id}:icon", root / icon["sourcePath"], "item_icon_asset")
        entity.update(
            {
                "key": f"{item_id}:icon",
                "assetKind": "item_icon",
                "itemId": item_id,
                "iconDataSymbol": icon["icon"],
                "paletteDataSymbol": icon["palette"],
                "tileTag": "TAG_NONE",
                "paletteTag": "TAG_NONE",
            }
        )
        write_entity(root, domain, f"assets/items/{item_id}.json", entity, entries, {"symbol": item_id, "assetKind": "item_icon"})
    write_domain_index(root, domain, entries, {"entityCount": len(entries), "entityKind": "sprite_asset"})
    write_domain_source_manifest(root, domain, DOMAIN_SOURCE_SPECS[domain])
    return {"spriteAssets": len(entries)}


def parse_species_tokens(root: Path) -> List[str]:
    return [token["symbol"] for token in parse_define_tokens(root, "include/constants/species.h", "SPECIES_")]


def extract_entity_battle_sprites(root: Path) -> Dict[str, Any]:
    domain = "battle_sprites"
    clear_domain(root, domain)
    entries: List[Dict[str, Any]] = []
    source_path = root / "src" / "data" / "pokemon_graphics" / "front_pic_table.h"
    for species in parse_species_tokens(root):
        for side, table, asset_prefix, source_rel in [
            ("BATTLE_SPRITE_SIDE_FRONT", "gMonFrontPicTable", "front", "src/data/pokemon_graphics/front_pic_table.h"),
            ("BATTLE_SPRITE_SIDE_BACK", "gMonBackPicTable", "back", "src/data/pokemon_graphics/back_pic_table.h"),
        ]:
            source = root / source_rel
            entity_id = f"{species}:{asset_prefix}"
            entity = entity_base(root, domain, entity_id, source, "pokemon_battle_sprite")
            entity.update(
                {
                    "key": entity_id,
                    "entityKind": "pokemon",
                    "species": species,
                    "form": 0,
                    "side": side,
                    "flags": 0,
                    "assetKey": f"{species}:{asset_prefix}",
                    "table": table,
                }
            )
            write_entity(root, domain, f"pokemon/{species}_{asset_prefix}.json", entity, entries, {"symbol": species, "legacyKind": "pokemon_battle_sprite"})
    trainer_path = root / "include" / "constants" / "trainers.h"
    for token in parse_define_tokens(root, "include/constants/trainers.h", "TRAINER_PIC_"):
        entity = entity_base(root, domain, f"{token['symbol']}:front", trainer_path, "trainer_front_battle_sprite")
        entity.update(
            {
                "key": f"{token['symbol']}:front",
                "entityKind": "trainer",
                "trainerId": token["symbol"],
                "side": "BATTLE_SPRITE_SIDE_FRONT",
                "flags": 0,
                "assetKey": f"{token['symbol']}:front",
                "table": "gTrainerFrontPicTable",
            }
        )
        write_entity(root, domain, f"trainers/{token['symbol']}_front.json", entity, entries, {"symbol": token["symbol"], "numericId": token["value"]})
    for token in parse_define_tokens(root, "include/constants/trainers.h", "TRAINER_BACK_PIC_"):
        entity = entity_base(root, domain, f"{token['symbol']}:back", trainer_path, "trainer_back_battle_sprite")
        entity.update(
            {
                "key": f"{token['symbol']}:back",
                "entityKind": "trainer",
                "trainerId": token["symbol"],
                "side": "BATTLE_SPRITE_SIDE_BACK",
                "flags": 0,
                "assetKey": f"{token['symbol']}:back",
                "table": "gTrainerBackPicTable",
            }
        )
        write_entity(root, domain, f"trainers/{token['symbol']}_back.json", entity, entries, {"symbol": token["symbol"], "numericId": token["value"]})
    write_domain_index(root, domain, entries, {"entityCount": len(entries), "entityKind": "battle_sprite"})
    write_domain_source_manifest(root, domain, DOMAIN_SOURCE_SPECS[domain])
    return {"battleSprites": len(entries)}


def extract_entity_followers(root: Path) -> Dict[str, Any]:
    domain = "followers"
    clear_domain(root, domain)
    entries: List[Dict[str, Any]] = []
    species_values = set(parse_species_tokens(root))
    source_path = root / "src" / "data" / "object_events" / "object_event_graphics_info_pointers.h"
    for row in parse_object_event_pointer_rows(root):
        species = "SPECIES_" + row["graphicsId"].replace("OBJ_EVENT_GFX_", "")
        if species not in species_values:
            continue
        entity = entity_base(root, domain, species, source_path, "species_overworld_follower")
        entity.update({"species": species, "form": 0, "shiny": False, "graphicsId": row["graphicsId"], "assetKey": row["graphicsInfoSymbol"]})
        write_entity(root, domain, f"followers/{species}.json", entity, entries, {"symbol": species})
    write_domain_index(root, domain, entries, {"entityCount": len(entries), "entityKind": "follower_sprite"})
    write_domain_source_manifest(root, domain, DOMAIN_SOURCE_SPECS[domain])
    return {"followers": len(entries)}


def extract_entity_outfits(root: Path) -> Dict[str, Any]:
    domain = "outfits"
    clear_domain(root, domain)
    entries: List[Dict[str, Any]] = []
    rows = {row["graphicsId"]: row for row in parse_object_event_pointer_rows(root)}
    source_path = root / "src" / "field_player_avatar.c"
    for row in sorted(rows.values(), key=lambda item: str(item["graphicsId"])):
        graphics_id = row["graphicsId"]
        if not any(token in graphics_id for token in ("BRENDAN", "MAY")):
            continue
        if any(token in graphics_id for token in ("RIVAL_", "LINK_", "RS_")):
            continue
        gender = "male" if "BRENDAN" in graphics_id else "female"
        state = graphics_id.replace("OBJ_EVENT_GFX_BRENDAN_", "").replace("OBJ_EVENT_GFX_MAY_", "").lower()
        entity_id = f"{gender}:{state}"
        entity = entity_base(root, domain, entity_id, source_path, "player_outfit")
        entity.update({"gender": gender, "state": state, "graphicsId": graphics_id, "assetKey": row["graphicsInfoSymbol"], "paletteTag": row["fields"].get("paletteTag")})
        write_entity(root, domain, f"outfits/{gender}_{state}.json", entity, entries, {"symbol": graphics_id})
    write_domain_index(root, domain, entries, {"entityCount": len(entries), "entityKind": "player_outfit"})
    write_domain_source_manifest(root, domain, DOMAIN_SOURCE_SPECS[domain])
    return {"outfits": len(entries)}


def decode_c_string_literal(value: str) -> str:
    escapes = {"n": "\n", "r": "\r", "t": "\t", "\\": "\\", '"': '"'}
    decoded = []
    i = 0
    while i < len(value):
        if value[i] == "\\" and i + 1 < len(value):
            nxt = value[i + 1]
            if nxt in escapes:
                decoded.append(escapes[nxt])
            else:
                decoded.append("\\" + nxt)
            i += 2
            continue
        decoded.append(value[i])
        i += 1
    return "".join(decoded)


def extract_c_language_strings(root: Path) -> Dict[str, str]:
    path = root / "src" / "strings.c"
    text = read_text(path)
    strings: Dict[str, str] = {}
    for match in re.finditer(r"(?:ALIGNED\([0-9]+\)\s*)?const\s+u8\s+([A-Za-z0-9_]+)\[\]\s*=\s*_\(\s*\"((?:\\.|[^\"\\])*)\"\s*\)\s*;", text, re.MULTILINE | re.DOTALL):
        strings[match.group(1)] = decode_c_string_literal(match.group(2))
    return strings


def extract_text_inc_labels(path: Path) -> Dict[str, str]:
    labels: Dict[str, str] = {}
    lines = read_text(path).splitlines()
    i = 0
    label_re = re.compile(r"^([A-Za-z_][A-Za-z0-9_]*)::\s*$")
    while i < len(lines):
        match = label_re.match(lines[i].strip())
        if not match:
            i += 1
            continue
        label = match.group(1)
        start = i + 1
        i += 1
        while i < len(lines) and not label_re.match(lines[i].strip()):
            i += 1
        labels[label] = "\n".join(lines[start:i]).strip()
    return labels


def extract_entity_language(root: Path) -> Dict[str, Any]:
    domain = "language"
    clear_domain(root, domain)
    entries: List[Dict[str, Any]] = []
    source_path = root / "src" / "strings.c"
    core_strings = extract_c_language_strings(root)
    if core_strings:
        entity = entity_base(root, domain, "en:core_strings", source_path, "c_string_table")
        entity.update({"language": "en", "group": "core_strings", "runtime": True, "strings": core_strings})
        write_entity(root, domain, "en/core_strings.json", entity, entries, {"language": "en", "group": "core_strings"})
    for text_path in sorted((root / "data" / "text").glob("*.inc")):
        labels = extract_text_inc_labels(text_path)
        if not labels:
            continue
        group = text_path.stem
        entity = entity_base(root, domain, f"en:{group}", text_path, "asm_text_labels")
        entity.update({"language": "en", "group": group, "runtime": False, "strings": labels})
        write_entity(root, domain, f"en/{safe_name(group)}.json", entity, entries, {"language": "en", "group": group})
    write_domain_index(root, domain, entries, {"entityCount": len(entries), "entityKind": "language_group"})
    write_domain_source_manifest(root, domain, DOMAIN_SOURCE_SPECS[domain])
    return {"languageGroups": len(entries), "strings": sum(len(read_json(root / "mods" / MOD_ID / domain / entry["path"]).get("strings", {})) for entry in entries)}


def infer_script_kind(label: str, raw_script: str) -> str:
    if label.endswith("_MapScripts") or "map_script" in raw_script:
        return "map_scripts"
    if "Movement_" in label or re.search(r"(?m)^\s*(walk_|delay_|step_|face_)", raw_script):
        return "movement"
    if "_Text_" in label or label.endswith("_Text"):
        return "text"
    if "EventScript" in label:
        return "event_script"
    return "script"


def extract_script_references(raw_script: str) -> Dict[str, List[str]]:
    references: Dict[str, List[str]] = {}
    patterns = {
        "flags": r"\bFLAG_[A-Z0-9_]+\b",
        "vars": r"\bVAR_[A-Z0-9_]+\b",
        "items": r"\bITEM_[A-Z0-9_]+\b",
        "trainers": r"\bTRAINER_[A-Z0-9_]+\b",
        "maps": r"\bMAP_[A-Z0-9_]+\b",
    }
    for key, pattern in patterns.items():
        values = sorted(set(re.findall(pattern, raw_script)))
        if values:
            references[key] = values
    return references


def split_script_labels(path: Path) -> List[Dict[str, Any]]:
    labels: List[Dict[str, Any]] = []
    lines = read_text(path).splitlines()
    label_re = re.compile(r"^([A-Za-z_][A-Za-z0-9_]*)(::|:)\s*$")
    i = 0
    while i < len(lines):
        match = label_re.match(lines[i].strip())
        if not match:
            i += 1
            continue
        label = match.group(1)
        exported = match.group(2) == "::"
        start = i + 1
        i += 1
        while i < len(lines) and not label_re.match(lines[i].strip()):
            i += 1
        raw = "\n".join(lines[start:i]).strip()
        labels.append({"label": label, "exported": exported, "rawScript": raw})
    return labels


def extract_entity_events(root: Path) -> Dict[str, Any]:
    domain = "events"
    clear_domain(root, domain)
    entries: List[Dict[str, Any]] = []
    script_roots = [root / "data" / "scripts", root / "data" / "maps"]
    for script_root in script_roots:
        for path in sorted(script_root.rglob("*.inc")):
            for label in split_script_labels(path):
                if label["rawScript"] == "":
                    continue
                entity = entity_base(root, domain, label["label"], path, "script_label", {"rawScript": label["rawScript"]})
                entity.update(
                    {
                        "label": label["label"],
                        "exported": label["exported"],
                        "kind": infer_script_kind(label["label"], label["rawScript"]),
                        "sourceFile": rel(root, path),
                        "references": extract_script_references(label["rawScript"]),
                    }
                )
                write_entity(root, domain, f"scripts/{safe_name(label['label'])}.json", entity, entries, {"symbol": label["label"], "kind": entity["kind"]})
    write_domain_index(root, domain, entries, {"entityCount": len(entries), "entityKind": "script_label"})
    write_domain_source_manifest(root, domain, DOMAIN_SOURCE_SPECS[domain])
    return {"scripts": len(entries)}


def extract_entity_state(root: Path) -> Dict[str, Any]:
    domain = "state"
    clear_domain(root, domain)
    entries: List[Dict[str, Any]] = []
    state_files = [
        (
            "save_blocks",
            root / "include" / "global.h",
            "save_block_contract",
            {
                "containers": ["SaveBlock1", "SaveBlock2", "PokemonStorage", "MapHeader"],
                "legacyFiles": ["include/global.h", "include/save.h", "src/save.c"],
            },
        ),
        (
            "runtime_modes",
            root / "include" / "global.fieldmap.h",
            "runtime_state_contract",
            {
                "avatarStates": [token["symbol"] for token in parse_define_tokens(root, "include/global.fieldmap.h", "PLAYER_AVATAR_FLAG_")],
                "legacyFiles": ["include/global.fieldmap.h", "src/field_player_avatar.c", "src/overworld.c"],
            },
        ),
        (
            "multiplayer_options",
            root / "include" / "multiplayer" / "types.h",
            "multiplayer_state_contract",
            {
                "resourceLocks": ["MAP_SCRIPT", "OBJECT_EVENT", "TRAINER_BATTLE", "SCRIPT"],
                "commitKinds": sorted(set(re.findall(r"\bMULTIPLAYER_COMMIT_[A-Z0-9_]+\b", read_text(root / "include" / "multiplayer" / "types.h")))),
                "legacyFiles": ["include/multiplayer/types.h", "src/multiplayer/commit.c"],
            },
        ),
    ]
    for entity_id, source_path, legacy_kind, payload in state_files:
        entity = entity_base(root, domain, entity_id, source_path, legacy_kind)
        entity.update(payload)
        write_entity(root, domain, f"{entity_id}.json", entity, entries, {"symbol": entity_id})
    write_domain_index(root, domain, entries, {"entityCount": len(entries), "entityKind": "state_contract"})
    write_domain_source_manifest(root, domain, DOMAIN_SOURCE_SPECS[domain])
    return {"stateContracts": len(entries)}


ENTITY_EXTRACTORS = {
    "maps": extract_entity_maps,
    "npcs": extract_entity_npcs,
    "trainers": extract_entity_trainers,
    "trainer_parties": extract_entity_trainer_parties,
    "weather": extract_entity_weather,
    "time": extract_entity_time,
    "flags": extract_entity_flags,
    "engine_rulesets": extract_entity_engine_rulesets,
    "quests": extract_entity_quests,
    "wild_encounters": extract_entity_wild_encounters,
    "items": lambda root: extract_token_domain(root, "items", "include/constants/items.h", "ITEM_", "items", "item_define"),
    "pokemon": lambda root: extract_token_domain(root, "pokemon", "include/constants/species.h", "SPECIES_", "species", "species_define"),
    "moves": lambda root: extract_token_domain(root, "moves", "include/constants/moves.h", "MOVE_", "moves", "move_define"),
    "shops": extract_entity_shops,
    "events": extract_entity_events,
    "language": extract_entity_language,
    "sprite_assets": extract_entity_sprite_assets,
    "overworld_sprites": extract_entity_overworld_sprites,
    "battle_sprites": extract_entity_battle_sprites,
    "followers": extract_entity_followers,
    "outfits": extract_entity_outfits,
    "pokeballs": extract_entity_pokeballs,
    "state": extract_entity_state,
}


def extract_domain(root: Path, domain: str) -> Dict[str, Any]:
    extractor = ENTITY_EXTRACTORS.get(domain)
    if extractor is not None:
        return extractor(root)
    return extract_source_file_entities(root, domain, DOMAIN_SOURCE_SPECS.get(domain, []))


def extract(root: Path, domains: Sequence[str]) -> None:
    ensure_mod_skeleton(root)
    write_c_adapters(root, domains)
    write_wrapper_scripts(root)
    write_all_schemas(root, domains)
    counts: Dict[str, Dict[str, Any]] = {}
    for domain in domains:
        counts[domain] = extract_domain(root, domain)
    for domain in domains:
        write_expectation(root, domain, counts.get(domain))
    write_json(
        root / "mods" / MOD_ID / "extraction_summary.json",
        {
            "mod": MOD_ID,
            "schemaVersion": SCHEMA_VERSION,
            "layout": "entity_index_v2",
            "domains": API_DOMAINS,
            "counts": counts,
        },
    )


def strip_map_metadata(data: Dict[str, Any]) -> Dict[str, Any]:
    return {key: value for key, value in data.items() if key not in METADATA_KEYS}


def check_entity_sources(root: Path, domains: Sequence[str]) -> List[str]:
    errors = []
    for domain in domains:
        domain_root = root / "mods" / MOD_ID / domain
        index_path = domain_root / "index.json"
        if not index_path.exists():
            errors.append(f"missing index: {rel(root, index_path)}")
            continue
        for entry in read_json(index_path).get("entries", []):
            path = domain_root / str(entry.get("path", ""))
            data = read_json(path)
            source = data.get("source", {}) if isinstance(data, dict) else {}
            source_path = root / source.get("path", "")
            if not source_path.exists():
                errors.append(f"{rel(root, path)}: missing source {source.get('path')}")
            elif source.get("sha1") != sha1_file(source_path):
                errors.append(f"{rel(root, path)}: source hash drift for {source.get('path')}")
    return errors


def materialize(root: Path, domains: Sequence[str], check_only: bool) -> None:
    if not check_only:
        extract(root, domains)
        return
    errors = check_entity_sources(root, domains)
    if "maps" in domains:
        for map_entity in sorted((root / "mods" / MOD_ID / "maps").glob("*/map.json")):
            data = read_json(map_entity)
            source_path = root / data.get("source", {}).get("path", "")
            if source_path.exists() and strip_map_metadata(data) != read_json(source_path):
                errors.append(f"{rel(root, map_entity)}: map data differs from {rel(root, source_path)}")
            scripts_source = data.get("legacy", {}).get("scriptsSourcePath")
            scripts_mirror = map_entity.parent / "scripts.inc"
            if scripts_source:
                source = root / scripts_source
                if not scripts_mirror.exists():
                    errors.append(f"{rel(root, scripts_mirror)}: missing script mirror")
                elif source.read_bytes() != scripts_mirror.read_bytes():
                    errors.append(f"{rel(root, scripts_mirror)}: script mirror differs from {rel(root, source)}")
    if errors:
        raise SystemExit("\n".join(errors))


def check_expectations(root: Path, domains: Sequence[str]) -> None:
    errors = []
    for domain in domains:
        expected_path = root / "mods" / MOD_ID / "expectations" / domain / "baseline.expected.json"
        schema_path = root / "docs" / "mod_api_schemas" / f"{domain}.schema.json"
        index_path = root / "mods" / MOD_ID / domain / "index.json"
        source_manifest_path = root / "mods" / MOD_ID / domain / "_source_manifest.json"
        if not schema_path.exists():
            errors.append(f"missing schema: {rel(root, schema_path)}")
        if not index_path.exists():
            errors.append(f"missing domain index: {rel(root, index_path)}")
        if not source_manifest_path.exists():
            errors.append(f"missing source manifest: {rel(root, source_manifest_path)}")
        if not expected_path.exists():
            errors.append(f"missing expectation: {rel(root, expected_path)}")
            continue
        expected = read_json(expected_path)
        current = expectation_for_domain(root, domain)
        for key in ["fileCount", "jsonFileCount", "bytes", "sha1"]:
            if expected.get(key) != current.get(key):
                errors.append(f"{domain}: expectation mismatch for {key}: expected {expected.get(key)!r}, got {current.get(key)!r}")
    errors.extend(check_entity_sources(root, domains))
    if errors:
        raise SystemExit("\n".join(errors))


def main(argv: Optional[Sequence[str]] = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="command", required=True)

    extract_parser = subparsers.add_parser("extract")
    extract_parser.add_argument("--root", default=".")
    extract_parser.add_argument("--domain", default="all")

    materialize_parser = subparsers.add_parser("materialize")
    materialize_parser.add_argument("--root", default=".")
    materialize_parser.add_argument("--domain", default="all")
    materialize_parser.add_argument("--check", action="store_true")

    check_parser = subparsers.add_parser("check")
    check_parser.add_argument("--root", default=".")
    check_parser.add_argument("--domain", default="all")

    args = parser.parse_args(argv)
    root = Path(args.root).resolve()
    domains = parse_domains(getattr(args, "domain", "all"))

    if args.command == "extract":
        extract(root, domains)
    elif args.command == "materialize":
        materialize(root, domains, bool(args.check))
    elif args.command == "check":
        check_expectations(root, domains)
        materialize(root, domains, True)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
