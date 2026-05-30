#!/usr/bin/env python3
"""Validate, generate, and simulate the Battle Weather Stack config."""

from __future__ import annotations

import argparse
import json
import random
import re
import sys
from collections import Counter
from pathlib import Path
from typing import Any


MOD_ROOT = Path(__file__).resolve().parents[2]
CONFIG_JSON = MOD_ROOT / "src" / "tools" / "weather_config.json"
GENERATED_INC = MOD_ROOT / "weather" / "generated_config.inc"
MINUTES_PER_DAY = 24 * 60
MAX_RANDOM_SLOTS = 2

LAYER_CONSTS = {
    "rain": "BWS_LAYER_RAIN",
    "snow": "BWS_LAYER_SNOW",
    "sand": "BWS_LAYER_SAND",
    "heat": "BWS_LAYER_HEAT",
    "darkness": "BWS_LAYER_DARKNESS",
    "wind": "BWS_LAYER_WIND",
    "silence": "BWS_LAYER_SILENCE",
    "light": "BWS_LAYER_LIGHT",
    "ash_rain": "BWS_LAYER_ASH_RAIN",
    "pollen": "BWS_LAYER_POLLEN",
}

TYPE_CONSTS = {
    "TYPE_NORMAL",
    "TYPE_FIGHTING",
    "TYPE_FLYING",
    "TYPE_POISON",
    "TYPE_GROUND",
    "TYPE_ROCK",
    "TYPE_BUG",
    "TYPE_GHOST",
    "TYPE_STEEL",
    "TYPE_FIRE",
    "TYPE_WATER",
    "TYPE_GRASS",
    "TYPE_ELECTRIC",
    "TYPE_PSYCHIC",
    "TYPE_ICE",
    "TYPE_DRAGON",
    "TYPE_DARK",
}

STAT_CONSTS = {
    "STAT_ATK",
    "STAT_DEF",
    "STAT_SPATK",
    "STAT_SPDEF",
    "STAT_SPEED",
}

BATTLE_WEATHER_FLAGS = {
    "B_WEATHER_RAIN_PERMANENT",
    "B_WEATHER_SANDSTORM_PERMANENT",
    "B_WEATHER_SUN_PERMANENT",
}

RESIDUAL_FLAGS = {
    "BWS_RESIDUAL_SAND_CHIP",
    "BWS_RESIDUAL_POISON",
    "BWS_RESIDUAL_TOXIC_POISON",
    "BWS_RESIDUAL_LEECH",
    "BWS_RESIDUAL_HEAL_1_16",
}

VISUAL_WEATHERS = {
    "WEATHER_NONE",
    "WEATHER_RAIN",
    "WEATHER_RAIN_THUNDERSTORM",
    "WEATHER_SNOW",
    "WEATHER_SANDSTORM",
    "WEATHER_SUNNY",
    "WEATHER_SUNNY_CLOUDS",
    "WEATHER_DROUGHT",
    "WEATHER_SHADE",
    "WEATHER_VOLCANIC_ASH",
}

MAP_RE = re.compile(r"^(MAPSEC_[A-Z0-9_]+|BWS_MAPSEC_ANY)$")
ROUTE_MAP_RE = re.compile(r"^MAPSEC_[A-Z0-9_]+$")


def load_config(path: Path = CONFIG_JSON) -> dict[str, Any]:
    with path.open("r", encoding="utf-8") as handle:
        data = json.load(handle)
    if not isinstance(data, dict):
        raise ValueError(f"{path} must contain a JSON object")
    return data


def layer_ids(config: dict[str, Any]) -> set[str]:
    return {layer["id"] for layer in config.get("layers", []) if isinstance(layer, dict)}


def layer_by_id(config: dict[str, Any]) -> dict[str, dict[str, Any]]:
    return {layer["id"]: layer for layer in config.get("layers", []) if isinstance(layer, dict)}


def message_symbol(message_key: str) -> str:
    tail = message_key.rsplit(".", 1)[-1]
    return re.sub(r"[^A-Za-z0-9_]", "_", tail)


def c_string(value: str) -> str:
    return json.dumps(value, ensure_ascii=True)


