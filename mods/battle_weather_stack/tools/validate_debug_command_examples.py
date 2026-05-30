#!/usr/bin/env python3
"""Validate battle weather debug/admin command examples."""

from __future__ import annotations

import json
from pathlib import Path


MOD_ROOT = Path(__file__).resolve().parents[1]
COMMANDS = MOD_ROOT / "tools" / "debug_command_examples.json"
HEADER = MOD_ROOT / "src" / "battle_weather_stack.h"


def fail(message: str) -> None:
    raise SystemExit(f"battle_weather_stack debug command validation failed: {message}")


def main() -> int:
    data = json.loads(COMMANDS.read_text(encoding="utf-8"))
    commands = data.get("commands") if isinstance(data, dict) else None
    if not isinstance(commands, list) or len(commands) < 4:
        fail("commands must contain at least four examples")
    header = HEADER.read_text(encoding="utf-8")
    seen: set[str] = set()
    for index, command in enumerate(commands):
        if not isinstance(command, dict):
            fail(f"commands[{index}] must be an object")
        command_id = command.get("id")
        if not isinstance(command_id, str) or not command_id:
            fail(f"commands[{index}].id is required")
        if command_id in seen:
            fail(f"duplicate command id {command_id}")
        seen.add(command_id)
        call = command.get("call")
        if not isinstance(call, str) or not call.startswith("BattleWeatherStack_"):
            fail(f"{command_id}.call must start with BattleWeatherStack_")
        function = call.split("(", 1)[0]
        if function not in header:
            fail(f"{command_id}.call references function missing from header: {function}")
    print(f"battle_weather_stack debug command examples OK: {len(commands)} commands")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
