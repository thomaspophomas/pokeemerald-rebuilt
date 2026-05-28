"""Battle move domain collector."""

from __future__ import annotations

from typing import Any, Dict, List, Tuple

from ..common import *
from ..errors import ModgenError
from .keys import key_for

def collect_battle_moves(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    moves = []
    seen = set()
    for mod in mods:
        for path in iter_json_files(mod["root"] / "battle" / "moves"):
            for index, item in enumerate(read_domain_list(path, "moves")):
                key = key_for(mod["id"], item, f"move_{index}")
                if key in seen:
                    raise ModgenError(f"Duplicate battle move key {key!r}")
                seen.add(key)
                inferred = 0
                for json_key, flag_key in [
                    ("effect", "EFFECT"),
                    ("power", "POWER"),
                    ("type", "TYPE"),
                    ("accuracy", "ACCURACY"),
                    ("pp", "PP"),
                    ("secondaryEffectChance", "SECONDARY"),
                    ("target", "TARGET"),
                    ("priority", "PRIORITY"),
                    ("moveFlags", "FLAGS"),
                ]:
                    if json_key in item:
                        inferred |= BATTLE_MOVE_OVERRIDE_FLAGS[flag_key]
                override_flags = parse_u16_flags(item.get("overrideFlags", item.get("override_flags")), BATTLE_MOVE_OVERRIDE_FLAGS, "battle move", inferred)
                if override_flags == 0:
                    raise ModgenError(f"{key}: battle move overrideFlags must be non-zero")
                moves.append(
                    {
                        "key": key,
                        "move": c_int_or_token(item.get("move"), "MOVE_NONE"),
                        "priority": int(item.get("priorityOrder", item.get("priority_order", 1000))),
                        "override_flags": override_flags,
                        "effect": c_int_or_token(item.get("effect"), "0"),
                        "power": c_int_or_token(item.get("power"), "0"),
                        "type": c_int_or_token(item.get("type"), "0"),
                        "accuracy": c_int_or_token(item.get("accuracy"), "0"),
                        "pp": c_int_or_token(item.get("pp"), "0"),
                        "secondary": c_int_or_token(item.get("secondaryEffectChance", item.get("secondary_effect_chance")), "0"),
                        "target": c_int_or_token(item.get("target"), "0"),
                        "move_priority": c_int_or_token(item.get("movePriority", item.get("move_priority")), "0"),
                        "move_flags": c_int_or_token(item.get("moveFlags", item.get("move_flags")), "0"),
                    }
                )
    moves.sort(key=lambda move: (move["priority"], move["key"]))
    return moves

