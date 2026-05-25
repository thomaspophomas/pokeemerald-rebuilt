# Multiplayer Bridge Contract

<!-- last_updated: 2026-05-24 -->

This document is the normative ROM-side contract for the future emulator bridge
and authoritative server. The repository currently implements the ROM-side
adapter and validation scaffolding only. It does not include a production
bridge process, server, matchmaking layer, or persistence mirror.

## Status

- `FEATURE_MULTIPLAYER=1` compiles the session, overworld, battle, trade,
  clock, and commit foundation.
- `FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT=1` enables the EWRAM mailbox
  transport adapter in `src/multiplayer/transport_emulator.c`.
- `FEATURE_MULTIPLAYER_LINK_TRANSPORT=1` enables the native link-gateway frame
  adapter in `src/multiplayer/transport_link.c`.
- `FEATURE_MODS=0` is the default; in that mode profile/catalog metadata is
  advertised as zero-capability and the base multiplayer build does not depend
  on generated mod registries.
- The emulator mailbox and link-gateway transports are mutually exclusive.
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
  "emulatorBridgeVersion": 10,
  "buildId": "0x0001000C",
  "rulesetHash": "0x00000003",
  "profileProtocolVersion": 1,
  "profileCapabilityHash": "0x00000006",
  "modCatalogSchemaHash": "0x00000006",
  "modCatalogHash": "0x2EB46D50",
  "transportMode": "server_bridge",
  "transportModeValue": 1,
  "maxNetPlayers": 8,
  "reliableQueueSize": 8,
  "packetHeaderSize": 22,
  "packetPayloadSize": 96,
  "profileChunkDataSize": 80,
  "catalogChunkEntryCount": 7,
  "battleProfilePacket": "NET_PACKET_PLAYER_BATTLE_PROFILE",
  "battleProfilePublishIntervalFrames": 60,
  "battleProfilePublishRetryFrames": 10,
  "commitLogSize": 16,
  "authoritativeServer": true,
  "mailbox": {
    "symbol": "gNetEmulatorBridgeMailbox",
    "struct": "NetEmulatorBridgeBuffer",
    "memoryDomain": "EWRAM",
    "minAddress": "0x02000000",
    "maxAddress": "0x0203FFFF",
    "maxSize": 4096
  }
}
```

`scripts/ci/check_net_manifest.py` must pass whenever multiplayer/profile/catalog
contract constants or this manifest change.

With `FEATURE_MODS=1`, offline gameplay uses the mods compiled into the ROM.
Online gameplay is server-authoritative through a runtime profile: after
`ClientHello`, the server may send a bounded delta profile that overrides
supported mod API surfaces such as text, weather, engine ruleset ID, NPC
definitions, sprite assets, badge effects, fishing actions, encounter
definitions, shop inventories, item metadata, reward tables, Pokemon species
data, battle move data, and trainer definitions. The server can choose a
different profile per room without requiring a new ROM build, as long as the ROM
advertises the required profile protocol, capabilities, and mod catalog
metadata. With `FEATURE_MODS=0`, those capability/catalog fields are zero and
the server must treat the client as a base-game build.

`buildId`, `rulesetHash`, protocol version, bridge version, feature flags, and
transport mode remain the base compatibility gate. `rulesetHash` is not the
server modpack selector; it describes the compiled engine/protocol rules the
runtime profile depends on. `profileCapabilityHash` identifies the runtime
profile schema supported by this ROM. `MOD_CATALOG_SCHEMA_HASH` identifies the
catalog entry taxonomy used to interpret generated catalog rows. `modCatalogHash`
identifies the generated ROM mod catalog contents so servers can avoid sending
data already present in the ROM, and each server profile carries its own
`profileHash`.

## EWRAM Mailbox

The emulator bridge must resolve the exported ROM symbol
`gNetEmulatorBridgeMailbox` from the generated `.sym` file or an equivalent
emulator symbol API, then read/write that EWRAM address. The bridge must not
use a fixed host-side pseudo-address. The mailbox is owned by the ROM image and
is constrained by `NET_EMULATOR_MAILBOX_MAX_SIZE` (`4096` bytes) so feature
builds keep enough EWRAM headroom.

The ROM accepts the mailbox only when all header checks pass:

- `magic == NET_EMULATOR_BRIDGE_MAGIC` (`"NED8"`)
- `version == NET_EMULATOR_BRIDGE_VERSION` (`10`)
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

## Link Gateway Frames

The native link-gateway transport is for emulators that expose a link-cable or
Wi-Fi-link path instead of mGBA's Lua memory API. The ROM uses the normal Emerald
link block layer and exchanges fixed `256` byte frames declared in
`include/multiplayer/link_transport.h`.

Every frame carries:

- `magic == NET_LINK_GATEWAY_FRAME_MAGIC` (`0x31474C4E`)
- `version == NET_LINK_GATEWAY_FRAME_VERSION` (`1`)
- `frameType`
- `sequence`
- bounded payload bytes

The server side must translate these frames through `pokeonline-link-gateway`.
Current frame types cover session headers, server player snapshots, server
subsessions, server packet slots, client player snapshots, and client packet
slots. This keeps the ROM gameplay layer on the same `NetTransport_*` contract
used by the EWRAM mailbox transport.

The checked-in host gateway currently validates the fixed-frame contract with a
raw test client. MyBoy, Linkboy, and Pizza Boy support still requires a real
adapter that maps each emulator's external link protocol to these frames.

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
- `profileProtocolVersion`
- `profileCapabilityFlags`
- `profileCapabilityHash`
- `modCatalogHash`
- `modCatalogCount`
- `transportMode`

The server must reject mismatches for protocol version, bridge version, build
ID, ruleset hash, required feature flags, profile protocol/capabilities, and
transport mode. Emulator name/version is diagnostic only and is not a trust
boundary. `romHash` is reserved for a future bridge that can hash the actual
ROM file; it is not the current modpack selection mechanism.
Catalog responses later carry `MOD_CATALOG_SCHEMA_HASH` in
`NET_PACKET_CLIENT_CATALOG_BEGIN`; it is separate from the hello/profile
capability fields.

## Server Runtime Profile

The runtime profile lane lets the server dictate supported mods without
requiring a ROM rebuild. Servers should treat profiles as deltas against the
ROM mod catalog: if a desired text, weather/ruleset, NPC, sprite asset, badge
effect, fishing-action, encounter, shop, item, reward, Pokemon data, battle
move, or trainer entry already exists in the ROM with the same key/content
hash, the server omits that record and lets the ROM fallback path use its
generated registry. The profile itself is a compact TLV blob carried over the
reliable packet lane:

1. Server sends `NET_PACKET_SERVER_PROFILE_BEGIN` with `profileHash`,
   `profileSize`, `chunkCount`, `profileProtocolVersion`, `capabilityFlags`,
   and `capabilityHash`.
2. Server sends `NET_PACKET_SERVER_PROFILE_CHUNK` packets. Each chunk carries
   an offset and up to `NET_PROFILE_CHUNK_DATA_SIZE` (`80`) profile bytes.
3. Server sends `NET_PACKET_SERVER_PROFILE_COMMIT`.
4. ROM validates chunk coverage and hash, parses the profile, then replies with
   `NET_PACKET_SERVER_PROFILE_ACK`.

The current ROM accepts at most `MOD_RUNTIME_PROFILE_MAX_BLOB_SIZE` (`8192`)
bytes. It supports text, weather, engine ruleset ID, NPC records, references to
compiled sprite assets, small inline sprite sheets/palettes, and badge effect
records for mod APIs that query badge-owned modifiers, encounter definitions,
shop inventories, item metadata/use hooks, reward tables, Pokemon species data,
battle move data, and trainer definitions. Fishing-action, encounter, item, and
reward profile records may only reference compiled ROM hooks by `sourceKey` and
`hookKey`; the server never sends function pointers, scripts, or hook bytecode.
Shop, Pokemon-data, battle-move, and trainer records are bounded data-only
records. It does not accept arbitrary executable code, new maps, audio, scripts,
save-schema changes, unrestricted asset packs, or unbounded battle formula
changes. Unsupported capability bits, unsupported profile versions, hash
mismatches, malformed records, unknown hook references, or oversized profiles
must fail closed and produce a non-OK ACK.

The receive blob is temporary. After hash validation, the ROM pre-scans the
profile and allocates only the exact text/inline-asset buffers needed by the
delta records, then frees the receive blob after a successful commit.

With `FEATURE_MODS=1` and no server profile active, mod APIs read the ROM's
generated registry. When a server profile is active, supported API lookups
prefer the server profile and fall back to the generated registry only for
missing keys. Leaving or losing the online session clears the runtime profile
and restores local ROM mods. With `FEATURE_MODS=0`, the extension-profile facade
has no generated registry and all mod-domain calls resolve to vanilla-compatible
stubs.

If the server does not recognize a nonzero `modCatalogHash`, it may request the
generated catalog through `NET_PACKET_SERVER_CATALOG_REQUEST`. The ROM responds
with `NET_PACKET_CLIENT_CATALOG_BEGIN` and `NET_PACKET_CLIENT_CATALOG_CHUNK`
packets. The begin packet carries `schemaHash == MOD_CATALOG_SCHEMA_HASH`; a
bridge or server must reject unknown schema hashes and must not build a delta
profile from that catalog. Each catalog entry contains a type, key hash, and
content hash, not full asset or text payloads. The server caches that catalog by
hash and uses it to build future delta profiles only after the schema hash is
accepted.

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
- `sessionEpoch` must match the current bridge header. `playerToken` and
  `joinNonce` stay in hello/heartbeat/session state instead of the hot snapshot.
- The ROM increments `localSnapshotSequence`, writes `localSnapshot`, then
  increments `localSnapshotSequence` again.
- The ROM writes only when hot overworld state changes or every
  `NET_PLAYER_SNAPSHOT_IDLE_REPUBLISH_FRAMES` (`15`) frames as an idle refresh.

Snapshots are for remote-avatar composition and low-value overworld state. They
are not authoritative movement, inventory, story, battle, trade, reward, or time
commits. They also do not carry party/battle profile data; that profile uses the
bounded reliable packet lane so the EWRAM mailbox does not retain full party
snapshots in every latest-wins player slot.

The server view publishes player snapshots back in `serverPlayers`. Session
validation drops snapshots with invalid player IDs, stale/future ticks, wrong
epochs, invalid map/coordinate ranges, invalid flags, stale disconnect state,
or unsafe subsession state.

## Battle Profile Lane

The ROM publishes `struct NetPlayerBattleProfile` with
`NET_PACKET_PLAYER_BATTLE_PROFILE`. The bridge/server must treat it as cached
client-provided data for partner previews and must still validate any battle or
party side effect authoritatively. The ROM rebuilds and sends the profile at
most every `NET_PLAYER_PROFILE_PUBLISH_INTERVAL_FRAMES` (`60`) frames, retrying
after `NET_PLAYER_PROFILE_PUBLISH_RETRY_FRAMES` (`10`) frames if the reliable
queue is full.

The profile contains only the compact partner material needed by the current ROM
battle adapter: trainer gender, player name, up to
`NET_PLAYER_PARTY_SNAPSHOT_SIZE` (`3`) usable party members, held items, HP, and
move IDs. It is fan-out packet state, not mailbox view state.

## Packet Lanes

`NetTransport_SendUnreliablePacket` writes `latestUnreliable`. It is latest-wins
and suitable only for replaceable state.

`NetTransport_SendPacket` writes to `reliableOutbound`. This bounded ring buffer
is used for gameplay actions and session control:

- Queue size: `NET_RELIABLE_QUEUE_SIZE` (`8`)
- Slot header size: `NET_TRANSPORT_PACKET_HEADER_SIZE` (`22`)
- Payload capacity: `NET_TRANSPORT_PACKET_PAYLOAD_SIZE` (`96`)
- Sequence numbers are nonzero and monotonic per session identity.
- Full queues return `FALSE`; callers must keep behavior fail-closed.
- Each mailbox slot stores only session ID, session epoch, tick, packet type,
  player ID, sequence, payload size, and checksum. The ROM reconstructs the
  full `NetPacketEnvelope` with protocol magic/version/header-size fields when
  gameplay code receives the packet.

Mailbox packet slots use this packed header:

```c
struct NetTransportPacketHeader
{
    u32 sessionId;
    u32 sessionEpoch;
    u32 tick;
    u32 sequence;
    u16 payloadSize;
    u16 checksum;
    u8 packetType;
    u8 playerId;
};
```

`NetTransport_ReceivePacket` reads `reliableInbound`. The ROM drops inbound
packets with:

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
- `NET_PACKET_PLAYER_BATTLE_PROFILE`
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
- `NET_PACKET_PLAYER_BATTLE_PROFILE`
- `NET_PACKET_SERVER_PROFILE_BEGIN`
- `NET_PACKET_SERVER_PROFILE_CHUNK`
- `NET_PACKET_SERVER_PROFILE_COMMIT`
- `NET_PACKET_SERVER_CATALOG_REQUEST`

Client profile/catalog packets:

- `NET_PACKET_SERVER_PROFILE_ACK`
- `NET_PACKET_CLIENT_CATALOG_BEGIN`
- `NET_PACKET_CLIENT_CATALOG_CHUNK`

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

The ROM commit log holds `NET_COMMIT_LOG_SIZE` (`16`) entries and caches
duplicate/retry results. Pending entries are never evicted; once all slots are
occupied, old committed or rolled-back entries may be reused for newer
transactions. Trade, item, battle, story-flag, outfit, and weather-reward
commits are fail-closed locally until a server mirror owns the affected state.

## Server Authority

Client data is always a request. The server must be authoritative for:

- player slot assignment and session identity
- movement cadence, position, map, hidden/stale state, and interaction range
- script, warp, battle invite, and trade invite barriers
- battle actions and battle outcomes
- trade prepare, escrow, commit, rollback, timeout, and disconnect handling
- item, party, money, reward, story flag, outfit, and daily-event state
- online time, weather cycles, and time-gated rewards
- runtime profile selection and profile hash
- ROM mod catalog hashing and delta-profile generation
- build ID, ruleset hash, profile capabilities, feature flags, protocol
  version, bridge version, and future ROM hash checks

The emulator client may fast-forward, pause, frame advance, load save states,
rewind, run scripts, edit RAM, use cheats, change RTC, or run a mismatched ROM.
The protocol must remain correct under those assumptions.

## Host Stack Policy

Host-side checks in this ROM repository are Python invariant simulations under
`scripts/ci/`. A production emulator bridge or authoritative server should live
outside this repository or be introduced here only after an ADR covers the
toolchain, lockfile, CI, ownership, deployment, and security model. See
`docs/adr/0001-host-stack-policy.md`.

## Open Work

Before this becomes a playable online path, the project still needs:

- an emulator-side bridge implementation for this EWRAM mailbox contract
- an authoritative server with player/session/matchmaking lifecycle
- server mirrors for party, inventory, money, flags, rewards, outfits, time,
  battle results, and trade escrow
- BattleSession controller synchronization behind `multiplayer/battle`
- client recovery UX for resync, disconnect, and rejected commits
- CI or integration tests that run the ROM adapter against a real bridge
