#!/usr/bin/env python3
"""Preview active cap, EXP percent, and Rare Candy behavior."""

from __future__ import annotations

import argparse
import json
from pathlib import Path
from typing import Any


MOD_ROOT = Path(__file__).resolve().parents[1]
DEFAULT_CONFIG = MOD_ROOT / "level_caps" / "default.json"
FEATURES = MOD_ROOT / "tools" / "config" / "features.json"


def load_json(path: Path) -> dict[str, Any]:
    data = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(data, dict):
        raise SystemExit(f"{path} must contain an object")
    return data


def curve_percent(curve: Any, delta: int) -> int:
    if curve is None:
        return 100
    if isinstance(curve, list) and all(isinstance(value, int) and not isinstance(value, bool) for value in curve):
        return curve[max(-100, min(99, delta)) + 100]
    if isinstance(curve, dict):
        key = str(max(-100, min(99, delta)))
        return int(curve.get(key, 100))
    if isinstance(curve, list):
        for entry in curve:
            if not isinstance(entry, dict):
                continue
            if "delta" in entry and int(entry["delta"]) == delta:
                return int(entry["percent"])
            minimum = int(entry.get("minDelta", entry.get("min_delta", entry.get("from", -999))))
            maximum = int(entry.get("maxDelta", entry.get("max_delta", entry.get("to", -999))))
            if minimum <= delta <= maximum:
                return int(entry["percent"])
    return 100


def active_cap(stages: list[Any], flags: set[str]) -> int:
    cap = 100
    for stage in stages:
        if isinstance(stage, int):
            cap = stage
            continue
        if not isinstance(stage, dict):
            continue
        level = int(stage.get("level", stage.get("cap", stage.get("maxLevel", 100))))
        flag = stage.get("flag") or stage.get("vanillaFlag") or stage.get("vanilla_flag") or stage.get("modFlag") or stage.get("mod_flag")
        if not flag or str(flag) in flags:
            cap = max(cap if flag else 0, level)
    return cap


def rare_candy_allowed(cap: dict[str, Any], pokemon_level: int, limit: int) -> bool:
    rare_candy_policy = str(cap.get("rareCandy", "ALLOW")).upper()
    return rare_candy_policy != "BLOCK_AT_CAP" or pokemon_level < limit


def effective_percent(cap: dict[str, Any], pokemon_level: int, limit: int, features: dict[str, Any]) -> int:
    delta = pokemon_level - limit
    percent = 100 if str(cap.get("capType", "SOFT")).upper() == "NONE" else curve_percent(cap.get("softExpCurve"), delta)
    if str(cap.get("capType", "SOFT")).upper() == "HARD" and pokemon_level >= limit:
        percent = 0

    grace_range = features.get("graceRange", {})
    if isinstance(grace_range, dict) and bool(grace_range.get("enabled", False)) and percent == 0:
        levels = int(grace_range.get("levels", 0))
        minimum_percent = int(grace_range.get("minimumPercent", 0))
        if 0 < delta <= levels:
            percent = max(percent, minimum_percent)
    return percent


def main() -> int:
    features = load_json(FEATURES)
    defaults = features.get("previewTool", {}) if isinstance(features.get("previewTool"), dict) else {}

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--config", type=Path, default=DEFAULT_CONFIG)
    parser.add_argument("--cap-id")
    parser.add_argument("--pokemon-level", type=int, default=int(defaults.get("defaultPokemonLevel", 15)))
    parser.add_argument("--flag", action="append", default=list(defaults.get("defaultFlags", [])))
    parser.add_argument("--battle-source", choices=["wild", "trainer", "daycare"], default="wild")
    parser.add_argument("--table", action="store_true", help="Print level 1..100 preview table and exit")
    args = parser.parse_args()

    data = load_json(args.config)
    caps = data.get("caps")
    if not isinstance(caps, list) or not caps:
        raise SystemExit("config has no caps")
    cap = next((item for item in caps if isinstance(item, dict) and item.get("id") == args.cap_id), caps[0])
    if not isinstance(cap, dict):
        raise SystemExit("selected cap must be an object")

    stages = cap.get("capStages", cap.get("cap_stages", cap.get("stages")))
    if not isinstance(stages, list):
        raise SystemExit("selected cap has no stages")
    unlocked = set(args.flag)
    limit = active_cap(stages, unlocked)
    delta = args.pokemon_level - limit
    percent = effective_percent(cap, args.pokemon_level, limit, features)
    candy_allowed = rare_candy_allowed(cap, args.pokemon_level, limit)

    if args.table:
        print("level,delta,battleExpPercent,rareCandyAllowed")
        for pokemon_level in range(1, 101):
            row_delta = pokemon_level - limit
            row_percent = effective_percent(cap, pokemon_level, limit, features)
            row_candy_allowed = rare_candy_allowed(cap, pokemon_level, limit)
            print(f"{pokemon_level},{row_delta},{row_percent},{str(row_candy_allowed).lower()}")
        return 0

    print(f"config={args.config}")
    print(f"capId={cap.get('id', '<first>')}")
    print(f"battleSource={args.battle_source}")
    print(f"enabledFlags={','.join(sorted(unlocked)) or 'none'}")
    print(f"activeCap={limit}")
    print(f"pokemonLevel={args.pokemon_level}")
    print(f"levelDelta={delta}")
    print(f"battleExpPercent={percent}")
    print(f"rareCandyAllowed={str(candy_allowed).lower()}")
    print(f"inGameHintsEnabled={str(bool(features.get('inGameHints', {}).get('enabled', False))).lower()}")
    print(f"separateBattleSourceRulesEnabled={str(bool(features.get('separateBattleSourceRules', {}).get('enabled', False))).lower()}")
    print(f"graceRangeEnabled={str(bool(features.get('graceRange', {}).get('enabled', False))).lower()}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
