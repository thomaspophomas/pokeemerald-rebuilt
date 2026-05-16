#!/usr/bin/env python3
"""Reference host-side rules for idempotent multiplayer commits."""

from __future__ import annotations

from dataclasses import dataclass
import random


NET_RELIABLE_QUEUE_SIZE = 16
MODE_SOLO = "solo"
MODE_ONLINE = "online"

PACKET_INTERACT_INTENT = 6
PACKET_BATTLE_ACTION = 7
PACKET_TRADE_ACTION = 8

COMMIT_NONE = 0
COMMIT_TRADE = 1
COMMIT_ITEM = 2
COMMIT_BATTLE = 3
COMMIT_STORY_FLAG = 4
COMMIT_OUTFIT = 5
COMMIT_WEATHER_REWARD = 6
COMMIT_MONEY = 7
COMMIT_POKEMON = 8

STATE_PREPARED = "prepared"
STATE_COMMITTED = "committed"
STATE_ROLLED_BACK = "rolled_back"

RESULT_PENDING = "pending"
RESULT_OK = "ok"
RESULT_REJECTED = "rejected"
RESULT_ROLLED_BACK = "rolled_back"
STATUS_CONNECTING = "connecting"
STATUS_CONNECTED = "connected"
STATUS_BACKPRESSURE = "backpressure"
NPC_POLICY_EXCLUSIVE = "exclusive"
NPC_POLICY_SHARED_READONLY = "shared_readonly"
NPC_POLICY_DISABLED_ONLINE = "disabled_online"
LOCK_GRANTED = "granted"
LOCK_BUSY = "busy"
LOCK_DENIED = "denied"
ACTION_READONLY_SCRIPT = "readonly_script"
ACTION_EXCLUSIVE_SCRIPT = "exclusive_script"
ACTION_WARP = "warp"
ACTION_DISABLED_ONLINE = "disabled_online"

FAIL_CLOSED_TYPES = {
    COMMIT_TRADE,
    COMMIT_ITEM,
    COMMIT_BATTLE,
    COMMIT_STORY_FLAG,
    COMMIT_OUTFIT,
    COMMIT_WEATHER_REWARD,
    COMMIT_MONEY,
    COMMIT_POKEMON,
}


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


@dataclass(frozen=True)
class InteractionTarget:
    map_group: int
    map_num: int
    local_id: int
    x: int
    y: int
    elevation: int
    target_kind: str = "npc"
    script_hash: int = 1


@dataclass(frozen=True)
class ResourceKey:
    kind: str
    map_group: int
    map_num: int
    local_id: int = 0
    x: int = 0
    y: int = 0
    elevation: int = 0
    resource_id: int = 0


@dataclass(frozen=True)
class InteractionRequest:
    server_tick: int
    player_id: int
    action_sequence: int
    target: InteractionTarget
    policy: str = NPC_POLICY_EXCLUSIVE
    action_type: str = ACTION_EXCLUSIVE_SCRIPT
    barrier_id: int = 1
    resources: tuple[ResourceKey, ...] = ()


@dataclass(frozen=True)
class PendingAction:
    target: InteractionTarget
    facing: int
    current_map: tuple[int, int]

    def grant_is_still_valid(self, player_position: tuple[int, int], player_facing: int, current_map: tuple[int, int], target_exists: bool = True) -> bool:
        if current_map != self.current_map:
            return False
        if player_facing != self.facing:
            return False
        if not target_exists:
            return False
        return abs(player_position[0] - self.target.x) + abs(player_position[1] - self.target.y) <= 2


@dataclass(frozen=True)
class CommitResult:
    key: TransactionKey
    result: str
    server_revision: int
    checksum: int


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

    def result_for(self, key: TransactionKey) -> CommitResult:
        entry = self.entries[key]
        return CommitResult(key=entry.key, result=entry.result, server_revision=entry.server_revision, checksum=entry.checksum)

    def apply_result(self, result: CommitResult) -> CommitEntry | None:
        entry = self.entries.get(result.key)
        if entry is None:
            return None
        if result.server_revision == 0:
            return entry
        if result.checksum != entry.checksum:
            return entry
        if entry.server_revision and result.server_revision < entry.server_revision:
            return entry
        if entry.state in {STATE_COMMITTED, STATE_ROLLED_BACK} and entry.server_revision:
            return entry
        entry.result = result.result
        entry.server_revision = result.server_revision
        return entry


class PendingQueue:
    def __init__(self, capacity: int) -> None:
        self.capacity = capacity
        self.pending: dict[TransactionKey, tuple[int, bytes, int]] = {}

    def add(self, key: TransactionKey, packet_type: int, payload: bytes) -> bool:
        if key in self.pending:
            return True
        if len(self.pending) >= self.capacity:
            return False
        self.pending[key] = (packet_type, payload, 0)
        return True

    def ack_commit_result(self, result: CommitResult) -> None:
        self.pending.pop(result.key, None)

    def retry_due(self) -> list[TransactionKey]:
        due = []
        for key, (packet_type, payload, retries) in list(self.pending.items()):
            self.pending[key] = (packet_type, payload, retries + 1)
            due.append(key)
        return due


