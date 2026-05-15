# Modular Multiplayer Architecture

This project is moving toward a mod-first architecture. The goal is not to
preserve vanilla compare behavior at all costs, but to keep new systems
attachable and removable without hardwiring them through the existing global
engine state.

## Feature Gates

- Compile-time features live in `include/config/features.h`.
- Make can override important gates, for example:
  - `make FEATURE_MULTIPLAYER=1`
  - `make FEATURE_MULTIPLAYER=1 FEATURE_MULTIPLAYER_AUTOCONNECT=1`
- Engine rules that can destabilize a running save, such as generation rule
  sets, should remain compile-time.
- Save-friendly systems, such as weather layers, following Pokemon, story
  progress, and multiplayer modes, should expose runtime state through a
  versioned module state API. The initial in-memory API is
  `engine/runtime_state`; SaveBlock persistence should be added only after a
  dedicated save-layout migration pass.

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

The first transport target is emulator-first. The ROM talks to a small
`NetTransport_*` interface. The emulator bridge owns player-slot assignment,
host authority, session IDs, and packet fan-out.

The emulator bridge memory layout is intentionally private to
`src/multiplayer/transport_emulator.c`. Battle and overworld code must not read
or write bridge addresses directly.

## Current Foundation

This pass adds:

- Build-time feature gates
- Module registry hooks
- Versioned runtime-state API for future save-backed toggles
- A versioned packet envelope
- An 8-player session model
- Emulator transport adapter skeleton
- Overworld snapshot publication
- Remote avatar pool for up to 7 visible remote players, limited by available
  object-event slots on dense maps
- Battle and trade subsession request APIs

The next implementation pass should replace the bridge skeleton with an actual
emulator-side bridge, then move battle-controller synchronization behind
`BattleSession` instead of adding ad hoc checks to battle controllers.
