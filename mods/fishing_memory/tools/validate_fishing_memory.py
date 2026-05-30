#!/usr/bin/env python3
"""Validate Fishing Memory config and action wiring."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path


MOD_ROOT = Path(__file__).resolve().parents[1]
CONFIG = MOD_ROOT / "tools" / "config" / "default.json"
PRESET_ROOT = MOD_ROOT / "tools" / "presets"
ACTIONS = MOD_ROOT / "fishing" / "actions.json"
SOURCE = MOD_ROOT / "src" / "fishing_memory.c"
GENERATOR = MOD_ROOT / "tools" / "generate_fishing_memory.py"
SIMULATOR = MOD_ROOT / "tools" / "simulate_fishing_memory.py"
MAX_LEVEL = 100
VALID_OUTCOMES = {"GOT_AWAY", "ON_HOOK"}
RODS = {"OLD", "GOOD", "SUPER"}


def fail(message: str) -> None:
    raise SystemExit(f"fishing_memory validation failed: {message}")


def require_int(value: object, label: str, minimum: int, maximum: int) -> int:
    if isinstance(value, bool) or not isinstance(value, int):
        fail(f"{label} must be an integer")
    if value < minimum or value > maximum:
        fail(f"{label} must be in [{minimum}, {maximum}]")
    return value


def validate_config(config: dict[str, object]) -> None:
    require_int(config.get("maxEarnedLevel"), "maxEarnedLevel", 1, MAX_LEVEL)
    require_int(config.get("safeMinLevel", 1), "safeMinLevel", 1, MAX_LEVEL)
    require_int(config.get("showFrames"), "showFrames", 1, 600)
    require_int(config.get("inputFrames"), "inputFrames", 1, 600)
    require_int(config.get("warningFrames", 0), "warningFrames", 0, 600)
    rod_settings = config.get("rodSettings")
    if not isinstance(rod_settings, dict) or set(rod_settings) != RODS:
        fail(f"rodSettings must define exactly {sorted(RODS)}")
    for rod, raw_setting in rod_settings.items():
        if not isinstance(raw_setting, dict):
            fail(f"rodSettings.{rod} must be an object")
        require_int(raw_setting.get("maxEarnedLevel", config["maxEarnedLevel"]), f"rodSettings.{rod}.maxEarnedLevel", 1, MAX_LEVEL)
        require_int(raw_setting.get("showFrames", config["showFrames"]), f"rodSettings.{rod}.showFrames", 1, 600)
        input_frames = require_int(raw_setting.get("inputFrames", config["inputFrames"]), f"rodSettings.{rod}.inputFrames", 1, 600)
        warning_frames = require_int(raw_setting.get("warningFrames", config.get("warningFrames", 0)), f"rodSettings.{rod}.warningFrames", 0, 600)
        if warning_frames >= input_frames:
            fail(f"rodSettings.{rod}.warningFrames must be lower than inputFrames")
    difficulty = config.get("targetLevelDifficulty", {})
    if not isinstance(difficulty, dict):
        fail("targetLevelDifficulty must be an object")
    if not isinstance(difficulty.get("enabled", False), bool):
        fail("targetLevelDifficulty.enabled must be boolean")
    require_int(difficulty.get("levelStep", 10), "targetLevelDifficulty.levelStep", 1, 100)
    require_int(difficulty.get("showFramePenalty", 0), "targetLevelDifficulty.showFramePenalty", 0, 600)
    require_int(difficulty.get("inputFramePenalty", 0), "targetLevelDifficulty.inputFramePenalty", 0, 600)
    require_int(difficulty.get("minimumShowFrames", 1), "targetLevelDifficulty.minimumShowFrames", 1, 600)
    require_int(difficulty.get("minimumInputFrames", 1), "targetLevelDifficulty.minimumInputFrames", 1, 600)

    if config.get("zeroScoreOutcome") not in VALID_OUTCOMES:
        fail("zeroScoreOutcome must be GOT_AWAY or ON_HOOK")
    if config.get("successOutcome") not in VALID_OUTCOMES:
        fail("successOutcome must be GOT_AWAY or ON_HOOK")

    stages = config.get("capStages")
    if not isinstance(stages, list) or not stages:
        fail("capStages must be a non-empty list")

    seen_flags: set[str] = set()
    base_count = 0
    previous_level = 0
    for index, stage in enumerate(stages):
        label = f"capStages[{index}]"
        if not isinstance(stage, dict):
            fail(f"{label} must be an object")
        level = require_int(stage.get("level"), f"{label}.level", 1, MAX_LEVEL)
        if level < previous_level:
            fail("capStages must not decrease")
        previous_level = level

        flag = stage.get("flag")
        if flag in (None, ""):
            base_count += 1
            continue
        if not isinstance(flag, str):
            fail(f"{label}.flag must be a string")
        if flag in seen_flags:
            fail(f"{label}.flag duplicates {flag}")
        seen_flags.add(flag)

    if base_count != 1:
        fail("capStages needs exactly one base stage without a flag")


def validate_actions(config: dict[str, object], actions: dict[str, object]) -> None:
    raw_actions = actions.get("actions")
    if not isinstance(raw_actions, list) or len(raw_actions) != len(RODS):
        fail("fishing/actions.json must define one action per rod")

    seen_rods: set[str] = set()
    for action in raw_actions:
        if not isinstance(action, dict):
            fail("action must be an object")
        rods = action.get("rods")
        if not isinstance(rods, list) or len(rods) != 1 or rods[0] not in RODS:
            fail("each action must target exactly one known rod")
        rod = rods[0]
        seen_rods.add(rod)
        setting = config["rodSettings"][rod]  # type: ignore[index]
        expected_params = [
            setting.get("maxEarnedLevel", config["maxEarnedLevel"]),
            setting.get("showFrames", config["showFrames"]),
            setting.get("inputFrames", config["inputFrames"]),
            setting.get("warningFrames", config.get("warningFrames", 0)),
        ]
        if action.get("hook") != "FishingMemory_Start":
            fail(f"{rod} action.hook must be FishingMemory_Start")
        if action.get("phases") != ["INPUT_WINDOW"]:
            fail(f"{rod} action.phases must be [INPUT_WINDOW]")
        if action.get("failureOutcome") != config.get("zeroScoreOutcome"):
            fail(f"{rod} action.failureOutcome must match zeroScoreOutcome")
        if action.get("timeoutFrames") != expected_params[2]:
            fail(f"{rod} action.timeoutFrames must match inputFrames")
        if action.get("params") != expected_params:
            fail(f"{rod} action.params must be {expected_params}")
    if seen_rods != RODS:
        fail(f"actions must cover rods {sorted(RODS)}")


def validate_source(config: dict[str, object]) -> None:
    text = SOURCE.read_text(encoding="utf-8")
    for stage in config["capStages"]:  # type: ignore[index]
        if not isinstance(stage, dict):
            continue
        flag = stage.get("flag")
        if isinstance(flag, str) and flag not in (MOD_ROOT / "src" / "generated_cap_stages.inc").read_text(encoding="utf-8"):
            fail(f"{flag} is configured but not present in generated_cap_stages.inc")
    if "ModFlag_Get" not in text:
        fail("fishing_memory.c must use ModFlag_Get for cap stages")
    if "FishingMemory_PrintScore" not in text:
        fail("fishing_memory.c must print score feedback")


def main() -> int:
    config = json.loads(CONFIG.read_text(encoding="utf-8"))
    actions = json.loads(ACTIONS.read_text(encoding="utf-8"))
    if not isinstance(config, dict):
        fail("config/default.json must contain an object")
    if not isinstance(actions, dict):
        fail("fishing/actions.json must contain an object")

    validate_config(config)
    for preset in sorted(PRESET_ROOT.glob("*.json")):
        preset_data = json.loads(preset.read_text(encoding="utf-8"))
        if not isinstance(preset_data, dict):
            fail(f"{preset.relative_to(MOD_ROOT)} must contain an object")
        validate_config(preset_data)
    validate_actions(config, actions)
    validate_source(config)
    for command in (
        [sys.executable, str(GENERATOR), "--check"],
        [sys.executable, str(SIMULATOR), "--self-test"],
    ):
        result = subprocess.run(command, cwd=MOD_ROOT, check=False)
        if result.returncode != 0:
            return result.returncode
    print("fishing_memory validation OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
