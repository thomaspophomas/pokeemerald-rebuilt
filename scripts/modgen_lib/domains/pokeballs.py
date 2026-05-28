"""Pokeball domain collector."""

from __future__ import annotations

from typing import Any, Dict, List, Tuple

from ..common import *
from ..errors import ModgenError
from .keys import key_for

def collect_pokeballs(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    balls = []
    seen_items = set()
    seen_keys = set()
    for mod in mods:
        for path in iter_json_files(mod["root"] / "pokeballs"):
            for item in read_domain_list(path, "pokeballs"):
                key = key_for(mod["id"], item, path.stem)
                if key in seen_keys:
                    raise ModgenError(f"Duplicate pokeball key {key!r}")
                seen_keys.add(key)
                item_id = c_int_or_token(item.get("itemId", item.get("item_id")), "ITEM_POKE_BALL")
                if item_id in seen_items:
                    raise ModgenError(f"Duplicate pokeball item id {item_id!r}")
                seen_items.add(item_id)
                balls.append(
                    {
                        "key": key,
                        "item": item_id,
                        "ball": c_int_or_token(item.get("ballId", item.get("ball_id")), "BALL_POKE"),
                        "modifier": c_int_or_token(item.get("catchModifier", item.get("catch_modifier")), "10"),
                        "flags": c_int_or_token(item.get("flags"), "0"),
                        "modifier_hook": c_func(item.get("catchModifierHook", item.get("catch_modifier_hook"))),
                        "battle_script": c_symbol(item.get("battleScript", item.get("battle_script"))),
                        "commit_hook": c_func(item.get("commitHook", item.get("commit_hook"))),
                    }
                )
    return balls

