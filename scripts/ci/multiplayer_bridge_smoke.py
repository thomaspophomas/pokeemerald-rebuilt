#!/usr/bin/env python3
"""Smoke checks for the reference multiplayer bridge memory contract."""

from __future__ import annotations

import importlib.util
import sys
import tempfile
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
BRIDGE_PATH = ROOT / "tools" / "multiplayer_bridge" / "bridge.py"


def load_bridge():
    spec = importlib.util.spec_from_file_location("multiplayer_bridge", BRIDGE_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError("could not load multiplayer bridge")
    module = importlib.util.module_from_spec(spec)
    sys.modules["multiplayer_bridge"] = module
    spec.loader.exec_module(module)
    return module


def pack_profile(bridge, active: int, port: int, ipv4: tuple[int, int, int, int], label: bytes = b"TAILSCALE") -> bytes:
    return bridge.SERVER_PROFILE.pack(active, 0, port, *ipv4, label[:13].ljust(13, b"\0"), 0)


def pack_config(bridge, selected_slot: int, active: bool = True) -> bytes:
    header = bridge.SERVER_CONFIG.pack(
        bridge.SERVER_CONFIG_MAGIC,
        bridge.SERVER_CONFIG_VERSION,
        bridge.SERVER_CONFIG_SIZE,
        selected_slot,
        0,
        1,
    )
    profiles = [
        pack_profile(bridge, 1 if active else 0, 7777, (100, 64, 0, 1)),
        pack_profile(bridge, 0, 0, (0, 0, 0, 0)),
        pack_profile(bridge, 0, 0, (0, 0, 0, 0)),
    ]
    return (header + b"".join(profiles)).ljust(bridge.SERVER_CONFIG_SIZE, b"\0")


def main() -> int:
    bridge = load_bridge()

    with tempfile.TemporaryDirectory() as tmp:
        memory = bridge.BridgeMemory(Path(tmp) / "bridge.bin")
        memory.init_header()
        header = memory.read_header()
        assert bridge.HEADER.size == 80
        assert bridge.SERVER_CONFIG_SIZE == 80
        assert header["magic"] == bridge.BRIDGE_MAGIC
        assert header["version"] == bridge.BRIDGE_VERSION
        assert header["transport_mode"] == bridge.TRANSPORT_MODE_SERVER_BRIDGE
        assert header["connection_status"] == bridge.NET_CONNECTION_STATUS_CONNECTING

        session_payload = bridge.SERVER_SESSION.pack(1, 2, 0, 3, 100, 200, 300, 400, 500, 600)
        memory.apply_server_session(session_payload)
        header = memory.read_header()
        assert header["connected"] == 1
        assert header["local_player_id"] == 2
        assert header["session_epoch"] == 200
        assert header["player_token"] == 300
        assert header["connection_status"] == bridge.NET_CONNECTION_STATUS_CONNECTED

        slot = bytes(bridge.PACKET_SLOT_SIZE)
        assert memory.push_reliable_inbound(slot)
        header = memory.read_header()
        assert header["reliable_inbound_head"] == 1

        memory.write_server_view(bytes(bridge.NET_PLAYER_SNAPSHOT_SIZE * bridge.MAX_NET_PLAYERS))
        header = memory.read_header()
        assert header["view_sequence"] % 2 == 0

        config = pack_config(bridge, 0)
        memory.write(bridge.SERVER_CONFIG_OFFSET, config)
        endpoint = bridge.endpoint_from_config(memory)
        assert endpoint == bridge.Endpoint("100.64.0.1", 7777)

        memory.write(bridge.SERVER_CONFIG_OFFSET, pack_config(bridge, 0, active=False))
        assert bridge.endpoint_from_config(memory) is None

    print("multiplayer bridge smoke OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
