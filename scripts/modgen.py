#!/usr/bin/env python3
"""Generate static mod registries from mods/<modId> manifests."""

from __future__ import annotations

import argparse
import json
import os
import re
from pathlib import Path
from typing import Any, Dict, Iterable, List, Optional, Tuple


IDENT_RE = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")
MOD_ID_RE = re.compile(r"^[a-z0-9][a-z0-9_-]*$")
FNV_OFFSET = 2166136261
FNV_PRIME = 16777619

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

BADGES = {
    "STONE": "MOD_BADGE_STONE",
    "KNUCKLE": "MOD_BADGE_KNUCKLE",
    "DYNAMO": "MOD_BADGE_DYNAMO",
    "HEAT": "MOD_BADGE_HEAT",
    "BALANCE": "MOD_BADGE_BALANCE",
    "FEATHER": "MOD_BADGE_FEATHER",
    "MIND": "MOD_BADGE_MIND",
    "RAIN": "MOD_BADGE_RAIN",
}

BADGE_EFFECT_TYPES = {
    "NONE": "MOD_BADGE_EFFECT_TYPE_NONE",
    "RESISTANCE_PERCENT": "MOD_BADGE_EFFECT_TYPE_RESISTANCE_PERCENT",
    "DAMAGE_PERCENT": "MOD_BADGE_EFFECT_TYPE_DAMAGE_PERCENT",
    "STAT_PERCENT": "MOD_BADGE_EFFECT_TYPE_STAT_PERCENT",
}

BADGE_EFFECT_TYPE_VALUES = {
    "0": 0,
    "NONE": 0,
    "MOD_BADGE_EFFECT_TYPE_NONE": 0,
    "1": 1,
    "RESISTANCE_PERCENT": 1,
    "MOD_BADGE_EFFECT_TYPE_RESISTANCE_PERCENT": 1,
    "2": 2,
    "DAMAGE_PERCENT": 2,
    "MOD_BADGE_EFFECT_TYPE_DAMAGE_PERCENT": 2,
    "3": 3,
    "STAT_PERCENT": 3,
    "MOD_BADGE_EFFECT_TYPE_STAT_PERCENT": 3,
}

BADGE_VALUES = {
    "0": 0,
    "STONE": 0,
    "MOD_BADGE_STONE": 0,
    "1": 1,
    "KNUCKLE": 1,
    "MOD_BADGE_KNUCKLE": 1,
    "2": 2,
    "DYNAMO": 2,
    "MOD_BADGE_DYNAMO": 2,
    "3": 3,
    "HEAT": 3,
    "MOD_BADGE_HEAT": 3,
    "4": 4,
    "BALANCE": 4,
    "MOD_BADGE_BALANCE": 4,
    "5": 5,
    "FEATHER": 5,
    "MOD_BADGE_FEATHER": 5,
    "6": 6,
    "MIND": 6,
    "MOD_BADGE_MIND": 6,
    "7": 7,
    "RAIN": 7,
    "MOD_BADGE_RAIN": 7,
}

POKEMON_TYPE_VALUES = {
    "TYPE_NORMAL": 0,
    "TYPE_FIGHTING": 1,
    "TYPE_FLYING": 2,
    "TYPE_POISON": 3,
    "TYPE_GROUND": 4,
    "TYPE_ROCK": 5,
    "TYPE_BUG": 6,
    "TYPE_GHOST": 7,
    "TYPE_STEEL": 8,
    "TYPE_MYSTERY": 9,
    "TYPE_FIRE": 10,
    "TYPE_WATER": 11,
    "TYPE_GRASS": 12,
    "TYPE_ELECTRIC": 13,
    "TYPE_PSYCHIC": 14,
    "TYPE_ICE": 15,
    "TYPE_DRAGON": 16,
    "TYPE_DARK": 17,
    "TYPE_NONE": 255,
}

BATTLE_STAT_VALUES = {
    "STAT_HP": 0,
    "STAT_ATK": 1,
    "STAT_DEF": 2,
    "STAT_SPEED": 3,
    "STAT_SPATK": 4,
    "STAT_SPDEF": 5,
    "STAT_ACC": 6,
    "STAT_EVASION": 7,
}

FISHING_ROD_VALUES = {
    "OLD": 1 << 0,
    "OLD_ROD": 1 << 0,
    "GOOD": 1 << 1,
    "GOOD_ROD": 1 << 1,
    "SUPER": 1 << 2,
    "SUPER_ROD": 1 << 2,
}

FISHING_ROD_MASK_ALL = (1 << 0) | (1 << 1) | (1 << 2)

FISHING_PHASE_VALUES = {
    "START": 0,
    "ROUND_START": 1,
    "DOT_CONFIG": 2,
    "BITE_CHECK": 3,
    "INPUT_WINDOW": 4,
    "MORE_DOTS_CHECK": 5,
    "BEFORE_ENCOUNTER": 6,
    "END": 7,
}

FISHING_PHASE_MASK_ALL = (1 << len(FISHING_PHASE_VALUES)) - 1

FISHING_OUTCOME_VALUES = {
    "CONTINUE": 0,
    "NO_BITE": 1,
    "GOT_AWAY": 2,
    "START_ROUND": 3,
    "ON_HOOK": 4,
    "START_ENCOUNTER": 5,
    "CANCEL": 6,
}

ENCOUNTER_AREA_VALUES = {
    "LAND": "MOD_ENCOUNTER_AREA_LAND",
    "GRASS": "MOD_ENCOUNTER_AREA_LAND",
    "WATER": "MOD_ENCOUNTER_AREA_WATER",
    "SURF": "MOD_ENCOUNTER_AREA_WATER",
    "ROCK": "MOD_ENCOUNTER_AREA_ROCK_SMASH",
    "ROCK_SMASH": "MOD_ENCOUNTER_AREA_ROCK_SMASH",
    "FISHING": "MOD_ENCOUNTER_AREA_FISHING",
    "FISH": "MOD_ENCOUNTER_AREA_FISHING",
}

ENCOUNTER_AREA_NUMBERS = {
    "MOD_ENCOUNTER_AREA_LAND": 0,
    "MOD_ENCOUNTER_AREA_WATER": 1,
    "MOD_ENCOUNTER_AREA_ROCK_SMASH": 2,
    "MOD_ENCOUNTER_AREA_FISHING": 3,
}

SHOP_TYPE_VALUES = {
    "NORMAL": "MOD_SHOP_TYPE_NORMAL",
    "MART": "MOD_SHOP_TYPE_NORMAL",
    "POKEMART": "MOD_SHOP_TYPE_NORMAL",
    "DECOR": "MOD_SHOP_TYPE_DECOR",
    "DECORATION": "MOD_SHOP_TYPE_DECOR",
    "DECOR2": "MOD_SHOP_TYPE_DECOR2",
    "DECORATION2": "MOD_SHOP_TYPE_DECOR2",
}

ITEM_OVERRIDE_FLAGS = {
    "NAME": 1 << 0,
    "DESCRIPTION": 1 << 1,
    "PRICE": 1 << 2,
    "HOLD_EFFECT": 1 << 3,
    "HOLD_EFFECT_PARAM": 1 << 4,
    "IMPORTANCE": 1 << 5,
    "POCKET": 1 << 6,
    "TYPE": 1 << 7,
    "FIELD_USE": 1 << 8,
    "BATTLE_USAGE": 1 << 9,
    "BATTLE_USE": 1 << 10,
    "SECONDARY_ID": 1 << 11,
}

REWARD_SOURCE_VALUES = {
    "PICKUP_COMMON": "MOD_REWARD_SOURCE_PICKUP_COMMON",
    "PICKUP": "MOD_REWARD_SOURCE_PICKUP_COMMON",
    "PICKUP_RARE": "MOD_REWARD_SOURCE_PICKUP_RARE",
    "PYRAMID_PICKUP": "MOD_REWARD_SOURCE_PYRAMID_PICKUP",
}

REWARD_SOURCE_NUMBERS = {
    "MOD_REWARD_SOURCE_PICKUP_COMMON": 1,
    "MOD_REWARD_SOURCE_PICKUP_RARE": 2,
    "MOD_REWARD_SOURCE_PYRAMID_PICKUP": 3,
}

POKEMON_OVERRIDE_FLAGS = {
    "BASE_STATS": 1 << 0,
    "TYPES": 1 << 1,
    "CATCH_RATE": 1 << 2,
    "EXP_YIELD": 1 << 3,
    "EV_YIELD": 1 << 4,
    "ITEMS": 1 << 5,
    "GENDER": 1 << 6,
    "EGG_CYCLES": 1 << 7,
    "FRIENDSHIP": 1 << 8,
    "GROWTH_RATE": 1 << 9,
    "EGG_GROUPS": 1 << 10,
    "ABILITIES": 1 << 11,
    "SAFARI_FLEE": 1 << 12,
    "BODY_COLOR": 1 << 13,
    "LEVEL_UP": 1 << 14,
    "EVOLUTIONS": 1 << 15,
}

BATTLE_MOVE_OVERRIDE_FLAGS = {
    "EFFECT": 1 << 0,
    "POWER": 1 << 1,
    "TYPE": 1 << 2,
    "ACCURACY": 1 << 3,
    "PP": 1 << 4,
    "SECONDARY": 1 << 5,
    "TARGET": 1 << 6,
    "PRIORITY": 1 << 7,
    "FLAGS": 1 << 8,
}

TRAINER_OVERRIDE_FLAGS = {
    "CLASS": 1 << 0,
    "PIC": 1 << 1,
    "MUSIC_GENDER": 1 << 2,
    "ITEMS": 1 << 3,
    "DOUBLE": 1 << 4,
    "AI": 1 << 5,
    "PARTY": 1 << 6,
}

