"""Engine domain collector."""

from __future__ import annotations

from typing import Any, Dict, List, Tuple

from ..common import *
from ..errors import ModgenError
from .keys import key_for

def collect_engines(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    engines = [
        {
            "id": "engine:gen3",
            "name": "Generation III",
            "version": 1,
            "flags": "ENGINE_RULESET_FLAG_SAVE_COMPATIBLE",
            "capture": "NULL",
            "battle_weather": "NULL",
        }
    ]
    seen = {"engine:gen3"}
    for mod in mods:
        for path in iter_json_files(mod["root"] / "engines"):
            for item in read_domain_list(path, "engines"):
                key = key_for(mod["id"], item, path.stem)
                if key in seen:
                    raise ModgenError(f"Duplicate engine key {key!r}")
                seen.add(key)
                flags = "ENGINE_RULESET_FLAG_SAVE_COMPATIBLE" if item.get("saveCompatible", False) else "0"
                engines.append(
                    {
                        "id": key,
                        "name": str(item.get("name", key)),
                        "version": int(item.get("version", 1)),
                        "flags": flags,
                        "capture": c_func(item.get("captureHook")),
                        "battle_weather": c_func(item.get("battleWeatherHook")),
                    }
                )
    return engines

