"""Badge effect domain collector."""

from __future__ import annotations

from typing import Any, Dict, List, Tuple

from ..common import *
from ..errors import ModgenError
from .keys import key_for

def collect_badge_effects(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    effects = []
    seen = set()
    for mod in mods:
        for path in iter_json_files(mod["root"] / "badges"):
            for index, item in enumerate(read_domain_list(path, "effects")):
                key = key_for(mod["id"], item, f"effect_{index}")
                if key in seen:
                    raise ModgenError(f"Duplicate badge effect key {key!r}")
                seen.add(key)

                percent = int(item.get("percentPerLevel", item.get("percent_per_level", 0)))
                max_level = int(item.get("maxLevel", item.get("max_level", 1)))
                badge = normalize_badge_id(item.get("badge", item.get("badgeId", item.get("badge_id"))))
                effect = normalize_badge_effect_type(item.get("effect", item.get("effectKind", item.get("effect_kind", "NONE"))))
                target = item.get("target")
                target_value = badge_effect_target_value(target)
                validate_badge_effect(key, badge, effect, target_value, percent, max_level)

                effects.append(
                    {
                        "key": key,
                        "badge": badge,
                        "effect": effect,
                        "target": c_int_or_token(target, "0"),
                        "percent": percent,
                        "max_level": max_level,
                        "flags": c_int_or_token(item.get("flags"), "0"),
                    }
                )
    effects.sort(key=lambda effect: effect["key"])
    return effects

