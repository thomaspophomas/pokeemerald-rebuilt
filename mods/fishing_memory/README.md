# Fishing Memory

Replaces the normal post-bite fishing input with a Simon-style up/down memory
sequence. After a bite, the textbox shows the current arrow sequence for a
short moment, then waits for D-pad up/down input. Each correct button press
adds one level to the hooked Pokemon. A wrong input or timeout starts the
encounter at the earned level; zero correct inputs makes the Pokemon get away.
The earned level is capped by configured story flags in
`tools/config/default.json`. The bundled default uses badge flags: no badge
allows level 10, badge 1 allows level 20, and badge 8 allows level 100.

`tools/generate_fishing_memory.py` turns the config into the live
`fishing/actions.json` rows and `src/generated_cap_stages.inc` C table. The
default config has rod-specific timing: Old Rod is forgiving, Good Rod is
normal, and Super Rod is faster. `safeMinLevel` can keep non-zero catches from
starting too low, and `targetLevelDifficulty` can tighten timing for higher
target encounter levels.

The generated action params are:

1. max earned level before the badge cap, clamped to the game max level
2. sequence display frames
3. per-input timeout frames
4. warning frames before timeout; the prompt changes from `?` to `? ?`

`fishing/actions.json` must stay in sync with `tools/config/default.json`; the
local validator checks that the action params match the JSON tuning and that
every configured cap flag is represented in the source table.

```bash
cd /home/thomas/pokeemerald-mods
python3 mods/fishing_memory/tools/validate_fishing_memory.py
python3 mods/fishing_memory/tools/simulate_fishing_memory.py --self-test
python3 mods/fishing_memory/tools/simulate_fishing_memory.py --seed 7 --rounds 8 --inputs UDU
```

Presets in `tools/presets/` can be copied over `tools/config/default.json` and
then regenerated with:

```bash
python3 mods/fishing_memory/tools/generate_fishing_memory.py
```
