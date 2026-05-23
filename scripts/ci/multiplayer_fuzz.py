#!/usr/bin/env python3
"""Lightweight host-side invariant checks for the multiplayer hardening layer."""

from __future__ import annotations

from dataclasses import dataclass
import random


MAX_NET_PLAYERS = 8
MAX_NET_BATTLE_PLAYERS = 4
MAX_NET_SUBSESSIONS = 4
NET_SUBSESSION_NONE = 0
NET_PACKET_NONE = 0
NET_PACKET_COUNT = 30
NET_PLAYER_SNAPSHOT_TTL_FRAMES = 60 * 5
NET_PLAYER_SNAPSHOT_FUTURE_SKEW_FRAMES = 30
NET_PLAYER_STALE_FRAMES = 60 * 2
NET_PLAYER_DISCONNECT_FRAMES = 60 * 10
NET_PLAYER_COORD_MIN = -512
NET_PLAYER_COORD_MAX = 8191
NET_PROTOCOL_VERSION = 2
NET_EMULATOR_BRIDGE_VERSION = 5
NET_TRANSPORT_MODE_SERVER_BRIDGE = 1
NET_COMMIT_LOG_SIZE = 32
NET_RELIABLE_QUEUE_SIZE = 16
NET_TRANSPORT_PACKET_PAYLOAD_SIZE = 128
NET_PENDING_TX_COUNT = 16
MOD_RUNTIME_PROFILE_PROTOCOL_VERSION = 1
MOD_RUNTIME_PROFILE_CAPABILITY_HASH = 0x00000006
MOD_RUNTIME_PROFILE_CAP_TEXT = 1 << 0
MOD_RUNTIME_PROFILE_CAP_WEATHER = 1 << 1
MOD_RUNTIME_PROFILE_CAP_ENGINE = 1 << 2
MOD_RUNTIME_PROFILE_CAP_NPC = 1 << 3
MOD_RUNTIME_PROFILE_CAP_ASSET_REF = 1 << 4
MOD_RUNTIME_PROFILE_CAP_INLINE_ASSET = 1 << 5
MOD_RUNTIME_PROFILE_CAP_BADGE_EFFECTS = 1 << 6
MOD_RUNTIME_PROFILE_CAP_FISHING_ACTIONS = 1 << 7
MOD_RUNTIME_PROFILE_CAP_ENCOUNTERS = 1 << 8
MOD_RUNTIME_PROFILE_CAP_SHOPS = 1 << 9
MOD_RUNTIME_PROFILE_CAP_ITEMS = 1 << 10
MOD_RUNTIME_PROFILE_CAP_REWARDS = 1 << 11
MOD_RUNTIME_PROFILE_CAP_POKEMON_DATA = 1 << 12
MOD_RUNTIME_PROFILE_CAP_BATTLE_DATA = 1 << 13
MOD_RUNTIME_PROFILE_CAP_TRAINERS = 1 << 14
MOD_RUNTIME_PROFILE_CAPABILITIES = (
    MOD_RUNTIME_PROFILE_CAP_TEXT
    | MOD_RUNTIME_PROFILE_CAP_WEATHER
    | MOD_RUNTIME_PROFILE_CAP_ENGINE
    | MOD_RUNTIME_PROFILE_CAP_NPC
    | MOD_RUNTIME_PROFILE_CAP_ASSET_REF
    | MOD_RUNTIME_PROFILE_CAP_INLINE_ASSET
    | MOD_RUNTIME_PROFILE_CAP_BADGE_EFFECTS
    | MOD_RUNTIME_PROFILE_CAP_FISHING_ACTIONS
    | MOD_RUNTIME_PROFILE_CAP_ENCOUNTERS
    | MOD_RUNTIME_PROFILE_CAP_SHOPS
    | MOD_RUNTIME_PROFILE_CAP_ITEMS
    | MOD_RUNTIME_PROFILE_CAP_REWARDS
    | MOD_RUNTIME_PROFILE_CAP_POKEMON_DATA
    | MOD_RUNTIME_PROFILE_CAP_BATTLE_DATA
    | MOD_RUNTIME_PROFILE_CAP_TRAINERS
)
MOD_CATALOG_HASH = 0x2EB46D50
MOD_CATALOG_COUNT = 1
OPTIONS_MULTIPLAYER_MODE_SOLO = 0
OPTIONS_MULTIPLAYER_MODE_ONLINE = 1
OPTIONS_MULTIPLAYER_MODE_COUNT = 2

