# Fishing Memory

Replaces the normal post-bite fishing input with a Simon-style up/down memory
sequence. After a bite, the textbox shows the current arrow sequence for a
short moment, then waits for D-pad up/down input. Each correct button press
adds one level to the hooked Pokemon. A wrong input or timeout starts the
encounter at the earned level; zero correct inputs makes the Pokemon get away.

The action params are:

1. max earned level, clamped to the game max level
2. sequence display frames
3. per-input timeout frames
4. reserved
