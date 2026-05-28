"""Shop domain collector."""

from __future__ import annotations

from typing import Any, Dict, List, Tuple

from ..common import *
from ..errors import ModgenError
from .keys import key_for

def collect_shops(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    shops = []
    seen = set()
    for mod in mods:
        for path in iter_json_files(mod["root"] / "shops"):
            for index, item in enumerate(read_domain_list(path, "shops")):
                key = key_for(mod["id"], item, f"shop_{index}")
                if key in seen:
                    raise ModgenError(f"Duplicate shop key {key!r}")
                seen.add(key)

                group, num = parse_map_group_num(item, key)
                mart_type = normalize_shop_type(item.get("type", item.get("martType", item.get("mart_type", "NORMAL"))))
                raw_items = item.get("items", [])
                if not isinstance(raw_items, list):
                    raise ModgenError(f"{key}: shop items must be a list")
                if len(raw_items) == 0 or len(raw_items) > 32:
                    raise ModgenError(f"{key}: shops must contain 1..32 items")
                items = [c_int_or_token(raw_item, "ITEM_NONE") for raw_item in raw_items]
                while len(items) < 32:
                    items.append("ITEM_NONE")
                priority = int(item.get("priority", 1000))
                if priority < -32768 or priority > 32767:
                    raise ModgenError(f"{key}: priority must be in signed 16-bit range")
                shops.append(
                    {
                        "key": key,
                        "group": group,
                        "num": num,
                        "mart_type": mart_type,
                        "item_count": len(raw_items),
                        "priority": priority,
                        "flags": c_int_or_token(item.get("flags"), "0"),
                        "items": items,
                    }
                )
    shops.sort(key=lambda shop: (shop["priority"], shop["key"]))
    return shops