NET_PLAYER_FLAG_BUSY = 1 << 0
NET_PLAYER_FLAG_IN_SUBSESSION = 1 << 1
NET_PLAYER_FLAG_HIDDEN = 1 << 2
NET_PLAYER_FLAG_STALE = 1 << 3
NET_PLAYER_FLAG_RESYNC = 1 << 4
NET_PLAYER_FLAGS_MASK = (
    NET_PLAYER_FLAG_BUSY
    | NET_PLAYER_FLAG_IN_SUBSESSION
    | NET_PLAYER_FLAG_HIDDEN
    | NET_PLAYER_FLAG_STALE
    | NET_PLAYER_FLAG_RESYNC
)

MULTIPLAYER_INTERACTION_NONE = 0
MULTIPLAYER_INTERACTION_SCRIPT = 1
MULTIPLAYER_INTERACTION_WARP = 2
MULTIPLAYER_INTERACTION_BATTLE = 3
MULTIPLAYER_INTERACTION_TRADE = 4

MULTIPLAYER_SUBSESSION_STATE_NONE = 0
MULTIPLAYER_SUBSESSION_STATE_INVITING = 1
MULTIPLAYER_SUBSESSION_STATE_READY = 2
MULTIPLAYER_SUBSESSION_STATE_ACTIVE = 3
MULTIPLAYER_SUBSESSION_STATE_ENDING = 4
MULTIPLAYER_SUBSESSION_STATE_ERROR = 5

DIR_SOUTH = 1
DIR_NORTH = 2
DIR_WEST = 3
DIR_EAST = 4

ELEVATION_TRANSITION = 0
ELEVATION_SURF = 1
ELEVATION_DEFAULT = 3
ELEVATION_MULTI_LEVEL = 15


@dataclass
class Snapshot:
    active: bool = True
    player_id: int = 0
    map_group: int = 1
    map_num: int = 1
    x: int = 10
    y: int = 10
    elevation: int = ELEVATION_DEFAULT
    direction: int = DIR_SOUTH
    interaction_state: int = MULTIPLAYER_INTERACTION_NONE
    subsession_id: int = NET_SUBSESSION_NONE
    subsession_state: int = MULTIPLAYER_SUBSESSION_STATE_NONE
    flags: int = 0
    tick: int = 100
    server_tick_seen: int = 100
    sequence: int = 1
    session_epoch: int = 11
    stale_frames: int = 0
    anomaly_score: int = 0


@dataclass
class Packet:
    packet_type: int
    sequence: int
    player_id: int = 0
    session_id: int = 7
    session_epoch: int = 11
    payload_size: int = 0


@dataclass
class ClientHello:
    protocol_version: int = NET_PROTOCOL_VERSION
    bridge_version: int = NET_EMULATOR_BRIDGE_VERSION
    build_id: int = 0x00010003
    ruleset_hash: int = 0x00000003
    profile_protocol_version: int = MOD_RUNTIME_PROFILE_PROTOCOL_VERSION
    profile_capability_flags: int = MOD_RUNTIME_PROFILE_CAPABILITIES
    profile_capability_hash: int = MOD_RUNTIME_PROFILE_CAPABILITY_HASH
    mod_catalog_hash: int = MOD_CATALOG_HASH
    mod_catalog_count: int = MOD_CATALOG_COUNT
    transport_mode: int = NET_TRANSPORT_MODE_SERVER_BRIDGE


