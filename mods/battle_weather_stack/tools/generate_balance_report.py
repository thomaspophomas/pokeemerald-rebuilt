#!/usr/bin/env python3
"""Generate per-route battle weather balance artifacts."""

from __future__ import annotations

import argparse
import importlib.util
import json
import re
import sys
from collections import Counter
from pathlib import Path
from typing import Any


MOD_ROOT = Path(__file__).resolve().parents[1]
WEATHER_RULES = MOD_ROOT / "src" / "tools" / "weather_rules.py"
OUTPUT_JSON = MOD_ROOT / "tools" / "docs" / "balance_report.json"
OUTPUT_MD = MOD_ROOT / "tools" / "docs" / "balance_report.md"
STATE_RE = re.compile(r"^\s+([^:]+):\s+([0-9.]+)%$")
sys.dont_write_bytecode = True


def load_weather_rules():
    spec = importlib.util.spec_from_file_location("weather_rules", WEATHER_RULES)
    if spec is None or spec.loader is None:
        raise RuntimeError("failed to load weather_rules.py")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def parse_top_states(simulation: str) -> list[dict[str, Any]]:
    states: list[dict[str, Any]] = []
    in_states = False
    for line in simulation.splitlines():
        if line == "top combined states:":
            in_states = True
            continue
        if not in_states:
            continue
        match = STATE_RE.match(line)
        if match is None:
            continue
        states.append({"state": match.group(1), "percent": float(match.group(2))})
    return states


def route_rule_summary(config: dict[str, Any]) -> dict[str, Counter[str]]:
    result: dict[str, Counter[str]] = {}
    for rule in config["overworldRules"]:
        result.setdefault(rule["mapSection"], Counter())[rule["layer"]] += int(rule["chancePerMinute"])
    return result


def build_report(days: int, seed: int) -> dict[str, Any]:
    weather_rules = load_weather_rules()
    config = weather_rules.checked_config(weather_rules.CONFIG_JSON)
    summaries = route_rule_summary(config)
    routes = []
    for route in sorted(summaries):
        simulation = weather_rules.simulate(config, route, days, seed)
        top_states = parse_top_states(simulation)
        routes.append(
            {
                "mapSection": route,
                "ruleChanceByLayer": dict(sorted(summaries[route].items())),
                "topCombinedStates": top_states[:8],
            }
        )
    return {
        "schemaVersion": 1,
        "days": days,
        "seed": seed,
        "routeCount": len(routes),
        "routes": routes,
    }


def render_markdown(report: dict[str, Any]) -> str:
    lines = [
        "# Battle Weather Stack Balance Report",
        "",
        f"- Days simulated per route: {report['days']}",
        f"- Seed: {report['seed']}",
        f"- Routes: {report['routeCount']}",
        "",
        "| Route | Rule Chance By Layer | Dominant Combined States |",
        "| --- | --- | --- |",
    ]
    for route in report["routes"]:
        chance = ", ".join(f"{layer}:{value}" for layer, value in route["ruleChanceByLayer"].items())
        states = ", ".join(f"{item['state']} {item['percent']:.2f}%" for item in route["topCombinedStates"][:4])
        lines.append(f"| `{route['mapSection']}` | {chance or 'none'} | {states or 'none'} |")
    return "\n".join(lines) + "\n"


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--days", type=int, default=10)
    parser.add_argument("--seed", type=int, default=1)
    parser.add_argument("--json-output", type=Path, default=OUTPUT_JSON)
    parser.add_argument("--md-output", type=Path, default=OUTPUT_MD)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()

    report = build_report(args.days, args.seed)
    json_content = json.dumps(report, indent=2, ensure_ascii=True) + "\n"
    md_content = render_markdown(report)

    if args.check:
        current_json = args.json_output.read_text(encoding="utf-8") if args.json_output.exists() else ""
        current_md = args.md_output.read_text(encoding="utf-8") if args.md_output.exists() else ""
        if current_json != json_content or current_md != md_content:
            raise SystemExit("battle weather balance report is out of date")
        print("battle_weather_stack balance report is up to date")
        return 0

    args.json_output.parent.mkdir(parents=True, exist_ok=True)
    args.json_output.write_text(json_content, encoding="utf-8")
    args.md_output.write_text(md_content, encoding="utf-8")
    print(f"wrote {args.json_output}")
    print(f"wrote {args.md_output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