def layer_mask_expr(layers: list[str]) -> str:
    if not layers:
        return "BWS_LAYER_NONE"
    return " | ".join(LAYER_CONSTS[layer] for layer in layers)


def type_mask_expr(types: list[str]) -> str:
    if not types:
        return "0"
    return " | ".join(f"(1u << {type_const})" for type_const in types)


def stat_mask_expr(stats: list[str]) -> str:
    if not stats:
        return "0"
    return " | ".join(f"(1u << {stat_const})" for stat_const in stats)


def flag_expr(flags: list[str]) -> str:
    if not flags:
        return "0"
    return " | ".join(flags)


def validate_int(errors: list[str], prefix: str, field: str, value: Any, minimum: int, maximum: int) -> None:
    if not isinstance(value, int):
        errors.append(f"{prefix}: {field} must be an integer")
    elif not minimum <= value <= maximum:
        errors.append(f"{prefix}: {field} must be between {minimum} and {maximum}")


def validate_types(errors: list[str], prefix: str, field: str, values: Any) -> None:
    if not isinstance(values, list):
        errors.append(f"{prefix}: {field} must be a list")
        return
    for value in values:
        if value not in TYPE_CONSTS:
            errors.append(f"{prefix}: unknown type {value!r}")


def validate_stats(errors: list[str], prefix: str, field: str, values: Any) -> None:
    if not isinstance(values, list):
        errors.append(f"{prefix}: {field} must be a list")
        return
    for value in values:
        if value not in STAT_CONSTS:
            errors.append(f"{prefix}: unknown stat {value!r}")


