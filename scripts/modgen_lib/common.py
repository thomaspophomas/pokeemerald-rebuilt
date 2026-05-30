"""Shared parsing, normalization, and C literal helpers for modgen."""

from __future__ import annotations

import json
import re
from pathlib import Path
from typing import Any, Dict, Iterable, List, Optional, Tuple

from .errors import ModgenError

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

LEVEL_CAP_MODES = {
    "NONE": "MOD_LEVEL_CAP_MODE_NONE",
    "SOFT": "MOD_LEVEL_CAP_MODE_SOFT",
    "HARD": "MOD_LEVEL_CAP_MODE_HARD",
}

LEVEL_CAP_MODE_VALUES = {
    "0": 0,
    "NONE": 0,
    "MOD_LEVEL_CAP_MODE_NONE": 0,
    "1": 1,
    "SOFT": 1,
    "MOD_LEVEL_CAP_MODE_SOFT": 1,
    "2": 2,
    "HARD": 2,
    "MOD_LEVEL_CAP_MODE_HARD": 2,
}

LEVEL_CAP_RARE_CANDY_POLICIES = {
    "ALLOW": "MOD_LEVEL_CAP_RARE_CANDY_ALLOW",
    "BLOCK": "MOD_LEVEL_CAP_RARE_CANDY_BLOCK_AT_CAP",
    "BLOCK_AT_CAP": "MOD_LEVEL_CAP_RARE_CANDY_BLOCK_AT_CAP",
}

LEVEL_CAP_MAX_STAGES = 16
LEVEL_CAP_ALWAYS_FLAG = "MOD_LEVEL_CAP_FLAG_ALWAYS"
LEVEL_CAP_EXP_DELTA_MIN = -99
LEVEL_CAP_EXP_DELTA_MAX = 99
LEVEL_CAP_EXP_DELTA_COUNT = LEVEL_CAP_EXP_DELTA_MAX - LEVEL_CAP_EXP_DELTA_MIN + 1
LEVEL_CAP_BADGE_FLAGS = [
    "FLAG_BADGE01_GET",
    "FLAG_BADGE02_GET",
    "FLAG_BADGE03_GET",
    "FLAG_BADGE04_GET",
    "FLAG_BADGE05_GET",
    "FLAG_BADGE06_GET",
    "FLAG_BADGE07_GET",
    "FLAG_BADGE08_GET",
]

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


def read_domain_list(path: Path, key: str) -> List[Any]:
    try:
        return as_list(read_json(path), key)
    except ModgenError as exc:
        raise ModgenError(f"{path}: {exc}") from exc


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


def normalize_level_cap_mode(value: Any) -> str:
    if isinstance(value, int):
        return str(value)
    if value is None:
        return "MOD_LEVEL_CAP_MODE_SOFT"
    if not isinstance(value, str):
        raise ModgenError(f"Level cap mode {value!r} is invalid")
    stripped = value.strip()
    if stripped.startswith("MOD_LEVEL_CAP_MODE_"):
        return stripped
    if re.fullmatch(r"0x[0-9A-Fa-f]+|[0-9]+", stripped):
        return str(int(stripped, 0))
    upper = stripped.upper()
    if upper not in LEVEL_CAP_MODES:
        raise ModgenError(f"Unknown level cap mode {value!r}")
    return LEVEL_CAP_MODES[upper]


def level_cap_mode_value(value: str) -> int:
    key = value.strip().upper()
    if key in LEVEL_CAP_MODE_VALUES:
        return LEVEL_CAP_MODE_VALUES[key]
    if re.fullmatch(r"0x[0-9A-Fa-f]+|[0-9]+", value.strip()):
        return int(value, 0)
    raise ModgenError(f"Level cap mode {value!r} is invalid")


