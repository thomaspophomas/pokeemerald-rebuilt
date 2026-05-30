#!/usr/bin/env python3
"""Validate drop-in mod manifests before running a full ROM build."""

from __future__ import annotations

import argparse
import importlib.util
import json
import re
import sys
from pathlib import Path
from typing import Any


sys.dont_write_bytecode = True

MOD_ID_RE = re.compile(r"^[a-z0-9][a-z0-9_-]*$")
IDENT_RE = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")
FEATURE_RE = re.compile(r"^[A-Z0-9_]+$")
SDK_REQUIREMENT_RE = re.compile(r"^(>=|<=|==|=|>|<)?\s*(\d+)$")
KNOWN_MANIFEST_FIELDS = {
    "id",
    "name",
    "version",
    "requiresSdk",
    "priority",
    "featureFlags",
    "features",
    "dependencies",
    "loadAfter",
    "conflictsWith",
    "requiresFeatures",
    "stateVersion",
    "stateBytes",
    "entrypoints",
}
FORBIDDEN_ENTRYPOINT_INCLUDES = {
    "global.h",
    "generated/mod_registry.h",
}
HOOK_MODES = {"observe", "modify", "replace", "claim"}
KNOWN_DOMAIN_DIRS = {
    "badges",
    "battle",
    "encounters",
    "engines",
    "events",
    "fishing",
    "flags",
    "graphics",
    "followers",
    "items",
    "lang",
    "level_caps",
    "maps",
    "npcs",
    "outfits",
    "pokeballs",
    "pokemon",
    "rewards",
    "shops",
    "sprites",
    "src",
    "time",
    "tools",
    "trainers",
    "weather",
}
DOMAIN_LIST_KEYS = {
    "badges": "effects",
    "battle": "moves",
    "encounters": "encounters",
    "engines": "engines",
    "events": "subscriptions",
    "fishing": "actions",
    "flags": "flags",
    "followers": "followers",
    "items": "items",
    "lang": "strings",
    "level_caps": "caps",
    "maps": "maps",
    "npcs": "npcs",
    "pokeballs": "pokeballs",
    "pokemon": "pokemon",
    "rewards": "rewards",
    "shops": "shops",
    "time": "segments",
    "trainers": "trainers",
    "weather": "providers",
}


class ModCheckError(Exception):
    pass


def read_json(path: Path) -> Any:
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        raise ModCheckError(f"{path}: invalid JSON: {exc}") from exc


def require_string(manifest: dict[str, Any], key: str, path: Path) -> str:
    value = manifest.get(key)
    if not isinstance(value, str) or not value.strip():
        raise ModCheckError(f"{path}: {key} must be a non-empty string")
    return value


def require_u16(value: Any, label: str, path: Path, default: int = 0) -> int:
    if value is None:
        return default
    if isinstance(value, bool) or not isinstance(value, int):
        raise ModCheckError(f"{path}: {label} must be an integer")
    if value < 0 or value > 0xFFFF:
        raise ModCheckError(f"{path}: {label} must fit in u16")
    return value


def require_id_list(value: Any, label: str, path: Path) -> list[str]:
    if value is None:
        return []
    if not isinstance(value, list):
        raise ModCheckError(f"{path}: {label} must be a list")
    seen: set[str] = set()
    out: list[str] = []
    for item in value:
        if not isinstance(item, str) or not MOD_ID_RE.match(item):
            raise ModCheckError(f"{path}: {label} entry {item!r} is not a valid mod id")
        if item in seen:
            raise ModCheckError(f"{path}: duplicate {label} entry {item!r}")
        seen.add(item)
        out.append(item)
    return out


def require_feature_list(value: Any, path: Path) -> list[str]:
    if value is None:
        return []
    if not isinstance(value, list):
        raise ModCheckError(f"{path}: requiresFeatures must be a list")
    seen: set[str] = set()
    out: list[str] = []
    for item in value:
        if not isinstance(item, str) or not FEATURE_RE.match(item):
            raise ModCheckError(f"{path}: requiresFeatures entry {item!r} is invalid")
        if item in seen:
            raise ModCheckError(f"{path}: duplicate requiresFeatures entry {item!r}")
        seen.add(item)
        out.append(item)
    return out


def require_sdk_requirement(value: Any, path: Path) -> str | None:
    if value is None:
        return None
    if isinstance(value, bool):
        raise ModCheckError(f"{path}: requiresSdk must be an integer or version requirement")
    if isinstance(value, int):
        return f">={value}"
    if not isinstance(value, str) or not SDK_REQUIREMENT_RE.match(value.strip()):
        raise ModCheckError(f"{path}: requiresSdk must look like >=1, ==1, or 1")
    return value.strip()


def current_sdk_version(root: Path) -> int:
    for header in (root / "include" / "mod" / "base.h", root / "include" / "mod_sdk.h"):
        text = header.read_text(encoding="utf-8")
        match = re.search(r"^\s*#define\s+MOD_SDK_VERSION\s+(\d+)", text, re.MULTILINE)
        if match:
            return int(match.group(1))
    raise ModCheckError(f"{root / 'include' / 'mod' / 'base.h'}: missing MOD_SDK_VERSION")


