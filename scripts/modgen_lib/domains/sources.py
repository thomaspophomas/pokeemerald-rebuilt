"""Mod C source collector."""

from __future__ import annotations

from typing import Any, Dict, List, Tuple

from ..common import *
from ..errors import ModgenError

def collect_mod_sources(mods: List[Dict[str, Any]]) -> List[str]:
    sources = []
    seen = set()
    for mod in mods:
        for path in sorted((mod["root"] / "src").rglob("*.c")) if (mod["root"] / "src").exists() else []:
            rel = path.relative_to(mod["repo_root"]).as_posix()
            if rel not in seen:
                sources.append(rel)
                seen.add(rel)
        for entry in mod["manifest"].get("entrypoints", []):
            path = mod["root"] / str(entry)
            rel = path.relative_to(mod["repo_root"]).as_posix()
            if path.suffix == ".c" and path.exists() and rel not in seen:
                sources.append(rel)
                seen.add(rel)
    return sources

