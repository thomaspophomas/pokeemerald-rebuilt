#!/usr/bin/env python3
"""Reference host-side rules for idempotent multiplayer commits."""

from __future__ import annotations

from dataclasses import dataclass, field
import random


NET_RELIABLE_QUEUE_SIZE = 16
NET_FEATURE_FLAG_MULTIPLAYER = 1 << 0
NET_FEATURE_FLAG_EMULATOR_TRANSPORT = 1 << 1
NET_REQUIRED_ONLINE_FEATURE_FLAGS = NET_FEATURE_FLAG_MULTIPLAYER | NET_FEATURE_FLAG_EMULATOR_TRANSPORT
MOD_RUNTIME_PROFILE_PROTOCOL_VERSION = 1
MOD_RUNTIME_PROFILE_CAPABILITY_HASH = 0x00000001
MOD_RUNTIME_PROFILE_CAP_TEXT = 1 << 0
MOD_RUNTIME_PROFILE_CAP_WEATHER = 1 << 1
MOD_RUNTIME_PROFILE_CAP_ENGINE = 1 << 2
MOD_RUNTIME_PROFILE_CAP_NPC = 1 << 3
MOD_RUNTIME_PROFILE_CAP_ASSET_REF = 1 << 4
MOD_RUNTIME_PROFILE_CAP_INLINE_ASSET = 1 << 5
MOD_RUNTIME_PROFILE_CAPABILITIES = (
    MOD_RUNTIME_PROFILE_CAP_TEXT
    | MOD_RUNTIME_PROFILE_CAP_WEATHER
    | MOD_RUNTIME_PROFILE_CAP_ENGINE
    | MOD_RUNTIME_PROFILE_CAP_NPC
    | MOD_RUNTIME_PROFILE_CAP_ASSET_REF
    | MOD_RUNTIME_PROFILE_CAP_INLINE_ASSET
)
MOD_CATALOG_HASH = 0xE64BCD9E
MOD_CATALOG_COUNT = 1
MODE_SOLO = "solo"
MODE_ONLINE = "online"

PACKET_TRADE_ACTION = 7
PACKET_BATTLE_ACTION = 6
PACKET_BATTLE_INPUT = 21

COMMIT_NONE = 0
COMMIT_TRADE = 1
COMMIT_ITEM = 2
COMMIT_BATTLE = 3
COMMIT_STORY_FLAG = 4
COMMIT_OUTFIT = 5
COMMIT_WEATHER_REWARD = 6

STATE_PREPARED = "prepared"
STATE_COMMITTED = "committed"
STATE_ROLLED_BACK = "rolled_back"

RESULT_PENDING = "pending"
RESULT_OK = "ok"
RESULT_REJECTED = "rejected"
RESULT_ROLLED_BACK = "rolled_back"

FAIL_CLOSED_TYPES = {
    COMMIT_TRADE,
    COMMIT_ITEM,
    COMMIT_BATTLE,
    COMMIT_STORY_FLAG,
    COMMIT_OUTFIT,
    COMMIT_WEATHER_REWARD,
}


def catalog_hash(*parts: object) -> int:
    value = 2166136261
    data = "\x1f".join(str(part) for part in parts).encode()
    for byte in data:
        value ^= byte
        value = (value * 16777619) & 0xFFFFFFFF
    return value


@dataclass(frozen=True)
class TransactionKey:
    session_epoch: int
    player_id: int
    packet_type: int
    action_sequence: int
    subsession_id: int


@dataclass
class CommitEntry:
    key: TransactionKey
    commit_type: int
    checksum: int
    state: str = STATE_PREPARED
    result: str = RESULT_PENDING
    server_revision: int = 0


