"""Trainer domain collector."""

from __future__ import annotations

from typing import Any, Dict, List, Tuple

from ..common import *
from ..errors import ModgenError
from .keys import key_for

def collect_trainers(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    trainers = []
    seen = set()
    for mod in mods:
        for path in iter_json_files(mod["root"] / "trainers"):
            for index, item in enumerate(read_domain_list(path, "trainers")):
                key = key_for(mod["id"], item, f"trainer_{index}")
                if key in seen:
                    raise ModgenError(f"Duplicate trainer key {key!r}")
                seen.add(key)
                inferred = 0
                if "trainerClass" in item or "trainer_class" in item:
                    inferred |= TRAINER_OVERRIDE_FLAGS["CLASS"]
                if "trainerPic" in item or "trainer_pic" in item:
                    inferred |= TRAINER_OVERRIDE_FLAGS["PIC"]
                if "encounterMusicGender" in item or "encounter_music_gender" in item:
                    inferred |= TRAINER_OVERRIDE_FLAGS["MUSIC_GENDER"]
                if "items" in item:
                    inferred |= TRAINER_OVERRIDE_FLAGS["ITEMS"]
                if "doubleBattle" in item or "double_battle" in item:
                    inferred |= TRAINER_OVERRIDE_FLAGS["DOUBLE"]
                if "aiFlags" in item or "ai_flags" in item:
                    inferred |= TRAINER_OVERRIDE_FLAGS["AI"]
                party = item.get("party", [])
                if party:
                    inferred |= TRAINER_OVERRIDE_FLAGS["PARTY"]
                flags = parse_u16_flags(item.get("flags"), TRAINER_OVERRIDE_FLAGS, "trainer", inferred)
                if len(party) > 6:
                    raise ModgenError(f"{key}: trainer party may contain at most 6 mons")
                mons = []
                for mon in party:
                    moves = [c_int_or_token(move, "MOVE_NONE") for move in mon.get("moves", [])]
                    if len(moves) > 4:
                        raise ModgenError(f"{key}: trainer mon moves may contain at most 4 moves")
                    while len(moves) < 4:
                        moves.append("MOVE_NONE")
                    level = int(mon.get("level", mon.get("lvl", 1)))
                    if level < 1 or level > 100:
                        raise ModgenError(f"{key}: trainer mon level must be in [1, 100]")
                    mons.append(
                        {
                            "iv": c_int_or_token(mon.get("iv"), "0"),
                            "level": level,
                            "species": c_int_or_token(mon.get("species"), "SPECIES_NONE"),
                            "item": c_int_or_token(mon.get("heldItem", mon.get("held_item")), "ITEM_NONE"),
                            "moves": moves,
                        }
                    )
                while len(mons) < 6:
                    mons.append({"iv": "0", "level": 0, "species": "SPECIES_NONE", "item": "ITEM_NONE", "moves": ["MOVE_NONE"] * 4})
                items = [c_int_or_token(value, "ITEM_NONE") for value in item.get("items", [])]
                if len(items) > 4:
                    raise ModgenError(f"{key}: trainer items may contain at most 4 items")
                while len(items) < 4:
                    items.append("ITEM_NONE")
                trainers.append(
                    {
                        "key": key,
                        "trainer": c_int_or_token(item.get("trainerId", item.get("trainer_id")), "0"),
                        "priority": int(item.get("priority", 1000)),
                        "flags": flags,
                        "class": c_int_or_token(item.get("trainerClass", item.get("trainer_class")), "0"),
                        "music_gender": c_int_or_token(item.get("encounterMusicGender", item.get("encounter_music_gender")), "0"),
                        "pic": c_int_or_token(item.get("trainerPic", item.get("trainer_pic")), "0"),
                        "items": items,
                        "double": c_bool(item.get("doubleBattle", item.get("double_battle", False))),
                        "ai": c_int_or_token(item.get("aiFlags", item.get("ai_flags")), "0"),
                        "party_count": len(party),
                        "party": mons,
                    }
                )
    trainers.sort(key=lambda trainer: (trainer["priority"], trainer["key"]))
    return trainers

