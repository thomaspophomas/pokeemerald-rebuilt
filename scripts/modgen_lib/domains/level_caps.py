"""Level cap domain collector."""

from __future__ import annotations

from typing import Any, Dict, List, Optional

from ..common import *
from ..errors import ModgenError
from .keys import key_for


def flag_ids_by_key(flags: Optional[List[Dict[str, Any]]]) -> Dict[str, int]:
    if flags is None:
        return {}
    return {flag["key"]: int(flag["id"]) for flag in flags}


def normalize_mod_flag(value: Any, mod_id: str, key: str, flag_ids: Dict[str, int]) -> str:
    if isinstance(value, int):
        return str(value)
    if not isinstance(value, str) or not value.strip():
        raise ModgenError(f"{key}: modFlag must be a mod flag id or key")

    stripped = value.strip()
    if re.fullmatch(r"0x[0-9A-Fa-f]+|[0-9]+", stripped):
        return str(int(stripped, 0))

    flag_key = stripped if ":" in stripped else f"{mod_id}:{stripped}"
    if flag_key not in flag_ids:
        raise ModgenError(f"{key}: unknown modFlag {stripped!r}")
    return str(flag_ids[flag_key])


def normalize_unlock_flag(stage: Dict[str, Any], mod_id: str, key: str, flag_ids: Dict[str, int]) -> str:
    has_mod_flag = "modFlag" in stage or "mod_flag" in stage
    has_encoded_flag = "flagId" in stage or "flag_id" in stage
    has_vanilla_flag = "flag" in stage or "vanillaFlag" in stage or "vanilla_flag" in stage
    used = sum(1 for present in (has_mod_flag, has_encoded_flag, has_vanilla_flag) if present)
    if used > 1:
        raise ModgenError(f"{key}: each cap stage may use only one flag field")
    if used == 0:
        return LEVEL_CAP_ALWAYS_FLAG
    if has_mod_flag:
        return normalize_mod_flag(stage.get("modFlag", stage.get("mod_flag")), mod_id, key, flag_ids)
    if has_encoded_flag:
        return c_int_or_token(stage.get("flagId", stage.get("flag_id")), LEVEL_CAP_ALWAYS_FLAG)

    vanilla_flag = stage.get("flag", stage.get("vanillaFlag", stage.get("vanilla_flag")))
    if vanilla_flag in (None, "", LEVEL_CAP_ALWAYS_FLAG):
        return LEVEL_CAP_ALWAYS_FLAG
    return f"MOD_FLAG_TO_VANILLA({c_int_or_token(vanilla_flag)})"


def normalize_level_cap_stage(raw_stage: Any, mod_id: str, key: str, index: int, flag_ids: Dict[str, int]) -> Dict[str, Any]:
    if isinstance(raw_stage, int):
        return {"level": parse_level_cap_level(raw_stage, key, f"stages[{index}]"), "flag": LEVEL_CAP_ALWAYS_FLAG}
    if not isinstance(raw_stage, dict):
        raise ModgenError(f"{key}: stages[{index}] must be an object")

    raw_level = raw_stage.get("level", raw_stage.get("cap", raw_stage.get("maxLevel", raw_stage.get("max_level"))))
    if raw_level is None:
        raise ModgenError(f"{key}: stages[{index}].level is required")

    return {
        "level": parse_level_cap_level(raw_level, key, f"stages[{index}].level"),
        "flag": normalize_unlock_flag(raw_stage, mod_id, f"{key}: stages[{index}]", flag_ids),
    }