def is_subsession_state(state: int) -> bool:
    return state in {
        MULTIPLAYER_SUBSESSION_STATE_INVITING,
        MULTIPLAYER_SUBSESSION_STATE_READY,
        MULTIPLAYER_SUBSESSION_STATE_ACTIVE,
        MULTIPLAYER_SUBSESSION_STATE_ENDING,
        MULTIPLAYER_SUBSESSION_STATE_ERROR,
    }


def blocks_interaction(state: int) -> bool:
    return state in {
        MULTIPLAYER_SUBSESSION_STATE_INVITING,
        MULTIPLAYER_SUBSESSION_STATE_READY,
        MULTIPLAYER_SUBSESSION_STATE_ACTIVE,
    }


def snapshot_tick_is_fresh(snapshot: Snapshot, current_tick: int) -> bool:
    if snapshot.tick == 0 or current_tick == 0:
        return True
    if snapshot.tick > current_tick:
        return snapshot.tick - current_tick <= NET_PLAYER_SNAPSHOT_FUTURE_SKEW_FRAMES
    return current_tick - snapshot.tick <= NET_PLAYER_SNAPSHOT_TTL_FRAMES


def snapshot_server_tick_is_fresh(snapshot: Snapshot, current_tick: int) -> bool:
    if snapshot.server_tick_seen == 0 or current_tick == 0:
        return True
    if snapshot.server_tick_seen > current_tick:
        return snapshot.server_tick_seen - current_tick <= NET_PLAYER_SNAPSHOT_FUTURE_SKEW_FRAMES
    return current_tick - snapshot.server_tick_seen <= NET_PLAYER_DISCONNECT_FRAMES


def snapshot_is_valid(snapshot: Snapshot, slot: int, current_tick: int, session_epoch: int = 11) -> bool:
    if not snapshot.active:
        return True
    if snapshot.player_id != slot:
        return False
    if snapshot.session_epoch != session_epoch:
        return False
    if snapshot.sequence == 0:
        return False
    if snapshot.map_group == 0xFF or snapshot.map_num == 0xFF:
        return False
    if not (NET_PLAYER_COORD_MIN <= snapshot.x <= NET_PLAYER_COORD_MAX):
        return False
    if not (NET_PLAYER_COORD_MIN <= snapshot.y <= NET_PLAYER_COORD_MAX):
        return False
    if snapshot.elevation not in {
        ELEVATION_TRANSITION,
        ELEVATION_SURF,
        ELEVATION_DEFAULT,
        ELEVATION_MULTI_LEVEL,
    }:
        return False
    if snapshot.direction not in {DIR_NORTH, DIR_SOUTH, DIR_WEST, DIR_EAST}:
        return False
    if snapshot.flags & ~NET_PLAYER_FLAGS_MASK:
        return False
    if snapshot.stale_frames > NET_PLAYER_DISCONNECT_FRAMES:
        return False
    if snapshot.anomaly_score >= 8:
        return False
    if not snapshot_tick_is_fresh(snapshot, current_tick):
        return False
    if not snapshot_server_tick_is_fresh(snapshot, current_tick):
        return False
    if snapshot.subsession_id > MAX_NET_SUBSESSIONS:
        return False
    if snapshot.flags & NET_PLAYER_FLAG_IN_SUBSESSION:
        if snapshot.subsession_id == NET_SUBSESSION_NONE:
            return False
        if not is_subsession_state(snapshot.subsession_state):
            return False
        return snapshot.interaction_state in {
            MULTIPLAYER_INTERACTION_BATTLE,
            MULTIPLAYER_INTERACTION_TRADE,
        }
    if snapshot.subsession_id != NET_SUBSESSION_NONE:
        return False
    if snapshot.subsession_state != MULTIPLAYER_SUBSESSION_STATE_NONE:
        return False
    if snapshot.interaction_state in {
        MULTIPLAYER_INTERACTION_BATTLE,
        MULTIPLAYER_INTERACTION_TRADE,
    }:
        return False
    if snapshot.interaction_state in {
        MULTIPLAYER_INTERACTION_SCRIPT,
        MULTIPLAYER_INTERACTION_WARP,
    }:
        return bool(snapshot.flags & NET_PLAYER_FLAG_BUSY)
    return True