BUTTON_VALUES = {
    "A": 0x0001,
    "A_BUTTON": 0x0001,
    "B": 0x0002,
    "B_BUTTON": 0x0002,
    "SELECT": 0x0004,
    "SELECT_BUTTON": 0x0004,
    "START": 0x0008,
    "START_BUTTON": 0x0008,
    "RIGHT": 0x0010,
    "DPAD_RIGHT": 0x0010,
    "LEFT": 0x0020,
    "DPAD_LEFT": 0x0020,
    "UP": 0x0040,
    "DPAD_UP": 0x0040,
    "DOWN": 0x0080,
    "DPAD_DOWN": 0x0080,
    "R": 0x0100,
    "R_BUTTON": 0x0100,
    "L": 0x0200,
    "L_BUTTON": 0x0200,
}

KEYS_MASK = 0x03FF


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


def fnv1a(data: bytes) -> int:
    value = FNV_OFFSET
    for byte in data:
        value ^= byte
        value = (value * FNV_PRIME) & 0xFFFFFFFF
    return value


def hash_text(value: str) -> int:
    return fnv1a(value.encode("utf-8"))


def hash_parts(*parts: Any) -> int:
    return hash_text("\x1f".join(str(part) for part in parts))


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


def normalize_badge_id(value: Any) -> str:
    if isinstance(value, int):
        return str(value)
    if not isinstance(value, str):
        raise ModgenError(f"Badge id {value!r} is invalid")
    value = value.strip()
    if value.startswith("MOD_BADGE_"):
        return value
    upper = value.upper()
    if upper in BADGES:
        return BADGES[upper]
    raise ModgenError(f"Unknown badge id {value!r}")


def normalize_badge_effect_type(value: Any) -> str:
    if isinstance(value, int):
        return str(value)
    if not isinstance(value, str):
        raise ModgenError(f"Badge effect type {value!r} is invalid")
    value = value.strip()
    if value.startswith("MOD_BADGE_EFFECT_TYPE_"):
        return value
    upper = value.upper()
    if upper in BADGE_EFFECT_TYPES:
        return BADGE_EFFECT_TYPES[upper]
    raise ModgenError(f"Unknown badge effect type {value!r}")


def badge_value(value: str) -> int:
    key = value.strip().upper()
    if key in BADGE_VALUES:
        return BADGE_VALUES[key]
    if re.fullmatch(r"0x[0-9A-Fa-f]+|-?[0-9]+", value.strip()):
        return int(value, 0)
    raise ModgenError(f"Badge id {value!r} is invalid")


def badge_effect_type_value(value: str) -> int:
    key = value.strip().upper()
    if key in BADGE_EFFECT_TYPE_VALUES:
        return BADGE_EFFECT_TYPE_VALUES[key]
    if re.fullmatch(r"0x[0-9A-Fa-f]+|-?[0-9]+", value.strip()):
        return int(value, 0)
    raise ModgenError(f"Badge effect type {value!r} is invalid")


def badge_effect_target_value(value: Any) -> int:
    if isinstance(value, int):
        return value
    if value is None:
        return 0
    if not isinstance(value, str):
        raise ModgenError(f"Badge effect target {value!r} is invalid")
    stripped = value.strip()
    upper = stripped.upper()
    if upper in POKEMON_TYPE_VALUES:
        return POKEMON_TYPE_VALUES[upper]
    if upper in BATTLE_STAT_VALUES:
        return BATTLE_STAT_VALUES[upper]
    if re.fullmatch(r"0x[0-9A-Fa-f]+|-?[0-9]+", stripped):
        return int(stripped, 0)
    raise ModgenError(f"Badge effect target {value!r} is not a known Pokemon type, battle stat, or integer")


def validate_badge_effect(key: str, badge: str, effect: str, target_value: int, percent: int, max_level: int) -> None:
    badge_id = badge_value(badge)
    effect_kind = badge_effect_type_value(effect)

    if badge_id < 0 or badge_id >= 8:
        raise ModgenError(f"{key}: badge must be in [0, 7]")
    if effect_kind == 0:
        raise ModgenError(f"{key}: effect NONE is reserved for empty default records")
    if effect_kind > 3:
        raise ModgenError(f"{key}: effect must be RESISTANCE_PERCENT, DAMAGE_PERCENT, or STAT_PERCENT")
    if percent < -100 or percent > 100:
        raise ModgenError(f"{key}: percentPerLevel must be in [-100, 100]")
    if max_level < 1 or max_level > 10:
        raise ModgenError(f"{key}: maxLevel must be in [1, 10]")
    if effect_kind in (1, 2):
        if target_value == 255 or target_value < 0 or target_value >= 18:
            raise ModgenError(f"{key}: target must be a real Pokemon type in [0, 17]")
    elif target_value < 0 or target_value >= 8:
        raise ModgenError(f"{key}: target must be a battle stat in [0, 7]")


def parse_named_mask(value: Any, names: Dict[str, int], all_value: int, label: str, allow_zero: bool = False) -> int:
    if value is None:
        return all_value
    if isinstance(value, int):
        mask = value
    elif isinstance(value, str):
        stripped = value.strip()
        if re.fullmatch(r"0x[0-9A-Fa-f]+|[0-9]+", stripped):
            mask = int(stripped, 0)
        else:
            mask = 0
            for part in re.split(r"[|,+ ]+", stripped):
                if not part:
                    continue
                upper = part.upper()
                if upper not in names:
                    raise ModgenError(f"Unknown {label} {part!r}")
                mask |= names[upper]
    elif isinstance(value, list):
        mask = 0
        for part in value:
            mask |= parse_named_mask(part, names, all_value, label, allow_zero=False)
    else:
        raise ModgenError(f"{label} mask {value!r} is invalid")
    if (mask == 0 and not allow_zero) or (mask & ~all_value) != 0:
        raise ModgenError(f"{label} mask {value!r} is invalid")
    return mask


def parse_fishing_rod_mask(item: Dict[str, Any]) -> int:
    value = item.get("rodMask", item.get("rod_mask"))
    if value is None and "rods" in item:
        value = item.get("rods")
    return parse_named_mask(value, FISHING_ROD_VALUES, FISHING_ROD_MASK_ALL, "fishing rod")


def parse_fishing_phase_mask(item: Dict[str, Any]) -> int:
    value = item.get("phaseMask", item.get("phase_mask"))
    if value is None and "phases" in item:
        value = item.get("phases")
    if value is None:
        return FISHING_PHASE_MASK_ALL
    if isinstance(value, int):
        mask = value
    elif isinstance(value, str) and re.fullmatch(r"0x[0-9A-Fa-f]+|[0-9]+", value.strip()):
        mask = int(value, 0)
    else:
        phase_bits = {key: 1 << phase for key, phase in FISHING_PHASE_VALUES.items()}
        mask = parse_named_mask(value, phase_bits, FISHING_PHASE_MASK_ALL, "fishing phase")
    if mask == 0 or (mask & ~FISHING_PHASE_MASK_ALL) != 0:
        raise ModgenError(f"fishing phase mask {value!r} is invalid")
    return mask


def parse_button_mask(value: Any, default: int = 0) -> int:
    if value is None:
        return default
    return parse_named_mask(value, BUTTON_VALUES, KEYS_MASK, "button", allow_zero=False)


def parse_fishing_outcome(value: Any, default: str) -> int:
    if value is None:
        value = default
    if isinstance(value, int):
        outcome = value
    elif isinstance(value, str):
        stripped = value.strip()
        if re.fullmatch(r"0x[0-9A-Fa-f]+|[0-9]+", stripped):
            outcome = int(stripped, 0)
        else:
            upper = stripped.upper()
            if upper.startswith("FISHING_OUTCOME_"):
                upper = upper[len("FISHING_OUTCOME_") :]
            if upper not in FISHING_OUTCOME_VALUES:
                raise ModgenError(f"Unknown fishing outcome {value!r}")
            outcome = FISHING_OUTCOME_VALUES[upper]
    else:
        raise ModgenError(f"Fishing outcome {value!r} is invalid")
    if outcome < 0 or outcome > max(FISHING_OUTCOME_VALUES.values()):
        raise ModgenError(f"Fishing outcome {value!r} is invalid")
    return outcome


def normalize_encounter_area(value: Any) -> str:
    if isinstance(value, int):
        if value < 0 or value > 3:
            raise ModgenError(f"Encounter area {value!r} is invalid")
        return str(value)
    if not isinstance(value, str):
        raise ModgenError(f"Encounter area {value!r} is invalid")
    stripped = value.strip()
    if re.fullmatch(r"0x[0-9A-Fa-f]+|[0-9]+", stripped):
        raw = int(stripped, 0)
        if raw < 0 or raw > 3:
            raise ModgenError(f"Encounter area {value!r} is invalid")
        return str(raw)
    if stripped.startswith("MOD_ENCOUNTER_AREA_"):
        return stripped
    upper = stripped.upper()
    if upper not in ENCOUNTER_AREA_VALUES:
        raise ModgenError(f"Unknown encounter area {value!r}")
    return ENCOUNTER_AREA_VALUES[upper]


def encounter_area_value(value: str) -> int:
    stripped = value.strip()
    if stripped in ENCOUNTER_AREA_NUMBERS:
        return ENCOUNTER_AREA_NUMBERS[stripped]
    if re.fullmatch(r"0x[0-9A-Fa-f]+|[0-9]+", stripped):
        raw = int(stripped, 0)
        if raw < 0 or raw > 3:
            raise ModgenError(f"Encounter area {value!r} is invalid")
        return raw
    raise ModgenError(f"Encounter area {value!r} is invalid")