def validate_sdk_requirement(requirement: str | None, current: int, path: Path) -> None:
    if requirement is None:
        return
    match = SDK_REQUIREMENT_RE.match(requirement)
    if not match:
        raise ModCheckError(f"{path}: invalid requiresSdk {requirement!r}")
    op = match.group(1) or ">="
    wanted = int(match.group(2))
    checks = {
        ">=": current >= wanted,
        "<=": current <= wanted,
        "==": current == wanted,
        "=": current == wanted,
        ">": current > wanted,
        "<": current < wanted,
    }
    if not checks[op]:
        raise ModCheckError(f"{path}: requiresSdk {requirement!r}, current SDK is {current}")


def validate_entrypoints(mod_root: Path, manifest: dict[str, Any], path: Path) -> list[str]:
    value = manifest.get("entrypoints", [])
    if not isinstance(value, list):
        raise ModCheckError(f"{path}: entrypoints must be a list")
    out: list[str] = []
    seen: set[str] = set()
    for entry in value:
        if not isinstance(entry, str) or not entry.endswith(".c"):
            raise ModCheckError(f"{path}: entrypoint {entry!r} must be a C source path")
        entry_path = (mod_root / entry).resolve()
        try:
            entry_path.relative_to(mod_root.resolve())
        except ValueError as exc:
            raise ModCheckError(f"{path}: entrypoint {entry!r} escapes the mod folder") from exc
        if not entry_path.exists():
            raise ModCheckError(f"{path}: entrypoint {entry!r} does not exist")
        validate_entrypoint_includes(entry_path)
        normalized = entry_path.relative_to(mod_root.resolve()).as_posix()
        if normalized in seen:
            raise ModCheckError(f"{path}: duplicate entrypoint {entry!r}")
        seen.add(normalized)
        out.append(normalized)
    return out


def validate_entrypoint_includes(path: Path) -> None:
    include_re = re.compile(r'^\s*#\s*include\s+["<]([^">]+)[">]')
    for line_number, line in enumerate(path.read_text(encoding="utf-8").splitlines(), start=1):
        match = include_re.match(line)
        if not match:
            continue
        include = match.group(1)
        if include in FORBIDDEN_ENTRYPOINT_INCLUDES or include.startswith("generated/") or include.startswith("src/"):
            raise ModCheckError(f"{path}:{line_number}: mod entrypoints must not include {include!r}; include mod_sdk.h")
        if include.startswith("mod/"):
            raise ModCheckError(f"{path}:{line_number}: include mod_sdk.h instead of individual SDK port {include!r}")


def validate_domain_dirs(mod_root: Path, path: Path) -> list[str]:
    unknown: list[str] = []
    for child in sorted(mod_root.iterdir()):
        if not child.is_dir():
            continue
        if child.name not in KNOWN_DOMAIN_DIRS:
            unknown.append(child.name)
    if unknown:
        raise ModCheckError(f"{path}: unknown domain folders: {', '.join(unknown)}")
    return sorted(child.name for child in mod_root.iterdir() if child.is_dir())


def require_object(value: Any, path: Path, label: str) -> dict[str, Any]:
    if not isinstance(value, dict):
        raise ModCheckError(f"{path}: {label} must be an object")
    return value


def require_items(data: Any, key: str, path: Path) -> list[Any]:
    if isinstance(data, list):
        return data
    if isinstance(data, dict):
        unknown = sorted(set(data) - {key, "_meta"})
        if unknown:
            raise ModCheckError(f"{path}: unknown top-level fields: {', '.join(unknown)}")
        value = data.get(key, [])
        if isinstance(value, list):
            return value
    raise ModCheckError(f"{path}: expected list or object with {key!r} list")


def get_item_key(item: dict[str, Any], fallback: str) -> str:
    return str(item.get("key") or item.get("id") or item.get("name") or fallback)


def validate_hook_mode(item: dict[str, Any], path: Path, label: str) -> str:
    mode = item.get("mode", "observe")
    if not isinstance(mode, str) or mode not in HOOK_MODES:
        raise ModCheckError(f"{path}: {label}.mode must be one of {', '.join(sorted(HOOK_MODES))}")
    return mode


def require_int(value: Any, path: Path, label: str, minimum: int | None = None, maximum: int | None = None) -> int:
    if isinstance(value, bool) or not isinstance(value, int):
        raise ModCheckError(f"{path}: {label} must be an integer")
    if minimum is not None and value < minimum:
        raise ModCheckError(f"{path}: {label} must be >= {minimum}")
    if maximum is not None and value > maximum:
        raise ModCheckError(f"{path}: {label} must be <= {maximum}")
    return value


def require_optional_int(item: dict[str, Any], key: str, path: Path, label: str, minimum: int | None = None, maximum: int | None = None) -> None:
    if key in item:
        require_int(item[key], path, label, minimum, maximum)


def require_ident(value: Any, path: Path, label: str, required: bool = False) -> None:
    if value is None:
        if required:
            raise ModCheckError(f"{path}: {label} is required")
        return
    if value == "NULL":
        return
    if not isinstance(value, str) or not IDENT_RE.match(value):
        raise ModCheckError(f"{path}: {label} must be a C identifier")


def require_token(value: Any, path: Path, label: str, required: bool = False) -> None:
    if value is None:
        if required:
            raise ModCheckError(f"{path}: {label} is required")
        return
    if isinstance(value, bool) or not isinstance(value, (int, str)):
        raise ModCheckError(f"{path}: {label} must be an integer or token string")
    if isinstance(value, str) and value == "":
        raise ModCheckError(f"{path}: {label} must be non-empty")


