#!/usr/bin/env python3
"""Reference emulator bridge for the ROM multiplayer shared-memory contract.

The default adapter is file-backed so CI and local development can exercise the
bridge without depending on a specific emulator memory API. A production adapter
can replace BridgeMemory with direct emulator shared-memory access.
"""

from __future__ import annotations

import argparse
import os
import socket
import struct
import time
from dataclasses import dataclass
from pathlib import Path


BRIDGE_MAGIC = 0x3844454E
BRIDGE_VERSION = 6
TRANSPORT_MODE_SERVER_BRIDGE = 1
DEFAULT_SIZE = 64 * 1024
DEFAULT_PORT = 7777
NET_SERVER_PROFILE_COUNT = 3
NET_RELIABLE_QUEUE_SIZE = 16
NET_TRANSPORT_PACKET_PAYLOAD_SIZE = 256
NET_CONNECTION_STATUS_CONNECTING = 2
NET_CONNECTION_STATUS_CONNECTED = 3
NET_CONNECTION_STATUS_DISCONNECTED = 8
NET_CONNECTION_STATUS_BACKPRESSURE = 9
NET_CONNECTION_STATUS_BAD_SERVER_CONFIG = 10

FRAME_CLIENT_HELLO = 1
FRAME_ROM_PACKET = 2
FRAME_SERVER_VIEW = 3
FRAME_DISCONNECT = 4
FRAME_SERVER_CONFIG = 5
FRAME_SERVER_SESSION = 6
MAX_TCP_FRAME_SIZE = 64 * 1024

HEADER = struct.Struct("<I H 6B 16I B 3x")
ENVELOPE = struct.Struct("<I H H I I I B B B B I I H H")
SERVER_SESSION = struct.Struct("<B B B B I I I I I I")
SERVER_CONFIG = struct.Struct("<I H H B B H")
SERVER_PROFILE = struct.Struct("<B B H 4B 13s B")
PACKET_SLOT_SIZE = ENVELOPE.size + NET_TRANSPORT_PACKET_PAYLOAD_SIZE
SERVER_CONFIG_MAGIC = 0x4E534346
SERVER_CONFIG_VERSION = 1
SERVER_CONFIG_SIZE = 80
NET_PLAYER_SNAPSHOT_SIZE = 52
MULTIPLAYER_SUBSESSION_SIZE = 12
MAX_NET_PLAYERS = 8
MAX_NET_SUBSESSIONS = 4

SERVER_CONFIG_OFFSET = HEADER.size
SERVER_PLAYERS_OFFSET = SERVER_CONFIG_OFFSET + SERVER_CONFIG_SIZE
SERVER_SUBSESSIONS_OFFSET = SERVER_PLAYERS_OFFSET + (NET_PLAYER_SNAPSHOT_SIZE * MAX_NET_PLAYERS)
LOCAL_SNAPSHOT_OFFSET = SERVER_SUBSESSIONS_OFFSET + (MULTIPLAYER_SUBSESSION_SIZE * MAX_NET_SUBSESSIONS)
LATEST_UNRELIABLE_OFFSET = LOCAL_SNAPSHOT_OFFSET + NET_PLAYER_SNAPSHOT_SIZE
RELIABLE_OUTBOUND_OFFSET = LATEST_UNRELIABLE_OFFSET + PACKET_SLOT_SIZE
RELIABLE_INBOUND_OFFSET = RELIABLE_OUTBOUND_OFFSET + (PACKET_SLOT_SIZE * NET_RELIABLE_QUEUE_SIZE)

HEADER_FIELDS = (
    "magic",
    "version",
    "connected",
    "local_player_id",
    "host_player_id",
    "player_count",
    "transport_mode",
    "reserved",
    "session_id",
    "session_epoch",
    "player_token",
    "join_nonce",
    "bridge_tick",
    "server_clock_seconds",
    "view_sequence",
    "local_snapshot_sequence",
    "reliable_outbound_head",
    "reliable_outbound_tail",
    "reliable_inbound_head",
    "reliable_inbound_tail",
    "latest_unreliable_sequence",
    "reliable_outbound_ack",
    "server_config_sequence",
    "server_config_ack_sequence",
    "connection_status",
)


