---
metadata:
  version: 0.1.0
---

# AGENTS.md - pokeemerald-rebuilt

<!-- last_updated: 2026-05-16 -->

Briefing for AI agents working in this repository. Read this first, then inspect
the relevant source files before changing code.

## TL;DR

This is a mod-first Pokemon Emerald decomp repo. The active architectural goal
is a multiplayer-first modular refactor:

- 8 online players in the overworld session.
- PvE battle subsessions with up to 2 human players.
- PvP battle subsessions with up to 4 human players as 2v2.
- Trade subsessions with 2 players.
- Emulator bridge transport adapter, isolated behind `NetTransport_*` and
  enabled only with `FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT=1`.
- Runtime-safe feature settings behind `engine/runtime_state`.
- Mod-facing APIs for weather, time, events, flags, sprites, language text,
  Pokeballs, engine rulesets, NPCs, and maps under `include/mod` and `src/mod`.
- Build-time mod registries generated from `mods/<modId>/...` by
  `scripts/modgen.py`.
- Multiplayer starts in Solo and is switched to Online in-game through
  runtime/session ports.
- Engine-generation rule sets remain compile-time.

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

The module registry is the feature attachment point. New systems should expose
ports and hooks rather than patching broad legacy globals directly.

## Conventions

- Use `include/config/features.h` for build-time feature gates.
- Modder-facing content belongs in `mods/<modId>/...`; do not require manual
  edits to central registries for new flags, events, weather providers,
  rulesets, NPC definitions, maps, or mod C sources.
- `scripts/modgen.py` owns generated registries. Do not hand-edit
  `include/generated` or `src/generated` outputs.
- New feature code should use `ModFlag_*`, `ModWeather_*`, `ModTime_*`,
  `ModEvent_*`, `SpriteAssetApi_*`, `OverworldSpriteApi_*`,
  `BattleSpriteApi_*`, `LanguageApi_*`, `PokeBallApi_*`, `EngineApi_*`,
  `NpcApi_*`, and `MapApi_*` instead of raw legacy globals.
- Raw `FlagSet`, `FlagClear`, `FlagGet`, weather internals, object-event
  globals, sprite tables, hard-coded text tables, Pokeball catch switches, and
  map-header mutation belong only in the relevant adapter files when writing
  new module code.
- Keep build defaults conservative: feature code should compile out or no-op
  when disabled.
- `FEATURE_MULTIPLAYER=1` must not read emulator bridge memory unless
  `FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT=1` is also set.
- New module code must not directly depend on transport memory addresses.
- Emulator bridge reads must use the authoritative server view; local client
  output must go through the write-only local snapshot/packet lanes.
- Online session identity must include `sessionEpoch`, `playerToken`,
  `joinNonce`, protocol/build/ruleset metadata, heartbeat, and server-bridge
  transport mode.
- Client data is untrusted online. Treat movement, interactions, battle input,
  trade input, party/inventory/state values, local RTC, and local flags as
  requests that require server validation.
- Online gameplay side effects must use `MultiplayerCommit_*` and stable
  transaction keys. Duplicate, replayed, retried, rolled-back, or late packets
  must return the cached result instead of applying twice.
- Trade, item, party, story-flag, outfit, reward, and battle-result commits are
  fail-closed until a server mirror exists for the affected state.
- Reliable gameplay actions belong on the bridge ringbuffer; overworld
  snapshots stay latest-wins.
- Remote avatars must use idempotent virtual-object create/update APIs so one
  remote player cannot spawn duplicate sprites across retries or resyncs.
- Time-based online systems must use `MultiplayerClock_*` and server time;
  Emerald RTC is only an offline compatibility fallback.
- Multiplayer overworld snapshot and remote-avatar ticks must be gated to real
  overworld callbacks, not menus, battle callbacks, or map-load gaps.
- New module code should not spread direct reads/writes of `gBattleTypeFlags`,
  `gBattleWeather`, `gLinkPlayers`, or SaveBlock internals.
- UI may switch multiplayer only through `engine/runtime_state` and
  `MultiplayerSession_*`; it must not call `NetTransport_*` directly.
- The Solo/Online setting uses existing SaveBlock2 option padding and must keep
  `sizeof(struct SaveBlock2) == 0xF2C`.
- Overworld-specific access to `gObjectEvents` belongs in the overworld port.
- Remote overworld players must be non-colliding virtual objects unless a
  dedicated collision policy is added. Their virtual object IDs are reserved
  in the high range and guarded at compile time.
- Battle synchronization belongs behind `BattleSession` or `multiplayer/battle`.
- Save layout changes require a named migration plan and tests; reusing padding
  still requires an exact size assertion.
- Prefer small adapter APIs over modifying large legacy structs.
- Keep C compatible with the existing GBA toolchains and repo style.
- Use ASCII in new source/docs unless a file already requires another charset.

## Build And Checks

Useful commands:

```bash
make -j"$(nproc)"
make -j"$(nproc)" modern
make -j"$(nproc)" modern FEATURE_MULTIPLAYER=1
make -j"$(nproc)" modern FEATURE_MULTIPLAYER=1 FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT=1
make -j"$(nproc)" FEATURE_MULTIPLAYER=1 COMPARE=0
sh scripts/ci/check_docs.sh
sh scripts/ci/architecture_guard.sh
python3 scripts/ci/multiplayer_fuzz.py
python3 scripts/ci/multiplayer_host_sim.py
python3 scripts/ci/check_net_manifest.py
python3 scripts/ci/modgen_smoke.py
```

If local Windows does not have `make`, rely on GitHub Actions or install the
toolchain described in `INSTALL.md`.

## Discovery Order

1. `README.md` for project setup and current feature direction.
2. `docs/modular_multiplayer_architecture.md` for multiplayer boundaries.
3. `include/config/features.h` for feature gates.
4. `include/engine/` and `src/engine/` for module attachment points.
5. `include/mod/` and `src/mod/` for mod-facing APIs.
6. `include/multiplayer/` and `src/multiplayer/` for network/session APIs.
7. Legacy Emerald systems only after reading the relevant adapter.

## Do NOT

- Do not upload or release built ROM artifacts from CI/CD.
- Do not copy a multiplayer fork wholesale into this repo.
- Do not put emulator bridge addresses outside the transport adapter.
- Do not make emulator transport the default for `FEATURE_MULTIPLAYER=1`.
- Do not spawn remote players as normal map `ObjectEvent`s.
- Do not add ad hoc `ReadConnectedByte()`-style checks to battle controllers.
- Do not alter SaveBlocks, `struct Pokemon`, `struct ObjectEvent`, or
  `struct LinkPlayer` without a migration plan.
- Do not bypass the module registry for new feature systems.
- Do not add new mod-facing systems by editing central registries manually;
  extend `scripts/modgen.py` or a domain adapter instead.
- Do not treat the 8-player network model as if Emerald's original 4-player
  link model can hold it directly.
- Do not commit online rewards, trades, battle outcomes, story flags, or daily
  events from client-provided ROM/RAM/RTC state.
