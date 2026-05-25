## Summary

<!-- Describe what changed and why. Be explicit about implemented behavior vs. future architecture. -->

## Scope

<!-- List the touched areas: docs, build, modgen, engine, multiplayer, maps, assets, etc. -->

## Checks

<!-- Mark every check you ran. Leave unchecked and explain below when a check is not applicable or could not run. -->

- [ ] `sh scripts/ci/check_docs.sh`
- [ ] `sh scripts/ci/architecture_guard.sh`
- [ ] `python3 scripts/ci/multiplayer_fuzz.py`
- [ ] `python3 scripts/ci/multiplayer_host_sim.py`
- [ ] `python3 scripts/ci/check_net_manifest.py`
- [ ] `python3 scripts/ci/modgen_smoke.py`
- [ ] `make generated FEATURE_MODS=1` or `python3 scripts/modgen.py --root .`
- [ ] Relevant `make` build lane:

## Feature Gates

<!-- Required for feature work. -->

- [ ] New feature code is behind the appropriate `include/config/features.h` gate or no-ops safely when disabled.
- [ ] `FEATURE_MULTIPLAYER=1` does not read emulator bridge memory unless `FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT=1` is enabled.
- [ ] UI/session changes use `engine/runtime_state` and `MultiplayerSession_*`, not direct transport calls.

## Save / State Impact

<!-- Required for SaveBlock or core struct changes. -->

- [ ] No SaveBlock, `struct Pokemon`, `struct ObjectEvent`, or `struct LinkPlayer` layout changes.
- [ ] If layout changed, this PR includes a named migration plan, compatibility notes, tests/assertions, and exact size checks.

## Multiplayer Authority

<!-- Required for online-facing changes. -->

- [ ] Online side effects go through `MultiplayerCommit_*` or fail closed.
- [ ] Client-provided movement, interaction, battle, trade, inventory, party, flag, reward, time, and weather data is not treated as authoritative.
- [ ] `docs/multiplayer_bridge.md` and `docs/multiplayer_net_manifest.json` were updated if the protocol/bridge contract changed.

## Modding Impact

<!-- Required for modgen or mod-facing API changes. -->

- [ ] Generated registries are not committed.
- [ ] `mods/README.md` was updated for manifest fields, hook signatures, or conflict-rule changes.

## Artifacts

- [ ] This PR does not include `.gba`, `.elf`, `.map`, `.sym`, save-state, save-file, or generated build artifacts.

## Notes

<!-- Known failures, checks not run, CI status, follow-up work. -->
