# Multiplayer Bridge Contract

<!-- last_updated: 2026-05-17 -->

This document is the normative ROM-side contract for the future emulator bridge
and authoritative server. The repository currently implements the ROM-side
adapter and validation scaffolding only. It does not include a production
bridge process, server, matchmaking layer, or persistence mirror.

## Status

- `FEATURE_MULTIPLAYER=1` compiles the session, overworld, battle, trade,
  clock, and commit foundation.
- `FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT=1` enables the EWRAM mailbox
  transport adapter in `src/multiplayer/transport_emulator.c`.
- Without both flags, `NetTransport_*` is a closed/no-op transport.
- Online gameplay state is not authoritative in the ROM. The future server must
  validate movement, interaction, battle, trade, party, inventory, time, story,
  reward, and persistence changes.

## Version Manifest

`docs/multiplayer_net_manifest.json` is the allowlist contract for bridge and
server builds. The current `master` values are:

```json
{
  "protocolVersion": 2,
  "emulatorBridgeVersion": 5,
  "buildId": "0x00010000",
  "rulesetHash": "0x00000003",
  "transportMode": "server_bridge",
  "transportModeValue": 1,
  "maxNetPlayers": 8,
  "reliableQueueSize": 16,
  "packetPayloadSize": 128,
  "commitLogSize": 32,
  "authoritativeServer": true,
  "mailbox": {
    "symbol": "gNetEmulatorBridgeMailbox",
    "struct": "NetEmulatorBridgeBuffer",
    "memoryDomain": "EWRAM",
    "minAddress": "0x02000000",
    "maxAddress": "0x0203FFFF",
    "maxSize": 8192
  }
}
```

`scripts/ci/check_net_manifest.py` must pass whenever constants in
`include/multiplayer/constants.h` or this manifest change.

## EWRAM Mailbox

The emulator bridge must resolve the exported ROM symbol
`gNetEmulatorBridgeMailbox` from the generated `.sym` file or an equivalent
emulator symbol API, then read/write that EWRAM address. The bridge must not
use a fixed host-side pseudo-address. The mailbox is owned by the ROM image and
is constrained by `NET_EMULATOR_MAILBOX_MAX_SIZE` (`8192` bytes) so feature
builds keep enough EWRAM headroom.

The ROM accepts the mailbox only when all header checks pass:

- `magic == NET_EMULATOR_BRIDGE_MAGIC` (`"NED8"`)
- `version == NET_EMULATOR_BRIDGE_VERSION` (`5`)
- `transportMode == NET_TRANSPORT_MODE_SERVER_BRIDGE` (`1`)
- `localPlayerId < MAX_NET_PLAYERS`
- `hostPlayerId < MAX_NET_PLAYERS`
- `playerCount <= MAX_NET_PLAYERS`
- `sessionEpoch`, `playerToken`, and `joinNonce` are nonzero

Current packed layout:

```c
struct NetEmulatorBridgeBuffer
{
    u32 magic;
    u16 version;
    u8 connected;
    u8 localPlayerId;
    u8 hostPlayerId;
    u8 playerCount;
    u8 transportMode;
    u8 reserved;
    u32 sessionId;
    u32 sessionEpoch;
    u32 playerToken;
    u32 joinNonce;
    u32 bridgeTick;
    u32 serverClockSeconds;
    u32 viewSequence;
    u32 localSnapshotSequence;
    u32 reliableOutboundHead;
    u32 reliableOutboundTail;
    u32 reliableInboundHead;
    u32 reliableInboundTail;
    u32 latestUnreliableSequence;
    struct NetPlayerSnapshot serverPlayers[MAX_NET_PLAYERS];
    struct MultiplayerSubsession serverSubsessions[MAX_NET_SUBSESSIONS];
    struct NetPlayerSnapshot localSnapshot;
    struct NetTransportPacketSlot latestUnreliable;
    struct NetTransportPacketSlot reliableOutbound[NET_RELIABLE_QUEUE_SIZE];
    struct NetTransportPacketSlot reliableInbound[NET_RELIABLE_QUEUE_SIZE];
};
```

The layout is declared in `include/multiplayer/bridge_mailbox.h` and consumed by
the transport adapter. Gameplay code must use `NetTransport_*`,
`MultiplayerSession_*`, and `MultiplayerCommit_*` rather than reading the
mailbox directly.

## Stable Server View

The bridge owns `serverPlayers`, `serverSubsessions`, `bridgeTick`,
`serverClockSeconds`, and session identity fields. The ROM reads them through
`NetTransport_ReadSessionView`.

Bridge write protocol:

1. Increment `viewSequence` to an odd value.
2. Write the full server view.
3. Increment `viewSequence` to the next even value.

ROM read protocol:

1. Read `viewSequence`.
2. Reject odd values.
3. Copy the server view.
4. Read `viewSequence` again.
5. Accept only if both reads match and the value is even.

The ROM currently tries this stable read three times per tick.

## Handshake

The bridge creates a session by filling the validated header fields and setting
`connected`. The ROM then sends `NET_PACKET_CLIENT_HELLO` through the reliable
queue with:

- `protocolVersion`
- `bridgeVersion`
- `buildId`
- `romHash`
- `rulesetHash`
- `featureFlags`
- `transportMode`

The server must reject mismatches for protocol version, bridge version, build
ID, ruleset hash, required feature flags, and transport mode. Emulator
name/version is diagnostic only and is not a trust boundary.

After hello, the ROM sends `NET_PACKET_HEARTBEAT` every
`NET_HEARTBEAT_INTERVAL_FRAMES` with:

