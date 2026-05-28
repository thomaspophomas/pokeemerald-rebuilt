"""Weather domain collector."""

from __future__ import annotations

from typing import Any, Dict, List, Tuple

from ..common import *
from ..errors import ModgenError
from .keys import key_for

def collect_weather(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    providers = []
    seen = set()
    for mod in mods:
        for path in iter_json_files(mod["root"] / "weather"):
            for item in read_domain_list(path, "providers"):
                key = key_for(mod["id"], item, path.stem)
                if key in seen:
                    raise ModgenError(f"Duplicate weather provider key {key!r}")
                seen.add(key)
                providers.append(
                    {
                        "id": key,
                        "priority": int(item.get("priority", 1000)),
                        "handler": c_func(item.get("handler")),
                    }
                )
    providers.sort(key=lambda provider: (provider["priority"], provider["id"]))
    return providers

