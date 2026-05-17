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
                "Deterministic JSON representation for the vanilla "
                f"{domain} mod API domain. Domain-specific files may be either "
                "typed records or lossless source manifests until the legacy "
                "pipeline for that domain is fully replaced."
            ),
            "oneOf": [{"type": "object"}, {"type": "array"}],
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
