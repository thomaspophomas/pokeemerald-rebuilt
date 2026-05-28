"""Shared key helpers for domain collectors."""

from __future__ import annotations

from typing import Any, Dict, List, Tuple

from ..common import *
from ..errors import ModgenError

def key_for(mod_id: str, item: Dict[str, Any], fallback: str) -> str:
    raw = item.get("key") or item.get("id") or item.get("name") or fallback
    raw = str(raw)
    if ":" in raw:
        return raw
    return f"{mod_id}:{raw}"