def normalize_rare_candy_policy(value: Any) -> str:
    if isinstance(value, int):
        return str(value)
    if value is None:
        return "MOD_LEVEL_CAP_RARE_CANDY_ALLOW"
    if not isinstance(value, str):
        raise ModgenError(f"Rare Candy policy {value!r} is invalid")
    stripped = value.strip()
    if stripped.startswith("MOD_LEVEL_CAP_RARE_CANDY_"):
        return stripped
    if re.fullmatch(r"0x[0-9A-Fa-f]+|[0-9]+", stripped):
        return str(int(stripped, 0))
    upper = stripped.upper().replace("-", "_")
    if upper not in LEVEL_CAP_RARE_CANDY_POLICIES:
        raise ModgenError(f"Unknown Rare Candy policy {value!r}")
    return LEVEL_CAP_RARE_CANDY_POLICIES[upper]


def rare_candy_policy_value(value: str) -> int:
    key = value.strip().upper()
    if key in ("0", "ALLOW", "MOD_LEVEL_CAP_RARE_CANDY_ALLOW"):
        return 0
    if key in ("1", "BLOCK", "BLOCK_AT_CAP", "MOD_LEVEL_CAP_RARE_CANDY_BLOCK_AT_CAP"):
        return 1
    if re.fullmatch(r"0x[0-9A-Fa-f]+|-?[0-9]+", value.strip()):
        return int(value, 0)
    raise ModgenError(f"Rare Candy policy {value!r} is invalid")


def parse_level_cap_level(value: Any, key: str, label: str) -> int:
    if isinstance(value, bool):
        raise ModgenError(f"{key}: {label} must be an integer level")
    try:
        level = int(value)
    except (TypeError, ValueError) as exc:
        raise ModgenError(f"{key}: {label} must be an integer level") from exc
    if level < 1 or level > 100:
        raise ModgenError(f"{key}: {label} must be in [1, 100]")
    return level


def default_soft_exp_percent(delta: int) -> int:
    if delta > 3:
        return 100
    if delta == 3:
        return 90
    if delta == 2:
        return 60
    if delta == 1:
        return 30
    if delta == 0:
        return 15
    if delta <= -3:
        return 0
    return 15 + (delta * 5)


def default_soft_exp_curve() -> List[int]:
    return [default_soft_exp_percent(delta) for delta in range(LEVEL_CAP_EXP_DELTA_MIN, LEVEL_CAP_EXP_DELTA_MAX + 1)]


def parse_level_cap_delta(value: Any, key: str, label: str) -> int:
    if isinstance(value, bool):
        raise ModgenError(f"{key}: {label} must be an integer level delta")
    try:
        delta = int(value)
    except (TypeError, ValueError) as exc:
        raise ModgenError(f"{key}: {label} must be an integer level delta") from exc
    if delta < LEVEL_CAP_EXP_DELTA_MIN or delta > LEVEL_CAP_EXP_DELTA_MAX:
        raise ModgenError(f"{key}: {label} must be in [{LEVEL_CAP_EXP_DELTA_MIN}, {LEVEL_CAP_EXP_DELTA_MAX}]")
    return delta


def parse_soft_exp_percent(value: Any, key: str, label: str) -> int:
    if isinstance(value, bool):
        raise ModgenError(f"{key}: {label} must be an integer percent")
    try:
        percent = int(value)
    except (TypeError, ValueError) as exc:
        raise ModgenError(f"{key}: {label} must be an integer percent") from exc
    if percent < 0 or percent > 100:
        raise ModgenError(f"{key}: {label} must be in [0, 100]")
    return percent


def parse_level_cap_table(value: Any, key: str) -> List[int]:
    if not isinstance(value, list):
        raise ModgenError(f"{key}: capsByBadge must be a list of 9 levels")
    if len(value) != 9:
        raise ModgenError(f"{key}: capsByBadge must contain exactly 9 levels for 0..8 badges")
    return [parse_level_cap_level(level, key, f"capsByBadge[{index}]") for index, level in enumerate(value)]


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