def validate_config(config: dict[str, Any]) -> list[str]:
    errors: list[str] = []
    chance_scale = config.get("chanceScale", 10000)
    max_layers = config.get("maxLayers", 3)
    messages = config.get("messages")
    ids = layer_ids(config)
    seen_ids: set[str] = set()

    validate_int(errors, "config", "chanceScale", chance_scale, 1, 1000000)
    validate_int(errors, "config", "maxLayers", max_layers, 1, 16)
    if not isinstance(messages, dict):
        errors.append("config: messages must be an object")
        messages = {}

    layers = config.get("layers")
    if not isinstance(layers, list) or not layers:
        errors.append("config: layers must be a non-empty list")
        layers = []

    for index, layer in enumerate(layers, 1):
        prefix = f"layer #{index}"
        if not isinstance(layer, dict):
            errors.append(f"{prefix}: must be an object")
            continue
        layer_id = layer.get("id")
        if layer_id not in LAYER_CONSTS:
            errors.append(f"{prefix}: unknown id {layer_id!r}")
        elif layer_id in seen_ids:
            errors.append(f"{prefix}: duplicate id {layer_id!r}")
        else:
            seen_ids.add(layer_id)
        if layer.get("constant") != LAYER_CONSTS.get(layer_id):
            errors.append(f"{prefix}: constant does not match id")
        for field in ("nameKey", "messageKey"):
            if not isinstance(layer.get(field), str):
                errors.append(f"{prefix}: {field} must be a string")
        if isinstance(messages, dict) and layer.get("messageKey") not in messages:
            errors.append(f"{prefix}: messageKey has no message text")

        battle = layer.get("battle")
        if not isinstance(battle, dict):
            errors.append(f"{prefix}: battle must be an object")
            battle = {}
        for flag in battle.get("battleWeatherFlags", []):
            if flag not in BATTLE_WEATHER_FLAGS:
                errors.append(f"{prefix}: unknown battle weather flag {flag!r}")
        if battle.get("weatherBallType", "TYPE_NORMAL") not in TYPE_CONSTS:
            errors.append(f"{prefix}: invalid weatherBallType")

        for effect_index, effect in enumerate(battle.get("movePower", []), 1):
            effect_prefix = f"{prefix}.battle.movePower[{effect_index}]"
            validate_types(errors, effect_prefix, "moveTypes", effect.get("moveTypes"))
            validate_int(errors, effect_prefix, "percent", effect.get("percent"), 1, 1000)
        for effect_index, effect in enumerate(battle.get("stat", []), 1):
            effect_prefix = f"{prefix}.battle.stat[{effect_index}]"
            validate_types(errors, effect_prefix, "pokemonTypes", effect.get("pokemonTypes"))
            validate_stats(errors, effect_prefix, "stats", effect.get("stats"))
            validate_int(errors, effect_prefix, "percent", effect.get("percent"), 1, 1000)
        for effect_index, effect in enumerate(battle.get("damageTaken", []), 1):
            effect_prefix = f"{prefix}.battle.damageTaken[{effect_index}]"
            validate_types(errors, effect_prefix, "pokemonTypes", effect.get("pokemonTypes"))
            validate_int(errors, effect_prefix, "percent", effect.get("percent"), 1, 1000)
        for effect_index, effect in enumerate(battle.get("residual", []), 1):
            effect_prefix = f"{prefix}.battle.residual[{effect_index}]"
            validate_types(errors, effect_prefix, "includeTypes", effect.get("includeTypes", []))
            validate_types(errors, effect_prefix, "excludeTypes", effect.get("excludeTypes", []))
            flags = effect.get("flags")
            if not isinstance(flags, list) or not flags:
                errors.append(f"{effect_prefix}: flags must be a non-empty list")
            else:
                for flag in flags:
                    if flag not in RESIDUAL_FLAGS:
                        errors.append(f"{effect_prefix}: unknown residual flag {flag!r}")

        overworld = layer.get("overworld")
        if not isinstance(overworld, dict):
            errors.append(f"{prefix}: overworld must be an object")
            overworld = {}
        if overworld.get("defaultVisual") not in VISUAL_WEATHERS:
            errors.append(f"{prefix}: invalid overworld.defaultVisual")
        if not isinstance(overworld.get("sprite"), str):
            errors.append(f"{prefix}: overworld.sprite must be a string")
        tint = overworld.get("tint")
        if tint is not None:
            if not isinstance(tint, dict):
                errors.append(f"{prefix}: overworld.tint must be null or an object")
            else:
                for channel in ("r", "g", "b"):
                    validate_int(errors, f"{prefix}.overworld.tint", channel, tint.get(channel), 0, 512)

    for field in ("trimOrder", "weatherBallOrder", "overworldVisualPriority"):
        values = config.get(field)
        if not isinstance(values, list):
            errors.append(f"config: {field} must be a list")
            continue
        for value in values:
            if value not in ids:
                errors.append(f"config: {field} references unknown layer {value!r}")

    for index, pair in enumerate(config.get("conflicts", []), 1):
        if not isinstance(pair, list) or len(pair) != 2:
            errors.append(f"conflict #{index}: must contain exactly two layers")
            continue
        for value in pair:
            if value not in ids:
                errors.append(f"conflict #{index}: unknown layer {value!r}")

    for index, combo in enumerate(config.get("combos", []), 1):
        prefix = f"combo #{index}"
        combo_layers = combo.get("layers")
        if not isinstance(combo_layers, list) or not combo_layers:
            errors.append(f"{prefix}: layers must be a non-empty list")
            combo_layers = []
        for value in combo_layers:
            if value not in ids:
                errors.append(f"{prefix}: unknown layer {value!r}")
        if len(set(combo_layers)) != len(combo_layers):
            errors.append(f"{prefix}: layers must be unique")
        for field in ("nameKey", "messageKey"):
            if not isinstance(combo.get(field), str):
                errors.append(f"{prefix}: {field} must be a string")
        if isinstance(messages, dict) and combo.get("messageKey") not in messages:
            errors.append(f"{prefix}: messageKey has no message text")

    for index, combo in enumerate(config.get("overworldVisualCombos", []), 1):
        prefix = f"overworldVisualCombo #{index}"
        combo_layers = combo.get("layers")
        if not isinstance(combo_layers, list) or not combo_layers:
            errors.append(f"{prefix}: layers must be a non-empty list")
            combo_layers = []
        for value in combo_layers:
            if value not in ids:
                errors.append(f"{prefix}: unknown layer {value!r}")
        if combo.get("vanillaWeather") not in VISUAL_WEATHERS:
            errors.append(f"{prefix}: invalid vanillaWeather")

    for index, rule in enumerate(config.get("lightingRules", []), 1):
        prefix = f"lightingRule #{index}"
        map_section = rule.get("mapSection")
        if not isinstance(map_section, str) or MAP_RE.fullmatch(map_section) is None:
            errors.append(f"{prefix}: invalid mapSection {map_section!r}")
        for field in ("lightStartMinute", "lightEndMinute", "darkStartMinute", "darkEndMinute"):
            validate_int(errors, prefix, field, rule.get(field), 0, MINUTES_PER_DAY - 1)

    seen_rules: set[tuple[str, str]] = set()
    for index, rule in enumerate(config.get("overworldRules", []), 1):
        prefix = f"overworldRule #{index}"
        map_section = rule.get("mapSection")
        layer = rule.get("layer")
        visual = rule.get("vanillaWeather")
        if not isinstance(map_section, str) or ROUTE_MAP_RE.fullmatch(map_section) is None:
            errors.append(f"{prefix}: invalid mapSection {map_section!r}")
        if layer not in ids:
            errors.append(f"{prefix}: unknown layer {layer!r}")
        if visual not in VISUAL_WEATHERS:
            errors.append(f"{prefix}: invalid vanillaWeather {visual!r}")
        validate_int(errors, prefix, "chancePerMinute", rule.get("chancePerMinute"), 0, chance_scale)
        validate_int(errors, prefix, "minDurationMinutes", rule.get("minDurationMinutes"), 1, 255)
        validate_int(errors, prefix, "maxDurationMinutes", rule.get("maxDurationMinutes"), 1, 255)
        validate_int(errors, prefix, "blockNewWeatherMinutes", rule.get("blockNewWeatherMinutes"), 0, 255)
        if (
            isinstance(rule.get("minDurationMinutes"), int)
            and isinstance(rule.get("maxDurationMinutes"), int)
            and rule["minDurationMinutes"] > rule["maxDurationMinutes"]
        ):
            errors.append(f"{prefix}: minDurationMinutes must be <= maxDurationMinutes")
        if isinstance(map_section, str) and isinstance(layer, str):
            key = (map_section, layer)
            if key in seen_rules:
                errors.append(f"{prefix}: duplicate {map_section}/{layer} rule")
            seen_rules.add(key)

    for required_message in ("battle_weather_stack:message.clear", "battle_weather_stack:message.composite"):
        if required_message not in messages:
            errors.append(f"config: missing {required_message}")

    return errors


