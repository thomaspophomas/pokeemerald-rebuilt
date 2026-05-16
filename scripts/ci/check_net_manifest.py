#!/usr/bin/env python3
"""Verify that the tracked multiplayer net manifest matches C constants."""

from __future__ import annotations

import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
CONSTANTS = ROOT / "include" / "multiplayer" / "constants.h"
MANIFEST = ROOT / "docs" / "multiplayer_net_manifest.json"


def read_define(name: str) -> str:
    text = CONSTANTS.read_text(encoding="utf-8")
    match = re.search(rf"^\s*#define\s+{re.escape(name)}\s+(.+?)\s*(?://.*)?$", text, re.MULTILINE)
    if not match:
        raise SystemExit(f"Missing {name} in {CONSTANTS}")
    return match.group(1).strip()


def parse_int(value: str) -> int:
    return int(value, 0)


def expect(name: str, actual: object, expected: object) -> None:
    if actual != expected:
        raise SystemExit(f"{name} mismatch: manifest={actual!r}, constants={expected!r}")


def main() -> None:
    manifest = json.loads(MANIFEST.read_text(encoding="utf-8"))

    expect("protocolVersion", manifest["protocolVersion"], parse_int(read_define("NET_PROTOCOL_VERSION")))
    expect("emulatorBridgeVersion", manifest["emulatorBridgeVersion"], parse_int(read_define("NET_EMULATOR_BRIDGE_VERSION")))
    expect("buildId", manifest["buildId"], f"0x{parse_int(read_define('NET_PROTOCOL_BUILD_ID')):08X}")
    expect("rulesetHash", manifest["rulesetHash"], f"0x{parse_int(read_define('NET_RULESET_HASH')):08X}")
    expect("transportModeValue", manifest["transportModeValue"], parse_int(read_define("NET_TRANSPORT_MODE_SERVER_BRIDGE")))
    expect("maxNetPlayers", manifest["maxNetPlayers"], parse_int(read_define("MAX_NET_PLAYERS")))
    expect("reliableQueueSize", manifest["reliableQueueSize"], parse_int(read_define("NET_RELIABLE_QUEUE_SIZE")))
    expect("commitLogSize", manifest["commitLogSize"], parse_int(read_define("NET_COMMIT_LOG_SIZE")))
    if manifest.get("transportMode") != "server_bridge":
        raise SystemExit("transportMode must be server_bridge")
    if manifest.get("authoritativeServer") is not True:
        raise SystemExit("authoritativeServer must be true")

    print("Multiplayer net manifest OK")


if __name__ == "__main__":
    main()