@dataclass(frozen=True)
class Endpoint:
    host: str
    port: int


@dataclass
class BridgeState:
    latest_unreliable_sequence: int = 0
    server_config_sequence: int = 0
    endpoint: Endpoint | None = None


class BridgeInstanceLock:
    def __init__(self, memory_path: Path) -> None:
        self.path = memory_path.with_suffix(memory_path.suffix + ".lock")
        self.fd: int | None = None

    def __enter__(self) -> "BridgeInstanceLock":
        self.path.parent.mkdir(parents=True, exist_ok=True)
        self.fd = os.open(self.path, os.O_CREAT | os.O_EXCL | os.O_RDWR)
        os.write(self.fd, str(os.getpid()).encode("ascii"))
        return self

    def __exit__(self, exc_type, exc, tb) -> None:
        if self.fd is not None:
            os.close(self.fd)
            self.fd = None
        try:
            self.path.unlink()
        except FileNotFoundError:
            pass


class BridgeMemory:
    def __init__(self, path: Path, size: int = DEFAULT_SIZE) -> None:
        self.path = path
        self.size = size
        if not path.exists() or path.stat().st_size < size:
            path.parent.mkdir(parents=True, exist_ok=True)
            with path.open("wb") as handle:
                handle.truncate(size)

    def read(self, offset: int, size: int) -> bytes:
        with self.path.open("rb") as handle:
            handle.seek(offset)
            return handle.read(size)

    def write(self, offset: int, data: bytes) -> None:
        with self.path.open("r+b") as handle:
            handle.seek(offset)
            handle.write(data)

    def init_header(self) -> None:
        raw = bytearray(self.read(0, HEADER.size))
        values = list(HEADER.unpack(raw))
        values[0] = BRIDGE_MAGIC
        values[1] = BRIDGE_VERSION
        values[2] = 0
        values[3] = 0
        values[4] = 0
        values[5] = 0
        values[6] = TRANSPORT_MODE_SERVER_BRIDGE
        for index in range(8, 14):
            values[index] = 0
        values[24] = NET_CONNECTION_STATUS_CONNECTING
        self.write(0, HEADER.pack(*values))

    def read_header(self) -> dict[str, int]:
        return dict(zip(HEADER_FIELDS, HEADER.unpack(self.read(0, HEADER.size))))

    def write_header(self, **updates: int) -> None:
        values = self.read_header()
        values.update(updates)
        self.write(0, HEADER.pack(*(values[name] for name in HEADER_FIELDS)))

    def read_packet_slot(self, offset: int) -> bytes:
        return self.read(offset, PACKET_SLOT_SIZE)

    def read_server_config(self) -> bytes:
        return self.read(SERVER_CONFIG_OFFSET, SERVER_CONFIG_SIZE)

    def ack_reliable_outbound(self, sequence: int) -> None:
        self.write_header(reliable_outbound_ack=sequence)

    def push_reliable_inbound(self, slot: bytes) -> bool:
        header = self.read_header()
        head = header["reliable_inbound_head"] % NET_RELIABLE_QUEUE_SIZE
        tail = header["reliable_inbound_tail"] % NET_RELIABLE_QUEUE_SIZE
        next_head = (head + 1) % NET_RELIABLE_QUEUE_SIZE

        if next_head == tail:
            self.write_header(connection_status=NET_CONNECTION_STATUS_BACKPRESSURE)
            return False

        self.write(RELIABLE_INBOUND_OFFSET + (head * PACKET_SLOT_SIZE), slot[:PACKET_SLOT_SIZE].ljust(PACKET_SLOT_SIZE, b"\0"))
        self.write_header(reliable_inbound_head=next_head)
        return True

    def write_server_view(self, payload: bytes) -> None:
        max_size = (NET_PLAYER_SNAPSHOT_SIZE * MAX_NET_PLAYERS) + (MULTIPLAYER_SUBSESSION_SIZE * MAX_NET_SUBSESSIONS)
        header = self.read_header()
        view_sequence = (header["view_sequence"] + 1) | 1

        self.write_header(view_sequence=view_sequence)
        self.write(SERVER_PLAYERS_OFFSET, payload[:max_size].ljust(max_size, b"\0"))
        self.write_header(view_sequence=view_sequence + 1)

    def apply_server_session(self, payload: bytes) -> None:
        if len(payload) < SERVER_SESSION.size:
            return

        connected, local_player_id, host_player_id, player_count, session_id, session_epoch, player_token, join_nonce, tick, clock = SERVER_SESSION.unpack(
            payload[: SERVER_SESSION.size]
        )
        self.write_header(
            connected=connected,
            local_player_id=local_player_id,
            host_player_id=host_player_id,
            player_count=player_count,
            session_id=session_id,
            session_epoch=session_epoch,
            player_token=player_token,
            join_nonce=join_nonce,
            bridge_tick=tick,
            server_clock_seconds=clock,
            connection_status=NET_CONNECTION_STATUS_CONNECTED if connected else NET_CONNECTION_STATUS_CONNECTING,
        )


