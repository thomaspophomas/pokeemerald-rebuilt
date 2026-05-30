"""Level cap domain collector."""

from __future__ import annotations

from typing import Any, Dict, List

from ..common import *
from ..errors import ModgenError
from .keys import key_for


def validate_level_cap(key: str, mode: str, soft_percent: int, rare_candy_policy: str, caps: List[int]) -> None:
    mode_value = level_cap_mode_value(mode)
    rare_candy_value = rare_candy_policy_value(rare_candy_policy)

    if mode_value < 0 or mode_value > 2:
        raise ModgenError(f"{key}: mode must be NONE, SOFT, or HARD")
    if soft_percent < 0 or soft_percent > 100:
        raise ModgenError(f"{key}: softExpPercent must be in [0, 100]")
    if rare_candy_value < 0 or rare_candy_value > 1:
        raise ModgenError(f"{key}: rareCandy must be ALLOW or BLOCK_AT_CAP")
    if len(caps) != 9:
        raise ModgenError(f"{key}: capsByBadge must contain 9 levels")


def collect_level_caps(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    caps = []
    seen = set()
    for mod in mods:
        for path in iter_json_files(mod["root"] / "level_caps"):
            for index, item in enumerate(read_domain_list(path, "caps")):
                key = key_for(mod["id"], item, f"cap_{index}")
                if key in seen:
                    raise ModgenError(f"Duplicate level cap key {key!r}")
                seen.add(key)

                mode = normalize_level_cap_mode(item.get("mode", item.get("capType", item.get("cap_type", "SOFT"))))
                soft_percent = int(item.get("softExpPercent", item.get("soft_exp_percent", 25)))
                rare_candy_policy = normalize_rare_candy_policy(item.get("rareCandy", item.get("rare_candy", "ALLOW")))
                cap_table = parse_level_cap_table(item.get("capsByBadge", item.get("caps_by_badge")), key)
                priority = int(item.get("priority", 1000))
                if priority < -32768 or priority > 32767:
                    raise ModgenError(f"{key}: priority must be in signed 16-bit range")
                validate_level_cap(key, mode, soft_percent, rare_candy_policy, cap_table)

                caps.append(
                    {
                        "key": key,
                        "mode": mode,
                        "soft_percent": soft_percent,
                        "rare_candy": rare_candy_policy,
                        "priority": priority,
                        "flags": c_int_or_token(item.get("flags"), "0"),
                        "caps": cap_table,
                    }
                )
    caps.sort(key=lambda cap: (cap["priority"], cap["key"]))
    return caps
