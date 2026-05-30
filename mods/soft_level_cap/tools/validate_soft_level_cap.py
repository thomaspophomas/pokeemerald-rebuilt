#!/usr/bin/env python3
"""Validate bundled and preset soft level cap configs."""

from __future__ import annotations

import json
import re
from pathlib import Path


MOD_ROOT = Path(__file__).resolve().parents[1]
CONFIG_ROOT = MOD_ROOT / "level_caps"
PRESET_ROOT = MOD_ROOT / "tools" / "presets"
FEATURES = MOD_ROOT / "tools" / "config" / "features.json"
DEFAULT_FLAGS_HEADER = Path("/home/thomas/pokeemerald-rebuilt/include/constants/flags.h")
VALID_CAP_TYPES = {"NONE", "SOFT", "HARD"}
VALID_RARE_CANDY = {"ALLOW", "BLOCK", "BLOCK_AT_CAP"}
MAX_STAGES = 16
MIN_DELTA = -100
MAX_DELTA = 99
DELTA_COUNT = MAX_DELTA - MIN_DELTA + 1
FLAG_RE = re.compile(r"^#define\s+(FLAG_[A-Z0-9_]+)\s+")


def fail(message: str) -> None:
    raise SystemExit(f"soft_level_cap validation failed: {message}")


def parse_delta(value: object, label: str) -> int:
    if isinstance(value, bool):
        fail(f"{label} must be an integer in [{MIN_DELTA}, {MAX_DELTA}]")
    try:
        delta = int(value)  # type: ignore[arg-type]
    except (TypeError, ValueError):
        fail(f"{label} must be an integer in [{MIN_DELTA}, {MAX_DELTA}]")
    if delta < MIN_DELTA or delta > MAX_DELTA:
        fail(f"{label} must be in [{MIN_DELTA}, {MAX_DELTA}]")
    return delta


def parse_percent(value: object, label: str) -> int:
    if isinstance(value, bool):
        fail(f"{label} must be an integer in [0, 100]")
    try:
        percent = int(value)  # type: ignore[arg-type]
    except (TypeError, ValueError):
        fail(f"{label} must be an integer in [0, 100]")
    if percent < 0 or percent > 100:
        fail(f"{label} must be in [0, 100]")
    return percent


def validate_soft_exp_curve(raw_curve: object, label: str) -> None:
    if raw_curve is None:
        return

    covered = [False] * DELTA_COUNT
    if isinstance(raw_curve, dict):
        entries = [{"delta": delta, "percent": percent} for delta, percent in raw_curve.items()]
    elif isinstance(raw_curve, list):
        if all(isinstance(percent, int) and not isinstance(percent, bool) for percent in raw_curve):
            if len(raw_curve) != DELTA_COUNT:
                fail(f"{label} table must contain exactly {DELTA_COUNT} entries")
            for percent_index, percent in enumerate(raw_curve):
                parse_percent(percent, f"{label}[{percent_index}]")
            return
        entries = raw_curve
    else:
        fail(f"{label} must be a map, a {DELTA_COUNT}-entry percent table, or a list of range objects")

    for curve_index, raw_entry in enumerate(entries):
        entry_label = f"{label}[{curve_index}]"
        if not isinstance(raw_entry, dict):
            fail(f"{entry_label} must be an object")
        parse_percent(raw_entry.get("percent"), f"{entry_label}.percent")
        raw_delta = raw_entry.get("delta", raw_entry.get("levelDelta", raw_entry.get("level_delta")))
        has_range_start = any(field in raw_entry for field in ("minDelta", "min_delta", "from"))
        has_range_end = any(field in raw_entry for field in ("maxDelta", "max_delta", "to"))
        if raw_delta is not None:
            if has_range_start or has_range_end:
                fail(f"{entry_label} must use either delta or minDelta/maxDelta")
            min_delta = max_delta = parse_delta(raw_delta, f"{entry_label}.delta")
        else:
            if not has_range_start or not has_range_end:
                fail(f"{entry_label} needs delta or minDelta/maxDelta")
            min_delta = parse_delta(raw_entry.get("minDelta", raw_entry.get("min_delta", raw_entry.get("from"))), f"{entry_label}.minDelta")
            max_delta = parse_delta(raw_entry.get("maxDelta", raw_entry.get("max_delta", raw_entry.get("to"))), f"{entry_label}.maxDelta")
        if min_delta > max_delta:
            fail(f"{entry_label}.minDelta must be <= maxDelta")
        for delta in range(min_delta, max_delta + 1):
            covered_index = delta - MIN_DELTA
            if covered[covered_index]:
                fail(f"{label} overlaps at delta {delta}")
            covered[covered_index] = True

    for covered_index, is_covered in enumerate(covered):
        if not is_covered:
            fail(f"{label} must cover every delta from {MIN_DELTA} to {MAX_DELTA}; first missing delta is {MIN_DELTA + covered_index}")


def flag_key(stage: dict[str, object], raw: bool = False) -> str | None:
    for field in ("flag", "vanillaFlag", "vanilla_flag", "modFlag", "mod_flag", "flagId", "flag_id"):
        value = stage.get(field)
        if value not in (None, ""):
            return str(value) if raw else f"{field}:{value}"
    return None