def generate_config_inc(config: dict[str, Any]) -> str:
    layers = layer_by_id(config)
    messages = config["messages"]
    lines: list[str] = [
        "/* Generated by src/tools/weather_rules.py generate. Do not edit by hand. */",
        "",
        f"#define BWS_OVERWORLD_CHANCE_SCALE {config.get('chanceScale', 10000)}",
        f"STATIC_ASSERT(BWS_MAX_LAYERS == {config.get('maxLayers', 3)}, BattleWeatherStackMaxLayersMatchesConfig);",
        "",
    ]

    for message_key, text in messages.items():
        lines.append(f"static const u8 sBwsMessage_{message_symbol(message_key)}[] = _({c_string(text)});")
    lines.extend(
        [
            "",
            "static const struct BattleWeatherStackMessageText sMessageText[] =",
            "{",
        ]
    )
    for message_key in messages:
        lines.append(f"    {{ {c_string(message_key)}, sBwsMessage_{message_symbol(message_key)} }},")
    lines.extend(["};", ""])

    lines.extend(["static const struct BattleWeatherStackCombo sBaseLayerText[] =", "{"])
    for layer in config["layers"]:
        lines.append(
            "    {{ {layer}, {name}, {message} }},".format(
                layer=layer["constant"],
                name=c_string(layer["nameKey"]),
                message=c_string(layer["messageKey"]),
            )
        )
    lines.extend(["};", ""])

    lines.extend(["static const struct BattleWeatherStackCombo sComboText[] =", "{"])
    for combo in config["combos"]:
        lines.append(
            "    {{ {layers}, {name}, {message} }},".format(
                layers=layer_mask_expr(combo["layers"]),
                name=c_string(combo["nameKey"]),
                message=c_string(combo["messageKey"]),
            )
        )
    lines.extend(["};", ""])

    lines.extend(["static const struct BattleWeatherStackLayerInfo sLayerInfo[] =", "{"])
    for layer in config["layers"]:
        battle = layer["battle"]
        overworld = layer["overworld"]
        tint = overworld.get("tint")
        has_tint = "TRUE" if tint is not None else "FALSE"
        tint_r = tint["r"] if tint is not None else 256
        tint_g = tint["g"] if tint is not None else 256
        tint_b = tint["b"] if tint is not None else 256
        lines.append(
            "    {{ {layer}, {battle_weather}, {weather_ball}, {default_visual}, {has_tint}, {r}, {g}, {b}, {sprite} }},".format(
                layer=layer["constant"],
                battle_weather=flag_expr(battle.get("battleWeatherFlags", [])),
                weather_ball=battle.get("weatherBallType", "TYPE_NORMAL"),
                default_visual=overworld["defaultVisual"],
                has_tint=has_tint,
                r=tint_r,
                g=tint_g,
                b=tint_b,
                sprite=c_string(overworld["sprite"]),
            )
        )
    lines.extend(["};", ""])

    for array_name, field_name in (
        ("sLayerTrimOrder", "trimOrder"),
        ("sWeatherBallOrder", "weatherBallOrder"),
        ("sOverworldVisualPriority", "overworldVisualPriority"),
    ):
        lines.extend([f"static const u16 {array_name}[] =", "{"])
        for layer_id in config[field_name]:
            lines.append(f"    {layers[layer_id]['constant']},")
        lines.extend(["};", ""])

    lines.extend(["static const struct BattleWeatherStackLayerPair sConflicts[] =", "{"])
    for first, second in config["conflicts"]:
        lines.append(f"    {{ {layers[first]['constant']}, {layers[second]['constant']} }},")
    lines.extend(["};", ""])

    lines.extend(["static const struct BattleWeatherStackVisualCombo sOverworldVisualCombos[] =", "{"])
    for combo in config["overworldVisualCombos"]:
        lines.append(f"    {{ {layer_mask_expr(combo['layers'])}, {combo['vanillaWeather']} }},")
    lines.extend(["};", ""])

    lines.extend(["static const struct BattleWeatherStackLightingRule sLightingRules[] =", "{"])
    for rule in config["lightingRules"]:
        lines.append(
            "    {{ {mapSection}, {lightStartMinute}, {lightEndMinute}, {darkStartMinute}, {darkEndMinute} }},".format(
                **rule
            )
        )
    lines.extend(["};", ""])

    lines.extend(["static const struct BattleWeatherStackOverworldRule sOverworldRules[] =", "{"])
    for rule in config["overworldRules"]:
        lines.append(
            "    {{ {mapSection}, {layer}, {chancePerMinute}, {minDurationMinutes}, {maxDurationMinutes}, {blockNewWeatherMinutes}, {vanillaWeather} }},".format(
                mapSection=rule["mapSection"],
                layer=layers[rule["layer"]]["constant"],
                chancePerMinute=rule["chancePerMinute"],
                minDurationMinutes=rule["minDurationMinutes"],
                maxDurationMinutes=rule["maxDurationMinutes"],
                blockNewWeatherMinutes=rule["blockNewWeatherMinutes"],
                vanillaWeather=rule["vanillaWeather"],
            )
        )
    lines.extend(["};", ""])

    lines.extend(["static const struct BattleWeatherStackMovePowerEffect sMovePowerEffects[] =", "{"])
    for layer in config["layers"]:
        for effect in layer["battle"].get("movePower", []):
            for move_type in effect["moveTypes"]:
                lines.append(f"    {{ {layer['constant']}, {move_type}, {effect['percent']} }},")
    lines.extend(["};", ""])

    lines.extend(["static const struct BattleWeatherStackStatEffect sStatEffects[] =", "{"])
    for layer in config["layers"]:
        for effect in layer["battle"].get("stat", []):
            lines.append(
                f"    {{ {layer['constant']}, {type_mask_expr(effect['pokemonTypes'])}, {stat_mask_expr(effect['stats'])}, {effect['percent']} }},"
            )
    lines.extend(["};", ""])

    lines.extend(["static const struct BattleWeatherStackDamageTakenEffect sDamageTakenEffects[] =", "{"])
    for layer in config["layers"]:
        for effect in layer["battle"].get("damageTaken", []):
            lines.append(f"    {{ {layer['constant']}, {type_mask_expr(effect['pokemonTypes'])}, {effect['percent']} }},")
    lines.extend(["};", ""])

    lines.extend(["static const struct BattleWeatherStackResidualEffect sResidualEffects[] =", "{"])
    for layer in config["layers"]:
        for effect in layer["battle"].get("residual", []):
            lines.append(
                "    {{ {layer}, {include}, {exclude}, {flags} }},".format(
                    layer=layer["constant"],
                    include=type_mask_expr(effect.get("includeTypes", [])),
                    exclude=type_mask_expr(effect.get("excludeTypes", [])),
                    flags=flag_expr(effect["flags"]),
                )
            )
    lines.extend(["};", ""])
    return "\n".join(lines).rstrip()


