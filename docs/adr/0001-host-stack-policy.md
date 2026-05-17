# ADR 0001: Host Stack Policy

<!-- last_updated: 2026-05-17 -->

## Status

Accepted.

## Context

This repository is primarily a Pokemon Emerald ROM decompilation. Its supported
local stack is C/Make for ROM builds, Python for CI simulations and invariant
checks, and shell for repository guards.

Adding a production host stack inside this repository changes the project
boundary. It requires a documented toolchain, lockfile policy, CI coverage,
ownership, deployment model, and security review before the code can be treated
as part of the maintained build.

## Decision

Do not add a new host programming-language stack to this ROM repository without
an explicit ADR.

For now, host-side multiplayer behavior in this repository stays limited to
Python simulations under `scripts/ci/`. A production emulator bridge or
authoritative server should either live in a separate host repository or be
introduced here only after a follow-up ADR covers:

- toolchain installation and version pinning
- lockfile and dependency-update policy
- CI jobs and required local checks
- module ownership and maintenance expectations
- deployment, operations, and observability
- account, authentication, persistence, and network security design

## Consequences

- The ROM protocol, EWRAM mailbox, manifest, and fail-closed commit contracts
  remain documented and checked in this repository.
- Python simulations may model host invariants, including room joins,
  subsession starts, stale packet rejection, and idempotent commits.
- Accounts, PostgreSQL, WebSockets, password hashing, and production matchmaking
  are open work, not current repository interfaces.

## Alternatives Considered

- Keep Python simulations only in this repository. This is the current choice
  because it matches the existing toolchain and CI.
- Move the production host to a separate repository. This keeps ROM and service
  concerns separate and can choose its own stack deliberately.
- Add a Rust host workspace to this repository. This remains possible only with
  a follow-up ADR plus lockfile, CI, documentation, ownership, deployment, and
  security coverage.
