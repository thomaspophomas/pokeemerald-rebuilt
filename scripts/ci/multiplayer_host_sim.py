#!/usr/bin/env python3
"""Reference host-side rules for idempotent multiplayer commits."""

from __future__ import annotations

from dataclasses import dataclass
import random


NET_RELIABLE_QUEUE_SIZE = 16
MODE_SOLO = "solo"
MODE_ONLINE = "online"

PACKET_TRADE_ACTION = 7
PACKET_BATTLE_ACTION = 6

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
    assert not session.can_publish_snapshot()
    assert not session.pending_transactions
    assert session.avatars.count_for_player(2) == 0


def main() -> None:
    test_duplicate_commit_once()
    test_fail_closed_trade()
    test_loss_reorder_and_retry()
    test_ringbuffer_backpressure()
    test_avatar_singleton()
    test_solo_online_toggle_cleanup()
    print("Multiplayer host simulator checks OK")


if __name__ == "__main__":
    main()
