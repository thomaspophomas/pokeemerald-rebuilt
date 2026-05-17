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
        "include/constants/pokeballs.h",
        "src/pokeball.c",
        "src/mod/pokeball.c",
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
                "source": {"type": "object"},
                "legacy": {"type": "object"},
                "entries": {"type": "array"},
            },
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
    write_domain_index(root, domain, entries, {"entityCount": len(entries)})
    write_domain_source_manifest(root, domain, specs)
    return {"sourceEntities": len(entries)}


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