def load_known_vanilla_flags(path: Path = DEFAULT_FLAGS_HEADER) -> set[str]:
    if not path.exists():
        return set()
    return set(FLAG_RE.findall(path.read_text(encoding="utf-8", errors="ignore")))


def validate_feature_toggles() -> None:
    data = json.loads(FEATURES.read_text(encoding="utf-8"))
    if not isinstance(data, dict):
        fail("tools/config/features.json must contain an object")
    for section_name in ("inGameHints", "separateBattleSourceRules", "previewTool", "graceRange"):
        section = data.get(section_name)
        if not isinstance(section, dict):
            fail(f"features.{section_name} must be an object")
        if not isinstance(section.get("enabled"), bool):
            fail(f"features.{section_name}.enabled must be boolean")
    grace_range = data["graceRange"]
    if not isinstance(grace_range.get("levels"), int) or isinstance(grace_range.get("levels"), bool):
        fail("features.graceRange.levels must be an integer")
    if grace_range["levels"] < 0 or grace_range["levels"] > 100:
        fail("features.graceRange.levels must be in [0, 100]")
    parse_percent(grace_range.get("minimumPercent"), "features.graceRange.minimumPercent")


def validate_config(path: Path, seen_ids: dict[str, Path], known_flags: set[str]) -> None:
    data = json.loads(path.read_text(encoding="utf-8"))
    caps = data.get("caps")
    if not isinstance(caps, list) or not caps:
        fail(f"{path.relative_to(MOD_ROOT)} must contain a non-empty caps list")

    for cap_index, cap in enumerate(caps):
        label = f"{path.relative_to(MOD_ROOT)} caps[{cap_index}]"
        if not isinstance(cap, dict):
            fail(f"{label} must be an object")
        cap_id = cap.get("id")
        if isinstance(cap_id, str) and cap_id:
            if cap_id in seen_ids:
                fail(f"{label}.id duplicates {cap_id!r} from {seen_ids[cap_id].relative_to(MOD_ROOT)}")
            seen_ids[cap_id] = path
        cap_type = str(cap.get("capType", cap.get("mode", "SOFT"))).upper()
        rare_candy = str(cap.get("rareCandy", "ALLOW")).upper().replace("-", "_")
        stages = cap.get("capStages", cap.get("cap_stages", cap.get("stages")))
        soft_exp_curve = cap.get("softExpCurve", cap.get("soft_exp_curve", cap.get("expCurve", cap.get("exp_curve"))))

        if cap_type not in VALID_CAP_TYPES:
            fail(f"{label}.capType must be one of {sorted(VALID_CAP_TYPES)}")
        if rare_candy not in VALID_RARE_CANDY:
            fail(f"{label}.rareCandy must be one of {sorted(VALID_RARE_CANDY)}")
        if not isinstance(stages, list) or not stages or len(stages) > MAX_STAGES:
            fail(f"{label}.capStages must contain 1..{MAX_STAGES} stages")
        previous_cap = 0
        base_stage_count = 0
        seen_stage_flags: set[str] = set()
        for stage_index, stage in enumerate(stages):
            if isinstance(stage, int):
                level_cap = stage
                stage_flag = None
            elif isinstance(stage, dict):
                level_cap = stage.get("level", stage.get("cap", stage.get("maxLevel", stage.get("max_level"))))
                stage_flag = flag_key(stage)
                raw_flag = flag_key(stage, raw=True)
                if raw_flag and raw_flag.startswith("FLAG_") and known_flags and raw_flag not in known_flags:
                    fail(f"{label}.capStages[{stage_index}] references unknown vanilla flag {raw_flag}")
            else:
                fail(f"{label}.capStages[{stage_index}] must be an object or integer")
            if not isinstance(level_cap, int) or level_cap < 1 or level_cap > 100:
                fail(f"{label}.capStages[{stage_index}].level must be in [1, 100]")
            if level_cap < previous_cap:
                fail(f"{label}.capStages must not decrease")
            previous_cap = level_cap
            if stage_flag is None:
                base_stage_count += 1
            elif stage_flag in seen_stage_flags:
                fail(f"{label}.capStages[{stage_index}] duplicates {stage_flag}")
            else:
                seen_stage_flags.add(stage_flag)
        if base_stage_count != 1:
            fail(f"{label}.capStages needs exactly one base stage without a flag")
        validate_soft_exp_curve(soft_exp_curve, f"{label}.softExpCurve")


def main() -> int:
    paths = sorted(CONFIG_ROOT.rglob("*.json")) + sorted(PRESET_ROOT.rglob("*.json"))
    if not paths:
        fail("level_caps must contain at least one JSON config")
    validate_feature_toggles()
    known_flags = load_known_vanilla_flags()
    seen_ids: dict[str, Path] = {}
    for path in paths:
        validate_config(path, seen_ids, known_flags)

    print(f"soft_level_cap validation OK: {len(paths)} configs, feature toggles loaded")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