def minute_in_window(minute: int, start: int, end: int) -> bool:
    if start == end:
        return False
    if start < end:
        return start <= minute < end
    return minute >= start or minute < end


def lighting_rule(config: dict[str, Any], map_section: str) -> dict[str, Any] | None:
    fallback = None
    for rule in config["lightingRules"]:
        if rule["mapSection"] == map_section:
            return rule
        if rule["mapSection"] == "BWS_MAPSEC_ANY":
            fallback = rule
    return fallback


def scheduled_lighting(config: dict[str, Any], map_section: str, minute_of_day: int) -> str | None:
    rule = lighting_rule(config, map_section)
    if rule is None:
        return None
    if minute_in_window(minute_of_day, rule["darkStartMinute"], rule["darkEndMinute"]):
        return "darkness"
    if minute_in_window(minute_of_day, rule["lightStartMinute"], rule["lightEndMinute"]):
        return "light"
    return None


def compatible(config: dict[str, Any], layers: set[str], candidate: str) -> bool:
    combined = set(layers)
    combined.add(candidate)
    if len(combined) > config.get("maxLayers", 3):
        return False
    for first, second in config["conflicts"]:
        if first in combined and second in combined:
            return False
    return True


def active_layers(slots: list[dict[str, Any]], minute: int) -> set[str]:
    return {slot["layer"] for slot in slots if slot["layer"] is not None and minute < slot["end"]}