class NpcLockManager:
    def __init__(self) -> None:
        self.locks: dict[tuple, InteractionRequest] = {}

    @staticmethod
    def winner(requests: list[InteractionRequest]) -> InteractionRequest:
        return sorted(requests, key=lambda request: (request.server_tick, request.action_sequence, request.player_id))[0]

    @staticmethod
    def lock_key(target: InteractionTarget) -> ResourceKey:
        if target.target_kind == "npc":
            return ResourceKey("npc", target.map_group, target.map_num, local_id=target.local_id)
        return ResourceKey(target.target_kind, target.map_group, target.map_num, local_id=target.local_id, x=target.x, y=target.y, elevation=target.elevation)

    def request_resources(self, request: InteractionRequest) -> tuple[ResourceKey, ...]:
        if request.resources:
            return request.resources
        return (self.lock_key(request.target),)

    def request(self, request: InteractionRequest) -> str:
        resource_keys = self.request_resources(request)

        if request.policy == NPC_POLICY_SHARED_READONLY or request.action_type == ACTION_READONLY_SCRIPT:
            return LOCK_GRANTED
        if request.policy == NPC_POLICY_DISABLED_ONLINE or request.action_type == ACTION_DISABLED_ONLINE:
            return LOCK_DENIED
        for resource_key in resource_keys:
            if resource_key in self.locks and self.locks[resource_key].player_id != request.player_id:
                return LOCK_BUSY
        for resource_key in resource_keys:
            self.locks[resource_key] = request
        return LOCK_GRANTED

    def release_for_player(self, player_id: int) -> None:
        for target, request in list(self.locks.items()):
            if request.player_id == player_id:
                del self.locks[target]


class HelloHandshake:
    def __init__(self) -> None:
        self.acked = False
        self.retries = 0
        self.status = STATUS_CONNECTING

    def send_hello(self, delivered: bool, accepted: bool = True, profile_ready: bool = True, control_ready: bool = True) -> None:
        if self.acked:
            return
        self.retries += 1
        if not profile_ready or not control_ready:
            self.status = "not_ready"
            return
        if delivered and accepted:
            self.acked = True
            self.status = STATUS_CONNECTED
        elif not delivered:
            self.status = STATUS_CONNECTING
        else:
            self.status = "refused"


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


def test_full_key_commit_result_clears_only_matching_pending() -> None:
    log = CommitLog()
    pending = PendingQueue(16)
    key_a = TransactionKey(3, 1, PACKET_BATTLE_ACTION, 9, 2)
    key_b = TransactionKey(4, 1, PACKET_BATTLE_ACTION, 9, 2)
    payload = b"same-transaction-id-risk"
    assert pending.add(key_a, PACKET_BATTLE_ACTION, payload)
    assert pending.add(key_b, PACKET_BATTLE_ACTION, payload)
    log.commit(key_a, COMMIT_NONE, payload)
    result = log.result_for(key_a)
    pending.ack_commit_result(result)
    assert key_a not in pending.pending
    assert key_b in pending.pending


def test_commit_result_revision_replay_is_noop() -> None:
    log = CommitLog()
    key = TransactionKey(3, 1, PACKET_BATTLE_ACTION, 9, 2)
    payload = b"revision-test"
    entry = log.commit(key, COMMIT_NONE, payload)
    assert entry.result == RESULT_OK
    stale = CommitResult(key=key, result=RESULT_REJECTED, server_revision=entry.server_revision - 1, checksum=checksum(payload))
    log.apply_result(stale)
    assert log.entries[key].result == RESULT_OK


def test_pending_retry_survives_backpressure() -> None:
    pending = PendingQueue(2)
    key_a = TransactionKey(3, 1, PACKET_BATTLE_ACTION, 9, 2)
    key_b = TransactionKey(3, 1, PACKET_BATTLE_ACTION, 10, 2)
    key_c = TransactionKey(3, 1, PACKET_BATTLE_ACTION, 11, 2)
    assert pending.add(key_a, PACKET_BATTLE_ACTION, b"a")
    assert pending.add(key_b, PACKET_BATTLE_ACTION, b"b")
    assert not pending.add(key_c, PACKET_BATTLE_ACTION, b"c")
    assert pending.retry_due() == [key_a, key_b]


def test_npc_lock_same_tick_deterministic() -> None:
    locks = NpcLockManager()
    target = InteractionTarget(1, 2, 7, 10, 11, 3)
    requests = [
        InteractionRequest(server_tick=20, player_id=2, action_sequence=5, target=target),
        InteractionRequest(server_tick=20, player_id=1, action_sequence=4, target=target),
    ]
    winner = locks.winner(requests)
    assert winner.player_id == 1
    assert locks.request(winner) == LOCK_GRANTED
    assert locks.request(requests[0]) == LOCK_BUSY