class CommitLog:
    def __init__(self) -> None:
        self.entries: dict[TransactionKey, CommitEntry] = {}
        self.revision = 0
        self.applied_effects: dict[TransactionKey, int] = {}

    def prepare(self, key: TransactionKey, commit_type: int, payload: bytes) -> CommitEntry:
        assert key.session_epoch != 0
        assert key.action_sequence != 0
        if key in self.entries:
            return self.entries[key]
        entry = CommitEntry(key=key, commit_type=commit_type, checksum=checksum(payload))
        self.entries[key] = entry
        return entry

    def commit(self, key: TransactionKey, commit_type: int, payload: bytes) -> CommitEntry:
        entry = self.prepare(key, commit_type, payload)
        if entry.state in {STATE_COMMITTED, STATE_ROLLED_BACK}:
            return entry
        self.revision += 1
        entry.server_revision = self.revision
        if checksum(payload) != entry.checksum or commit_type in FAIL_CLOSED_TYPES:
            entry.state = STATE_ROLLED_BACK
            entry.result = RESULT_REJECTED
            return entry
        entry.state = STATE_COMMITTED
        entry.result = RESULT_OK
        self.applied_effects[key] = self.applied_effects.get(key, 0) + 1
        return entry

    def rollback(self, key: TransactionKey, commit_type: int, payload: bytes) -> CommitEntry:
        entry = self.prepare(key, commit_type, payload)
        if entry.state not in {STATE_COMMITTED, STATE_ROLLED_BACK}:
            self.revision += 1
            entry.server_revision = self.revision
            entry.state = STATE_ROLLED_BACK
            entry.result = RESULT_ROLLED_BACK
        return entry


@dataclass
class SyntheticClientHello:
    feature_flags: int = NET_REQUIRED_ONLINE_FEATURE_FLAGS
    profile_protocol_version: int = MOD_RUNTIME_PROFILE_PROTOCOL_VERSION
    profile_capability_flags: int = MOD_RUNTIME_PROFILE_CAPABILITIES
    profile_capability_hash: int = MOD_RUNTIME_PROFILE_CAPABILITY_HASH
    mod_catalog_hash: int = MOD_CATALOG_HASH
    mod_catalog_count: int = MOD_CATALOG_COUNT


@dataclass(frozen=True)
class ServerRuntimeProfile:
    profile_hash: int
    text: dict[str, str] = field(default_factory=dict)
    weather: str | None = "map-default"
    ruleset: str = "engine:gen3"
    asset_keys: tuple[str, ...] = ()

    def delta_for(self, client: "SyntheticClient") -> "ServerRuntimeProfile":
        text_delta = {
            key: value
            for key, value in self.text.items()
            if client.local_catalog.get(f"text:{key}") != catalog_hash("text", key, value)
        }
        if self.weather is not None and client.local_catalog.get("weather") == catalog_hash("weather", self.weather):
            weather_delta = None
        else:
            weather_delta = self.weather
        asset_delta = tuple(
            key for key in self.asset_keys if client.local_catalog.get(f"asset:{key}") != catalog_hash("asset", key)
        )
        return ServerRuntimeProfile(
            profile_hash=self.profile_hash,
            text=text_delta,
            weather=weather_delta,
            ruleset=self.ruleset,
            asset_keys=asset_delta,
        )


@dataclass
class SyntheticClient:
    client_id: str
    hello: SyntheticClientHello = field(default_factory=SyntheticClientHello)
    local_text: dict[str, str] = field(default_factory=dict)
    local_weather: str = "local"
    local_catalog: dict[str, int] = field(default_factory=dict)
    active_profile: ServerRuntimeProfile | None = None
    applied_profile_record_count: int = 0
    player_id: int = -1
    room_id: str | None = None

    def apply_profile(self, profile: ServerRuntimeProfile) -> None:
        self.active_profile = profile
        self.applied_profile_record_count = len(profile.text) + len(profile.asset_keys) + (1 if profile.weather is not None else 0)

    def clear_profile(self) -> None:
        self.active_profile = None
        self.applied_profile_record_count = 0

    def text_for(self, key: str) -> str:
        if self.active_profile is not None and key in self.active_profile.text:
            return self.active_profile.text[key]
        return self.local_text.get(key, "")

    def weather(self) -> str:
        if self.active_profile is not None and self.active_profile.weather is not None:
            return self.active_profile.weather
        return self.local_weather