def require_short_key(value: Any, path: Path, label: str) -> None:
    if value is None:
        return
    if not isinstance(value, str) or value == "" or len(value) > 31:
        raise ModCheckError(f"{path}: {label} must be non-empty and <= 31 bytes")


def reject_unknown_fields(item: dict[str, Any], allowed: set[str], path: Path, label: str) -> None:
    unknown = sorted(set(item) - allowed)
    if unknown:
        raise ModCheckError(f"{path}: {label} has unknown fields: {', '.join(unknown)}")


def validate_domain_files(mod_root: Path) -> None:
    validators = {
        "badges": validate_simple_list_file("badges", {"id", "key", "name", "badge", "badgeId", "badge_id", "effect", "effectKind", "effect_kind", "target", "percentPerLevel", "percent_per_level", "maxLevel", "max_level", "flags", "mode"}),
        "battle": validate_simple_list_file("battle", {"id", "key", "name", "move", "moveId", "move_id", "priority", "flags", "effect", "power", "type", "accuracy", "pp", "secondary", "target", "movePriority", "move_priority", "moveFlags", "move_flags", "mode"}),
        "encounters": validate_simple_list_file("encounters", {"id", "key", "name", "map", "mapGroup", "map_group", "mapNum", "map_num", "area", "encounterRate", "encounter_rate", "slots", "priority", "flags", "hook", "hookSymbol", "hook_symbol", "hookKey", "hook_key", "rodMask", "rod_mask", "mode"}),
        "engines": validate_simple_list_file("engines", {"id", "key", "name", "version", "flags", "captureHook", "capture_hook", "battleWeatherHook", "battle_weather_hook", "mode"}),
        "events": validate_events_file,
        "fishing": validate_simple_list_file("fishing", {"id", "key", "name", "hook", "hookSymbol", "hook_symbol", "hookKey", "hook_key", "rods", "rodMask", "rod_mask", "phases", "phaseMask", "phase_mask", "buttonMask", "button_mask", "timeoutFrames", "timeout_frames", "successOutcome", "success_outcome", "failureOutcome", "failure_outcome", "promptKey", "prompt_key", "params", "priority", "flags", "mode"}),
        "flags": validate_flags_file,
        "followers": validate_followers_file,
        "items": validate_items_file,
        "lang": validate_language_file,
        "level_caps": validate_level_caps_file,
        "maps": validate_maps_root,
        "npcs": validate_npcs_file,
        "pokeballs": validate_simple_list_file("pokeballs", {"id", "key", "name", "itemId", "item_id", "ballId", "ball_id", "catchModifier", "catch_modifier", "modifierHook", "modifier_hook", "commitHook", "commit_hook", "battleScript", "battle_script", "flags", "mode"}),
        "pokemon": validate_simple_list_file("pokemon", {"id", "key", "name", "species", "speciesId", "species_id", "priority", "flags", "info", "baseStats", "base_stats", "types", "catchRate", "catch_rate", "expYield", "exp_yield", "evYields", "ev_yields", "items", "itemCommon", "item_common", "itemRare", "item_rare", "genderRatio", "gender_ratio", "eggCycles", "egg_cycles", "friendship", "growthRate", "growth_rate", "eggGroups", "egg_groups", "abilities", "safariZoneFleeRate", "safari_zone_flee_rate", "bodyColor", "body_color", "levelUpMoves", "level_up_moves", "evolutions", "mode"}),
        "rewards": validate_rewards_file,
        "shops": validate_shops_file,
        "time": validate_simple_list_file("time", {"id", "key", "name", "startMinute", "start_minute", "endMinute", "end_minute", "segment", "mode"}),
        "trainers": validate_simple_list_file("trainers", {"id", "key", "name", "trainerId", "trainer_id", "priority", "flags", "class", "trainerClass", "trainer_class", "musicGender", "music_gender", "pic", "items", "double", "ai", "party", "mode"}),
        "weather": validate_weather_file,
    }
    for domain, validator in validators.items():
        root = mod_root / domain
        if root.exists():
            if domain == "maps":
                validator(root)
                continue
            for path in sorted(root.rglob("*.json")):
                validator(path)
    src_root = mod_root / "src"
    if src_root.exists():
        for path in sorted(src_root.rglob("*.c")):
            validate_entrypoint_includes(path)


def validate_simple_list_file(domain: str, allowed: set[str]):
    def validate(path: Path) -> None:
        for index, raw_item in enumerate(require_items(read_json(path), DOMAIN_LIST_KEYS[domain], path)):
            if domain == "flags" and isinstance(raw_item, str):
                continue
            item = require_object(raw_item, path, f"{DOMAIN_LIST_KEYS[domain]}[{index}]")
            reject_unknown_fields(item, allowed, path, f"{DOMAIN_LIST_KEYS[domain]}[{index}]")
            validate_hook_mode(item, path, f"{DOMAIN_LIST_KEYS[domain]}[{index}]")
    return validate


def validate_events_file(path: Path) -> None:
    for index, raw_item in enumerate(require_items(read_json(path), DOMAIN_LIST_KEYS["events"], path)):
        item = require_object(raw_item, path, f"subscriptions[{index}]")
        reject_unknown_fields(item, {"id", "key", "name", "type", "handler", "priority", "mode"}, path, f"subscriptions[{index}]")
        validate_hook_mode(item, path, f"subscriptions[{index}]")
        if "type" in item and not isinstance(item["type"], str):
            raise ModCheckError(f"{path}: subscriptions[{index}].type must be a string")
        require_ident(item.get("handler"), path, f"subscriptions[{index}].handler", required=True)
        require_optional_int(item, "priority", path, f"subscriptions[{index}].priority", -32768, 32767)


