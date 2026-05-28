"""Reward domain collector."""

from __future__ import annotations

from typing import Any, Dict, List, Tuple

from ..common import *
from ..errors import ModgenError
from .keys import key_for

def normalize_reward_source(value: Any) -> Tuple[str, int]:
    if isinstance(value, int):
        if value <= 0 or value > 3:
            raise ModgenError(f"Reward source {value!r} is invalid")
        return str(value), value
    if not isinstance(value, str):
        raise ModgenError(f"Reward source {value!r} is invalid")
    stripped = value.strip()
    if re.fullmatch(r"0x[0-9A-Fa-f]+|[0-9]+", stripped):
        raw = int(stripped, 0)
        if raw <= 0 or raw > 3:
            raise ModgenError(f"Reward source {value!r} is invalid")
        return str(raw), raw
    if stripped.startswith("MOD_REWARD_SOURCE_"):
        if stripped not in REWARD_SOURCE_NUMBERS:
            raise ModgenError(f"Reward source {value!r} is invalid")
        return stripped, REWARD_SOURCE_NUMBERS[stripped]
    upper = stripped.upper()
    if upper not in REWARD_SOURCE_VALUES:
        raise ModgenError(f"Unknown reward source {value!r}")
    token = REWARD_SOURCE_VALUES[upper]
    return token, REWARD_SOURCE_NUMBERS[token]


def collect_rewards(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    rewards = []
    seen = set()
    for mod in mods:
        for path in iter_json_files(mod["root"] / "rewards"):
            for index, item in enumerate(read_domain_list(path, "rewards")):
                key = key_for(mod["id"], item, f"reward_{index}")
                if key in seen:
                    raise ModgenError(f"Duplicate reward key {key!r}")
                seen.add(key)
                source, source_value = normalize_reward_source(item.get("source", "PICKUP_COMMON"))
                min_level = int(item.get("minLevel", item.get("min_level", 1)))
                max_level = int(item.get("maxLevel", item.get("max_level", 100)))
                if min_level < 1 or min_level > 100 or max_level < 1 or max_level > 100 or min_level > max_level:
                    raise ModgenError(f"{key}: reward level range must be within [1, 100]")
                hook = c_func(item.get("hook", item.get("hookSymbol", item.get("hook_symbol"))))
                hook_key = item.get("hookKey", item.get("hook_key", hook if hook != "NULL" else None))
                if hook_key is not None:
                    hook_key = str(hook_key)
                    if hook_key == "" or len(hook_key) > 31:
                        raise ModgenError(f"{key}: hookKey must be non-empty and <= 31 bytes")
                quantity = int(item.get("quantity", 1))
                if quantity < 1 or quantity > 999:
                    raise ModgenError(f"{key}: quantity must be in [1, 999]")
                priority = int(item.get("priority", 1000))
                if priority < -32768 or priority > 32767:
                    raise ModgenError(f"{key}: priority must be in signed 16-bit range")
                rewards.append(
                    {
                        "key": key,
                        "hook": hook,
                        "hook_key": hook_key,
                        "source": source,
                        "source_value": source_value,
                        "min_level": min_level,
                        "max_level": max_level,
                        "priority": priority,
                        "flags": c_int_or_token(item.get("flags"), "0"),
                        "item": c_int_or_token(item.get("itemId", item.get("item_id")), "ITEM_NONE"),
                        "quantity": quantity,
                    }
                )
    rewards.sort(key=lambda reward: (reward["priority"], reward["key"]))
    return rewards