- `clientFrame`
- `serverTickSeen`
- `sessionEpoch`
- `playerToken`
- `joinNonce`

Old epochs, stale tokens, stale nonces, zero packet sequences, duplicate
transactions, and rewind/save-state regressions must trigger resync or cached
idempotent results.

## Snapshot Lane

`NetTransport_WriteLocalSnapshot` writes one latest-wins local snapshot:

- The snapshot must have the local `playerId`.
- `sessionEpoch`, `playerToken`, and `joinNonce` must match the current bridge
  header.
- The ROM increments `localSnapshotSequence`, writes `localSnapshot`, then
  increments `localSnapshotSequence` again.

Snapshots are for remote-avatar composition and low-value overworld state. They
are not authoritative movement, inventory, party, story, battle, trade, reward,
or time commits.

The server view publishes player snapshots back in `serverPlayers`. Session
validation drops snapshots with invalid player IDs, stale/future ticks, wrong
epochs, invalid map/coordinate ranges, invalid flags, stale disconnect state,
or unsafe subsession state.

## Packet Lanes

`NetTransport_SendUnreliablePacket` writes `latestUnreliable`. It is latest-wins
and suitable only for replaceable state.

`NetTransport_SendPacket` writes to `reliableOutbound`. This bounded ring buffer
is used for gameplay actions and session control:

- Queue size: `NET_RELIABLE_QUEUE_SIZE` (`16`)
- Payload capacity: `NET_TRANSPORT_PACKET_PAYLOAD_SIZE` (`128`)
- Sequence numbers are nonzero and monotonic per session identity.
- Full queues return `FALSE`; callers must keep behavior fail-closed.
- Each envelope carries protocol version, header size, session ID, session
  epoch, tick, packet type, player ID, sequence, payload size, and checksum.

`NetTransport_ReceivePacket` reads `reliableInbound`. The ROM drops inbound
packets with:

- wrong protocol magic or version
- wrong header size
- wrong session ID or epoch
- invalid packet type
- invalid player ID
- sequence `0`
- stale sequence
- oversized payload
- checksum mismatch

## Packet Taxonomy

Client-to-server packet types:

- `NET_PACKET_CLIENT_HELLO`
- `NET_PACKET_HEARTBEAT`
- `NET_PACKET_PLAYER_SNAPSHOT`
- `NET_PACKET_MOVE_INTENT`
- `NET_PACKET_INTERACT_INTENT`
- `NET_PACKET_BATTLE_ACTION`
- `NET_PACKET_TRADE_ACTION`
- `NET_PACKET_RESYNC_ACK`
- `NET_PACKET_SUBSESSION_INVITE`
- `NET_PACKET_SUBSESSION_ACCEPT`
- `NET_PACKET_SUBSESSION_DECLINE`
- `NET_PACKET_SUBSESSION_COMMIT`
- `NET_PACKET_SUBSESSION_ABORT`
- `NET_PACKET_BATTLE_INPUT`
- `NET_PACKET_TRADE_INPUT`

Server-to-client packet types:

- `NET_PACKET_SERVER_SESSION_VIEW`
- `NET_PACKET_SERVER_CLOCK`
- `NET_PACKET_RESYNC_REQUEST`
- `NET_PACKET_BARRIER_UPDATE`
- `NET_PACKET_SUBSESSION_UPDATE`
- `NET_PACKET_COMMIT_RESULT`
- `NET_PACKET_DISCONNECT_REASON`

The future bridge may fan out server views through the mailbox rather than the
packet queue, but packet envelopes must keep the same validation rules.

## Commit Results

Gameplay side effects use `MultiplayerCommit_*` and stable transaction keys:

```c
struct MultiplayerTransactionKey
{
    u32 sessionEpoch;
    u32 actionSequence;
    u8 playerId;
    u8 packetType;
    u8 subsessionId;
    u8 reserved;
};
```

Server replies use:

```c
struct NetCommitResult
{
    u32 transactionId;
    u32 serverRevision;
    u16 payloadChecksum;
    u8 commitType;
    u8 result;
    u16 detail;
};
```

The ROM commit log holds `NET_COMMIT_LOG_SIZE` (`32`) entries and caches
duplicate/retry results. Trade, item, battle, story-flag, outfit, and
weather-reward commits are fail-closed locally until a server mirror owns the
affected state.

## Server Authority

Client data is always a request. The server must be authoritative for:

- player slot assignment and session identity
- movement cadence, position, map, hidden/stale state, and interaction range
- script, warp, battle invite, and trade invite barriers
- battle actions and battle outcomes
- trade prepare, escrow, commit, rollback, timeout, and disconnect handling
- item, party, money, reward, story flag, outfit, and daily-event state
- online time, weather cycles, and time-gated rewards
- ROM hash, ruleset hash, feature flags, protocol version, and bridge version

The emulator client may fast-forward, pause, frame advance, load save states,
rewind, run scripts, edit RAM, use cheats, change RTC, or run a mismatched ROM.
The protocol must remain correct under those assumptions.

## Open Work

Before this becomes a playable online path, the project still needs:

- an emulator-side bridge implementation for this EWRAM mailbox contract
- an authoritative server with player/session/matchmaking lifecycle
- server mirrors for party, inventory, money, flags, rewards, outfits, time,
  battle results, and trade escrow
- BattleSession controller synchronization behind `multiplayer/battle`
- client recovery UX for resync, disconnect, and rejected commits
- CI or integration tests that run the ROM adapter against a real bridge