def validate_level_caps_file(path: Path) -> None:
    allowed = {
        "id", "key", "name", "mode", "capType", "cap_type", "capsByBadge", "caps_by_badge",
        "softExpPercent", "soft_exp_percent", "rareCandy", "rare_candy", "priority", "flags",
    }
    for index, raw_item in enumerate(require_items(read_json(path), DOMAIN_LIST_KEYS["level_caps"], path)):
        item = require_object(raw_item, path, f"caps[{index}]")
        reject_unknown_fields(item, allowed, path, f"caps[{index}]")
        caps = item.get("capsByBadge", item.get("caps_by_badge"))
        if not isinstance(caps, list) or len(caps) != 9:
            raise ModCheckError(f"{path}: caps[{index}].capsByBadge must be a list of 9 levels")
        for cap_index, cap in enumerate(caps):
            if not isinstance(cap, int) or cap < 1 or cap > 100:
                raise ModCheckError(f"{path}: caps[{index}].capsByBadge[{cap_index}] must be in [1, 100]")
        require_optional_int(item, "softExpPercent", path, f"caps[{index}].softExpPercent", 0, 100)
        require_optional_int(item, "soft_exp_percent", path, f"caps[{index}].soft_exp_percent", 0, 100)
        require_optional_int(item, "priority", path, f"caps[{index}].priority", -32768, 32767)


def validate_followers_file(path: Path) -> None:
    allowed = {
        "id", "key", "name", "species", "form", "shiny", "graphicsId", "graphics_id",
        "asset", "assetKey", "asset_key", "graphicsRevision", "graphics_revision",
        "graphicsInfoSymbol", "graphics_info_symbol", "mode",
    }
    for index, raw_item in enumerate(require_items(read_json(path), DOMAIN_LIST_KEYS["followers"], path)):
        item = require_object(raw_item, path, f"followers[{index}]")
        reject_unknown_fields(item, allowed, path, f"followers[{index}]")
        validate_hook_mode(item, path, f"followers[{index}]")
        require_token(item.get("species"), path, f"followers[{index}].species", required=True)
        require_optional_int(item, "form", path, f"followers[{index}].form", 0, 0xFFFF)
        if "shiny" in item and not isinstance(item["shiny"], bool):
            raise ModCheckError(f"{path}: followers[{index}].shiny must be boolean")
        require_token(item.get("graphicsId", item.get("graphics_id")), path, f"followers[{index}].graphicsId")
        for asset_key in ("asset", "assetKey", "asset_key"):
            if asset_key in item and (not isinstance(item[asset_key], str) or item[asset_key] == ""):
                raise ModCheckError(f"{path}: followers[{index}].{asset_key} must be a non-empty string")
        require_optional_int(item, "graphicsRevision", path, f"followers[{index}].graphicsRevision", 0, 0xFFFF)
        require_optional_int(item, "graphics_revision", path, f"followers[{index}].graphics_revision", 0, 0xFFFF)
        require_ident(item.get("graphicsInfoSymbol", item.get("graphics_info_symbol")), path, f"followers[{index}].graphicsInfoSymbol")


def validate_flags_file(path: Path) -> None:
    for index, raw_item in enumerate(require_items(read_json(path), DOMAIN_LIST_KEYS["flags"], path)):
        if isinstance(raw_item, str):
            continue
        item = require_object(raw_item, path, f"flags[{index}]")
        reject_unknown_fields(item, {"id", "key", "name", "initial"}, path, f"flags[{index}]")
        if not any(key in item for key in ("id", "key", "name")):
            raise ModCheckError(f"{path}: flags[{index}] needs id, key, or name")
        if "initial" in item and not isinstance(item["initial"], bool):
            raise ModCheckError(f"{path}: flags[{index}].initial must be boolean")


def validate_weather_file(path: Path) -> None:
    for index, raw_item in enumerate(require_items(read_json(path), DOMAIN_LIST_KEYS["weather"], path)):
        item = require_object(raw_item, path, f"providers[{index}]")
        reject_unknown_fields(item, {"id", "key", "name", "handler", "priority", "mode"}, path, f"providers[{index}]")
        validate_hook_mode(item, path, f"providers[{index}]")
        require_ident(item.get("handler"), path, f"providers[{index}].handler", required=True)
        require_optional_int(item, "priority", path, f"providers[{index}].priority", -32768, 32767)


def validate_items_file(path: Path) -> None:
    allowed = {
        "id", "key", "name", "itemId", "item_id", "nameKey", "name_key", "descriptionKey", "description_key",
        "price", "holdEffect", "hold_effect", "holdEffectParam", "hold_effect_param", "importance", "pocket",
        "type", "battleUsage", "battle_usage", "secondaryId", "secondary_id", "fieldUseHook", "field_use_hook",
        "fieldUseHookKey", "field_use_hook_key", "battleUseHook", "battle_use_hook", "battleUseHookKey",
        "battle_use_hook_key", "flags", "priority", "mode",
    }
    for index, raw_item in enumerate(require_items(read_json(path), DOMAIN_LIST_KEYS["items"], path)):
        item = require_object(raw_item, path, f"items[{index}]")
        reject_unknown_fields(item, allowed, path, f"items[{index}]")
        validate_hook_mode(item, path, f"items[{index}]")
        require_optional_int(item, "price", path, f"items[{index}].price", 0, 999999)
        require_optional_int(item, "priority", path, f"items[{index}].priority", -32768, 32767)
        require_ident(item.get("fieldUseHook", item.get("field_use_hook")), path, f"items[{index}].fieldUseHook")
        require_ident(item.get("battleUseHook", item.get("battle_use_hook")), path, f"items[{index}].battleUseHook")
        require_short_key(item.get("fieldUseHookKey", item.get("field_use_hook_key")), path, f"items[{index}].fieldUseHookKey")
        require_short_key(item.get("battleUseHookKey", item.get("battle_use_hook_key")), path, f"items[{index}].battleUseHookKey")