def parse_level_cap_stages(item: Dict[str, Any], mod_id: str, key: str, flag_ids: Dict[str, int]) -> List[Dict[str, Any]]:
    raw_stages = item.get("stages", item.get("capStages", item.get("cap_stages")))
    raw_badge_table = item.get("capsByBadge", item.get("caps_by_badge"))
    if raw_stages is not None and raw_badge_table is not None:
        raise ModgenError(f"{key}: use either stages/capStages or legacy capsByBadge, not both")

    if raw_stages is not None:
        if not isinstance(raw_stages, list):
            raise ModgenError(f"{key}: stages must be a list")
        if len(raw_stages) == 0 or len(raw_stages) > LEVEL_CAP_MAX_STAGES:
            raise ModgenError(f"{key}: stages must contain 1..{LEVEL_CAP_MAX_STAGES} entries")
        return [normalize_level_cap_stage(stage, mod_id, key, index, flag_ids) for index, stage in enumerate(raw_stages)]

    cap_table = parse_level_cap_table(raw_badge_table, key)
    stages = [{"level": cap_table[0], "flag": LEVEL_CAP_ALWAYS_FLAG}]
    stages.extend(
        {"level": level, "flag": f"MOD_FLAG_TO_VANILLA({LEVEL_CAP_BADGE_FLAGS[index - 1]})"}
        for index, level in enumerate(cap_table[1:], start=1)
    )
    return stages


def get_soft_exp_curve_value(item: Dict[str, Any]) -> Any:
    for field in ("softExpCurve", "soft_exp_curve", "expCurve", "exp_curve"):
        if field in item:
            return item[field]
    return None


def parse_curve_entry(entry: Dict[str, Any], key: str, label: str) -> tuple[int, int, int]:
    percent = parse_soft_exp_percent(entry.get("percent"), key, f"{label}.percent")
    delta_value = entry.get("delta", entry.get("levelDelta", entry.get("level_delta")))
    has_range_start = any(field in entry for field in ("minDelta", "min_delta", "from"))
    has_range_end = any(field in entry for field in ("maxDelta", "max_delta", "to"))

    if delta_value is not None:
        if has_range_start or has_range_end:
            raise ModgenError(f"{key}: {label} must use either delta or minDelta/maxDelta")
        delta = parse_level_cap_delta(delta_value, key, f"{label}.delta")
        return delta, delta, percent

    min_delta_value = entry.get("minDelta", entry.get("min_delta", entry.get("from")))
    max_delta_value = entry.get("maxDelta", entry.get("max_delta", entry.get("to")))
    if not has_range_start or not has_range_end:
        raise ModgenError(f"{key}: {label} needs delta or minDelta/maxDelta")

    min_delta = parse_level_cap_delta(min_delta_value, key, f"{label}.minDelta")
    max_delta = parse_level_cap_delta(max_delta_value, key, f"{label}.maxDelta")
    if min_delta > max_delta:
        raise ModgenError(f"{key}: {label}.minDelta must be <= maxDelta")
    return min_delta, max_delta, percent


def parse_soft_exp_curve(raw_curve: Any, key: str) -> List[int]:
    if raw_curve is None:
        return default_soft_exp_curve()

    curve: List[Optional[int]] = [None] * LEVEL_CAP_EXP_DELTA_COUNT
    if isinstance(raw_curve, dict):
        entries = [{"delta": delta, "percent": percent} for delta, percent in raw_curve.items()]
    elif isinstance(raw_curve, list):
        if all(isinstance(percent, int) and not isinstance(percent, bool) for percent in raw_curve):
            if len(raw_curve) != LEVEL_CAP_EXP_DELTA_COUNT:
                raise ModgenError(f"{key}: softExpCurve table must contain exactly {LEVEL_CAP_EXP_DELTA_COUNT} entries")
            return [parse_soft_exp_percent(percent, key, f"softExpCurve[{index}]") for index, percent in enumerate(raw_curve)]
        entries = raw_curve
    else:
        raise ModgenError(f"{key}: softExpCurve must be a map, a {LEVEL_CAP_EXP_DELTA_COUNT}-entry percent table, or a list of range objects")

    for index, raw_entry in enumerate(entries):
        if not isinstance(raw_entry, dict):
            raise ModgenError(f"{key}: softExpCurve[{index}] must be an object")
        min_delta, max_delta, percent = parse_curve_entry(raw_entry, key, f"softExpCurve[{index}]")
        for delta in range(min_delta, max_delta + 1):
            curve_index = delta - LEVEL_CAP_EXP_DELTA_MIN
            if curve[curve_index] is not None:
                raise ModgenError(f"{key}: softExpCurve overlaps at delta {delta}")
            curve[curve_index] = percent

    missing = [delta for delta in range(LEVEL_CAP_EXP_DELTA_MIN, LEVEL_CAP_EXP_DELTA_MAX + 1) if curve[delta - LEVEL_CAP_EXP_DELTA_MIN] is None]
    if missing:
        raise ModgenError(f"{key}: softExpCurve must cover every delta from {LEVEL_CAP_EXP_DELTA_MIN} to {LEVEL_CAP_EXP_DELTA_MAX}; first missing delta is {missing[0]}")
    return [int(percent) for percent in curve]


