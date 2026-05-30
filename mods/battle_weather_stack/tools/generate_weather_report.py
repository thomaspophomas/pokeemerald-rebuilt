#!/usr/bin/env python3
"""Generate a compact markdown report from the weather config."""

from __future__ import annotations

import argparse
import importlib.util
import sys
from collections import Counter
from pathlib import Path


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


def render_report() -> str:
    weather_rules = load_weather_rules()
    config = weather_rules.checked_config(weather_rules.CONFIG_JSON)
    route_counts = Counter(rule["mapSection"] for rule in config["overworldRules"])
    lines = [
        "# Battle Weather Stack Report",
        "",
        f"- Layers: {len(config['layers'])}",
        f"- Combos: {len(config['combos'])}",
        f"- Conflicts: {len(config['conflicts'])}",
        f"- Overworld rules: {len(config['overworldRules'])}",
        f"- Route/map sections with random rules: {len(route_counts)}",
        "",
        "## Layers",
        "",
    ]
    for layer in config["layers"]:
        battle = layer["battle"]
        overworld = layer["overworld"]
        lines.append(
            "- `{id}`: Weather Ball `{ball}`, visual `{visual}`, battle flags `{flags}`".format(
                id=layer["id"],
                ball=battle.get("weatherBallType", "TYPE_NORMAL"),
                visual=overworld["defaultVisual"],
                flags=", ".join(battle.get("battleWeatherFlags", [])) or "none",
            )
        )

    lines.extend(["", "## Random Rule Coverage", ""])
    for route, count in sorted(route_counts.items()):
        lines.append(f"- `{route}`: {count} rules")
    return "\n".join(lines) + "\n"


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path)
    return parser


def main() -> int:
    args = build_parser().parse_args()
    report = render_report()
    if args.output is None:
        print(report, end="")
    else:
        args.output.write_text(report, encoding="utf-8")
        print(f"wrote {args.output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
