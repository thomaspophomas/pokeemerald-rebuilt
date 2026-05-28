"""Item domain collector."""

from __future__ import annotations

from typing import Any, Dict, List, Tuple

from ..common import *
from ..errors import ModgenError
from .keys import key_for

def collect_items(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    items = []
    seen = set()
    for mod in mods:
        for path in iter_json_files(mod["root"] / "items"):
            for index, item in enumerate(read_domain_list(path, "items")):
                key = key_for(mod["id"], item, f"item_{index}")
                if key in seen:
                    raise ModgenError(f"Duplicate item key {key!r}")
                seen.add(key)

                inferred = 0
                name_key = item.get("nameKey", item.get("name_key"))
                if name_key is not None:
                    name_key = str(name_key)
                    if ":" not in name_key:
                        name_key = f"{mod['id']}:{name_key}"
                    inferred |= ITEM_OVERRIDE_FLAGS["NAME"]
                desc_key = item.get("descriptionKey", item.get("description_key"))
                if desc_key is not None:
                    desc_key = str(desc_key)
                    if ":" not in desc_key:
                        desc_key = f"{mod['id']}:{desc_key}"
                    inferred |= ITEM_OVERRIDE_FLAGS["DESCRIPTION"]
                if "price" in item:
                    inferred |= ITEM_OVERRIDE_FLAGS["PRICE"]
                if "holdEffect" in item or "hold_effect" in item:
                    inferred |= ITEM_OVERRIDE_FLAGS["HOLD_EFFECT"]
                if "holdEffectParam" in item or "hold_effect_param" in item:
                    inferred |= ITEM_OVERRIDE_FLAGS["HOLD_EFFECT_PARAM"]
                if "importance" in item:
                    inferred |= ITEM_OVERRIDE_FLAGS["IMPORTANCE"]
                if "pocket" in item:
                    inferred |= ITEM_OVERRIDE_FLAGS["POCKET"]
                if "type" in item:
                    inferred |= ITEM_OVERRIDE_FLAGS["TYPE"]
                if "battleUsage" in item or "battle_usage" in item:
                    inferred |= ITEM_OVERRIDE_FLAGS["BATTLE_USAGE"]
                if "secondaryId" in item or "secondary_id" in item:
                    inferred |= ITEM_OVERRIDE_FLAGS["SECONDARY_ID"]

                field_hook = c_func(item.get("fieldUseHook", item.get("field_use_hook")))
                field_hook_key = item.get("fieldUseHookKey", item.get("field_use_hook_key", field_hook if field_hook != "NULL" else None))
                if field_hook_key is not None:
                    field_hook_key = str(field_hook_key)
                    if field_hook_key == "" or len(field_hook_key) > 31:
                        raise ModgenError(f"{key}: fieldUseHookKey must be non-empty and <= 31 bytes")
                    inferred |= ITEM_OVERRIDE_FLAGS["FIELD_USE"]
                battle_hook = c_func(item.get("battleUseHook", item.get("battle_use_hook")))
                battle_hook_key = item.get("battleUseHookKey", item.get("battle_use_hook_key", battle_hook if battle_hook != "NULL" else None))
                if battle_hook_key is not None:
                    battle_hook_key = str(battle_hook_key)
                    if battle_hook_key == "" or len(battle_hook_key) > 31:
                        raise ModgenError(f"{key}: battleUseHookKey must be non-empty and <= 31 bytes")
                    inferred |= ITEM_OVERRIDE_FLAGS["BATTLE_USE"]

                flags = parse_u16_flags(item.get("flags"), ITEM_OVERRIDE_FLAGS, "item", inferred)
                item_id = c_int_or_token(item.get("itemId", item.get("item_id")), "ITEM_NONE")
                priority = int(item.get("priority", 1000))
                if priority < -32768 or priority > 32767:
                    raise ModgenError(f"{key}: priority must be in signed 16-bit range")
                items.append(
                    {
                        "key": key,
                        "item": item_id,
                        "name_key": name_key,
                        "description_key": desc_key,
                        "field_hook": field_hook,
                        "field_hook_key": field_hook_key,
                        "battle_hook": battle_hook,
                        "battle_hook_key": battle_hook_key,
                        "price": c_int_or_token(item.get("price"), "0"),
                        "flags": flags,
                        "hold_effect": c_int_or_token(item.get("holdEffect", item.get("hold_effect")), "0"),
                        "hold_effect_param": c_int_or_token(item.get("holdEffectParam", item.get("hold_effect_param")), "0"),
                        "importance": c_int_or_token(item.get("importance"), "0"),
                        "pocket": c_int_or_token(item.get("pocket"), "0"),
                        "type": c_int_or_token(item.get("type"), "0"),
                        "battle_usage": c_int_or_token(item.get("battleUsage", item.get("battle_usage")), "0"),
                        "secondary_id": c_int_or_token(item.get("secondaryId", item.get("secondary_id")), "0"),
                        "priority": priority,
                    }
                )
    items.sort(key=lambda item: (item["priority"], item["key"]))
    return items

