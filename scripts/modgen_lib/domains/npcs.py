"""NPC domain collector."""

from __future__ import annotations

from typing import Any, Dict, List, Tuple

from ..common import *
from ..errors import ModgenError
from .keys import key_for

def collect_npcs(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    npcs = []
    seen = set()
    next_id = 0
    for mod in mods:
        for path in iter_json_files(mod["root"] / "npcs"):
            for index, item in enumerate(read_domain_list(path, "npcs")):
                key = key_for(mod["id"], item, f"npc_{index}")
                if key in seen:
                    raise ModgenError(f"Duplicate NPC key {key!r}")
                seen.add(key)
                npcs.append(
                    {
                        "key": key,
                        "id": next_id,
                        "graphics": c_int_or_token(item.get("graphicsId", item.get("graphics_id")), "OBJ_EVENT_GFX_BOY_1"),
                        "movement": c_int_or_token(item.get("movementType", item.get("movement_type")), "MOVEMENT_TYPE_FACE_DOWN"),
                        "local": c_int_or_token(item.get("localId", item.get("local_id")), "0"),
                        "elevation": c_int_or_token(item.get("elevation"), "3"),
                        "flag": c_int_or_token(item.get("flagId", item.get("flag")), "0"),
                        "script": c_func(item.get("scriptSymbol", item.get("script"))),
                    }
                )
                next_id += 1
    return npcs

