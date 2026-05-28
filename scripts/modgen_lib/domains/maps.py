"""Map domain collector."""

from __future__ import annotations

from typing import Any, Dict, List, Tuple

from ..common import *
from ..errors import ModgenError
from .keys import key_for

def collect_maps(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    maps = []
    seen = set()
    next_id = 0
    for mod in mods:
        maps_root = mod["root"] / "maps"
        if not maps_root.exists():
            continue
        for path in sorted(maps_root.glob("*/map.json")):
            data = read_json(path)
            key = key_for(mod["id"], data, path.parent.name)
            if key in seen:
                raise ModgenError(f"Duplicate map key {key!r}")
            seen.add(key)
            script = data.get("script", data.get("scriptPath", data.get("script_path", "scripts.inc")))
            script_path = path.parent / str(script) if script is not None else None
            rel_script = script_path.relative_to(mod["repo_root"]).as_posix() if script_path is not None and script_path.exists() else None
            maps.append(
                {
                    "key": key,
                    "id": next_id,
                    "group": c_int_or_token(data.get("map_group"), "0"),
                    "num": c_int_or_token(data.get("map_num"), "0"),
                    "path": path.relative_to(mod["repo_root"]).as_posix(),
                    "script_path": rel_script,
                }
            )
            next_id += 1
    return maps
