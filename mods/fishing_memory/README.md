# Fishing Memory

Replaces the normal post-bite fishing input with a Simon-style up/down memory
sequence. After a bite, the textbox shows the current arrow sequence for a
short moment, then waits for D-pad up/down input. Each correct button press
adds one level to the hooked Pokemon. A wrong input or timeout starts the
encounter at the earned level; zero correct inputs makes the Pokemon get away.
The earned level is capped by badge count: 0 badges allow level 10, each badge
adds 10 levels, and all 8 badges allow level 100.

The action params are:

1. max earned level before the badge cap, clamped to the game max level
2. sequence display frames
3. per-input timeout frames
4. reserved