@dataclass(frozen=True)
class BattleInput:
    client_id: str
    sequence: int
    turn: int
    action: int
    target: int


@dataclass
class BattleSubsession:
    session_epoch: int
    subsession_id: int
    participants: tuple[str, ...]
    turn: int = 1
    last_sequences: dict[str, int] = field(default_factory=dict)
    inputs_by_turn: dict[int, dict[str, BattleInput]] = field(default_factory=dict)
    commit_log: CommitLog = field(default_factory=CommitLog)
    committed_turns: dict[int, CommitEntry] = field(default_factory=dict)

    def __post_init__(self) -> None:
        self.last_sequences = {client_id: 0 for client_id in self.participants}

    def submit_input(self, battle_input: BattleInput) -> bool:
        if battle_input.client_id not in self.last_sequences:
            return False
        if battle_input.turn != self.turn:
            return False
        if battle_input.sequence <= self.last_sequences[battle_input.client_id]:
            return False
        self.last_sequences[battle_input.client_id] = battle_input.sequence
        self.inputs_by_turn.setdefault(battle_input.turn, {})[battle_input.client_id] = battle_input
        return True

    def commit_turn(self, turn: int) -> CommitEntry | None:
        if turn in self.committed_turns:
            return self.committed_turns[turn]
        turn_inputs = self.inputs_by_turn.get(turn, {})
        if any(client_id not in turn_inputs for client_id in self.participants):
            return None
        payload = "|".join(
            f"{client_id}:{turn_inputs[client_id].sequence}:{turn_inputs[client_id].action}:{turn_inputs[client_id].target}"
            for client_id in self.participants
        ).encode()
        key = TransactionKey(self.session_epoch, 0, PACKET_BATTLE_INPUT, turn, self.subsession_id)
        entry = self.commit_log.commit(key, COMMIT_NONE, payload)
        self.committed_turns[turn] = entry
        if turn == self.turn:
            self.turn += 1
        return entry


class SyntheticRoom:
    def __init__(
        self,
        room_id: str,
        session_epoch: int,
        profile: ServerRuntimeProfile,
        required_feature_flags: int = NET_REQUIRED_ONLINE_FEATURE_FLAGS,
    ) -> None:
        self.room_id = room_id
        self.session_epoch = session_epoch
        self.profile = profile
        self.required_feature_flags = required_feature_flags
        self.clients: dict[str, SyntheticClient] = {}
        self.next_player_id = 0
        self.next_subsession_id = 1

    def client_is_allowed(self, client: SyntheticClient) -> bool:
        return (
            (client.hello.feature_flags & self.required_feature_flags) == self.required_feature_flags
            and client.hello.profile_protocol_version == MOD_RUNTIME_PROFILE_PROTOCOL_VERSION
            and client.hello.profile_capability_hash == MOD_RUNTIME_PROFILE_CAPABILITY_HASH
            and (client.hello.profile_capability_flags & MOD_RUNTIME_PROFILE_CAPABILITIES) == MOD_RUNTIME_PROFILE_CAPABILITIES
            and client.hello.mod_catalog_hash != 0
            and client.hello.mod_catalog_count != 0
        )

    def join(self, client: SyntheticClient) -> SyntheticClient | None:
        if not self.client_is_allowed(client):
            return None
        if client.client_id in self.clients:
            return client
        client.room_id = self.room_id
        client.player_id = self.next_player_id
        self.next_player_id += 1
        client.apply_profile(self.profile.delta_for(client))
        self.clients[client.client_id] = client
        return client

    def start_battle(self) -> BattleSubsession:
        if len(self.clients) < 2:
            raise ValueError("battle demo requires two clients")
        subsession = BattleSubsession(
            session_epoch=self.session_epoch,
            subsession_id=self.next_subsession_id,
            participants=tuple(self.clients),
        )
        self.next_subsession_id += 1
        return subsession


