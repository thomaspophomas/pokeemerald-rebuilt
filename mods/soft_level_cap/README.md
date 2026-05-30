# Soft Level Cap

Adds a configurable flag-based soft level cap. The active drop-in config lives
in `level_caps/default.json`; additional ready-to-copy presets live in
`tools/presets/` so rebuilt does not compile them accidentally.

`capStages` defines the cap unlocked by each story flag:

```json
[
  { "level": 15 },
  { "flag": "FLAG_BADGE01_GET", "level": 19 },
  { "flag": "FLAG_BADGE02_GET", "level": 24 }
]
```

The first stage has no flag and is always active. Later stages name vanilla
event flags, so you can change or add flags without changing ROM code. The
highest unlocked stage is the active cap.

`softExpCurve` defines the EXP percentage for every possible level delta:

```json
[
  { "minDelta": -100, "maxDelta": -4, "percent": 100 },
  { "delta": -3, "percent": 90 },
  { "delta": -2, "percent": 60 },
  { "delta": -1, "percent": 30 },
  { "delta": 0, "percent": 15 },
  { "delta": 1, "percent": 10 },
  { "delta": 2, "percent": 5 },
  { "minDelta": 3, "maxDelta": 99, "percent": 0 }
]
```

The delta is `pokemonLevel - activeCap`: negative values are below the cap,
positive values are above it. The supported range is `-100..99`, and a custom
curve must cover the full range. This default means:

- more than 3 levels below: 100%
- 3 levels below: 90%
- 2 levels below: 60%
- 1 level below: 30%
- at cap: 15%
- above cap: 5% less per level until 0%

`rareCandy` controls item bypass behavior:

- `BLOCK_AT_CAP`: Rare Candies stop working while the Pokemon is at or above
  the active cap.
- `ALLOW`: Rare Candies keep vanilla behavior.

`capType` can be `SOFT`, `HARD`, or `NONE`. `HARD` gives no battle EXP at or
above cap. `NONE` disables the rule without removing the mod folder.

## Presets

- `tools/presets/emerald_soft.json`: same behavior as the default.
- `tools/presets/nuzlocke_hard.json`: lower hard caps and Rare Candy blocking.
- `tools/presets/no_cap.json`: keeps the mod installed but disables caps.

Copy a preset over `level_caps/default.json` when you want it to become the
compiled rule for a ROM.

## Validate

```bash
cd /home/thomas/pokeemerald-mods
python3 mods/soft_level_cap/tools/validate_soft_level_cap.py
```

The validator checks live JSON under `level_caps/` plus preset JSON under
`tools/presets/` for duplicate IDs, exactly one base stage, duplicate stage
flags, nondecreasing caps, and full `-100..99` soft EXP curve coverage where a
curve is provided. It also checks vanilla flag names against rebuilt when
`/home/thomas/pokeemerald-rebuilt/include/constants/flags.h` is available.

Optional helper behavior lives in `tools/config/features.json`. The potentially
immersion-breaking parts, such as in-game hints or separate EXP rules by battle
source, are present but `enabled: false` by default.

The optional `graceRange` block is also disabled by default. When enabled, it
can keep a tiny EXP percentage for a configurable number of levels above the
cap, useful for challenge variants where a hard zero feels too abrupt.

Preview the active rule without compiling a ROM:

```bash
python3 mods/soft_level_cap/tools/preview_soft_level_cap.py \
  --pokemon-level 20 --flag FLAG_BADGE01_GET
```

Print the full level 1..100 preview table:

```bash
python3 mods/soft_level_cap/tools/preview_soft_level_cap.py --table
```

Apply a preset when you want to replace the live default config:

```bash
python3 mods/soft_level_cap/tools/apply_preset.py emerald_soft
```

Compare two presets before switching:

```bash
python3 mods/soft_level_cap/tools/diff_presets.py emerald_soft nuzlocke_hard
```