def parse_map_group_num(item: Dict[str, Any], key: str) -> Tuple[str, str]:
    map_value = item.get("map")
    if map_value is not None:
        token = c_int_or_token(map_value)
        return f"MAP_GROUP({token})", f"MAP_NUM({token})"
    group = c_int_or_token(item.get("mapGroup", item.get("map_group")), "0")
    num = c_int_or_token(item.get("mapNum", item.get("map_num")), "0")
    if group == "0" and num == "0":
        raise ModgenError(f"{key}: encounter requires map or mapGroup/mapNum")
    return group, num


def parse_encounter_slot(item: Dict[str, Any], key: str, area: int) -> Dict[str, Any]:
    species = c_int_or_token(item.get("species"), "SPECIES_NONE")
    min_level = int(item.get("minLevel", item.get("min_level", item.get("level", 1))))
    max_level = int(item.get("maxLevel", item.get("max_level", item.get("level", min_level))))
    weight = int(item.get("weight", 1))
    if min_level < 1 or min_level > 100 or max_level < 1 or max_level > 100:
        raise ModgenError(f"{key}: encounter slot levels must be in [1, 100]")
    if weight < 1 or weight > 255:
        raise ModgenError(f"{key}: encounter slot weight must be in [1, 255]")
    rod_mask = parse_fishing_rod_mask(item) if area == 3 else FISHING_ROD_MASK_ALL
    return {
        "min": min_level,
        "max": max_level,
        "species": species,
        "weight": weight,
        "rod_mask": rod_mask,
        "flags": c_int_or_token(item.get("flags"), "0"),
    }


def normalize_shop_type(value: Any) -> str:
    if isinstance(value, int):
        if value < 0 or value > 2:
            raise ModgenError(f"Shop type {value!r} is invalid")
        return str(value)
    if not isinstance(value, str):
        raise ModgenError(f"Shop type {value!r} is invalid")
    stripped = value.strip()
    if re.fullmatch(r"0x[0-9A-Fa-f]+|[0-9]+", stripped):
        raw = int(stripped, 0)
        if raw < 0 or raw > 2:
            raise ModgenError(f"Shop type {value!r} is invalid")
        return str(raw)
    if stripped.startswith("MOD_SHOP_TYPE_"):
        return stripped
    upper = stripped.upper()
    if upper not in SHOP_TYPE_VALUES:
        raise ModgenError(f"Unknown shop type {value!r}")
    return SHOP_TYPE_VALUES[upper]


def parse_s16_params(value: Any, key: str) -> List[int]:
    if value is None:
        values: List[int] = []
    elif isinstance(value, list):
        values = [int(item) for item in value]
    else:
        raise ModgenError(f"{key}: params must be a list")
    if len(values) > 4:
        raise ModgenError(f"{key}: params may contain at most 4 values")
    for param in values:
        if param < -32768 or param > 32767:
            raise ModgenError(f"{key}: params must be signed 16-bit values")
    return values + [0] * (4 - len(values))


def parse_u16_flags(value: Any, names: Dict[str, int], label: str, inferred: int = 0) -> int:
    if value is None:
        flags = inferred
    elif isinstance(value, int):
        flags = value | inferred
    elif isinstance(value, str):
        stripped = value.strip()
        if re.fullmatch(r"0x[0-9A-Fa-f]+|[0-9]+", stripped):
            flags = int(stripped, 0) | inferred
        else:
            flags = inferred
            for part in re.split(r"[|,+ ]+", stripped):
                if not part:
                    continue
                upper = part.upper()
                if upper not in names:
                    raise ModgenError(f"Unknown {label} flag {part!r}")
                flags |= names[upper]
    elif isinstance(value, list):
        flags = inferred
        for part in value:
            flags |= parse_u16_flags(part, names, label)
    else:
        raise ModgenError(f"{label} flags {value!r} are invalid")
    if flags < 0 or flags > 0xFFFF:
        raise ModgenError(f"{label} flags must fit in 16 bits")
    return flags


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