def packet_is_valid(packet: Packet, session_id: int, last_sequence: int, capacity: int) -> bool:
    if packet.session_id != session_id:
        return False
    if packet.session_epoch != 11:
        return False
    if packet.packet_type <= NET_PACKET_NONE or packet.packet_type >= NET_PACKET_COUNT:
        return False
    if packet.sequence == 0 or packet.sequence <= last_sequence:
        return False
    if packet.player_id >= MAX_NET_PLAYERS:
        return False
    if packet.payload_size > capacity:
        return False
    return True


def client_hello_is_compatible(hello: ClientHello) -> bool:
    return (
        hello.protocol_version == NET_PROTOCOL_VERSION
        and hello.bridge_version == NET_EMULATOR_BRIDGE_VERSION
        and hello.build_id == 0x00010003
        and hello.ruleset_hash == 0x00000003
        and hello.profile_protocol_version == MOD_RUNTIME_PROFILE_PROTOCOL_VERSION
        and hello.profile_capability_hash == MOD_RUNTIME_PROFILE_CAPABILITY_HASH
        and (hello.profile_capability_flags & MOD_RUNTIME_PROFILE_CAPABILITIES) == MOD_RUNTIME_PROFILE_CAPABILITIES
        and hello.mod_catalog_hash != 0
        and hello.mod_catalog_count != 0
        and hello.transport_mode == NET_TRANSPORT_MODE_SERVER_BRIDGE
    )


def make_transaction_id(session_epoch: int, player_id: int, packet_type: int, subsession_id: int, action_sequence: int) -> int:
    value = session_epoch ^ ((action_sequence * 0x45D9F3B) & 0xFFFFFFFF)
    value ^= player_id << 24
    value ^= packet_type << 16
    value ^= subsession_id << 8
    return value or 1


def reliable_queue_pushes_before_full(capacity: int) -> int:
    head = 0
    tail = 0
    pushes = 0
    while True:
        next_head = (head + 1) % capacity
        if next_head == tail:
            return pushes
        head = next_head
        pushes += 1


def players_are_within_range(coords: list[tuple[int, int]], max_distance: int) -> bool:
    origin_x, origin_y = coords[0]
    for x, y in coords[1:]:
        if abs(int(x) - int(origin_x)) + abs(int(y) - int(origin_y)) > max_distance:
            return False
    return True


def normalize_multiplayer_mode(mode: int) -> int:
    if mode >= OPTIONS_MULTIPLAYER_MODE_COUNT:
        return OPTIONS_MULTIPLAYER_MODE_SOLO
    return mode


def runtime_allows_online(mode: int, feature_enabled: bool = True) -> bool:
    return feature_enabled and normalize_multiplayer_mode(mode) == OPTIONS_MULTIPLAYER_MODE_ONLINE


