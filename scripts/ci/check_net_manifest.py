#!/usr/bin/env python3
"""Verify that the tracked multiplayer net manifest matches C constants."""

from __future__ import annotations

import json
import importlib.util
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
CONSTANTS = ROOT / "include" / "multiplayer" / "constants.h"
TRANSPORT = ROOT / "include" / "multiplayer" / "transport.h"
MAILBOX = ROOT / "include" / "multiplayer" / "bridge_mailbox.h"
PROFILE = ROOT / "include" / "mod" / "runtime_profile.h"
CATALOG = ROOT / "include" / "mod" / "catalog.h"
MODGEN = ROOT / "scripts" / "modgen.py"
MANIFEST = ROOT / "docs" / "multiplayer_net_manifest.json"


def read_define(path: Path, name: str) -> str:
    text = path.read_text(encoding="utf-8")
    match = re.search(rf"^\s*#define\s+{re.escape(name)}\s+(.+?)\s*(?://.*)?$", text, re.MULTILINE)
    if not match:
        raise SystemExit(f"Missing {name} in {path}")
    return match.group(1).strip()


def parse_int(value: str) -> int:
    return int(value, 0)


def parse_string(value: str) -> str:
    match = re.fullmatch(r'"([^"]*)"', value.strip())
    if not match:
        raise SystemExit(f"Expected string define, got {value!r}")
    return match.group(1)


def expect(name: str, actual: object, expected: object) -> None:
    if actual != expected:
        raise SystemExit(f"{name} mismatch: manifest={actual!r}, constants={expected!r}")


def calc_mod_catalog_hash() -> int:
    spec = importlib.util.spec_from_file_location("modgen", MODGEN)
    if spec is None or spec.loader is None:
        raise SystemExit(f"Cannot load {MODGEN}")
    modgen = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(modgen)

    mods = modgen.load_mods(ROOT)
    weather = modgen.collect_weather(mods)
    sprite_assets = modgen.collect_sprite_assets(mods)
    language_texts = modgen.collect_language_texts(mods)
    engines = modgen.collect_engines(mods)
    npcs = modgen.collect_npcs(mods)
    badge_effects = modgen.collect_badge_effects(mods)
    catalog_entries = modgen.collect_catalog_entries(weather, sprite_assets, language_texts, engines, npcs, badge_effects)
    return modgen.calc_catalog_hash(catalog_entries)


def main() -> None:
    manifest = json.loads(MANIFEST.read_text(encoding="utf-8"))

    expect("protocolVersion", manifest["protocolVersion"], parse_int(read_define(CONSTANTS, "NET_PROTOCOL_VERSION")))
    expect("emulatorBridgeVersion", manifest["emulatorBridgeVersion"], parse_int(read_define(CONSTANTS, "NET_EMULATOR_BRIDGE_VERSION")))
    expect("buildId", manifest["buildId"], f"0x{parse_int(read_define(CONSTANTS, 'NET_PROTOCOL_BUILD_ID')):08X}")
    expect("rulesetHash", manifest["rulesetHash"], f"0x{parse_int(read_define(CONSTANTS, 'NET_RULESET_HASH')):08X}")
    expect("profileProtocolVersion", manifest["profileProtocolVersion"], parse_int(read_define(PROFILE, "MOD_RUNTIME_PROFILE_PROTOCOL_VERSION")))
    expect("profileCapabilityHash", manifest["profileCapabilityHash"], f"0x{parse_int(read_define(PROFILE, 'MOD_RUNTIME_PROFILE_CAPABILITY_HASH')):08X}")
    expect("modCatalogSchemaHash", manifest["modCatalogSchemaHash"], f"0x{parse_int(read_define(CATALOG, 'MOD_CATALOG_SCHEMA_HASH')):08X}")
    expect("modCatalogHash", manifest["modCatalogHash"], f"0x{calc_mod_catalog_hash():08X}")
    expect("transportModeValue", manifest["transportModeValue"], parse_int(read_define(CONSTANTS, "NET_TRANSPORT_MODE_SERVER_BRIDGE")))
    expect("maxNetPlayers", manifest["maxNetPlayers"], parse_int(read_define(CONSTANTS, "MAX_NET_PLAYERS")))
    expect("reliableQueueSize", manifest["reliableQueueSize"], parse_int(read_define(CONSTANTS, "NET_RELIABLE_QUEUE_SIZE")))
    expect("packetPayloadSize", manifest["packetPayloadSize"], parse_int(read_define(TRANSPORT, "NET_TRANSPORT_PACKET_PAYLOAD_SIZE")))
    expect("commitLogSize", manifest["commitLogSize"], parse_int(read_define(CONSTANTS, "NET_COMMIT_LOG_SIZE")))
    if manifest.get("transportMode") != "server_bridge":
        raise SystemExit("transportMode must be server_bridge")
    if manifest.get("authoritativeServer") is not True:
        raise SystemExit("authoritativeServer must be true")
    mailbox = manifest.get("mailbox")
    if not isinstance(mailbox, dict):
        raise SystemExit("manifest mailbox object is required")
    expect("mailbox.symbol", mailbox.get("symbol"), parse_string(read_define(MAILBOX, "NET_EMULATOR_MAILBOX_SYMBOL_NAME")))
    expect("mailbox.struct", mailbox.get("struct"), "NetEmulatorBridgeBuffer")
    expect("mailbox.memoryDomain", mailbox.get("memoryDomain"), parse_string(read_define(MAILBOX, "NET_EMULATOR_MAILBOX_MEMORY_DOMAIN")))
    expect("mailbox.minAddress", mailbox.get("minAddress"), "0x02000000")
    expect("mailbox.maxAddress", mailbox.get("maxAddress"), "0x0203FFFF")
    expect("mailbox.maxSize", mailbox.get("maxSize"), parse_int(read_define(MAILBOX, "NET_EMULATOR_MAILBOX_MAX_SIZE")))
    if "NET_EMULATOR_SHARED_BASE" in CONSTANTS.read_text(encoding="utf-8"):
        raise SystemExit("NET_EMULATOR_SHARED_BASE must not be reintroduced")

    print("Multiplayer net manifest OK")


if __name__ == "__main__":
    main()