class SyntheticHost:
    def __init__(self) -> None:
        self.rooms: dict[str, SyntheticRoom] = {}
        self.next_room_id = 1
        self.next_session_epoch = 100

    def create_room(
        self,
        client: SyntheticClient,
        profile: ServerRuntimeProfile,
        required_feature_flags: int = NET_REQUIRED_ONLINE_FEATURE_FLAGS,
    ) -> SyntheticRoom:
        room_id = f"room-{self.next_room_id}"
        self.next_room_id += 1
        room = SyntheticRoom(room_id, self.next_session_epoch, profile, required_feature_flags)
        self.next_session_epoch += 1
        if room.join(client) is None:
            raise ValueError("host client cannot accept the server runtime profile")
        self.rooms[room_id] = room
        return room

    def join_room(self, room_id: str, client: SyntheticClient) -> SyntheticClient | None:
        return self.rooms[room_id].join(client)


class RingBuffer:
    def __init__(self, capacity: int) -> None:
        self.capacity = capacity
        self.items: list[bytes | None] = [None] * capacity
        self.head = 0
        self.tail = 0

    def push(self, item: bytes) -> bool:
        next_head = (self.head + 1) % self.capacity
        if next_head == self.tail:
            return False
        self.items[self.head] = item
        self.head = next_head
        return True

    def pop(self) -> bytes | None:
        if self.tail == self.head:
            return None
        item = self.items[self.tail]
        self.items[self.tail] = None
        self.tail = (self.tail + 1) % self.capacity
        return item


class AvatarRegistry:
    def __init__(self) -> None:
        self.sprites: dict[tuple[int, int, int], tuple[int, int]] = {}

    def create_or_update(self, session_epoch: int, player_id: int, virtual_id: int, graphics_id: int, graphics_revision: int) -> None:
        key = (session_epoch, player_id, virtual_id)
        self.sprites[key] = (graphics_id, graphics_revision)
        stale = [sprite_key for sprite_key in self.sprites if sprite_key[1:] == key[1:] and sprite_key[0] != session_epoch]
        for sprite_key in stale:
            del self.sprites[sprite_key]

    def count_for_player(self, player_id: int) -> int:
        return sum(1 for key in self.sprites if key[1] == player_id)

    def clear(self) -> None:
        self.sprites.clear()


class RuntimeSession:
    def __init__(self) -> None:
        self.mode = MODE_SOLO
        self.connected = False
        self.pending_transactions: set[TransactionKey] = set()
        self.avatars = AvatarRegistry()

    def request_online(self) -> None:
        self.mode = MODE_ONLINE
        self.connected = True

    def request_solo(self) -> None:
        self.mode = MODE_SOLO
        self.connected = False
        self.pending_transactions.clear()
        self.avatars.clear()

    def can_publish_snapshot(self) -> bool:
        return self.mode == MODE_ONLINE and self.connected


def checksum(payload: bytes) -> int:
    value = 0
    for byte in payload:
        value = ((value << 1) & 0xFFFF) ^ byte
    return value


def test_duplicate_commit_once() -> None:
    log = CommitLog()
    key = TransactionKey(3, 1, PACKET_BATTLE_ACTION, 9, 2)
    payload = b"safe-local-test-action"
    for _ in range(100):
        entry = log.commit(key, COMMIT_NONE, payload)
        assert entry.result == RESULT_OK
    assert log.applied_effects[key] == 1


def test_fail_closed_trade() -> None:
    log = CommitLog()
    key = TransactionKey(3, 1, PACKET_TRADE_ACTION, 10, 1)
    payload = b"trade-offer"
    assert log.prepare(key, COMMIT_TRADE, payload).result == RESULT_PENDING
    assert log.commit(key, COMMIT_TRADE, payload).result == RESULT_REJECTED
    assert log.rollback(key, COMMIT_TRADE, payload).result == RESULT_REJECTED
    assert key not in log.applied_effects