def test_npc_shared_and_disconnect_cleanup() -> None:
    locks = NpcLockManager()
    target = InteractionTarget(1, 2, 8, 10, 11, 3)
    assert locks.request(InteractionRequest(20, 1, 1, target, NPC_POLICY_SHARED_READONLY)) == LOCK_GRANTED
    assert locks.request(InteractionRequest(20, 2, 1, target, NPC_POLICY_SHARED_READONLY)) == LOCK_GRANTED
    assert not locks.locks

    assert locks.request(InteractionRequest(20, 1, 2, target, NPC_POLICY_EXCLUSIVE)) == LOCK_GRANTED
    locks.release_for_player(1)
    assert not locks.locks


def test_npc_lock_uses_stable_identity_when_npc_moves() -> None:
    locks = NpcLockManager()
    first_view = InteractionTarget(1, 2, 9, 10, 11, 3)
    moved_view = InteractionTarget(1, 2, 9, 12, 11, 3)
    assert locks.request(InteractionRequest(20, 1, 1, first_view, NPC_POLICY_EXCLUSIVE)) == LOCK_GRANTED
    assert locks.request(InteractionRequest(21, 2, 1, moved_view, NPC_POLICY_EXCLUSIVE)) == LOCK_BUSY


def test_resource_locks_prevent_cross_npc_reward_dupes() -> None:
    locks = NpcLockManager()
    flag_reward = ResourceKey("flag", 1, 2, resource_id=3001)
    item_reward = ResourceKey("item_reward", 1, 2, resource_id=42)
    npc_a = ResourceKey("npc", 1, 2, local_id=9)
    npc_b = ResourceKey("npc", 1, 2, local_id=10)
    target_a = InteractionTarget(1, 2, 9, 10, 11, 3)
    target_b = InteractionTarget(1, 2, 10, 12, 11, 3)

    assert locks.request(InteractionRequest(20, 1, 1, target_a, resources=(npc_a, flag_reward, item_reward))) == LOCK_GRANTED
    assert locks.request(InteractionRequest(20, 2, 1, target_b, resources=(npc_b, flag_reward, item_reward))) == LOCK_BUSY


def test_disabled_and_readonly_barrier_policies() -> None:
    locks = NpcLockManager()
    target = InteractionTarget(1, 2, 11, 10, 11, 3)
    readonly = InteractionRequest(20, 1, 1, target, NPC_POLICY_SHARED_READONLY, ACTION_READONLY_SCRIPT)
    disabled = InteractionRequest(20, 1, 2, target, NPC_POLICY_DISABLED_ONLINE, ACTION_DISABLED_ONLINE)

    assert locks.request(readonly) == LOCK_GRANTED
    assert not locks.locks
    assert locks.request(disabled) == LOCK_DENIED


def test_stale_grant_is_revalidated_client_side() -> None:
    pending = PendingAction(InteractionTarget(1, 2, 7, 10, 11, 3), facing=2, current_map=(1, 2))
    assert pending.grant_is_still_valid((10, 11), 2, (1, 2))
    assert not pending.grant_is_still_valid((14, 11), 2, (1, 2))
    assert not pending.grant_is_still_valid((10, 11), 3, (1, 2))
    assert not pending.grant_is_still_valid((10, 11), 2, (1, 3))
    assert not pending.grant_is_still_valid((10, 11), 2, (1, 2), target_exists=False)


def test_client_hello_requires_ack() -> None:
    hello = HelloHandshake()
    hello.send_hello(delivered=True, accepted=True, profile_ready=False, control_ready=True)
    hello.send_hello(delivered=True, accepted=True, profile_ready=True, control_ready=False)
    assert not hello.acked
    assert hello.status == "not_ready"
    for _ in range(10):
        hello.send_hello(delivered=False)
    assert not hello.acked
    assert hello.status == STATUS_CONNECTING
    hello.send_hello(delivered=True, accepted=True)
    assert hello.acked
    assert hello.status == STATUS_CONNECTED


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
    assert not session.can_publish_snapshot()
    assert not session.pending_transactions
    assert session.avatars.count_for_player(2) == 0


def main() -> None:
    test_duplicate_commit_once()
    test_fail_closed_trade()
    test_loss_reorder_and_retry()
    test_ringbuffer_backpressure()
    test_full_key_commit_result_clears_only_matching_pending()
    test_commit_result_revision_replay_is_noop()
    test_pending_retry_survives_backpressure()
    test_npc_lock_same_tick_deterministic()
    test_npc_shared_and_disconnect_cleanup()
    test_npc_lock_uses_stable_identity_when_npc_moves()
    test_resource_locks_prevent_cross_npc_reward_dupes()
    test_disabled_and_readonly_barrier_policies()
    test_stale_grant_is_revalidated_client_side()
    test_client_hello_requires_ack()
    test_avatar_singleton()
    test_solo_online_toggle_cleanup()
    print("Multiplayer host simulator checks OK")


if __name__ == "__main__":
    main()
