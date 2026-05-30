#!/usr/bin/env python3
"""Frame-level simulator for the Fishing Memory minigame."""

from __future__ import annotations

import argparse
import random
from dataclasses import dataclass


UP = "U"
DOWN = "D"


@dataclass(frozen=True)
class SimulationResult:
    score: int
    outcome: str
    timed_out: bool
    encounter_level: int


def generate_sequence(seed: int, length: int) -> str:
    rng = random.Random(seed)
    return "".join(UP if rng.randrange(2) == 0 else DOWN for _ in range(length))


def simulate(sequence: str, inputs: str, input_frames: int, max_level: int = 100, safe_min_level: int = 1) -> SimulationResult:
    score = 0
    input_index = 0
    timer = 0
    if not sequence:
        return SimulationResult(score=0, outcome="GOT_AWAY", timed_out=False, encounter_level=0)

    for value in inputs:
        if value == ".":
            timer += 1
            if timer >= input_frames:
                return SimulationResult(score=score, outcome="ON_HOOK" if score else "GOT_AWAY", timed_out=True, encounter_level=max(score, safe_min_level) if score else 0)
            continue
        if value not in (UP, DOWN):
            raise ValueError(f"unknown input {value!r}")
        expected = sequence[input_index]
        if value != expected:
            return SimulationResult(score=score, outcome="ON_HOOK" if score else "GOT_AWAY", timed_out=False, encounter_level=max(score, safe_min_level) if score else 0)
        score += 1
        if score >= max_level:
            return SimulationResult(score=score, outcome="ON_HOOK", timed_out=False, encounter_level=max(score, safe_min_level))
        input_index += 1
        timer = 0
        if input_index >= len(sequence):
            input_index = 0
    return SimulationResult(score=score, outcome="CONTINUE", timed_out=False, encounter_level=0)


def self_test() -> None:
    cases = [
        ("UD", "UD", 90, 1, SimulationResult(2, "CONTINUE", False, 0)),
        ("UD", "UU", 90, 1, SimulationResult(1, "ON_HOOK", False, 1)),
        ("UD", "UU", 90, 5, SimulationResult(1, "ON_HOOK", False, 5)),
        ("UD", "D", 90, 1, SimulationResult(0, "GOT_AWAY", False, 0)),
        ("UD", "." * 90, 90, 1, SimulationResult(0, "GOT_AWAY", True, 0)),
        ("UD", "U" + "." * 90, 90, 1, SimulationResult(1, "ON_HOOK", True, 1)),
    ]
    for sequence, inputs, input_frames, safe_min_level, expected in cases:
        actual = simulate(sequence, inputs, input_frames, safe_min_level=safe_min_level)
        if actual != expected:
            raise AssertionError(f"{sequence=} {inputs=!r}: {actual} != {expected}")
    if generate_sequence(seed=1, length=4) != generate_sequence(seed=1, length=4):
        raise AssertionError("seeded sequence generation must be stable")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--sequence", default="UDU")
    parser.add_argument("--seed", type=int)
    parser.add_argument("--rounds", type=int, default=8)
    parser.add_argument("--inputs", default="UDU")
    parser.add_argument("--input-frames", type=int, default=90)
    parser.add_argument("--max-level", type=int, default=100)
    parser.add_argument("--safe-min-level", type=int, default=1)
    parser.add_argument("--self-test", action="store_true")
    args = parser.parse_args()

    if args.self_test:
        self_test()
        print("fishing_memory simulator tests OK")
        return 0

    sequence = generate_sequence(args.seed, args.rounds) if args.seed is not None else args.sequence
    result = simulate(sequence, args.inputs, args.input_frames, args.max_level, args.safe_min_level)
    print(f"sequence={sequence}")
    print(f"score={result.score} outcome={result.outcome} timed_out={str(result.timed_out).lower()} encounterLevel={result.encounter_level}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
