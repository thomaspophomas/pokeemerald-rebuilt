"""Flag domain collector."""

from __future__ import annotations

from typing import Any, Dict, List, Tuple

from ..common import *
from ..errors import ModgenError
from .keys import key_for

def collect_flags(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    flags = []
    seen = set()
    next_id = 0
    for mod in mods:
        for path in iter_json_files(mod["root"] / "flags"):
            for index, item in enumerate(read_domain_list(path, "flags")):
                if isinstance(item, str):
                    item = {"id": item}
                key = key_for(mod["id"], item, f"flag_{index}")
                if key in seen:
                    raise ModgenError(f"Duplicate flag key {key!r}")
                seen.add(key)
                flags.append({"key": key, "id": next_id, "initial": bool(item.get("initial", False))})
                next_id += 1
    return flags