def packet_sequence(slot: bytes) -> int:
    if len(slot) < ENVELOPE.size:
        return 0
    return ENVELOPE.unpack(slot[: ENVELOPE.size])[10]


def endpoint_from_config_payload(payload: bytes) -> Endpoint | None:
    if len(payload) < SERVER_CONFIG_SIZE:
        return None

    magic, version, size, selected_slot, _status, _revision = SERVER_CONFIG.unpack(
        payload[: SERVER_CONFIG.size]
    )
    if magic != SERVER_CONFIG_MAGIC or version != SERVER_CONFIG_VERSION or size != SERVER_CONFIG_SIZE:
        return None
    if selected_slot >= NET_SERVER_PROFILE_COUNT:
        return None

    offset = SERVER_CONFIG.size + (selected_slot * SERVER_PROFILE.size)
    active, _flags, port, octet1, octet2, octet3, octet4, _label, _reserved = SERVER_PROFILE.unpack(
        payload[offset : offset + SERVER_PROFILE.size]
    )
    if not active or port == 0:
        return None

    return Endpoint(f"{octet1}.{octet2}.{octet3}.{octet4}", port)


def endpoint_from_config(memory: BridgeMemory) -> Endpoint | None:
    return endpoint_from_config_payload(memory.read_server_config())


def poll_rom(memory: BridgeMemory, sock: socket.socket, state: BridgeState, forced_endpoint: bool = False) -> bool:
    header = memory.read_header()

    config_sequence = header["server_config_sequence"]
    if config_sequence != 0 and config_sequence != state.server_config_sequence and config_sequence % 2 == 0:
        config_payload = memory.read_server_config()
        send_frame(sock, FRAME_SERVER_CONFIG, config_payload)
        memory.write_header(server_config_ack_sequence=config_sequence)
        state.server_config_sequence = config_sequence
        if not forced_endpoint and endpoint_from_config_payload(config_payload) != state.endpoint:
            return True

    latest_sequence = header["latest_unreliable_sequence"]
    if latest_sequence != 0 and latest_sequence != state.latest_unreliable_sequence:
        slot = memory.read_packet_slot(LATEST_UNRELIABLE_OFFSET)
        send_frame(sock, FRAME_ROM_PACKET, slot)
        state.latest_unreliable_sequence = latest_sequence

    head = header["reliable_outbound_head"] % NET_RELIABLE_QUEUE_SIZE
    tail = header["reliable_outbound_tail"] % NET_RELIABLE_QUEUE_SIZE
    while tail != head:
        slot = memory.read_packet_slot(RELIABLE_OUTBOUND_OFFSET + (tail * PACKET_SLOT_SIZE))
        sequence = packet_sequence(slot)
        if sequence == 0:
            break
        send_frame(sock, FRAME_ROM_PACKET, slot)
        memory.ack_reliable_outbound(sequence)
        tail = (tail + 1) % NET_RELIABLE_QUEUE_SIZE

    return False