def test_loss_reorder_and_retry() -> None:
    rng = random.Random(1)
    log = CommitLog()
    keys = [TransactionKey(8, i % 4, PACKET_BATTLE_ACTION, i + 1, 1) for i in range(40)]
    packets = [(key, COMMIT_NONE, f"payload-{key.action_sequence}".encode()) for key in keys]
    delivered = []
    for packet in packets:
        for _ in range(rng.randint(1, 5)):
            if rng.random() > 0.45:
                delivered.append(packet)
    rng.shuffle(delivered)
    delivered.extend(packets)
    for key, commit_type, payload in delivered:
        log.commit(key, commit_type, payload)
    for key in keys:
        assert log.applied_effects[key] == 1


def test_ringbuffer_backpressure() -> None:
    queue = RingBuffer(NET_RELIABLE_QUEUE_SIZE)
    for i in range(NET_RELIABLE_QUEUE_SIZE - 1):
        assert queue.push(bytes([i]))
    assert not queue.push(b"full")
    assert queue.pop() == b"\x00"
    assert queue.push(b"after-pop")


def test_avatar_singleton() -> None:
    registry = AvatarRegistry()
    for revision in range(20):
        registry.create_or_update(4, 2, 250, 1, revision)
        assert registry.count_for_player(2) == 1
    registry.create_or_update(5, 2, 250, 2, 1)
    assert registry.count_for_player(2) == 1


def test_solo_online_toggle_cleanup() -> None:
    session = RuntimeSession()
    key = TransactionKey(3, 1, PACKET_BATTLE_ACTION, 9, 2)
    assert not session.can_publish_snapshot()
    session.request_online()
    assert session.can_publish_snapshot()
    session.pending_transactions.add(key)
    session.avatars.create_or_update(3, 2, 250, 1, 1)
    session.request_solo()
    client = SyntheticClient("profiled", active_profile=ServerRuntimeProfile(1, {"title": "server"}))
    client.clear_profile()
    assert not session.can_publish_snapshot()
    assert not session.pending_transactions
    assert session.avatars.count_for_player(2) == 0
    assert client.text_for("title") == ""


def test_battle_demo_room_join_subsession_commit() -> None:
    host = SyntheticHost()
    alice = SyntheticClient("alice")
    bob = SyntheticClient("bob")
    profile = ServerRuntimeProfile(0xA11CE, {"battle": "server"}, weather="rain")

    room = host.create_room(alice, profile)
    host.join_room(room.room_id, bob)
    assert alice.player_id == 0
    assert bob.player_id == 1
    assert alice.room_id == room.room_id
    assert bob.room_id == room.room_id
    assert alice.text_for("battle") == "server"
    assert bob.weather() == "rain"

    battle = room.start_battle()
    assert battle.subsession_id == 1
    assert battle.participants == ("alice", "bob")

    assert battle.submit_input(BattleInput("alice", sequence=1, turn=1, action=1, target=0))
    assert not battle.submit_input(BattleInput("alice", sequence=1, turn=1, action=1, target=0))
    assert battle.commit_turn(1) is None

    assert battle.submit_input(BattleInput("bob", sequence=1, turn=1, action=1, target=0))
    entry = battle.commit_turn(1)
    assert entry is not None
    assert entry.result == RESULT_OK
    assert battle.commit_turn(1) is entry
    assert battle.commit_log.applied_effects[entry.key] == 1

    assert battle.turn == 2
    assert not battle.submit_input(BattleInput("bob", sequence=1, turn=2, action=1, target=0))
    assert battle.submit_input(BattleInput("bob", sequence=2, turn=2, action=1, target=0))