def validate_shops_file(path: Path) -> None:
    allowed = {"id", "key", "name", "map", "mapGroup", "map_group", "mapNum", "map_num", "type", "martType", "mart_type", "items", "priority", "flags", "mode"}
    for index, raw_item in enumerate(require_items(read_json(path), DOMAIN_LIST_KEYS["shops"], path)):
        item = require_object(raw_item, path, f"shops[{index}]")
        reject_unknown_fields(item, allowed, path, f"shops[{index}]")
        validate_hook_mode(item, path, f"shops[{index}]")
        has_group = "mapGroup" in item or "map_group" in item
        has_num = "mapNum" in item or "map_num" in item
        if "map" not in item and not (has_group and has_num):
            raise ModCheckError(f"{path}: shops[{index}] needs map or mapGroup/mapNum")
        items = item.get("items")
        if not isinstance(items, list) or len(items) == 0 or len(items) > 32:
            raise ModCheckError(f"{path}: shops[{index}].items must contain 1..32 entries")
        require_optional_int(item, "priority", path, f"shops[{index}].priority", -32768, 32767)


def validate_npcs_file(path: Path) -> None:
    allowed = {"id", "key", "name", "graphicsId", "graphics_id", "movementType", "movement_type", "localId", "local_id", "elevation", "flagId", "flag", "scriptSymbol", "script", "mode"}
    for index, raw_item in enumerate(require_items(read_json(path), DOMAIN_LIST_KEYS["npcs"], path)):
        item = require_object(raw_item, path, f"npcs[{index}]")
        reject_unknown_fields(item, allowed, path, f"npcs[{index}]")
        validate_hook_mode(item, path, f"npcs[{index}]")
        require_ident(item.get("scriptSymbol", item.get("script")), path, f"npcs[{index}].script")


def validate_rewards_file(path: Path) -> None:
    allowed = {"id", "key", "name", "source", "itemId", "item_id", "quantity", "minLevel", "min_level", "maxLevel", "max_level", "priority", "flags", "hook", "hookSymbol", "hook_symbol", "hookKey", "hook_key", "battleWeatherHook", "mode"}
    for index, raw_item in enumerate(require_items(read_json(path), DOMAIN_LIST_KEYS["rewards"], path)):
        item = require_object(raw_item, path, f"rewards[{index}]")
        reject_unknown_fields(item, allowed, path, f"rewards[{index}]")
        validate_hook_mode(item, path, f"rewards[{index}]")
        require_optional_int(item, "quantity", path, f"rewards[{index}].quantity", 1, 999)
        for key in ("minLevel", "min_level", "maxLevel", "max_level"):
            require_optional_int(item, key, path, f"rewards[{index}].{key}", 1, 100)
        min_level = item.get("minLevel", item.get("min_level", 1))
        max_level = item.get("maxLevel", item.get("max_level", 100))
        if isinstance(min_level, int) and isinstance(max_level, int) and min_level > max_level:
            raise ModCheckError(f"{path}: rewards[{index}] minLevel must be <= maxLevel")
        require_optional_int(item, "priority", path, f"rewards[{index}].priority", -32768, 32767)
        require_ident(item.get("hook", item.get("hookSymbol", item.get("hook_symbol"))), path, f"rewards[{index}].hook")
        require_ident(item.get("battleWeatherHook"), path, f"rewards[{index}].battleWeatherHook")
        require_short_key(item.get("hookKey", item.get("hook_key")), path, f"rewards[{index}].hookKey")


def validate_language_file(path: Path) -> None:
    data = require_object(read_json(path), path, "language file")
    reject_unknown_fields(data, {"language", "strings"}, path, "language file")
    if "language" in data and not isinstance(data["language"], str):
        raise ModCheckError(f"{path}: language must be a string")
    strings = data.get("strings")
    if not isinstance(strings, dict):
        raise ModCheckError(f"{path}: strings must be an object")
    for key, value in strings.items():
        if not isinstance(key, str) or key == "":
            raise ModCheckError(f"{path}: string keys must be non-empty strings")
        if not isinstance(value, str):
            raise ModCheckError(f"{path}: strings[{key!r}] must be a string")


def validate_maps_root(root: Path) -> None:
    for path in sorted(root.glob("*/map.json")):
        data = require_object(read_json(path), path, "map")
        reject_unknown_fields(data, {"id", "key", "name", "map_group", "mapGroup", "map_num", "mapNum", "script", "scriptPath", "script_path", "mode"}, path, "map")
        validate_hook_mode(data, path, "map")
        script = data.get("script", data.get("scriptPath", data.get("script_path", "scripts.inc")))
        if script is not None:
            if not isinstance(script, str) or script == "":
                raise ModCheckError(f"{path}: map.script must be a non-empty string")
            script_path = (path.parent / script).resolve()
            try:
                script_path.relative_to(path.parent.resolve())
            except ValueError as exc:
                raise ModCheckError(f"{path}: map.script escapes the map folder") from exc
            if script != "scripts.inc" or script_path.exists():
                if not script_path.exists():
                    raise ModCheckError(f"{path}: map script {script!r} does not exist")