def collect_badge_effects(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    effects = []
    seen = set()
    for mod in mods:
        for path in iter_json_files(mod["root"] / "badges"):
            for index, item in enumerate(as_list(read_json(path), "effects")):
                key = key_for(mod["id"], item, f"effect_{index}")
                if key in seen:
                    raise ModgenError(f"Duplicate badge effect key {key!r}")
                seen.add(key)

                percent = int(item.get("percentPerLevel", item.get("percent_per_level", 0)))
                max_level = int(item.get("maxLevel", item.get("max_level", 1)))
                badge = normalize_badge_id(item.get("badge", item.get("badgeId", item.get("badge_id"))))
                effect = normalize_badge_effect_type(item.get("effect", item.get("effectKind", item.get("effect_kind", "NONE"))))
                target = item.get("target")
                target_value = badge_effect_target_value(target)
                validate_badge_effect(key, badge, effect, target_value, percent, max_level)

                effects.append(
                    {
                        "key": key,
                        "badge": badge,
                        "effect": effect,
                        "target": c_int_or_token(target, "0"),
                        "percent": percent,
                        "max_level": max_level,
                        "flags": c_int_or_token(item.get("flags"), "0"),
                    }
                )
    effects.sort(key=lambda effect: effect["key"])
    return effects


def collect_fishing_actions(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    actions = []
    seen = set()
    for mod in mods:
        for path in iter_json_files(mod["root"] / "fishing"):
            for index, item in enumerate(as_list(read_json(path), "actions")):
                key = key_for(mod["id"], item, f"action_{index}")
                if key in seen:
                    raise ModgenError(f"Duplicate fishing action key {key!r}")
                seen.add(key)

                hook = c_func(item.get("hook", item.get("hookSymbol", item.get("hook_symbol"))))
                if hook == "NULL":
                    raise ModgenError(f"{key}: fishing action hook is required")
                hook_key = str(item.get("hookKey", item.get("hook_key", hook)))
                if hook_key == "" or len(hook_key) > 31:
                    raise ModgenError(f"{key}: hookKey must be non-empty and <= 31 bytes")
                prompt_key = item.get("promptKey", item.get("prompt_key"))
                if prompt_key is not None:
                    prompt_key = str(prompt_key)
                    if ":" not in prompt_key:
                        prompt_key = f"{mod['id']}:{prompt_key}"
                    if len(prompt_key) > 31:
                        raise ModgenError(f"{key}: promptKey must be <= 31 bytes")

                rod_mask = parse_fishing_rod_mask(item)
                phase_mask = parse_fishing_phase_mask(item)
                button_mask = parse_button_mask(item.get("buttonMask", item.get("button_mask")), 0)
                timeout = int(item.get("timeoutFrames", item.get("timeout_frames", 0)))
                if timeout < 0 or timeout > 32767:
                    raise ModgenError(f"{key}: timeoutFrames must be in [0, 32767]")
                params = parse_s16_params(item.get("params"), key)
                raw_flags = item.get("flags", 0)
                if isinstance(raw_flags, str):
                    if not re.fullmatch(r"0x[0-9A-Fa-f]+|[0-9]+", raw_flags.strip()):
                        raise ModgenError(f"{key}: flags must be an integer")
                    flags_value = int(raw_flags, 0)
                else:
                    flags_value = int(raw_flags or 0)
                if flags_value < 0 or flags_value > 0xFFFF:
                    raise ModgenError(f"{key}: flags must be in [0, 65535]")
                priority = int(item.get("priority", 1000))
                if priority < -32768 or priority > 32767:
                    raise ModgenError(f"{key}: priority must be in signed 16-bit range")

                actions.append(
                    {
                        "key": key,
                        "hook": hook,
                        "hook_key": hook_key,
                        "rod_mask": rod_mask,
                        "phase_mask": phase_mask,
                        "priority": priority,
                        "flags": flags_value,
                        "prompt_key": prompt_key,
                        "button_mask": button_mask,
                        "timeout": timeout,
                        "success_outcome": parse_fishing_outcome(item.get("successOutcome", item.get("success_outcome")), "CONTINUE"),
                        "failure_outcome": parse_fishing_outcome(item.get("failureOutcome", item.get("failure_outcome")), "GOT_AWAY"),
                        "params": params,
                    }
                )
    actions.sort(key=lambda action: (action["priority"], action["key"]))
    return actions


def collect_encounters(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    encounters = []
    seen = set()
    for mod in mods:
        for path in iter_json_files(mod["root"] / "encounters"):
            for index, item in enumerate(as_list(read_json(path), "encounters")):
                key = key_for(mod["id"], item, f"encounter_{index}")
                if key in seen:
                    raise ModgenError(f"Duplicate encounter key {key!r}")
                seen.add(key)

                area = normalize_encounter_area(item.get("area", "LAND"))
                area_value = encounter_area_value(area)
                group, num = parse_map_group_num(item, key)
                hook = c_func(item.get("hook", item.get("hookSymbol", item.get("hook_symbol"))))
                hook_key = item.get("hookKey", item.get("hook_key", hook if hook != "NULL" else None))
                if hook_key is not None:
                    hook_key = str(hook_key)
                    if hook_key == "" or len(hook_key) > 31:
                        raise ModgenError(f"{key}: hookKey must be non-empty and <= 31 bytes")
                rod_mask = parse_fishing_rod_mask(item) if area_value == 3 else FISHING_ROD_MASK_ALL
                encounter_rate = int(item.get("encounterRate", item.get("encounter_rate", 0)))
                if encounter_rate < 0 or encounter_rate > 255:
                    raise ModgenError(f"{key}: encounterRate must be in [0, 255]")
                priority = int(item.get("priority", 1000))
                if priority < -32768 or priority > 32767:
                    raise ModgenError(f"{key}: priority must be in signed 16-bit range")
                raw_slots = as_list(item.get("slots", []), "slots")
                if len(raw_slots) > 12:
                    raise ModgenError(f"{key}: encounters may contain at most 12 slots")
                if len(raw_slots) == 0 and hook == "NULL":
                    raise ModgenError(f"{key}: encounter requires slots or a hook")
                slots = [parse_encounter_slot(slot, key, area_value) for slot in raw_slots]
                while len(slots) < 12:
                    slots.append({"min": 0, "max": 0, "species": "SPECIES_NONE", "weight": 0, "rod_mask": 0, "flags": "0"})

                encounters.append(
                    {
                        "key": key,
                        "hook": hook,
                        "hook_key": hook_key,
                        "group": group,
                        "num": num,
                        "area": area,
                        "area_value": area_value,
                        "rod_mask": rod_mask,
                        "encounter_rate": encounter_rate,
                        "slot_count": len(raw_slots),
                        "priority": priority,
                        "flags": c_int_or_token(item.get("flags"), "0"),
                        "slots": slots,
                    }
                )
    encounters.sort(key=lambda encounter: (encounter["priority"], encounter["key"]))
    return encounters


def collect_shops(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    shops = []
    seen = set()
    for mod in mods:
        for path in iter_json_files(mod["root"] / "shops"):
            for index, item in enumerate(as_list(read_json(path), "shops")):
                key = key_for(mod["id"], item, f"shop_{index}")
                if key in seen:
                    raise ModgenError(f"Duplicate shop key {key!r}")
                seen.add(key)

                group, num = parse_map_group_num(item, key)
                mart_type = normalize_shop_type(item.get("type", item.get("martType", item.get("mart_type", "NORMAL"))))
                raw_items = item.get("items", [])
                if not isinstance(raw_items, list):
                    raise ModgenError(f"{key}: shop items must be a list")
                if len(raw_items) == 0 or len(raw_items) > 32:
                    raise ModgenError(f"{key}: shops must contain 1..32 items")
                items = [c_int_or_token(raw_item, "ITEM_NONE") for raw_item in raw_items]
                while len(items) < 32:
                    items.append("ITEM_NONE")
                priority = int(item.get("priority", 1000))
                if priority < -32768 or priority > 32767:
                    raise ModgenError(f"{key}: priority must be in signed 16-bit range")
                shops.append(
                    {
                        "key": key,
                        "group": group,
                        "num": num,
                        "mart_type": mart_type,
                        "item_count": len(raw_items),
                        "priority": priority,
                        "flags": c_int_or_token(item.get("flags"), "0"),
                        "items": items,
                    }
                )
    shops.sort(key=lambda shop: (shop["priority"], shop["key"]))
    return shops


def collect_items(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    items = []
    seen = set()
    for mod in mods:
        for path in iter_json_files(mod["root"] / "items"):
            for index, item in enumerate(as_list(read_json(path), "items")):
                key = key_for(mod["id"], item, f"item_{index}")
                if key in seen:
                    raise ModgenError(f"Duplicate item key {key!r}")
                seen.add(key)

                inferred = 0
                name_key = item.get("nameKey", item.get("name_key"))
                if name_key is not None:
                    name_key = str(name_key)
                    if ":" not in name_key:
                        name_key = f"{mod['id']}:{name_key}"
                    inferred |= ITEM_OVERRIDE_FLAGS["NAME"]
                desc_key = item.get("descriptionKey", item.get("description_key"))
                if desc_key is not None:
                    desc_key = str(desc_key)
                    if ":" not in desc_key:
                        desc_key = f"{mod['id']}:{desc_key}"
                    inferred |= ITEM_OVERRIDE_FLAGS["DESCRIPTION"]
                if "price" in item:
                    inferred |= ITEM_OVERRIDE_FLAGS["PRICE"]
                if "holdEffect" in item or "hold_effect" in item:
                    inferred |= ITEM_OVERRIDE_FLAGS["HOLD_EFFECT"]
                if "holdEffectParam" in item or "hold_effect_param" in item:
                    inferred |= ITEM_OVERRIDE_FLAGS["HOLD_EFFECT_PARAM"]
                if "importance" in item:
                    inferred |= ITEM_OVERRIDE_FLAGS["IMPORTANCE"]
                if "pocket" in item:
                    inferred |= ITEM_OVERRIDE_FLAGS["POCKET"]
                if "type" in item:
                    inferred |= ITEM_OVERRIDE_FLAGS["TYPE"]
                if "battleUsage" in item or "battle_usage" in item:
                    inferred |= ITEM_OVERRIDE_FLAGS["BATTLE_USAGE"]
                if "secondaryId" in item or "secondary_id" in item:
                    inferred |= ITEM_OVERRIDE_FLAGS["SECONDARY_ID"]

                field_hook = c_func(item.get("fieldUseHook", item.get("field_use_hook")))
                field_hook_key = item.get("fieldUseHookKey", item.get("field_use_hook_key", field_hook if field_hook != "NULL" else None))
                if field_hook_key is not None:
                    field_hook_key = str(field_hook_key)
                    if field_hook_key == "" or len(field_hook_key) > 31:
                        raise ModgenError(f"{key}: fieldUseHookKey must be non-empty and <= 31 bytes")
                    inferred |= ITEM_OVERRIDE_FLAGS["FIELD_USE"]
                battle_hook = c_func(item.get("battleUseHook", item.get("battle_use_hook")))
                battle_hook_key = item.get("battleUseHookKey", item.get("battle_use_hook_key", battle_hook if battle_hook != "NULL" else None))
                if battle_hook_key is not None:
                    battle_hook_key = str(battle_hook_key)
                    if battle_hook_key == "" or len(battle_hook_key) > 31:
                        raise ModgenError(f"{key}: battleUseHookKey must be non-empty and <= 31 bytes")
                    inferred |= ITEM_OVERRIDE_FLAGS["BATTLE_USE"]

                flags = parse_u16_flags(item.get("flags"), ITEM_OVERRIDE_FLAGS, "item", inferred)
                item_id = c_int_or_token(item.get("itemId", item.get("item_id")), "ITEM_NONE")
                priority = int(item.get("priority", 1000))
                if priority < -32768 or priority > 32767:
                    raise ModgenError(f"{key}: priority must be in signed 16-bit range")
                items.append(
                    {
                        "key": key,
                        "item": item_id,
                        "name_key": name_key,
                        "description_key": desc_key,
                        "field_hook": field_hook,
                        "field_hook_key": field_hook_key,
                        "battle_hook": battle_hook,
                        "battle_hook_key": battle_hook_key,
                        "price": c_int_or_token(item.get("price"), "0"),
                        "flags": flags,
                        "hold_effect": c_int_or_token(item.get("holdEffect", item.get("hold_effect")), "0"),
                        "hold_effect_param": c_int_or_token(item.get("holdEffectParam", item.get("hold_effect_param")), "0"),
                        "importance": c_int_or_token(item.get("importance"), "0"),
                        "pocket": c_int_or_token(item.get("pocket"), "0"),
                        "type": c_int_or_token(item.get("type"), "0"),
                        "battle_usage": c_int_or_token(item.get("battleUsage", item.get("battle_usage")), "0"),
                        "secondary_id": c_int_or_token(item.get("secondaryId", item.get("secondary_id")), "0"),
                        "priority": priority,
                    }
                )
    items.sort(key=lambda item: (item["priority"], item["key"]))
    return items


def normalize_reward_source(value: Any) -> Tuple[str, int]:
    if isinstance(value, int):
        if value <= 0 or value > 3:
            raise ModgenError(f"Reward source {value!r} is invalid")
        return str(value), value
    if not isinstance(value, str):
        raise ModgenError(f"Reward source {value!r} is invalid")
    stripped = value.strip()
    if re.fullmatch(r"0x[0-9A-Fa-f]+|[0-9]+", stripped):
        raw = int(stripped, 0)
        if raw <= 0 or raw > 3:
            raise ModgenError(f"Reward source {value!r} is invalid")
        return str(raw), raw
    if stripped.startswith("MOD_REWARD_SOURCE_"):
        if stripped not in REWARD_SOURCE_NUMBERS:
            raise ModgenError(f"Reward source {value!r} is invalid")
        return stripped, REWARD_SOURCE_NUMBERS[stripped]
    upper = stripped.upper()
    if upper not in REWARD_SOURCE_VALUES:
        raise ModgenError(f"Unknown reward source {value!r}")
    token = REWARD_SOURCE_VALUES[upper]
    return token, REWARD_SOURCE_NUMBERS[token]


def collect_rewards(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    rewards = []
    seen = set()
    for mod in mods:
        for path in iter_json_files(mod["root"] / "rewards"):
            for index, item in enumerate(as_list(read_json(path), "rewards")):
                key = key_for(mod["id"], item, f"reward_{index}")
                if key in seen:
                    raise ModgenError(f"Duplicate reward key {key!r}")
                seen.add(key)
                source, source_value = normalize_reward_source(item.get("source", "PICKUP_COMMON"))
                min_level = int(item.get("minLevel", item.get("min_level", 1)))
                max_level = int(item.get("maxLevel", item.get("max_level", 100)))
                if min_level < 1 or min_level > 100 or max_level < 1 or max_level > 100 or min_level > max_level:
                    raise ModgenError(f"{key}: reward level range must be within [1, 100]")
                hook = c_func(item.get("hook", item.get("hookSymbol", item.get("hook_symbol"))))
                hook_key = item.get("hookKey", item.get("hook_key", hook if hook != "NULL" else None))
                if hook_key is not None:
                    hook_key = str(hook_key)
                    if hook_key == "" or len(hook_key) > 31:
                        raise ModgenError(f"{key}: hookKey must be non-empty and <= 31 bytes")
                quantity = int(item.get("quantity", 1))
                if quantity < 1 or quantity > 999:
                    raise ModgenError(f"{key}: quantity must be in [1, 999]")
                priority = int(item.get("priority", 1000))
                if priority < -32768 or priority > 32767:
                    raise ModgenError(f"{key}: priority must be in signed 16-bit range")
                rewards.append(
                    {
                        "key": key,
                        "hook": hook,
                        "hook_key": hook_key,
                        "source": source,
                        "source_value": source_value,
                        "min_level": min_level,
                        "max_level": max_level,
                        "priority": priority,
                        "flags": c_int_or_token(item.get("flags"), "0"),
                        "item": c_int_or_token(item.get("itemId", item.get("item_id")), "ITEM_NONE"),
                        "quantity": quantity,
                    }
                )
    rewards.sort(key=lambda reward: (reward["priority"], reward["key"]))
    return rewards


def collect_pokemon_data(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    pokemon = []
    seen = set()
    for mod in mods:
        for path in iter_json_files(mod["root"] / "pokemon"):
            for index, item in enumerate(as_list(read_json(path), "pokemon")):
                key = key_for(mod["id"], item, f"pokemon_{index}")
                if key in seen:
                    raise ModgenError(f"Duplicate pokemon data key {key!r}")
                seen.add(key)
                inferred = 0
                info = item.get("info", item)
                base = info.get("baseStats", info.get("base_stats"))
                if base is not None:
                    inferred |= POKEMON_OVERRIDE_FLAGS["BASE_STATS"]
                types = info.get("types")
                if types is not None:
                    inferred |= POKEMON_OVERRIDE_FLAGS["TYPES"]
                if "catchRate" in info or "catch_rate" in info:
                    inferred |= POKEMON_OVERRIDE_FLAGS["CATCH_RATE"]
                if "expYield" in info or "exp_yield" in info:
                    inferred |= POKEMON_OVERRIDE_FLAGS["EXP_YIELD"]
                ev_yields = info.get("evYields", info.get("ev_yields"))
                if ev_yields is not None:
                    inferred |= POKEMON_OVERRIDE_FLAGS["EV_YIELD"]
                if "items" in info or "itemCommon" in info or "item_common" in info:
                    inferred |= POKEMON_OVERRIDE_FLAGS["ITEMS"]
                if "genderRatio" in info or "gender_ratio" in info:
                    inferred |= POKEMON_OVERRIDE_FLAGS["GENDER"]
                if "eggCycles" in info or "egg_cycles" in info:
                    inferred |= POKEMON_OVERRIDE_FLAGS["EGG_CYCLES"]
                if "friendship" in info:
                    inferred |= POKEMON_OVERRIDE_FLAGS["FRIENDSHIP"]
                if "growthRate" in info or "growth_rate" in info:
                    inferred |= POKEMON_OVERRIDE_FLAGS["GROWTH_RATE"]
                if "eggGroups" in info or "egg_groups" in info:
                    inferred |= POKEMON_OVERRIDE_FLAGS["EGG_GROUPS"]
                if "abilities" in info:
                    inferred |= POKEMON_OVERRIDE_FLAGS["ABILITIES"]
                if "safariZoneFleeRate" in info or "safari_zone_flee_rate" in info:
                    inferred |= POKEMON_OVERRIDE_FLAGS["SAFARI_FLEE"]
                if "bodyColor" in info or "body_color" in info:
                    inferred |= POKEMON_OVERRIDE_FLAGS["BODY_COLOR"]
                level_up = item.get("levelUpMoves", item.get("level_up_moves", []))
                if level_up:
                    inferred |= POKEMON_OVERRIDE_FLAGS["LEVEL_UP"]
                evolutions = item.get("evolutions", [])
                if evolutions:
                    inferred |= POKEMON_OVERRIDE_FLAGS["EVOLUTIONS"]
                flags = parse_u16_flags(item.get("flags"), POKEMON_OVERRIDE_FLAGS, "pokemon data", inferred)
                base_values = base or [0, 0, 0, 0, 0, 0]
                if len(base_values) != 6:
                    raise ModgenError(f"{key}: baseStats must contain six values")
                type_values = types or [0, 0]
                if len(type_values) != 2:
                    raise ModgenError(f"{key}: types must contain two values")
                ev_values = ev_yields or [0, 0, 0, 0, 0, 0]
                if len(ev_values) != 6:
                    raise ModgenError(f"{key}: evYields must contain six values")
                egg_groups = info.get("eggGroups", info.get("egg_groups", [0, 0]))
                abilities = info.get("abilities", [0, 0])
                if len(egg_groups) != 2 or len(abilities) != 2:
                    raise ModgenError(f"{key}: eggGroups and abilities must contain two values")
                level_moves = []
                for move in level_up:
                    level = int(move.get("level", 1))
                    if level < 1 or level > 100:
                        raise ModgenError(f"{key}: level up move level must be in [1, 100]")
                    level_moves.append({"level": level, "move": c_int_or_token(move.get("move"), "MOVE_NONE")})
                if len(level_moves) > 32:
                    raise ModgenError(f"{key}: levelUpMoves may contain at most 32 entries")
                while len(level_moves) < 32:
                    level_moves.append({"level": 0, "move": "MOVE_NONE"})
                evo_values = []
                for evo in evolutions:
                    evo_values.append(
                        {
                            "method": c_int_or_token(evo.get("method"), "EVO_NONE"),
                            "param": c_int_or_token(evo.get("param"), "0"),
                            "target": c_int_or_token(evo.get("targetSpecies", evo.get("target_species")), "SPECIES_NONE"),
                        }
                    )
                if len(evo_values) > 5:
                    raise ModgenError(f"{key}: evolutions may contain at most 5 entries")
                while len(evo_values) < 5:
                    evo_values.append({"method": "EVO_NONE", "param": "0", "target": "SPECIES_NONE"})
                pokemon.append(
                    {
                        "key": key,
                        "species": c_int_or_token(item.get("species"), "SPECIES_NONE"),
                        "priority": int(item.get("priority", 1000)),
                        "flags": flags,
                        "base": [c_int_or_token(v, "0") for v in base_values],
                        "types": [c_int_or_token(v, "0") for v in type_values],
                        "catch_rate": c_int_or_token(info.get("catchRate", info.get("catch_rate")), "0"),
                        "exp_yield": c_int_or_token(info.get("expYield", info.get("exp_yield")), "0"),
                        "ev_yields": [c_int_or_token(v, "0") for v in ev_values],
                        "item_common": c_int_or_token(info.get("itemCommon", info.get("item_common", (info.get("items") or [None, None])[0] if info.get("items") else None)), "ITEM_NONE"),
                        "item_rare": c_int_or_token(info.get("itemRare", info.get("item_rare", (info.get("items") or [None, None])[1] if info.get("items") else None)), "ITEM_NONE"),
                        "gender": c_int_or_token(info.get("genderRatio", info.get("gender_ratio")), "0"),
                        "egg_cycles": c_int_or_token(info.get("eggCycles", info.get("egg_cycles")), "0"),
                        "friendship": c_int_or_token(info.get("friendship"), "0"),
                        "growth_rate": c_int_or_token(info.get("growthRate", info.get("growth_rate")), "0"),
                        "egg_groups": [c_int_or_token(v, "0") for v in egg_groups],
                        "abilities": [c_int_or_token(v, "0") for v in abilities],
                        "safari_flee": c_int_or_token(info.get("safariZoneFleeRate", info.get("safari_zone_flee_rate")), "0"),
                        "body_color": c_int_or_token(info.get("bodyColor", info.get("body_color")), "0"),
                        "no_flip": c_bool(info.get("noFlip", info.get("no_flip", False))),
                        "level_moves": level_moves,
                        "level_move_count": len(level_up),
                        "evolutions": evo_values,
                        "evolution_count": len(evolutions),
                    }
                )
    pokemon.sort(key=lambda data: (data["priority"], data["key"]))
    return pokemon


def collect_battle_moves(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    moves = []
    seen = set()
    for mod in mods:
        for path in iter_json_files(mod["root"] / "battle" / "moves"):
            for index, item in enumerate(as_list(read_json(path), "moves")):
                key = key_for(mod["id"], item, f"move_{index}")
                if key in seen:
                    raise ModgenError(f"Duplicate battle move key {key!r}")
                seen.add(key)
                inferred = 0
                for json_key, flag_key in [
                    ("effect", "EFFECT"),
                    ("power", "POWER"),
                    ("type", "TYPE"),
                    ("accuracy", "ACCURACY"),
                    ("pp", "PP"),
                    ("secondaryEffectChance", "SECONDARY"),
                    ("target", "TARGET"),
                    ("priority", "PRIORITY"),
                    ("moveFlags", "FLAGS"),
                ]:
                    if json_key in item:
                        inferred |= BATTLE_MOVE_OVERRIDE_FLAGS[flag_key]
                override_flags = parse_u16_flags(item.get("overrideFlags", item.get("override_flags")), BATTLE_MOVE_OVERRIDE_FLAGS, "battle move", inferred)
                if override_flags == 0:
                    raise ModgenError(f"{key}: battle move overrideFlags must be non-zero")
                moves.append(
                    {
                        "key": key,
                        "move": c_int_or_token(item.get("move"), "MOVE_NONE"),
                        "priority": int(item.get("priorityOrder", item.get("priority_order", 1000))),
                        "override_flags": override_flags,
                        "effect": c_int_or_token(item.get("effect"), "0"),
                        "power": c_int_or_token(item.get("power"), "0"),
                        "type": c_int_or_token(item.get("type"), "0"),
                        "accuracy": c_int_or_token(item.get("accuracy"), "0"),
                        "pp": c_int_or_token(item.get("pp"), "0"),
                        "secondary": c_int_or_token(item.get("secondaryEffectChance", item.get("secondary_effect_chance")), "0"),
                        "target": c_int_or_token(item.get("target"), "0"),
                        "move_priority": c_int_or_token(item.get("movePriority", item.get("move_priority")), "0"),
                        "move_flags": c_int_or_token(item.get("moveFlags", item.get("move_flags")), "0"),
                    }
                )
    moves.sort(key=lambda move: (move["priority"], move["key"]))
    return moves


def collect_trainers(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    trainers = []
    seen = set()
    for mod in mods:
        for path in iter_json_files(mod["root"] / "trainers"):
            for index, item in enumerate(as_list(read_json(path), "trainers")):
                key = key_for(mod["id"], item, f"trainer_{index}")
                if key in seen:
                    raise ModgenError(f"Duplicate trainer key {key!r}")
                seen.add(key)
                inferred = 0
                if "trainerClass" in item or "trainer_class" in item:
                    inferred |= TRAINER_OVERRIDE_FLAGS["CLASS"]
                if "trainerPic" in item or "trainer_pic" in item:
                    inferred |= TRAINER_OVERRIDE_FLAGS["PIC"]
                if "encounterMusicGender" in item or "encounter_music_gender" in item:
                    inferred |= TRAINER_OVERRIDE_FLAGS["MUSIC_GENDER"]
                if "items" in item:
                    inferred |= TRAINER_OVERRIDE_FLAGS["ITEMS"]
                if "doubleBattle" in item or "double_battle" in item:
                    inferred |= TRAINER_OVERRIDE_FLAGS["DOUBLE"]
                if "aiFlags" in item or "ai_flags" in item:
                    inferred |= TRAINER_OVERRIDE_FLAGS["AI"]
                party = item.get("party", [])
                if party:
                    inferred |= TRAINER_OVERRIDE_FLAGS["PARTY"]
                flags = parse_u16_flags(item.get("flags"), TRAINER_OVERRIDE_FLAGS, "trainer", inferred)
                if len(party) > 6:
                    raise ModgenError(f"{key}: trainer party may contain at most 6 mons")
                mons = []
                for mon in party:
                    moves = [c_int_or_token(move, "MOVE_NONE") for move in mon.get("moves", [])]
                    if len(moves) > 4:
                        raise ModgenError(f"{key}: trainer mon moves may contain at most 4 moves")
                    while len(moves) < 4:
                        moves.append("MOVE_NONE")
                    level = int(mon.get("level", mon.get("lvl", 1)))
                    if level < 1 or level > 100:
                        raise ModgenError(f"{key}: trainer mon level must be in [1, 100]")
                    mons.append(
                        {
                            "iv": c_int_or_token(mon.get("iv"), "0"),
                            "level": level,
                            "species": c_int_or_token(mon.get("species"), "SPECIES_NONE"),
                            "item": c_int_or_token(mon.get("heldItem", mon.get("held_item")), "ITEM_NONE"),
                            "moves": moves,
                        }
                    )
                while len(mons) < 6:
                    mons.append({"iv": "0", "level": 0, "species": "SPECIES_NONE", "item": "ITEM_NONE", "moves": ["MOVE_NONE"] * 4})
                items = [c_int_or_token(value, "ITEM_NONE") for value in item.get("items", [])]
                if len(items) > 4:
                    raise ModgenError(f"{key}: trainer items may contain at most 4 items")
                while len(items) < 4:
                    items.append("ITEM_NONE")
                trainers.append(
                    {
                        "key": key,
                        "trainer": c_int_or_token(item.get("trainerId", item.get("trainer_id")), "0"),
                        "priority": int(item.get("priority", 1000)),
                        "flags": flags,
                        "class": c_int_or_token(item.get("trainerClass", item.get("trainer_class")), "0"),
                        "music_gender": c_int_or_token(item.get("encounterMusicGender", item.get("encounter_music_gender")), "0"),
                        "pic": c_int_or_token(item.get("trainerPic", item.get("trainer_pic")), "0"),
                        "items": items,
                        "double": c_bool(item.get("doubleBattle", item.get("double_battle", False))),
                        "ai": c_int_or_token(item.get("aiFlags", item.get("ai_flags")), "0"),
                        "party_count": len(party),
                        "party": mons,
                    }
                )
    trainers.sort(key=lambda trainer: (trainer["priority"], trainer["key"]))
    return trainers


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


def add_catalog_entry(entries: List[Dict[str, int]], type_name: str, key: str, *content: Any, flags: int = 0) -> None:
    entries.append(
        {
            "type": type_name,
            "flags": flags,
            "key_hash": hash_text(key),
            "content_hash": hash_parts(key, *content),
        }
    )


def collect_catalog_entries(
    weather: List[Dict[str, Any]],
    sprite_assets: List[Dict[str, Any]],
    language_texts: List[Dict[str, Any]],
    engines: List[Dict[str, Any]],
    npcs: List[Dict[str, Any]],
    badge_effects: List[Dict[str, Any]],
    fishing_actions: List[Dict[str, Any]],
    encounters: List[Dict[str, Any]],
    shops: List[Dict[str, Any]],
    items: List[Dict[str, Any]],
    rewards: List[Dict[str, Any]],
    pokemon_data: List[Dict[str, Any]],
    battle_moves: List[Dict[str, Any]],
    trainers: List[Dict[str, Any]],
) -> List[Dict[str, int]]:
    entries: List[Dict[str, int]] = []

    for text in language_texts:
        add_catalog_entry(
            entries,
            "MOD_CATALOG_ENTRY_TEXT",
            f"{text['language']}:{text['key']}",
            text["language"],
            text["text"],
        )
    for provider in weather:
        add_catalog_entry(
            entries,
            "MOD_CATALOG_ENTRY_WEATHER",
            provider["id"],
            provider["priority"],
            provider["handler"],
        )
    for engine in engines:
        add_catalog_entry(
            entries,
            "MOD_CATALOG_ENTRY_ENGINE",
            engine["id"],
            engine["name"],
            engine["version"],
            engine["flags"],
            engine["capture"],
            engine["battle_weather"],
        )
    for npc in npcs:
        add_catalog_entry(
            entries,
            "MOD_CATALOG_ENTRY_NPC",
            npc["key"],
            npc["graphics"],
            npc["movement"],
            npc["local"],
            npc["elevation"],
            npc["flag"],
            npc["script"],
        )
    for asset in sprite_assets:
        add_catalog_entry(
            entries,
            "MOD_CATALOG_ENTRY_SPRITE_ASSET",
            asset["key"],
            asset["sheet"],
            asset["compressed_sheet"],
            asset["palette"],
            asset["compressed_palette"],
            asset["template"],
            asset["tile_tag"],
            asset["palette_tag"],
        )
    for effect in badge_effects:
        add_catalog_entry(
            entries,
            "MOD_CATALOG_ENTRY_BADGE_EFFECT",
            effect["key"],
            effect["badge"],
            effect["effect"],
            effect["target"],
            effect["percent"],
            effect["max_level"],
            effect["flags"],
        )
    for action in fishing_actions:
        add_catalog_entry(
            entries,
            "MOD_CATALOG_ENTRY_FISHING_ACTION",
            action["key"],
            action["hook_key"],
            action["rod_mask"],
            action["phase_mask"],
            action["priority"],
            action["flags"],
            action["prompt_key"] or "",
            action["button_mask"],
            action["timeout"],
            action["success_outcome"],
            action["failure_outcome"],
            *action["params"],
        )
        add_catalog_entry(
            entries,
            "MOD_CATALOG_ENTRY_FISHING_ACTION",
            f"{action['key']}:{action['hook_key']}",
            "hook_ref",
            action["key"],
            action["hook_key"],
            flags=1,
        )
    for encounter in encounters:
        slot_parts: List[Any] = []
        for slot in encounter["slots"][: encounter["slot_count"]]:
            slot_parts.extend([slot["min"], slot["max"], slot["species"], slot["weight"], slot["rod_mask"], slot["flags"]])
        add_catalog_entry(
            entries,
            "MOD_CATALOG_ENTRY_ENCOUNTER",
            encounter["key"],
            encounter["hook_key"] or "",
            encounter["group"],
            encounter["num"],
            encounter["area"],
            encounter["rod_mask"],
            encounter["encounter_rate"],
            encounter["slot_count"],
            encounter["priority"],
            encounter["flags"],
            *slot_parts,
        )
        if encounter["hook_key"]:
            add_catalog_entry(
                entries,
                "MOD_CATALOG_ENTRY_ENCOUNTER",
                f"{encounter['key']}:{encounter['hook_key']}",
                "hook_ref",
                encounter["key"],
                encounter["hook_key"],
                flags=1,
            )
    for shop in shops:
        add_catalog_entry(
            entries,
            "MOD_CATALOG_ENTRY_SHOP",
            shop["key"],
            shop["group"],
            shop["num"],
            shop["mart_type"],
            shop["item_count"],
            shop["priority"],
            shop["flags"],
            *shop["items"][: shop["item_count"]],
        )
    for item in items:
        add_catalog_entry(
            entries,
            "MOD_CATALOG_ENTRY_ITEM",
            item["key"],
            item["item"],
            item["name_key"] or "",
            item["description_key"] or "",
            item["field_hook_key"] or "",
            item["battle_hook_key"] or "",
            item["price"],
            item["flags"],
            item["hold_effect"],
            item["hold_effect_param"],
            item["importance"],
            item["pocket"],
            item["type"],
            item["battle_usage"],
            item["secondary_id"],
            item["priority"],
        )
        for hook_key in (item["field_hook_key"], item["battle_hook_key"]):
            if hook_key:
                add_catalog_entry(
                    entries,
                    "MOD_CATALOG_ENTRY_ITEM",
                    f"{item['key']}:{hook_key}",
                    "hook_ref",
                    item["key"],
                    hook_key,
                    flags=1,
                )
    for reward in rewards:
        add_catalog_entry(
            entries,
            "MOD_CATALOG_ENTRY_REWARD",
            reward["key"],
            reward["hook_key"] or "",
            reward["source"],
            reward["min_level"],
            reward["max_level"],
            reward["priority"],
            reward["flags"],
            reward["item"],
            reward["quantity"],
        )
        if reward["hook_key"]:
            add_catalog_entry(
                entries,
                "MOD_CATALOG_ENTRY_REWARD",
                f"{reward['key']}:{reward['hook_key']}",
                "hook_ref",
                reward["key"],
                reward["hook_key"],
                flags=1,
            )
    for data in pokemon_data:
        add_catalog_entry(
            entries,
            "MOD_CATALOG_ENTRY_POKEMON_DATA",
            data["key"],
            data["species"],
            data["priority"],
            data["flags"],
            data["level_move_count"],
            data["evolution_count"],
        )
    for move in battle_moves:
        add_catalog_entry(
            entries,
            "MOD_CATALOG_ENTRY_BATTLE_MOVE",
            move["key"],
            move["move"],
            move["priority"],
            move["override_flags"],
            move["effect"],
            move["power"],
            move["type"],
            move["accuracy"],
            move["pp"],
            move["secondary"],
            move["target"],
            move["move_priority"],
            move["move_flags"],
        )
    for trainer in trainers:
        add_catalog_entry(
            entries,
            "MOD_CATALOG_ENTRY_TRAINER",
            trainer["key"],
            trainer["trainer"],
            trainer["priority"],
            trainer["flags"],
            trainer["party_count"],
        )

    entries.sort(key=lambda entry: (entry["type"], entry["flags"], entry["key_hash"], entry["content_hash"]))
    return entries


def calc_catalog_hash(entries: List[Dict[str, int]]) -> int:
    data = bytearray()
    for entry in entries:
        data.extend(str(entry["type"]).encode("ascii"))
        data.append(0)
        data.append(entry.get("flags", 0) & 0xFF)
        data.extend(entry["key_hash"].to_bytes(4, "little"))
        data.extend(entry["content_hash"].to_bytes(4, "little"))
    return fnv1a(bytes(data))


def write_header(path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(
        """#ifndef GUARD_GENERATED_MOD_REGISTRY_H
#define GUARD_GENERATED_MOD_REGISTRY_H

#include "global.h"
#include "mod/badge.h"
#include "mod/battle_data.h"
#include "mod/catalog.h"
#include "mod/battle_sprite.h"
#include "mod/encounter.h"
#include "mod/engine.h"
#include "mod/event.h"
#include "mod/fishing.h"
#include "mod/flags.h"
#include "mod/item.h"
#include "mod/language.h"
#include "mod/map.h"
#include "mod/npc.h"
#include "mod/overworld_sprite.h"
#include "mod/pokemon_data.h"
#include "mod/pokeball.h"
#include "mod/reward.h"
#include "mod/shop.h"
#include "mod/sprite_asset.h"
#include "mod/time.h"
#include "mod/trainer.h"
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
extern const struct ModBadgeEffectDefinition gModBadgeEffects[];
extern const u16 gModBadgeEffectCount;
extern const struct EngineRuleset gModEngineRulesets[];
extern const u16 gModEngineRulesetCount;
extern const struct ModNpcDefinition gModNpcDefinitions[];
extern const u16 gModNpcDefinitionCount;
extern const struct ModMapDefinition gModMapDefinitions[];
extern const u16 gModMapDefinitionCount;
extern const struct FishingActionDefinition gModFishingActions[];
extern const u16 gModFishingActionCount;
extern const struct ModEncounterDefinition gModEncounterDefinitions[];
extern const u16 gModEncounterDefinitionCount;
extern const struct ModShopDefinition gModShopDefinitions[];
extern const u16 gModShopDefinitionCount;
extern const struct ModItemDefinition gModItemDefinitions[];
extern const u16 gModItemDefinitionCount;
extern const struct ModRewardDefinition gModRewardDefinitions[];
extern const u16 gModRewardDefinitionCount;
extern const struct ModPokemonDataDefinition gModPokemonDataDefinitions[];
extern const u16 gModPokemonDataDefinitionCount;
extern const struct ModBattleMoveDefinition gModBattleMoveDefinitions[];
extern const u16 gModBattleMoveDefinitionCount;
extern const struct ModTrainerDefinition gModTrainerDefinitions[];
extern const u16 gModTrainerDefinitionCount;
extern const struct ModCatalogEntry gModCatalogEntries[];
extern const u16 gModCatalogEntryCount;
extern const u32 gModCatalogHash;

#endif // GUARD_GENERATED_MOD_REGISTRY_H
""",
        encoding="utf-8",
    )


def write_source(path: Path, mods: List[Dict[str, Any]], flags: List[Dict[str, Any]], events: List[Dict[str, Any]], weather: List[Dict[str, Any]], time_segments: List[Dict[str, Any]], badge_effects: List[Dict[str, Any]], fishing_actions: List[Dict[str, Any]], encounters: List[Dict[str, Any]], shops: List[Dict[str, Any]], items: List[Dict[str, Any]], rewards: List[Dict[str, Any]], pokemon_data: List[Dict[str, Any]], battle_moves: List[Dict[str, Any]], trainers: List[Dict[str, Any]], sprite_assets: List[Dict[str, Any]], overworld_sprites: List[Dict[str, Any]], battle_sprites: List[Dict[str, Any]], followers: List[Dict[str, Any]], language_texts: List[Dict[str, Any]], pokeballs: List[Dict[str, Any]], engines: List[Dict[str, Any]], npcs: List[Dict[str, Any]], maps: List[Dict[str, Any]], catalog_entries: List[Dict[str, int]], catalog_hash: int) -> None:
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
    fishing_hooks = sorted({action["hook"] for action in fishing_actions if action["hook"] != "NULL" and action["hook"] != "FishingApi_RequestConfiguredAction"})
    encounter_hooks = sorted({encounter["hook"] for encounter in encounters if encounter["hook"] != "NULL"})
    item_hooks = sorted({item["field_hook"] for item in items if item["field_hook"] != "NULL"} | {item["battle_hook"] for item in items if item["battle_hook"] != "NULL"})
    reward_hooks = sorted({reward["hook"] for reward in rewards if reward["hook"] != "NULL"})

    lines = [
        "/* Auto-generated by scripts/modgen.py. */",
        '#include "global.h"',
        '#include "constants/decorations.h"',
        '#include "constants/items.h"',
        '#include "constants/maps.h"',
        '#include "constants/moves.h"',
        '#include "constants/pokemon.h"',
        '#include "constants/species.h"',
        '#include "constants/trainers.h"',
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
    for hook in fishing_hooks:
        lines.append(f"extern u8 {hook}(const struct FishingActionDefinition *definition, struct FishingContext *context, struct FishingActionRequest *request);")
    for hook in encounter_hooks:
        lines.append(f"extern u8 {hook}(const struct ModEncounterDefinition *definition, struct ModEncounterContext *context);")
    for hook in item_hooks:
        lines.append(f"extern void {hook}(u8 taskId);")
    for hook in reward_hooks:
        lines.append(f"extern u8 {hook}(const struct ModRewardDefinition *definition, struct ModRewardContext *context);")
    if event_handlers or weather_handlers or capture_hooks or battle_weather_hooks or npc_scripts or sheet_symbols or compressed_sheet_symbols or palette_symbols or compressed_palette_symbols or template_symbols or pokeball_modifier_hooks or pokeball_commit_hooks or pokeball_scripts or fishing_hooks or encounter_hooks or item_hooks or reward_hooks:
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

    lines.append("const struct ModBadgeEffectDefinition gModBadgeEffects[] =")
    lines.append("{")
    if badge_effects:
        for effect in badge_effects:
            lines.append(f"    {{ {c_string(effect['key'])}, {effect['badge']}, {effect['effect']}, {effect['target']}, {effect['percent']}, {effect['max_level']}, {effect['flags']} }},")
    else:
        lines.append("    { NULL, 0, 0, 0, 0, 0, 0 },")
    lines.append("};")
    lines.append(f"const u16 gModBadgeEffectCount = {len(badge_effects)};")
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

    lines.append("const struct FishingActionDefinition gModFishingActions[] =")
    lines.append("{")
    if fishing_actions:
        for action in fishing_actions:
            params = ", ".join(str(param) for param in action["params"])
            lines.append(
                f"    {{ {c_string(action['key'])}, {c_string(action['hook_key'])}, {action['rod_mask']}, {action['phase_mask']}, {action['priority']}, {action['flags']}, {action['hook']}, {c_string(action['prompt_key'])}, {action['button_mask']}, {action['timeout']}, {action['success_outcome']}, {action['failure_outcome']}, {{ {params} }} }},"
            )
    else:
        lines.append("    { NULL, NULL, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, { 0, 0, 0, 0 } },")
    lines.append("};")
    lines.append(f"const u16 gModFishingActionCount = {len(fishing_actions)};")
    lines.append("")

    lines.append("const struct ModEncounterDefinition gModEncounterDefinitions[] =")
    lines.append("{")
    if encounters:
        for encounter in encounters:
            lines.append("    {")
            lines.append(f"        {c_string(encounter['key'])}, {c_string(encounter['hook_key'])}, {encounter['group']}, {encounter['num']}, {encounter['area']}, {encounter['rod_mask']}, {encounter['encounter_rate']}, {encounter['slot_count']}, {encounter['priority']}, {encounter['flags']}, {encounter['hook']},")
            lines.append("        {")
            for slot in encounter["slots"]:
                lines.append(f"            {{ {slot['min']}, {slot['max']}, {slot['species']}, {slot['weight']}, {slot['rod_mask']}, {slot['flags']} }},")
            lines.append("        }")
            lines.append("    },")
    else:
        lines.append("    { NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, { { 0, 0, SPECIES_NONE, 0, 0, 0 } } },")
    lines.append("};")
    lines.append(f"const u16 gModEncounterDefinitionCount = {len(encounters)};")
    lines.append("")

    lines.append("const struct ModShopDefinition gModShopDefinitions[] =")
    lines.append("{")
    if shops:
        for shop in shops:
            shop_items = ", ".join(str(item) for item in shop["items"])
            lines.append(f"    {{ {c_string(shop['key'])}, {shop['group']}, {shop['num']}, {shop['mart_type']}, {shop['item_count']}, {shop['priority']}, {shop['flags']}, {{ {shop_items} }} }},")
    else:
        lines.append("    { NULL, 0, 0, 0, 0, 0, 0, { ITEM_NONE } },")
    lines.append("};")
    lines.append(f"const u16 gModShopDefinitionCount = {len(shops)};")
    lines.append("")

    lines.append("const struct ModItemDefinition gModItemDefinitions[] =")
    lines.append("{")
    if items:
        for item in items:
            lines.append(
                f"    {{ {c_string(item['key'])}, {c_string(item['name_key'])}, {c_string(item['description_key'])}, {c_string(item['field_hook_key'])}, {c_string(item['battle_hook_key'])}, {item['item']}, {item['price']}, {item['flags']}, {item['hold_effect']}, {item['hold_effect_param']}, {item['importance']}, {item['pocket']}, {item['type']}, {item['battle_usage']}, {item['secondary_id']}, {item['priority']}, {item['field_hook']}, {item['battle_hook']} }},"
            )
    else:
        lines.append("    { NULL, NULL, NULL, NULL, NULL, ITEM_NONE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL },")
    lines.append("};")
    lines.append(f"const u16 gModItemDefinitionCount = {len(items)};")
    lines.append("")

    lines.append("const struct ModRewardDefinition gModRewardDefinitions[] =")
    lines.append("{")
    if rewards:
        for reward in rewards:
            lines.append(f"    {{ {c_string(reward['key'])}, {c_string(reward['hook_key'])}, {reward['source']}, {reward['min_level']}, {reward['max_level']}, {reward['priority']}, {reward['flags']}, {reward['item']}, {reward['quantity']}, {reward['hook']} }},")
    else:
        lines.append("    { NULL, NULL, 0, 0, 0, 0, 0, ITEM_NONE, 0, NULL },")
    lines.append("};")
    lines.append(f"const u16 gModRewardDefinitionCount = {len(rewards)};")
    lines.append("")

    lines.append("const struct ModPokemonDataDefinition gModPokemonDataDefinitions[] =")
    lines.append("{")
    if pokemon_data:
        for data in pokemon_data:
            lines.append("    {")
            lines.append(f"        {c_string(data['key'])}, {data['species']}, {data['priority']}, {data['flags']},")
            lines.append(
                f"        {{ {', '.join(data['base'])}, {{ {', '.join(data['types'])} }}, {data['catch_rate']}, {data['exp_yield']}, {{ {', '.join(data['ev_yields'])} }}, {data['item_common']}, {data['item_rare']}, {data['gender']}, {data['egg_cycles']}, {data['friendship']}, {data['growth_rate']}, {{ {', '.join(data['egg_groups'])} }}, {{ {', '.join(data['abilities'])} }}, {data['safari_flee']}, {data['body_color']}, {data['no_flip']} }},"
            )
            lines.append(f"        {data['level_move_count']},")
            lines.append("        {")
            for move in data["level_moves"]:
                lines.append(f"            {{ {move['level']}, {move['move']} }},")
            lines.append("        },")
            lines.append(f"        {data['evolution_count']},")
            lines.append("        {")
            for evo in data["evolutions"]:
                lines.append(f"            {{ {evo['method']}, {evo['param']}, {evo['target']} }},")
            lines.append("        }")
            lines.append("    },")
    else:
        lines.append("    { NULL, SPECIES_NONE, 0, 0, { 0 }, 0, { { 0, MOVE_NONE } }, 0, { { 0, 0, SPECIES_NONE } } },")
    lines.append("};")
    lines.append(f"const u16 gModPokemonDataDefinitionCount = {len(pokemon_data)};")
    lines.append("")

    lines.append("const struct ModBattleMoveDefinition gModBattleMoveDefinitions[] =")
    lines.append("{")
    if battle_moves:
        for move in battle_moves:
            lines.append(f"    {{ {c_string(move['key'])}, {move['move']}, {move['priority']}, {move['override_flags']}, {{ {move['effect']}, {move['power']}, {move['type']}, {move['accuracy']}, {move['pp']}, {move['secondary']}, {move['target']}, {move['move_priority']}, {move['move_flags']} }} }},")
    else:
        lines.append("    { NULL, MOVE_NONE, 0, 0, { 0 } },")
    lines.append("};")
    lines.append(f"const u16 gModBattleMoveDefinitionCount = {len(battle_moves)};")
    lines.append("")

    lines.append("const struct ModTrainerDefinition gModTrainerDefinitions[] =")
    lines.append("{")
    if trainers:
        for trainer in trainers:
            lines.append("    {")
            lines.append(f"        {c_string(trainer['key'])}, {trainer['trainer']}, {trainer['priority']}, {trainer['flags']}, {trainer['class']}, {trainer['music_gender']}, {trainer['pic']}, {{ {', '.join(trainer['items'])} }}, {trainer['double']}, {trainer['ai']}, {trainer['party_count']},")
            lines.append("        {")
            for mon in trainer["party"]:
                lines.append(f"            {{ {mon['iv']}, {mon['level']}, {mon['species']}, {mon['item']}, {{ {', '.join(mon['moves'])} }} }},")
            lines.append("        }")
            lines.append("    },")
    else:
        lines.append("    { NULL, 0, 0, 0, 0, 0, 0, { ITEM_NONE }, FALSE, 0, 0, { { 0, 0, SPECIES_NONE, ITEM_NONE, { MOVE_NONE } } } },")
    lines.append("};")
    lines.append(f"const u16 gModTrainerDefinitionCount = {len(trainers)};")
    lines.append("")

    lines.append("const struct ModCatalogEntry gModCatalogEntries[] =")
    lines.append("{")
    if catalog_entries:
        for entry in catalog_entries:
            lines.append(f"    {{ {entry['type']}, {entry.get('flags', 0)}, 0, 0x{entry['key_hash']:08X}, 0x{entry['content_hash']:08X} }},")
    else:
        lines.append("    { 0, 0, 0, 0, 0 },")
    lines.append("};")
    lines.append(f"const u16 gModCatalogEntryCount = {len(catalog_entries)};")
    lines.append(f"const u32 gModCatalogHash = 0x{catalog_hash:08X};")
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
    badge_effects = collect_badge_effects(mods)
    fishing_actions = collect_fishing_actions(mods)
    encounters = collect_encounters(mods)
    shops = collect_shops(mods)
    items = collect_items(mods)
    rewards = collect_rewards(mods)
    pokemon_data = collect_pokemon_data(mods)
    battle_moves = collect_battle_moves(mods)
    trainers = collect_trainers(mods)
    sprite_assets = collect_sprite_assets(mods)
    overworld_sprites = collect_overworld_sprites(mods)
    battle_sprites = collect_battle_sprites(mods)
    followers = collect_followers(mods)
    language_texts = collect_language_texts(mods)
    pokeballs = collect_pokeballs(mods)
    engines = collect_engines(mods)
    npcs = collect_npcs(mods)
    maps = collect_maps(mods)
    catalog_entries = collect_catalog_entries(weather, sprite_assets, language_texts, engines, npcs, badge_effects, fishing_actions, encounters, shops, items, rewards, pokemon_data, battle_moves, trainers)
    catalog_hash = calc_catalog_hash(catalog_entries)
    sources = collect_mod_sources(mods)

    write_header(root / args.out_header)
    write_source(
        root / args.out_source,
        mods,
        flags,
        events,
        weather,
        time_segments,
        badge_effects,
        fishing_actions,
        encounters,
        shops,
        items,
        rewards,
        pokemon_data,
        battle_moves,
        trainers,
        sprite_assets,
        overworld_sprites,
        battle_sprites,
        followers,
        language_texts,
        pokeballs,
        engines,
        npcs,
        maps,
        catalog_entries,
        catalog_hash,
    )
    write_make_fragment(root / args.out_make, sources)
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except ModgenError as exc:
        print(f"modgen: {exc}")
        raise SystemExit(1)
