# Install And Build

<!-- last_updated: 2026-05-17 -->

This file documents the setup for `thomaspophomas/pokeemerald-rebuilt`. The
tree still uses the usual `pret/pokeemerald` build model, but this fork also
has generated mod registries, multiplayer feature flags, and a CI matrix that
should be checked before claiming a build is healthy.

The repository intentionally does not ship built ROM artifacts.

## Requirements

Install these tools first:

- `git`
- `python3`
- GNU `make`
- a C compiler for host tools, usually `gcc` or `clang`
- `libpng` development headers
- `arm-none-eabi` binutils and GCC for modern builds
- `pret/agbcc` installed into this checkout for non-modern compare builds

Common package commands:

```bash
# Debian/Ubuntu/WSL
sudo apt update
sudo apt install build-essential gcc-arm-none-eabi binutils-arm-none-eabi git libpng-dev python3

# Arch
sudo pacman -S base-devel arm-none-eabi-gcc arm-none-eabi-binutils git libpng python

# macOS with Homebrew plus devkitPro/devkitARM
brew install libpng python
```

On Windows, WSL with Ubuntu is the least surprising path. MSYS2/devkitPro can
also work, but make sure `git`, `make`, Python, libpng, and the GBA toolchain
are all visible in the same shell.

## Clone

Clone this fork, not upstream `pret/pokeemerald`:

```bash
git clone https://github.com/thomaspophomas/pokeemerald-rebuilt.git
cd pokeemerald-rebuilt
```

For non-modern or compare builds, install `agbcc` next to the repository and
run its installer against `pokeemerald-rebuilt`:

```bash
cd ..
git clone https://github.com/pret/agbcc.git
cd agbcc
./build.sh
./install.sh ../pokeemerald-rebuilt
cd ../pokeemerald-rebuilt
```

If you already have a built `agbcc`, rerun only:

```bash
cd ../agbcc
./install.sh ../pokeemerald-rebuilt
cd ../pokeemerald-rebuilt
```

## Generated Files

Mod registries are generated from `mods/<modId>/...`:

```bash
make generated
```

The direct generator command is useful while editing manifests:

```bash
python3 scripts/modgen.py --root .
```

Generated outputs live under `include/generated`, `src/generated`, and
`build/generated`. They are build outputs and should not be edited or committed.
Use this when you need a clean generated state:

```bash
make clean-generated
make generated
```

## Build Commands

Non-modern build:

```bash
make -j"$(nproc)"
```

Modern compiler build:

```bash
make -j"$(nproc)" modern
```

Compare against the expected vanilla ROM hash only when intentionally checking
byte identity with upstream vanilla Emerald:

```bash
make -j"$(nproc)" compare
```

Enable the multiplayer module layer:

```bash
make -j"$(nproc)" modern FEATURE_MULTIPLAYER=1
```

Enable the ROM-side emulator bridge adapter:

```bash
make -j"$(nproc)" modern FEATURE_MULTIPLAYER=1 FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT=1
```

Enable autoconnect only when you also provide a compatible bridge:

```bash
make -j"$(nproc)" modern FEATURE_MULTIPLAYER=1 FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT=1 FEATURE_MULTIPLAYER_AUTOCONNECT=1
```

Non-modern feature coverage, matching the CI feature lane:

```bash
make -j"$(nproc)" FEATURE_MULTIPLAYER=1 FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT=1 COMPARE=0
```

On macOS, replace `nproc` with `sysctl -n hw.ncpu`.

## Feature Flags

The main build-time flags are:

- `FEATURE_MODS=1`: enables generated mod registries.
- `FEATURE_MULTIPLAYER=1`: compiles the session/overworld/battle/trade
  multiplayer foundation and exposes the Solo/Online runtime mode.
- `FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT=1`: enables the ROM-side EWRAM
  mailbox transport adapter.
- `FEATURE_MULTIPLAYER_AUTOCONNECT=1`: starts Online mode automatically, only
  useful with a compatible emulator bridge.

Other gates live in `include/config/features.h`. Engine generation rules and
similar save-risky behavior should remain compile-time; runtime-safe choices
should go through `engine/runtime_state`.

## Local Checks

Run these before opening a PR that touches docs, architecture, mod manifests,
or multiplayer code:

```bash
sh scripts/ci/check_docs.sh
sh scripts/ci/architecture_guard.sh
python3 scripts/ci/multiplayer_fuzz.py
python3 scripts/ci/multiplayer_host_sim.py
python3 scripts/ci/check_net_manifest.py
python3 scripts/ci/modgen_smoke.py
python3 scripts/modgen.py --root .
```

For build-facing changes, also run the relevant build lanes from
[Build Commands](#build-commands).

## CI Matrix

GitHub Actions runs on pushes to `master`, pull requests, manual dispatch, and
`v*` tags. The workflow covers:

- docs and architecture checks
- multiplayer fuzz and host simulator checks
- multiplayer net-manifest drift checks
- mod generator smoke tests
- whitespace checks
- non-modern `COMPARE=0` build with `.sym` generation. This fork is no longer
  byte-identical to upstream vanilla Emerald, so the legacy `make compare`
  checksum is a targeted matching check rather than a required CI lane.
- modern builds with multiplayer off, multiplayer on, and emulator transport on
- non-modern feature build with multiplayer and emulator transport enabled
- source-only releases for tags

As of the 2026-05-17 audit, the checked `master` run for commit `8b3f7f703`
was red: docs/architecture passed, but the build lanes failed. This working
tree carries fixes for the known checkout and compile failures; treat local
builds as required verification until a newer pushed CI run replaces that
status.

## Troubleshooting

- If switching between WSL, MSYS2, and other shells, run `make clean-tools`
  before rebuilding.
- If `make generated` fails, run `python3 scripts/modgen.py --root .` directly
  to get a shorter manifest error.
- If a non-modern build cannot find `tools/agbcc`, reinstall `agbcc` into this
  checkout with `./install.sh ../pokeemerald-rebuilt`.
- Do not attach `.gba`, `.elf`, `.map`, `.sym`, save states, or other ROM/build
  artifacts to issues, PRs, releases, or CI output.
