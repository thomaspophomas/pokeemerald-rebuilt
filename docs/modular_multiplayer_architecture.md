# Modular Multiplayer Architecture

<!-- last_updated: 2026-05-17 -->

This project is moving toward a mod-first architecture. The goal is not to
preserve vanilla compare behavior at all costs, but to keep new systems
attachable and removable without hardwiring them through the existing global
engine state.

This file describes the architecture boundary and target shape. For the
normative emulator bridge/server contract, see
[multiplayer_bridge.md](multiplayer_bridge.md). For current build status and
known limits, start with [README.md](../README.md#current-status).

## Feature Gates

- Compile-time features live in `include/config/features.h`.
- Make can override important gates, for example:
  - `make FEATURE_MULTIPLAYER=1`
  - `make FEATURE_MULTIPLAYER=1 FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT=1`
  - `make FEATURE_MULTIPLAYER=1 FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT=1 FEATURE_MULTIPLAYER_AUTOCONNECT=1`
- Engine rules that can destabilize a running save, such as generation rule
  sets, should remain compile-time.
- Save-friendly systems, such as weather layers, following Pokemon, story
  progress, and multiplayer modes, should expose runtime state through a
  versioned module state API. `engine/runtime_state` is the port for those
  settings; the multiplayer Solo/Online choice is persisted in existing
  SaveBlock2 option padding and protected by an exact SaveBlock2 size assert.

## Module Boundary

New feature modules should enter the game through `engine/module_registry`.
The registry currently supports:

- `Init`
- `Frame`
- `MapLoad`
- `PlayerStep`
- `BattleStart`
- `BattleEnd`

New modules should not directly spread reads or writes of the large legacy
globals through feature code. Prefer a port/adapter layer for:

- Overworld object events and player movement
- Battle type flags, battle weather, parties, and controller ownership
- Link and transport details
- Script flags, vars, and dialogue choices
- Save data and runtime settings

## Mod API Layer

The mod-facing API layer lives under `include/mod` and `src/mod`. It is the
new default boundary for systems that should be extended by adding files rather
than editing central code.

`scripts/modgen.py` scans `mods/<modId>/mod.json` plus domain folders for:

- flags
- events
- weather providers
- engine rulesets
- NPC definitions
- map definitions
- mod C sources

The generator emits `include/generated/mod_registry.h`,
`src/generated/mod_registry.c`, and `build/generated/mod_sources.mk`. These are
build outputs. They provide deterministic generated registries while keeping an
empty-mod build equivalent to vanilla.

Current ports:

- `ModFlag_*` stores mod flags in a versioned state block inside existing
  unused SaveBlock1 space and emits `MOD_EVENT_FLAG_CHANGED`.
- `ModEvent_*` provides a bounded event bus for map load, player step, battle,
  time, weather, flag, NPC, and custom hooks.
- `ModWeather_*` resolves displayed weather from map defaults, providers, and
  overrides while preserving composable layers such as rain plus darkness.
- `ModTime_*` exposes day count, minute of day, and time segment. Offline uses
  RTC; online can use the multiplayer server clock.
- `SpriteAssetApi_*` owns shared sheet/palette/template loading for generated
  mod sprite assets.
- `OverworldSpriteApi_*` owns map-facing graphics IDs, player outfits,
  follower sprites, and idempotent virtual avatars.
- `BattleSpriteApi_*` owns battle-facing Pokemon/trainer/move sprites and
  routes battle ball graphics through the mod layer.
- `LanguageApi_*` resolves stable mod text keys through active-language and
  default-language fallback.
- `PokeBallApi_*` owns catch modifiers, throw results, battle scripts, ball
  graphics, and catch-commit hooks.
- `EngineApi_*` exposes generated rulesets, with Gen3 as the default ruleset.
- `NpcApi_*` wraps object-event spawning and visibility for mod-owned NPCs.
- `MapApi_*` wraps map header, connection, warp, and coordinate queries.

New module code should use these ports instead of directly touching raw
weather state, script flags, object-event globals, sprite tables, hard-coded
text tables, Pokeball catch switches, map headers, or ruleset compile flags.
Legacy code may remain direct until it is intentionally migrated.

## Multiplayer Shape

The multiplayer model is intentionally separate from Emerald's original
4-player link model.

- `MAX_NET_PLAYERS` is 8 for the online overworld session.
- PvE battles support up to 2 human players.
- PvP battles support up to 4 human players as 2v2.
- Trade sessions support 2 players.
- Battles and trades are subsessions, so players who are not involved stay in
  the overworld session.

## Transport

The first transport target is an explicit emulator bridge. The ROM talks to a
small `NetTransport_*` interface. The emulator bridge owns player-slot
assignment, host authority, session IDs, and packet fan-out.

`FEATURE_MULTIPLAYER=1` must not read bridge memory by itself. Bridge memory is
used only when `FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT=1` is set.

The emulator bridge memory layout is intentionally private to
`src/multiplayer/transport_emulator.c`. Battle and overworld code must not read
or write bridge addresses directly.

The bridge layout separates an authoritative server view from local client
output. The ROM reads `serverPlayers`/`serverSubsessions`, writes latest-wins
local snapshots, and sends gameplay actions through a bounded reliable
ringbuffer. Packet sequence `0`, stale sequences, unknown packet types,
oversized payloads, mismatched session IDs, and mismatched session epochs are
dropped before they reach session state.

Every online bridge session must expose `transportMode=server_bridge`,
`sessionEpoch`, `playerToken`, `joinNonce`, `serverClockSeconds`,
`protocolVersion`, `bridgeVersion`, `buildId`, `rulesetHash`, and
`featureFlags`. The tracked manifest in `docs/multiplayer_net_manifest.json`
is the build artifact contract that the future server should allowlist. The
emulator-side bridge must resolve `gNetEmulatorBridgeMailbox` from symbols and
write that EWRAM mailbox; fixed pseudo-addresses such as `0x10000000` are not
part of the ROM contract.

Client data is always a request, never authority. The ROM sends
`ClientHello`, `Heartbeat`, `LocalSnapshot`, `MoveIntent`, `InteractIntent`,
`BattleAction`, `TradeAction`, and `ResyncAck` messages. The server/bridge
composes `ServerSessionView`, `ServerClock`, `ResyncRequest`, `BarrierUpdate`,
`SubsessionUpdate`, `CommitResult`, and `DisconnectReason` messages.

Online time must use `MultiplayerClock_*`. Offline systems may continue to use
Emerald RTC behavior, but online rewards, daily limits, weather cycles, and
story progression must commit against server-provided epoch time.

The player chooses Solo or Online in-game. `FEATURE_MULTIPLAYER=1` exposes the
Options/Start-menu toggle, but the session layer must still no-op in Solo:
no transport polling, no local snapshot publishing, and no remote avatar sync.
Switching back to Solo aborts barriers/subsessions, rolls back pending online
work through the commit layer, and clears remote avatars.

## Emulator Risk Model

The multiplayer design assumes emulator clients can fast-forward, pause, frame
advance, load save states, rewind, run scripts, edit RAM, use cheats, change
RTC, and run mismatched ROM/core builds. The hardening rules are:

- Server authority runs at a fixed tick rate and accepts at most one useful
  movement update per player per server tick.
- The emulator transport exposes only one latest snapshot lane, while gameplay
  actions use `NET_RELIABLE_QUEUE_SIZE` ringbuffer slots instead of overwriting
  a prior action packet.
- Reliable packet payloads are capped at `NET_TRANSPORT_PACKET_PAYLOAD_SIZE`
  (`128`) so the mailbox fits inside the current EWRAM budget.
- Snapshots carry `clientFrame`, `serverTickSeen`, `sequence`, and
  `sessionEpoch`; future frames, frame regressions, and stale sequences cause
  drop/resync paths.
- Heartbeat cadence is 500 ms. Players become stale after 2 seconds and
  disconnected after 10 seconds; stale players are hidden/non-interactive.
- Save-state and rewind recovery is epoch based. Old epochs, old sequences, and
  duplicate transaction IDs must trigger resync or idempotent commit results.
- Battle, trade, and story commits need a server commit log. Trade uses
  prepare/lock-escrow/commit with rollback on timeout or disconnect.
- Trade, item, party, story-flag, outfit, reward, and battle-result commits are
  fail-closed until a server mirror exists for the affected state.
- Cheats and RAM edits cannot authoritatively set money, items, Pokemon, flags,
  position, battle values, busy state, or hidden state online.
- Lua/scripted input is rate-limited and auditable instead of treated as
  impossible to detect in the ROM.
- Emulator netplay/runahead/rollback must not be mixed with this protocol; the
  bridge advertises `server_bridge`, and frame rollback triggers resync.
- ROM hash, ruleset hash, feature flags, protocol version, and bridge version
  are part of the join handshake. Emulator name/version is diagnostic only.

## Multiplayer Safety Nets

- `MultiplayerSession_Tick` publishes overworld snapshots and syncs remote
  avatars only while the real overworld callbacks are active.
- Remote snapshots are validated in the session layer for tick freshness,
  server tick freshness, session epoch, sequence, map IDs, coordinates,
  elevation, direction, flags, stale/disconnect state, and subsession state.
- Future snapshots are accepted only inside a small skew window.
- Subsessions treat `INVITING`, `READY`, and `ACTIVE` as blocking states with
  timeouts; `ENDING` and `ERROR` are terminal cleanup states that do not keep
  players busy forever.
- Interaction barriers reserve a short owner/participant/map scoped window for
  script, warp, battle-invite, and trade-invite flows.
- Remote players are non-colliding virtual objects. Their virtual object IDs
  use the reserved high range `NET_REMOTE_PLAYER_VIRTUAL_ID_BASE` through
  `NET_REMOTE_PLAYER_VIRTUAL_ID_END`, above the known scripted audience IDs
  and Union Room virtual-object range.
- If sprite capacity is tight, remote avatars are shown deterministically by
  distance and player slot priority instead of relying on random spawn failure.
- Remote-avatar spawns are idempotent per session epoch, player slot, and
  virtual object ID. Graphics changes update an existing sprite instead of
  creating a second one.
- `scripts/ci/multiplayer_fuzz.py` covers host-side invariants for snapshot
  validation, packet envelopes, stale/future ticks, epochs, handshake mismatch,
  and extreme coordinates.
- `scripts/ci/multiplayer_host_sim.py` models idempotent server commits,
  packet loss, duplicate delivery, retry, trade rollback, ringbuffer
  backpressure, and remote-avatar singleton ownership.
- `scripts/ci/check_net_manifest.py` prevents protocol/build/bridge constants
  from drifting away from the manifest consumed by the future server allowlist.

## Current Foundation

This pass adds:

- Build-time feature gates
- Module registry hooks
- Versioned runtime-state API for future save-backed toggles
- A versioned packet envelope
- An 8-player session model
- Emulator transport adapter skeleton
- Overworld snapshot publication
- Remote avatar pool for up to 7 visible remote players rendered as
  non-colliding virtual objects
- Battle and trade subsession request APIs
- Session-side snapshot and subsession validation
- Emulator-bridge server-view/local-output separation
- Host-side multiplayer fuzz checks
- Epoch/token/nonce handshake fields
- Client hello and heartbeat packets
- Move/interaction/battle/trade/server-clock packet taxonomy
- Server-clock port for future online daily events and weather cycles
- Multiplayer net manifest CI check
- Idempotent `MultiplayerCommit_*` API with stable transaction keys and
  fail-closed online side effects
- Reliable bridge ringbuffer for gameplay actions
- Remote-avatar create/update singleton behavior for graphics changes
- Build-time mod generator and generated mod registry contract
- Mod-facing APIs for flags, events, weather, time, engine rulesets, NPCs, and
  maps
- Mod-facing APIs for shared sprite assets, overworld sprites, battle sprites,
  language text, and Pokeball rules

This is still foundation work. It is not a finished online multiplayer release:
there is no production emulator-side bridge, authoritative server, matchmaking,
server persistence mirror, or public online play path in this repository.

The next implementation pass should replace the bridge skeleton with an actual
emulator-side bridge, move battle-controller synchronization behind
`BattleSession`, and progressively migrate legacy story/weather/NPC/map code to
the new mod ports. Fully new item IDs, shops, quests, dialogs, encounters, and
audio should get their own ports before being exposed to mods.