def parse_endpoint(value: str) -> Endpoint:
    if ":" not in value:
        return Endpoint(value, DEFAULT_PORT)
    host, port = value.rsplit(":", 1)
    return Endpoint(host, int(port))


def recv_exact(sock: socket.socket, size: int) -> bytes:
    chunks: list[bytes] = []
    remaining = size
    while remaining:
        chunk = sock.recv(remaining)
        if not chunk:
            raise ConnectionError("socket closed")
        chunks.append(chunk)
        remaining -= len(chunk)
    return b"".join(chunks)


def send_frame(sock: socket.socket, frame_type: int, payload: bytes) -> None:
    body = bytes([frame_type]) + payload
    sock.sendall(struct.pack(">I", len(body)) + body)


def recv_frame(sock: socket.socket) -> tuple[int, bytes]:
    (size,) = struct.unpack(">I", recv_exact(sock, 4))
    if size == 0 or size > MAX_TCP_FRAME_SIZE:
        raise ConnectionError(f"invalid bridge frame size: {size}")
    body = recv_exact(sock, size)
    return body[0], body[1:]


def choose_endpoint(memory: BridgeMemory, default_endpoint: Endpoint | None, force_endpoint: bool) -> Endpoint | None:
    if force_endpoint:
        return default_endpoint
    return endpoint_from_config(memory) or default_endpoint


def run_bridge(memory: BridgeMemory, default_endpoint: Endpoint | None, reconnect_delay: float, force_endpoint: bool = False) -> None:
    memory.init_header()
    with BridgeInstanceLock(memory.path):
        while True:
            endpoint = choose_endpoint(memory, default_endpoint, force_endpoint)
            if endpoint is None:
                memory.write_header(connected=0, connection_status=NET_CONNECTION_STATUS_BAD_SERVER_CONFIG)
                time.sleep(reconnect_delay)
                continue

            state = BridgeState(endpoint=endpoint)
            try:
                with socket.create_connection((endpoint.host, endpoint.port), timeout=5.0) as sock:
                    sock.settimeout(0.1)
                    memory.init_header()
                    send_frame(sock, FRAME_CLIENT_HELLO, b"bridge-v6")
                    while True:
                        if poll_rom(memory, sock, state, force_endpoint):
                            memory.write_header(connected=0, connection_status=NET_CONNECTION_STATUS_CONNECTING)
                            break
                        try:
                            frame_type, payload = recv_frame(sock)
                        except socket.timeout:
                            time.sleep(0.01)
                            continue
                        if frame_type == FRAME_SERVER_VIEW:
                            memory.write_server_view(payload)
                        elif frame_type == FRAME_ROM_PACKET:
                            memory.push_reliable_inbound(payload)
                        elif frame_type == FRAME_SERVER_SESSION:
                            memory.apply_server_session(payload)
                        elif frame_type == FRAME_DISCONNECT:
                            memory.write_header(connected=0, connection_status=NET_CONNECTION_STATUS_DISCONNECTED)
                            break
                        else:
                            continue
            except OSError:
                memory.write_header(connected=0, connection_status=NET_CONNECTION_STATUS_DISCONNECTED)
                time.sleep(reconnect_delay)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--memory-file", required=True, type=Path)
    parser.add_argument("--server", help="Default IPv4:port if the ROM has not published an active profile")
    parser.add_argument("--force-server", action="store_true", help="Ignore the ROM-selected server profile")
    parser.add_argument("--reconnect-delay", type=float, default=2.0)
    args = parser.parse_args()

    if os.name == "nt" and args.memory_file.is_absolute() is False:
        args.memory_file = Path.cwd() / args.memory_file
    default_endpoint = parse_endpoint(args.server) if args.server else None
    run_bridge(BridgeMemory(args.memory_file), default_endpoint, args.reconnect_delay, args.force_server)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