def test_snapshot_edges() -> None:
    base = Snapshot(player_id=2, tick=100)
    assert snapshot_is_valid(base, 2, 100)
    assert not snapshot_is_valid(Snapshot(player_id=2, tick=131), 2, 100)
    assert snapshot_is_valid(Snapshot(player_id=2, tick=130), 2, 100)
    assert not snapshot_is_valid(Snapshot(player_id=2, tick=100, sequence=0), 2, 100)
    assert not snapshot_is_valid(Snapshot(player_id=2, tick=100, session_epoch=10), 2, 100)
    assert not snapshot_is_valid(Snapshot(player_id=2, tick=100, server_tick_seen=131), 2, 100)
    assert snapshot_is_valid(Snapshot(player_id=2, tick=100, server_tick_seen=130), 2, 100)
    assert not snapshot_is_valid(Snapshot(player_id=2, tick=100, stale_frames=NET_PLAYER_DISCONNECT_FRAMES + 1), 2, 100)
    assert not snapshot_is_valid(Snapshot(player_id=2, tick=100, x=32767), 2, 100)
    assert not snapshot_is_valid(Snapshot(player_id=2, tick=100, y=-32768), 2, 100)
    assert not snapshot_is_valid(Snapshot(player_id=2, tick=100, direction=9), 2, 100)
    assert not snapshot_is_valid(Snapshot(player_id=2, tick=100, elevation=9), 2, 100)
    assert not snapshot_is_valid(Snapshot(player_id=2, tick=100, map_group=0xFF), 2, 100)

    active_battle = Snapshot(
        player_id=2,
        tick=100,
        flags=NET_PLAYER_FLAG_BUSY | NET_PLAYER_FLAG_IN_SUBSESSION,
        subsession_id=1,
        subsession_state=MULTIPLAYER_SUBSESSION_STATE_ACTIVE,
        interaction_state=MULTIPLAYER_INTERACTION_BATTLE,
    )
    assert snapshot_is_valid(active_battle, 2, 100)
    active_battle.subsession_state = MULTIPLAYER_SUBSESSION_STATE_NONE
    assert not snapshot_is_valid(active_battle, 2, 100)


def test_packet_edges() -> None:
    assert packet_is_valid(Packet(packet_type=1, sequence=1), 7, 0, NET_TRANSPORT_PACKET_PAYLOAD_SIZE)
    assert not packet_is_valid(Packet(packet_type=NET_PACKET_NONE, sequence=1), 7, 0, NET_TRANSPORT_PACKET_PAYLOAD_SIZE)
    assert not packet_is_valid(Packet(packet_type=NET_PACKET_COUNT, sequence=1), 7, 0, NET_TRANSPORT_PACKET_PAYLOAD_SIZE)
    assert not packet_is_valid(Packet(packet_type=1, sequence=0), 7, 0, NET_TRANSPORT_PACKET_PAYLOAD_SIZE)
    assert not packet_is_valid(Packet(packet_type=1, sequence=4), 7, 4, NET_TRANSPORT_PACKET_PAYLOAD_SIZE)
    assert not packet_is_valid(Packet(packet_type=1, sequence=5, session_id=8), 7, 4, NET_TRANSPORT_PACKET_PAYLOAD_SIZE)
    assert not packet_is_valid(Packet(packet_type=1, sequence=5, session_epoch=10), 7, 4, NET_TRANSPORT_PACKET_PAYLOAD_SIZE)
    assert not packet_is_valid(Packet(packet_type=1, sequence=5, player_id=8), 7, 4, NET_TRANSPORT_PACKET_PAYLOAD_SIZE)
    assert not packet_is_valid(Packet(packet_type=1, sequence=5, payload_size=NET_TRANSPORT_PACKET_PAYLOAD_SIZE + 1), 7, 4, NET_TRANSPORT_PACKET_PAYLOAD_SIZE)


def test_handshake_edges() -> None:
    assert client_hello_is_compatible(ClientHello())
    assert not client_hello_is_compatible(ClientHello(protocol_version=1))
    assert not client_hello_is_compatible(ClientHello(bridge_version=3))
    assert not client_hello_is_compatible(ClientHello(build_id=0xDEADBEEF))
    assert not client_hello_is_compatible(ClientHello(ruleset_hash=0))
    assert not client_hello_is_compatible(ClientHello(profile_protocol_version=0))
    assert not client_hello_is_compatible(ClientHello(profile_capability_hash=0))
    assert not client_hello_is_compatible(ClientHello(profile_capability_flags=MOD_RUNTIME_PROFILE_CAP_TEXT))
    assert not client_hello_is_compatible(ClientHello(mod_catalog_hash=0))
    assert not client_hello_is_compatible(ClientHello(mod_catalog_count=0))
    assert not client_hello_is_compatible(ClientHello(transport_mode=0))


