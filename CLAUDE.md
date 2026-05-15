---
metadata:
  version: 0.1.0
---

# CLAUDE.md - pokeemerald-rebuilt

<!-- last_updated: 2026-05-15 -->

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
- Transport: emulator-first and isolated behind `NetTransport_*`.
- Runtime-safe feature toggles: `engine/runtime_state`.
- Engine rule sets: compile-time feature gates.

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

Attach new features through the registry and ports. Avoid spreading direct
legacy-global access through feature code.

## Conventions

- Feature gates live in `include/config/features.h`.
- Runtime-safe module state starts in `engine/runtime_state`.
- Multiplayer bridge memory is private to `src/multiplayer/transport_*`.
- `gObjectEvents` access is allowed only inside the overworld adapter layer.
- Battle controller synchronization belongs behind `multiplayer/battle`.
- Save layout changes require a migration plan before implementation.
- Do not upload built ROM artifacts from CI/CD.

## Discovery Order

1. `README.md`
2. `AGENTS.md`
3. `docs/modular_multiplayer_architecture.md`
4. `include/config/features.h`
5. `include/engine/` and `src/engine/`
6. `include/multiplayer/` and `src/multiplayer/`

## Do NOT

- Do not copy external multiplayer forks wholesale.
- Do not add direct emulator-address reads outside the transport adapter.
- Do not add ad hoc transport checks to legacy battle controllers.
- Do not change SaveBlocks or core legacy structs without a migration plan.