def validate_manifest(path: Path, sdk_version: int) -> dict[str, Any]:
    manifest = read_json(path)
    if not isinstance(manifest, dict):
        raise ModCheckError(f"{path}: manifest must be an object")

    unknown = sorted(set(manifest) - KNOWN_MANIFEST_FIELDS)
    if unknown:
        raise ModCheckError(f"{path}: unknown manifest fields: {', '.join(unknown)}")

    mod_id = require_string(manifest, "id", path)
    if not MOD_ID_RE.match(mod_id):
        raise ModCheckError(f"{path}: id must match {MOD_ID_RE.pattern}")
    if path.parent.name != mod_id:
        raise ModCheckError(f"{path}: id {mod_id!r} must match folder name {path.parent.name!r}")

    name = require_string(manifest, "name", path)
    version = require_string(manifest, "version", path)
    requires_sdk = require_sdk_requirement(manifest.get("requiresSdk"), path)
    validate_sdk_requirement(requires_sdk, sdk_version, path)
    priority = require_u16(manifest.get("priority"), "priority", path, 1000)
    feature_flags = require_u16(manifest.get("featureFlags", manifest.get("features")), "featureFlags", path, 0)
    dependencies = require_id_list(manifest.get("dependencies"), "dependencies", path)
    load_after = require_id_list(manifest.get("loadAfter"), "loadAfter", path)
    conflicts = require_id_list(manifest.get("conflictsWith"), "conflictsWith", path)
    requires_features = require_feature_list(manifest.get("requiresFeatures"), path)
    state_version = require_u16(manifest.get("stateVersion"), "stateVersion", path, 0)
    state_bytes = require_u16(manifest.get("stateBytes"), "stateBytes", path, 0)
    entrypoints = validate_entrypoints(path.parent, manifest, path)
    domains = validate_domain_dirs(path.parent, path)
    validate_domain_files(path.parent)

    return {
        "id": mod_id,
        "name": name,
        "version": version,
        "requiresSdk": requires_sdk,
        "priority": priority,
        "featureFlags": feature_flags,
        "dependencies": dependencies,
        "loadAfter": load_after,
        "conflictsWith": conflicts,
        "requiresFeatures": requires_features,
        "stateVersion": state_version,
        "stateBytes": state_bytes,
        "entrypoints": entrypoints,
        "domains": domains,
        "path": path,
    }


def installed_manifest_paths(root: Path) -> list[Path]:
    mods_dir = root / "mods"
    if not mods_dir.exists():
        return []
    return sorted(path for path in mods_dir.glob("*/mod.json") if path.parent.name != "examples")


def load_enabled_mod_ids(root: Path) -> list[str] | None:
    path = root / "mods" / "enabled.json"
    if not path.exists():
        return None
    data = read_json(path)
    enabled = data.get("enabled") if isinstance(data, dict) else data
    if not isinstance(enabled, list):
        raise ModCheckError(f"{path}: expected list or object with 'enabled' list")
    out: list[str] = []
    seen: set[str] = set()
    for item in enabled:
        if not isinstance(item, str) or not MOD_ID_RE.match(item):
            raise ModCheckError(f"{path}: enabled entry {item!r} is not a valid mod id")
        if item in seen:
            raise ModCheckError(f"{path}: duplicate enabled mod {item!r}")
        seen.add(item)
        out.append(item)
    return out


def active_manifest_paths(root: Path) -> list[Path]:
    installed = installed_manifest_paths(root)
    enabled = load_enabled_mod_ids(root)
    if enabled is None:
        return installed
    by_id = {path.parent.name: path for path in installed}
    missing = sorted(set(enabled) - set(by_id))
    if missing:
        raise ModCheckError(f"{root / 'mods' / 'enabled.json'}: enabled mods missing manifests: {', '.join(missing)}")
    return [by_id[mod_id] for mod_id in enabled]


def example_manifest_paths(root: Path) -> list[Path]:
    examples_dir = root / "mods" / "examples"
    if not examples_dir.exists():
        return []
    return sorted(examples_dir.glob("*/mod.json"))


def validate_relationships(mods: list[dict[str, Any]], state_budget: int) -> None:
    by_id = {mod["id"]: mod for mod in mods}
    if len(by_id) != len(mods):
        raise ModCheckError("duplicate mod ids")

    load_index = {mod["id"]: index for index, mod in enumerate(sorted(mods, key=lambda item: (item["priority"], item["id"])))}
    for mod in mods:
        for dep in mod["dependencies"]:
            if dep not in by_id:
                raise ModCheckError(f"{mod['id']}: missing dependency {dep!r}")
            if load_index[dep] > load_index[mod["id"]]:
                raise ModCheckError(f"{mod['id']}: dependency {dep!r} loads after dependent; adjust priority")
        for prior in mod["loadAfter"]:
            if prior in by_id and load_index[prior] > load_index[mod["id"]]:
                raise ModCheckError(f"{mod['id']}: loadAfter {prior!r} loads later; adjust priority")
        for conflict in mod["conflictsWith"]:
            if conflict in by_id:
                raise ModCheckError(f"{mod['id']}: conflicts with enabled mod {conflict!r}")

    state_bytes = sum(mod["stateBytes"] for mod in mods)
    if state_bytes > state_budget:
        raise ModCheckError(f"declared mod stateBytes {state_bytes} exceeds budget {state_budget}")
    if sum(1 for mod in mods if mod["stateBytes"]) > 16:
        raise ModCheckError("mods with stateBytes are limited to 16")