def test_transport_rate_limit() -> None:
    assert reliable_queue_pushes_before_full(NET_RELIABLE_QUEUE_SIZE) == NET_RELIABLE_QUEUE_SIZE - 1


def test_transaction_edges() -> None:
    assert NET_COMMIT_LOG_SIZE == 32
    assert NET_PENDING_TX_COUNT == 16
    assert make_transaction_id(1, 0, 7, 1, 1) != 0
    assert make_transaction_id(1, 0, 7, 1, 1) == make_transaction_id(1, 0, 7, 1, 1)
    assert make_transaction_id(1, 0, 7, 1, 1) != make_transaction_id(1, 0, 7, 1, 2)
    assert make_transaction_id(1, 0, 7, 1, 1) != make_transaction_id(2, 0, 7, 1, 1)


def test_range_edges() -> None:
    assert players_are_within_range([(0, 0), (2, 0)], 2)
    assert not players_are_within_range([(32760, 32760), (-32760, -32760)], 2)


def test_runtime_mode_edges() -> None:
    assert normalize_multiplayer_mode(OPTIONS_MULTIPLAYER_MODE_SOLO) == OPTIONS_MULTIPLAYER_MODE_SOLO
    assert normalize_multiplayer_mode(OPTIONS_MULTIPLAYER_MODE_ONLINE) == OPTIONS_MULTIPLAYER_MODE_ONLINE
    assert normalize_multiplayer_mode(OPTIONS_MULTIPLAYER_MODE_COUNT) == OPTIONS_MULTIPLAYER_MODE_SOLO
    assert not runtime_allows_online(OPTIONS_MULTIPLAYER_MODE_SOLO)
    assert runtime_allows_online(OPTIONS_MULTIPLAYER_MODE_ONLINE)
    assert not runtime_allows_online(OPTIONS_MULTIPLAYER_MODE_ONLINE, feature_enabled=False)


def fuzz_snapshots() -> None:
    rng = random.Random(0)
    for _ in range(5000):
        slot = rng.randrange(MAX_NET_PLAYERS)
        snap = Snapshot(
            active=True,
            player_id=slot if rng.randrange(4) else rng.randrange(MAX_NET_PLAYERS),
            map_group=rng.choice([0, 1, 2, 0xFF]),
            map_num=rng.choice([0, 1, 2, 0xFF]),
            x=rng.choice([rng.randrange(-600, 8300), -32768, 32767]),
            y=rng.choice([rng.randrange(-600, 8300), -32768, 32767]),
            elevation=rng.randrange(16),
            direction=rng.randrange(10),
            tick=rng.randrange(0, 450),
            server_tick_seen=rng.randrange(0, 450),
            sequence=rng.choice([0, rng.randrange(1, 10000)]),
            session_epoch=rng.choice([11, 10, 0]),
        )
        valid = snapshot_is_valid(snap, slot, 100)
        if valid:
            assert snap.player_id == slot
            assert snap.session_epoch == 11
            assert snap.sequence != 0
            assert snap.map_group != 0xFF and snap.map_num != 0xFF
            assert NET_PLAYER_COORD_MIN <= snap.x <= NET_PLAYER_COORD_MAX
            assert NET_PLAYER_COORD_MIN <= snap.y <= NET_PLAYER_COORD_MAX
            assert snapshot_tick_is_fresh(snap, 100)
            assert snapshot_server_tick_is_fresh(snap, 100)


def main() -> None:
    test_snapshot_edges()
    test_packet_edges()
    test_handshake_edges()
    test_transport_rate_limit()
    test_transaction_edges()
    test_range_edges()
    test_runtime_mode_edges()
    fuzz_snapshots()
    print("Multiplayer fuzz checks OK")


if __name__ == "__main__":
    main()