def validate_level_cap(key: str, mode: str, rare_candy_policy: str, stages: List[Dict[str, Any]], soft_exp_curve: List[int]) -> None:
    mode_value = level_cap_mode_value(mode)
    rare_candy_value = rare_candy_policy_value(rare_candy_policy)

    if mode_value < 0 or mode_value > 2:
        raise ModgenError(f"{key}: mode must be NONE, SOFT, or HARD")
    if rare_candy_value < 0 or rare_candy_value > 1:
        raise ModgenError(f"{key}: rareCandy must be ALLOW or BLOCK_AT_CAP")
    if len(stages) == 0 or len(stages) > LEVEL_CAP_MAX_STAGES:
        raise ModgenError(f"{key}: stages must contain 1..{LEVEL_CAP_MAX_STAGES} entries")
    if len(soft_exp_curve) != LEVEL_CAP_EXP_DELTA_COUNT:
        raise ModgenError(f"{key}: softExpCurve must contain {LEVEL_CAP_EXP_DELTA_COUNT} entries")
    for index, percent in enumerate(soft_exp_curve):
        if percent < 0 or percent > 100:
            delta = LEVEL_CAP_EXP_DELTA_MIN + index
            raise ModgenError(f"{key}: softExpCurve delta {delta} must be in [0, 100]")


def collect_level_caps(mods: List[Dict[str, Any]], flags: Optional[List[Dict[str, Any]]] = None) -> List[Dict[str, Any]]:
    caps = []
    seen = set()
    flag_ids = flag_ids_by_key(flags)
    for mod in mods:
        for path in iter_json_files(mod["root"] / "level_caps"):
            for index, item in enumerate(read_domain_list(path, "caps")):
                key = key_for(mod["id"], item, f"cap_{index}")
                if key in seen:
                    raise ModgenError(f"Duplicate level cap key {key!r}")
                seen.add(key)

                mode = normalize_level_cap_mode(item.get("mode", item.get("capType", item.get("cap_type", "SOFT"))))
                rare_candy_policy = normalize_rare_candy_policy(item.get("rareCandy", item.get("rare_candy", "ALLOW")))
                stages = parse_level_cap_stages(item, mod["id"], key, flag_ids)
                soft_exp_curve = parse_soft_exp_curve(get_soft_exp_curve_value(item), key)
                priority = int(item.get("priority", 1000))
                if priority < -32768 or priority > 32767:
                    raise ModgenError(f"{key}: priority must be in signed 16-bit range")
                validate_level_cap(key, mode, rare_candy_policy, stages, soft_exp_curve)

                caps.append(
                    {
                        "key": key,
                        "mode": mode,
                        "stage_count": len(stages),
                        "rare_candy": rare_candy_policy,
                        "priority": priority,
                        "flags": c_int_or_token(item.get("flags"), "0"),
                        "stages": stages,
                        "soft_exp_curve": soft_exp_curve,
                    }
                )
    caps.sort(key=lambda cap: (cap["priority"], cap["key"]))
    return caps
