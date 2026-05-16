---
metadata:
  version: 0.1.0
---

# CLAUDE.md - pokeemerald-rebuilt

<!-- last_updated: 2026-05-16 -->

Briefing for Claude and other AI consumers. This file mirrors the agent-facing
rules in [AGENTS.md](AGENTS.md); keep both in sync when changing repository
conventions.

## TL;DR

Mod-first Pokemon Emerald decomp repo. The current architecture work is an
8-player online overworld plus battle/trade subsessions:

- Overworld: up to 8 online players.
- PvE battle: up to 2 human players.
- PvP battle: up to 4 human players as 2v2.
- Trade: 2-player subsession.
- Transport: emulator bridge adapter, isolated behind `NetTransport_*` and
  opt-in with `FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT=1`.
- Runtime-safe feature toggles: `engine/runtime_state`.
- Mod-facing APIs: weather, time, events, flags, sprites, language text,
  Pokeballs, engine rulesets, NPCs, and maps under `include/mod` and `src/mod`.
- Static mod registries generated from `mods/<modId>/...` by
  `scripts/modgen.py`.
- Multiplayer starts in Solo and switches to Online in-game through
  runtime/session ports.
- Online server selection uses save-backed `NetServerConfig` profiles; the ROM
  publishes IPv4/port config to the local bridge and never opens sockets.
- Engine rule sets: compile-time feature gates.

## Architecture

```text
main.c / overworld.c / battle_main.c
    -> engine/module_registry
        -> engine/runtime_state
        -> mod/core
        -> multiplayer/session
            -> multiplayer/transport
            -> multiplayer/overworld
            -> multiplayer/battle
            -> multiplayer/trade
```

Attach new features through the registry and ports. Avoid spreading direct
legacy-global access through feature code.

## Conventions

- Feature gates live in `include/config/features.h`.
- Mod manifests and content live under `mods/<modId>/...`; generated
  registries are build outputs, not hand-edited source.
- Use the mod ports for new systems: `ModFlag_*`, `ModWeather_*`,
  `ModTime_*`, `ModEvent_*`, `SpriteAssetApi_*`, `OverworldSpriteApi_*`,
  `BattleSpriteApi_*`, `LanguageApi_*`, `PokeBallApi_*`, `EngineApi_*`,
  `NpcApi_*`, and `MapApi_*`.
- Raw flag/weather/object/sprite/text/Pokeball/map globals belong in domain
  adapters only when adding new module-facing code.
- Runtime-safe module state starts in `engine/runtime_state`.
- Multiplayer bridge memory is private to `src/multiplayer/transport_*`.
- `FEATURE_MULTIPLAYER=1` must not read emulator bridge memory unless
  `FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT=1` is enabled.
- Emulator bridge reads consume the authoritative server view; local client
  writes go only to local snapshot/packet output lanes.
- Online session identity must include `sessionEpoch`, `playerToken`,
  `joinNonce`, protocol/build/ruleset metadata, ROM hash, heartbeat,
  `ServerHelloAck`, and server-bridge transport mode.
- Client data is untrusted online. Treat movement, interactions, battle input,
  trade input, party/inventory/state values, local RTC, and local flags as
  requests that require server validation.
- Online gameplay side effects must use `MultiplayerCommit_*` and stable
  transaction keys. Duplicate, replayed, retried, rolled-back, or late packets
  must return the cached result instead of applying twice.
- Online NPC/script interactions must pass through the multiplayer interaction
  preflight. Unknown targets are exclusive; only explicit `SHARED_READONLY`
  mod NPCs may start dialog locally without a server lock grant.
- Trade, item, party, story-flag, outfit, reward, and battle-result commits are
  fail-closed until a server mirror exists for the affected state.
- Reliable gameplay actions belong on the bridge ringbuffer and pending
  transaction table; overworld snapshots stay latest-wins.
- Remote avatars must use idempotent virtual-object create/update APIs so one
  remote player cannot spawn duplicate sprites across retries or resyncs.
- Time-based online systems must use `MultiplayerClock_*` and server time;
  Emerald RTC is only an offline compatibility fallback.
- Multiplayer overworld ticks must be gated to real overworld callbacks, not
  menus, battle callbacks, or map-load gaps.
- `gObjectEvents` access is allowed only inside the overworld adapter layer.
- Remote overworld players are rendered as non-colliding virtual objects with
  a reserved high virtual-ID range.
- Battle controller synchronization belongs behind `multiplayer/battle`.
- UI may switch multiplayer only through `engine/runtime_state` and
  `MultiplayerSession_*`; it must not call `NetTransport_*` directly.
- The Solo/Online setting uses existing SaveBlock2 option padding and must keep
  `sizeof(struct SaveBlock2) == 0xF2C`.
- Additional multiplayer server profile state belongs in `ModSaveState`, not
  in a widened `SaveBlock2`.
- Save layout changes require a migration plan before implementation; reusing
  padding still requires an exact size assertion.
- Do not upload built ROM artifacts from CI/CD.

## Discovery Order

1. `README.md`
2. `AGENTS.md`
3. `docs/modular_multiplayer_architecture.md`
4. `include/config/features.h`
5. `include/engine/` and `src/engine/`
6. `include/mod/` and `src/mod/`
7. `include/multiplayer/` and `src/multiplayer/`

## Do NOT

- Do not copy external multiplayer forks wholesale.
- Do not add direct emulator-address reads outside the transport adapter.
- Do not hand-edit generated mod registries or require central registry edits
  for new mod content.
- Do not make emulator transport implicit for `FEATURE_MULTIPLAYER=1`.
- Do not spawn remote players as normal map `ObjectEvent`s.
- Do not add ad hoc transport checks to legacy battle controllers.
- Do not change SaveBlocks or core legacy structs without a migration plan.
- Do not commit online rewards, trades, battle outcomes, story flags, or daily
  events from client-provided ROM/RAM/RTC state.
