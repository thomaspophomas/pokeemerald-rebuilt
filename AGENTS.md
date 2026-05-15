---
metadata:
  version: 0.1.0
---

# AGENTS.md - pokeemerald-rebuilt

<!-- last_updated: 2026-05-15 -->

Briefing for AI agents working in this repository. Read this first, then inspect
the relevant source files before changing code.

## TL;DR

This is a mod-first Pokemon Emerald decomp repo. The active architectural goal
is a multiplayer-first modular refactor:

- 8 online players in the overworld session.
- PvE battle subsessions with up to 2 human players.
- PvP battle subsessions with up to 4 human players as 2v2.
- Trade subsessions with 2 players.
- Emulator-first transport adapter, isolated behind `NetTransport_*`.
- Runtime-safe feature settings behind `engine/runtime_state`.
- Engine-generation rule sets remain compile-time.

## Architecture

```text
main.c / overworld.c / battle_main.c
    -> engine/module_registry
        -> engine/runtime_state
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
- Keep build defaults conservative: feature code should compile out or no-op
  when disabled.
- New module code must not directly depend on transport memory addresses.
- New module code should not spread direct reads/writes of `gBattleTypeFlags`,
  `gBattleWeather`, `gLinkPlayers`, or SaveBlock internals.
- Overworld-specific access to `gObjectEvents` belongs in the overworld port.
- Battle synchronization belongs behind `BattleSession` or `multiplayer/battle`.
- Save layout changes require a named migration plan and tests.
- Prefer small adapter APIs over modifying large legacy structs.
- Keep C compatible with the existing GBA toolchains and repo style.
- Use ASCII in new source/docs unless a file already requires another charset.

## Build And Checks

Useful commands:

```bash
make -j"$(nproc)"
make -j"$(nproc)" modern
make -j"$(nproc)" modern FEATURE_MULTIPLAYER=1
make -j"$(nproc)" FEATURE_MULTIPLAYER=1 COMPARE=0
sh scripts/ci/check_docs.sh
sh scripts/ci/architecture_guard.sh
```

If local Windows does not have `make`, rely on GitHub Actions or install the
toolchain described in `INSTALL.md`.

## Discovery Order

1. `README.md` for project setup and current feature direction.
2. `docs/modular_multiplayer_architecture.md` for multiplayer boundaries.
3. `include/config/features.h` for feature gates.
4. `include/engine/` and `src/engine/` for module attachment points.
5. `include/multiplayer/` and `src/multiplayer/` for network/session APIs.
6. Legacy Emerald systems only after reading the relevant adapter.

## Do NOT

- Do not upload or release built ROM artifacts from CI/CD.
- Do not copy a multiplayer fork wholesale into this repo.
- Do not put emulator bridge addresses outside the transport adapter.
- Do not add ad hoc `ReadConnectedByte()`-style checks to battle controllers.
- Do not alter SaveBlocks, `struct Pokemon`, `struct ObjectEvent`, or
  `struct LinkPlayer` without a migration plan.
- Do not bypass the module registry for new feature systems.
- Do not treat the 8-player network model as if Emerald's original 4-player
  link model can hold it directly.
