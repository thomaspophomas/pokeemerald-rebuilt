"""Encounter domain collector."""

from __future__ import annotations

from typing import Any, Dict, List, Tuple

from ..common import *
from ..errors import ModgenError
from .keys import key_for

def collect_encounters(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    encounters = []
    seen = set()
    for mod in mods:
        for path in iter_json_files(mod["root"] / "encounters"):
            for index, item in enumerate(read_domain_list(path, "encounters")):
                key = key_for(mod["id"], item, f"encounter_{index}")
                if key in seen:
                    raise ModgenError(f"Duplicate encounter key {key!r}")
                seen.add(key)

                area = normalize_encounter_area(item.get("area", "LAND"))
                area_value = encounter_area_value(area)
                group, num = parse_map_group_num(item, key)
                hook = c_func(item.get("hook", item.get("hookSymbol", item.get("hook_symbol"))))
                hook_key = item.get("hookKey", item.get("hook_key", hook if hook != "NULL" else None))
                if hook_key is not None:
                    hook_key = str(hook_key)
                    if hook_key == "" or len(hook_key) > 31:
                        raise ModgenError(f"{key}: hookKey must be non-empty and <= 31 bytes")
                rod_mask = parse_fishing_rod_mask(item) if area_value == 3 else FISHING_ROD_MASK_ALL
                encounter_rate = int(item.get("encounterRate", item.get("encounter_rate", 0)))
                if encounter_rate < 0 or encounter_rate > 255:
                    raise ModgenError(f"{key}: encounterRate must be in [0, 255]")
                priority = int(item.get("priority", 1000))
                if priority < -32768 or priority > 32767:
                    raise ModgenError(f"{key}: priority must be in signed 16-bit range")
                raw_slots = as_list(item.get("slots", []), "slots")
                if len(raw_slots) > 12:
                    raise ModgenError(f"{key}: encounters may contain at most 12 slots")
                if len(raw_slots) == 0 and hook == "NULL":
                    raise ModgenError(f"{key}: encounter requires slots or a hook")
                slots = [parse_encounter_slot(slot, key, area_value) for slot in raw_slots]
                while len(slots) < 12:
                    slots.append({"min": 0, "max": 0, "species": "SPECIES_NONE", "weight": 0, "rod_mask": 0, "flags": "0"})

                encounters.append(
                    {
                        "key": key,
                        "hook": hook,
                        "hook_key": hook_key,
                        "group": group,
                        "num": num,
                        "area": area,
                        "area_value": area_value,
                        "rod_mask": rod_mask,
                        "encounter_rate": encounter_rate,
                        "slot_count": len(raw_slots),
                        "priority": priority,
                        "flags": c_int_or_token(item.get("flags"), "0"),
                        "slots": slots,
                    }
                )
    encounters.sort(key=lambda encounter: (encounter["priority"], encounter["key"]))
    return encounters