def test_server_runtime_profile_capability_gate() -> None:
    host = SyntheticHost()
    alice = SyntheticClient("alice")
    profile = ServerRuntimeProfile(0xCAFE, {"greeting": "server"}, weather="ash", asset_keys=("server:avatar",))
    room = host.create_room(alice, profile)

    wrong_profile_version = SyntheticClient(
        "wrong-version",
        SyntheticClientHello(profile_protocol_version=MOD_RUNTIME_PROFILE_PROTOCOL_VERSION + 1),
    )
    missing_feature = SyntheticClient(
        "missing-feature",
        SyntheticClientHello(profile_capability_flags=MOD_RUNTIME_PROFILE_CAP_TEXT),
    )
    missing_catalog = SyntheticClient(
        "missing-catalog",
        SyntheticClientHello(mod_catalog_hash=0),
    )
    matching = SyntheticClient("matching")

    assert host.join_room(room.room_id, wrong_profile_version) is None
    assert wrong_profile_version.player_id == -1
    assert wrong_profile_version.room_id is None

    assert host.join_room(room.room_id, missing_feature) is None
    assert missing_feature.player_id == -1
    assert missing_feature.room_id is None

    assert host.join_room(room.room_id, missing_catalog) is None
    assert missing_catalog.player_id == -1
    assert missing_catalog.room_id is None

    assert host.join_room(room.room_id, matching) is matching
    assert matching.player_id == 1
    assert matching.room_id == room.room_id
    assert matching.text_for("greeting") == "server"


def test_server_profiles_change_without_rom_rebuild() -> None:
    host = SyntheticHost()
    client = SyntheticClient("player", local_text={"title": "local"})
    alpha = ServerRuntimeProfile(0xA1FA, {"title": "alpha"}, weather="rain")
    beta = ServerRuntimeProfile(0xB37A, {"title": "beta"}, weather="sun")

    alpha_room = host.create_room(client, alpha)
    assert client.room_id == alpha_room.room_id
    assert client.text_for("title") == "alpha"
    assert client.weather() == "rain"

    client.clear_profile()
    client.room_id = None
    client.player_id = -1
    assert client.text_for("title") == "local"
    assert client.weather() == "local"

    beta_room = host.create_room(client, beta)
    assert client.room_id == beta_room.room_id
    assert client.text_for("title") == "beta"
    assert client.weather() == "sun"


def test_server_delta_profile_reuses_rom_catalog() -> None:
    host = SyntheticHost()
    client = SyntheticClient(
        "player",
        local_text={"title": "already-local"},
        local_weather="rain",
        local_catalog={
            "text:title": catalog_hash("text", "title", "already-local"),
            "weather": catalog_hash("weather", "rain"),
            "asset:avatar": catalog_hash("asset", "avatar"),
        },
    )
    profile = ServerRuntimeProfile(
        0xD117A,
        {"title": "already-local", "subtitle": "server-only"},
        weather="rain",
        asset_keys=("avatar", "server-avatar"),
    )

    room = host.create_room(client, profile)
    assert client.room_id == room.room_id
    assert client.text_for("title") == "already-local"
    assert client.text_for("subtitle") == "server-only"
    assert client.weather() == "rain"
    assert client.active_profile is not None
    assert "title" not in client.active_profile.text
    assert client.active_profile.asset_keys == ("server-avatar",)
    assert client.applied_profile_record_count == 2


def main() -> None:
    test_duplicate_commit_once()
    test_fail_closed_trade()
    test_loss_reorder_and_retry()
    test_ringbuffer_backpressure()
    test_avatar_singleton()
    test_solo_online_toggle_cleanup()
    test_battle_demo_room_join_subsession_commit()
    test_server_runtime_profile_capability_gate()
    test_server_profiles_change_without_rom_rebuild()
    test_server_delta_profile_reuses_rom_catalog()
    print("Multiplayer host simulator checks OK")


if __name__ == "__main__":
    main()