def combo_name(layers: set[str]) -> str:
    return "+".join(sorted(layers)) if layers else "clear"


def simulate(config: dict[str, Any], map_section: str, days: int, seed: int) -> str:
    rng = random.Random(seed)
    route_rules = [rule for rule in config["overworldRules"] if rule["mapSection"] == map_section]
    if not route_rules:
        raise ValueError(f"no rules for {map_section}")

    slots = [{"layer": None, "end": 0}, {"layer": None, "end": 0}]
    block_until = 0
    starts: Counter[str] = Counter()
    uptime: Counter[str] = Counter()
    combos: Counter[str] = Counter()
    blocked_minutes = 0
    total_minutes = days * MINUTES_PER_DAY
    chance_scale = config.get("chanceScale", 10000)

    for minute in range(total_minutes):
        for slot in slots:
            if slot["layer"] is not None and minute >= slot["end"]:
                slot["layer"] = None
                slot["end"] = 0

        random_layers = active_layers(slots, minute)
        free_slot = next((slot for slot in slots if slot["layer"] is None), None)

        if free_slot is None or minute < block_until:
            blocked_minutes += 1
        else:
            candidates = [
                rule
                for rule in route_rules
                if rule["layer"] not in random_layers
                and rule["chancePerMinute"] > 0
                and compatible(config, random_layers, rule["layer"])
            ]
            total_chance = sum(rule["chancePerMinute"] for rule in candidates)
            should_start = total_chance >= chance_scale or rng.randrange(chance_scale) < total_chance
            if candidates and should_start:
                pick = rng.randrange(total_chance)
                selected = candidates[-1]
                for rule in candidates:
                    if pick < rule["chancePerMinute"]:
                        selected = rule
                        break
                    pick -= rule["chancePerMinute"]
                duration = rng.randint(selected["minDurationMinutes"], selected["maxDurationMinutes"])
                free_slot["layer"] = selected["layer"]
                free_slot["end"] = minute + duration
                block_until = minute + selected["blockNewWeatherMinutes"]
                starts[selected["layer"]] += 1
                random_layers = active_layers(slots, minute)

        combined = set(random_layers)
        light = scheduled_lighting(config, map_section, minute % MINUTES_PER_DAY)
        if light is not None and compatible(config, combined, light):
            combined.add(light)

        for layer in random_layers:
            uptime[layer] += 1
        combos[combo_name(combined)] += 1

    lines = [
        f"route: {map_section}",
        f"days: {days}",
        f"seed: {seed}",
        f"minutes: {total_minutes}",
        f"blocked or full minutes: {blocked_minutes} ({blocked_minutes / total_minutes:.2%})",
        "",
        "starts:",
    ]
    for layer, count in sorted(starts.items()):
        lines.append(f"  {layer}: {count}")
    lines.extend(["", "random uptime:"])
    for layer, count in sorted(uptime.items()):
        lines.append(f"  {layer}: {count / total_minutes:.2%}")
    lines.extend(["", "top combined states:"])
    for combo, count in combos.most_common(12):
        lines.append(f"  {combo}: {count / total_minutes:.2%}")
    return "\n".join(lines)


