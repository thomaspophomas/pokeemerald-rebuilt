#!/usr/bin/env python3
"""Simulate Battle Weather Stack residual flags for a Pokemon typing."""

from __future__ import annotations

import argparse
import importlib.util
import sys
from pathlib import Path
from typing import Any


MOD_ROOT = Path(__file__).resolve().parents[1]
WEATHER_RULES = MOD_ROOT / "src" / "tools" / "weather_rules.py"
sys.dont_write_bytecode = True


def load_weather_rules():
    spec = importlib.util.spec_from_file_location("weather_rules", WEATHER_RULES)
    if spec is None or spec.loader is None:
        raise RuntimeError("failed to load weather_rules.py")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


POISON_FLAGS = {"BWS_RESIDUAL_POISON", "BWS_RESIDUAL_TOXIC_POISON"}


def residual_flags(config: dict[str, Any], layers: set[str], pokemon_types: set[str]) -> set[str]:
    flags: set[str] = set()
    for layer in config["layers"]:
        if layer["id"] not in layers:
            continue
        for effect in layer["battle"].get("residual", []):
            include = set(effect.get("includeTypes", []))
            exclude = set(effect.get("excludeTypes", []))
            if include and not (pokemon_types & include):
                continue
            if exclude and pokemon_types & exclude:
                continue
            flags.update(effect["flags"])
    return flags


def legal_residual_flags(
    flags: set[str],
    *,
    status: str = "healthy",
    leech_seeded: bool = False,
    hp_full: bool = False,
) -> set[str]:
    legal = set(flags)
    if status in {"poison", "toxic"}:
        legal -= POISON_FLAGS
    elif "BWS_RESIDUAL_TOXIC_POISON" in legal:
        legal.discard("BWS_RESIDUAL_POISON")

    if leech_seeded:
        legal.discard("BWS_RESIDUAL_LEECH")
    if hp_full:
        legal.discard("BWS_RESIDUAL_HEAL_1_16")
    return legal


def self_test() -> None:
    weather_rules = load_weather_rules()
    config = weather_rules.checked_config(weather_rules.CONFIG_JSON)
    assert "BWS_RESIDUAL_SAND_CHIP" not in residual_flags(config, {"sand"}, {"TYPE_ROCK"})
    assert "BWS_RESIDUAL_SAND_CHIP" in residual_flags(config, {"sand"}, {"TYPE_NORMAL"})
    assert "BWS_RESIDUAL_HEAL_1_16" in residual_flags(config, {"pollen"}, {"TYPE_GRASS"})
    assert "BWS_RESIDUAL_TOXIC_POISON" in residual_flags(config, {"ash_rain"}, {"TYPE_GRASS"})
    assert "BWS_RESIDUAL_POISON" not in legal_residual_flags(
        residual_flags(config, {"ash_rain"}, {"TYPE_NORMAL"}),
        status="poison",
    )
    assert legal_residual_flags({"BWS_RESIDUAL_POISON", "BWS_RESIDUAL_TOXIC_POISON"}) == {"BWS_RESIDUAL_TOXIC_POISON"}
    assert "BWS_RESIDUAL_LEECH" not in legal_residual_flags({"BWS_RESIDUAL_LEECH"}, leech_seeded=True)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--layers", default="sand", help="Comma-separated layer ids")
    parser.add_argument("--types", default="TYPE_NORMAL", help="Comma-separated TYPE_* constants")
    parser.add_argument("--status", choices=["healthy", "poison", "toxic"], default="healthy")
    parser.add_argument("--leech-seeded", action="store_true")
    parser.add_argument("--hp-full", action="store_true")
    parser.add_argument("--self-test", action="store_true")
    args = parser.parse_args()

    if args.self_test:
        self_test()
        print("battle_weather_stack residual simulation tests OK")
        return 0

    weather_rules = load_weather_rules()
    config = weather_rules.checked_config(weather_rules.CONFIG_JSON)
    layers = {value.strip() for value in args.layers.split(",") if value.strip()}
    pokemon_types = {value.strip() for value in args.types.split(",") if value.strip()}
    flags = sorted(residual_flags(config, layers, pokemon_types))
    legal_flags = sorted(
        legal_residual_flags(
            set(flags),
            status=args.status,
            leech_seeded=args.leech_seeded,
            hp_full=args.hp_full,
        )
    )
    print(f"layers={','.join(sorted(layers)) or 'none'}")
    print(f"types={','.join(sorted(pokemon_types)) or 'none'}")
    print(f"status={args.status}")
    print(f"leechSeeded={str(args.leech_seeded).lower()}")
    print(f"hpFull={str(args.hp_full).lower()}")
    print(f"residualFlags={','.join(flags) or 'none'}")
    print(f"legalResidualFlags={','.join(legal_flags) or 'none'}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
