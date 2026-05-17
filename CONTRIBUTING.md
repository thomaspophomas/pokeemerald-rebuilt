# Contributing

<!-- last_updated: 2026-05-17 -->

This fork is a mod-first Pokemon Emerald decomp project. Keep changes scoped,
source-only, and honest about the current foundation state: online multiplayer
is not playable until a real bridge/server and green build lanes exist.

## Required Checks

Run the checks that match your change before opening a PR:

```bash
sh scripts/ci/check_docs.sh
sh scripts/ci/architecture_guard.sh
python3 scripts/ci/multiplayer_fuzz.py
python3 scripts/ci/multiplayer_host_sim.py
python3 scripts/ci/check_net_manifest.py
python3 scripts/ci/modgen_smoke.py
python3 scripts/modgen.py --root .
```

For build-facing changes, also run the relevant build lane:

```bash
make -j"$(nproc)"
make -j"$(nproc)" modern
make -j"$(nproc)" modern FEATURE_MULTIPLAYER=1
make -j"$(nproc)" modern FEATURE_MULTIPLAYER=1 FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT=1
make -j"$(nproc)" FEATURE_MULTIPLAYER=1 FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT=1 COMPARE=0
```

If you cannot run a lane locally, state that in the PR and point to the closest
CI coverage.

## Feature Gates

- Compile-time gates live in `include/config/features.h`.
- Keep default builds conservative. New feature code should compile out or
  no-op when its gate is disabled.
- `FEATURE_MULTIPLAYER=1` must not read emulator bridge memory unless
  `FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT=1` is also enabled.
- UI code may switch Solo/Online only through `engine/runtime_state` and
  `MultiplayerSession_*`.
- Bridge memory remains private to `src/multiplayer/transport_*`.

## Modding Changes

- Put mod-facing content under `mods/<modId>/...`.
- Extend `scripts/modgen.py` and a domain adapter when adding a new generated
  domain; do not require hand-edited central registries.
- Generated outputs under `include/generated`, `src/generated`, and
  `build/generated` are build outputs and should not be committed.
- Keep `mods/README.md` current when manifest fields, hook signatures, or
  conflict rules change.

## Save And State

Do not change SaveBlock layouts, `struct Pokemon`, `struct ObjectEvent`, or
`struct LinkPlayer` casually. Any save-affecting change must include:

- a named migration plan
- compatibility notes for existing saves
- tests or assertions that cover the new layout
- an exact size assertion when reusing padding

The current Solo/Online setting uses existing SaveBlock2 option padding and must
preserve `sizeof(struct SaveBlock2) == 0xF2C`.

## Multiplayer Authority

Client data is untrusted online. Movement, interactions, battle actions, trade
actions, items, party data, flags, money, rewards, time, and weather must be
server-validated before they mutate authoritative online state.

Use `MultiplayerCommit_*` with stable transaction keys for online side effects.
Fail closed when a server mirror does not own the affected state yet.

Update `docs/multiplayer_bridge.md` and
`docs/multiplayer_net_manifest.json` together when protocol, bridge, build,
ruleset, queue, or commit-log contracts change.

## No ROM Artifacts

Do not commit, upload, or attach built ROM artifacts or derived binaries:

- `.gba`
- `.elf`
- `.map`
- `.sym`
- save states or save files
- generated build directories

Releases must remain source-only unless project policy changes explicitly.

## Documentation

Documentation should distinguish implemented behavior from target architecture.
If CI is red on `master`, say so in status-oriented docs rather than implying
the tree is ready to build and play.