def checked_config(path: Path) -> dict[str, Any]:
    config = load_config(path)
    errors = validate_config(config)
    if errors:
        for error in errors:
            print(f"error: {error}", file=sys.stderr)
        raise SystemExit(1)
    return config


def command_validate(args: argparse.Namespace) -> int:
    config = checked_config(args.config)
    print(
        "validated {layers} weather layers, {combos} combos, {rules} overworld rules".format(
            layers=len(config["layers"]),
            combos=len(config["combos"]),
            rules=len(config["overworldRules"]),
        )
    )
    return 0


def command_generate(args: argparse.Namespace) -> int:
    config = checked_config(args.config)
    content = generate_config_inc(config) + "\n"
    if args.check:
        existing = args.output.read_text(encoding="utf-8") if args.output.exists() else ""
        if existing != content:
            print(f"error: {args.output} is out of date", file=sys.stderr)
            return 1
        print(f"{args.output} is up to date")
        return 0
    args.output.write_text(content, encoding="utf-8")
    print(f"wrote {args.output}")
    return 0


def command_simulate(args: argparse.Namespace) -> int:
    config = checked_config(args.config)
    print(simulate(config, args.map_section, args.days, args.seed))
    return 0


def command_list_routes(args: argparse.Namespace) -> int:
    config = checked_config(args.config)
    for route in sorted({rule["mapSection"] for rule in config["overworldRules"]}):
        print(route)
    return 0


def add_config_arg(parser: argparse.ArgumentParser) -> None:
    parser.add_argument("--config", type=Path, default=CONFIG_JSON)
    parser.add_argument("--rules", type=Path, dest="config", help=argparse.SUPPRESS)


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="command", required=True)

    validate = subparsers.add_parser("validate")
    add_config_arg(validate)
    validate.set_defaults(func=command_validate)

    generate = subparsers.add_parser("generate")
    add_config_arg(generate)
    generate.add_argument("--output", type=Path, default=GENERATED_INC)
    generate.add_argument("--check", action="store_true")
    generate.set_defaults(func=command_generate)

    simulate_cmd = subparsers.add_parser("simulate")
    add_config_arg(simulate_cmd)
    simulate_cmd.add_argument("--map-section", required=True)
    simulate_cmd.add_argument("--days", type=int, default=1000)
    simulate_cmd.add_argument("--seed", type=int, default=1)
    simulate_cmd.set_defaults(func=command_simulate)

    list_routes = subparsers.add_parser("list-routes")
    add_config_arg(list_routes)
    list_routes.set_defaults(func=command_list_routes)
    return parser


def main() -> int:
    parser = build_parser()
    args = parser.parse_args()
    return args.func(args)


if __name__ == "__main__":
    raise SystemExit(main())