def iter_domain_items(mod: dict[str, Any], domain: str, list_key: str):
    root = mod["path"].parent / domain
    if not root.exists():
        return
    for path in sorted(root.rglob("*.json")):
        if domain == "maps" and path.name != "map.json":
            continue
        if domain == "maps":
            yield path, require_object(read_json(path), path, "map")
            continue
        for item in require_items(read_json(path), list_key, path):
            if isinstance(item, dict):
                yield path, item


def validate_hook_graph(mods: list[dict[str, Any]]) -> None:
    resources: dict[tuple[str, str], list[str]] = {}
    domain_keys = {
        "events": "subscriptions",
        "weather": "providers",
        "shops": "shops",
        "items": "items",
        "rewards": "rewards",
        "encounters": "encounters",
        "maps": "maps",
    }
    for mod in mods:
        for domain, list_key in domain_keys.items():
            for path, item in iter_domain_items(mod, domain, list_key):
                mode = item.get("mode", "observe")
                if mode not in {"replace", "claim"}:
                    continue
                if domain == "events":
                    resource = str(item.get("type", "CUSTOM"))
                elif domain == "maps":
                    resource = path.parent.name
                else:
                    resource = get_item_key(item, path.stem)
                resources.setdefault((domain, resource), []).append(f"{mod['id']}:{mode}:{path}")
    for (domain, resource), owners in sorted(resources.items()):
        if len(owners) > 1:
            raise ModCheckError(f"hook conflict on {domain}:{resource}: {', '.join(owners)}")


def first_present(item: dict[str, Any], keys: tuple[str, ...], default: Any = None) -> Any:
    for key in keys:
        if key in item:
            return item[key]
    return default


def normalize_mod_key(mod_id: str, value: Any) -> str | None:
    if value is None:
        return None
    key = str(value)
    if ":" in key:
        return key
    return f"{mod_id}:{key}"


def validate_content_contracts(mods: list[dict[str, Any]]) -> list[str]:
    warnings: list[str] = []
    sprite_assets: set[str] = set()
    sprite_tags: dict[tuple[str, str], list[str]] = {}
    follower_slots: dict[tuple[str, str, bool], list[str]] = {}

    for mod in mods:
        for path, item in iter_domain_items(mod, "sprites/assets", "assets"):
            asset_key = normalize_mod_key(mod["id"], get_item_key(item, path.stem))
            if asset_key is not None:
                sprite_assets.add(asset_key)
            for label, value in (
                ("tileTag", first_present(item, ("tileTag", "tile_tag"), "TAG_NONE")),
                ("paletteTag", first_present(item, ("paletteTag", "palette_tag"), "TAG_NONE")),
            ):
                tag = str(value)
                if tag == "TAG_NONE":
                    continue
                sprite_tags.setdefault((label, tag), []).append(f"{asset_key}@{path}")

    for mod in mods:
        for path, item in iter_domain_items(mod, "followers", "followers"):
            species = str(first_present(item, ("species",), "0"))
            form = str(first_present(item, ("form",), 0))
            shiny = bool(first_present(item, ("shiny",), False))
            owner = f"{mod['id']}:{get_item_key(item, path.stem)}@{path}"
            follower_slots.setdefault((species, form, shiny), []).append(owner)

            graphics_info = first_present(item, ("graphicsInfoSymbol", "graphics_info_symbol"))
            asset_key = normalize_mod_key(mod["id"], first_present(item, ("asset", "assetKey", "asset_key")))
            if graphics_info is not None and asset_key is None:
                warnings.append(f"{owner}: graphicsInfoSymbol is set without an asset for palette loading")
            if asset_key is not None and asset_key not in sprite_assets:
                warnings.append(f"{owner}: follower asset {asset_key!r} is not registered in sprites/assets")

    for (label, tag), owners in sorted(sprite_tags.items()):
        if len(owners) > 1:
            raise ModCheckError(f"sprite asset tag conflict on {label}:{tag}: {', '.join(owners)}")

    for (species, form, shiny), owners in sorted(follower_slots.items()):
        if len(owners) > 1:
            raise ModCheckError(f"follower mapping conflict on {species}/form={form}/shiny={shiny}: {', '.join(owners)}")

    return warnings


def mod_state_reserved_budget(root: Path) -> int:
    header = root / "include" / "mod" / "state.h"
    text = header.read_text(encoding="utf-8")
    match = re.search(r"^\s*#define\s+MOD_SAVE_RESERVED_BYTES\s+\((\d+)\s+-\s*MOD_SAVE_BADGE_LEVEL_BYTES(?:\s+-\s*MOD_SAVE_STATE_VERSION_BYTES)?\)", text, re.MULTILINE)
    if match:
        badge_match = re.search(r"^\s*#define\s+MOD_SAVE_BADGE_LEVEL_COUNT\s+(\d+)", text, re.MULTILINE)
        version_match = re.search(r"^\s*#define\s+MOD_SAVE_MAX_STATE_BLOCKS\s+(\d+)", text, re.MULTILINE)
        version_bytes = int(version_match.group(1)) * 2 if version_match else 0
        return int(match.group(1)) - int(badge_match.group(1) if badge_match else 8) - version_bytes
    return 216


def load_modgen(root: Path):
    path = root / "scripts" / "modgen.py"
    spec = importlib.util.spec_from_file_location("modgen", path)
    if spec is None or spec.loader is None:
        raise ModCheckError(f"cannot load {path}")
    modgen = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(modgen)
    return modgen


def run_generator_collectors(root: Path) -> dict[str, int]:
    modgen = load_modgen(root)
    mods = modgen.load_mods(root)
    counts: dict[str, int] = {"mods": len(mods)}
    collectors = {
        "flags": modgen.collect_flags,
        "events": modgen.collect_events,
        "weather": modgen.collect_weather,
        "timeSegments": modgen.collect_time_segments,
        "badgeEffects": modgen.collect_badge_effects,
        "fishingActions": modgen.collect_fishing_actions,
        "encounters": modgen.collect_encounters,
        "shops": modgen.collect_shops,
        "items": modgen.collect_items,
        "rewards": modgen.collect_rewards,
        "pokemonData": modgen.collect_pokemon_data,
        "battleMoves": modgen.collect_battle_moves,
        "trainers": modgen.collect_trainers,
        "spriteAssets": modgen.collect_sprite_assets,
        "overworldSprites": modgen.collect_overworld_sprites,
        "battleSprites": modgen.collect_battle_sprites,
        "followers": modgen.collect_followers,
        "languageTexts": modgen.collect_language_texts,
        "levelCaps": modgen.collect_level_caps,
        "pokeballs": modgen.collect_pokeballs,
        "engines": modgen.collect_engines,
        "npcs": modgen.collect_npcs,
        "maps": modgen.collect_maps,
        "modSources": modgen.collect_mod_sources,
    }
    for key, collector in collectors.items():
        counts[key] = len(collector(mods))
    return counts


def build_report(root: Path, include_examples: bool) -> dict[str, Any]:
    sdk_version = current_sdk_version(root)
    installed_paths = installed_manifest_paths(root)
    enabled_ids = load_enabled_mod_ids(root)
    active_mods = [validate_manifest(path, sdk_version) for path in active_manifest_paths(root)]
    state_budget = mod_state_reserved_budget(root)
    validate_relationships(active_mods, state_budget)
    validate_hook_graph(active_mods)
    warnings = validate_content_contracts(active_mods)
    counts = run_generator_collectors(root)

    example_mods: list[dict[str, Any]] = []
    if include_examples:
        example_mods = [validate_manifest(path, sdk_version) for path in example_manifest_paths(root)]
        validate_relationships(example_mods, state_budget)
        validate_hook_graph(example_mods)
        warnings.extend(validate_content_contracts(example_mods))

    return {
        "ok": True,
        "sdkVersion": sdk_version,
        "stateBudgetBytes": state_budget,
        "installedMods": [path.parent.name for path in installed_paths],
        "enabledMods": enabled_ids,
        "disabledMods": sorted(set(path.parent.name for path in installed_paths) - set(mod["id"] for mod in active_mods)),
        "activeMods": [
            {key: value for key, value in mod.items() if key != "path"}
            for mod in sorted(active_mods, key=lambda item: (item["priority"], item["id"]))
        ],
        "examples": [
            {key: value for key, value in mod.items() if key != "path"}
            for mod in sorted(example_mods, key=lambda item: (item["priority"], item["id"]))
        ],
        "generatedCounts": counts,
        "warnings": warnings,
    }


def print_text_report(report: dict[str, Any]) -> None:
    print("Mod check OK")
    print(f"sdk version: {report['sdkVersion']}")
    print(f"active mods: {len(report['activeMods'])}")
    if report["enabledMods"] is not None:
        print(f"disabled mods: {len(report['disabledMods'])}")
    print(f"example mods: {len(report['examples'])}")
    print(f"state budget: {report['stateBudgetBytes']} bytes")
    if report["activeMods"]:
        print("load order:")
        for mod in report["activeMods"]:
            state = f", state={mod['stateBytes']}B/v{mod['stateVersion']}" if mod["stateBytes"] else ""
            print(f"  {mod['priority']:5d} {mod['id']} {mod['version']}{state}")
    counts = report["generatedCounts"]
    interesting = ["flags", "events", "weather", "items", "npcs", "maps", "levelCaps", "modSources"]
    print("generated counts: " + ", ".join(f"{key}={counts[key]}" for key in interesting))
    if report["warnings"]:
        print("warnings:")
        for warning in report["warnings"]:
            print(f"  {warning}")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", default=".")
    parser.add_argument("--json", action="store_true", help="emit a machine-readable report")
    parser.add_argument("--no-examples", action="store_true", help="skip validating mods/examples")
    args = parser.parse_args()

    root = Path(args.root).resolve()
    try:
        report = build_report(root, include_examples=not args.no_examples)
    except Exception as exc:
        if args.json:
            print(json.dumps({"ok": False, "error": str(exc)}, indent=2))
        else:
            print(f"mod-check failed: {exc}", file=sys.stderr)
        return 1

    if args.json:
        print(json.dumps(report, indent=2))
    else:
        print_text_report(report)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
