"""Event domain collector."""

from __future__ import annotations

from typing import Any, Dict, List, Tuple

from ..common import *
from ..errors import ModgenError
from .keys import key_for

def collect_events(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    events = []
    for mod in mods:
        for path in iter_json_files(mod["root"] / "events"):
            for item in read_domain_list(path, "subscriptions"):
                handler = c_func(item.get("handler"))
                events.append(
                    {
                        "type": normalize_event_type(item.get("type", "CUSTOM")),
                        "priority": int(item.get("priority", 1000)),
                        "handler": handler,
                        "source": key_for(mod["id"], item, path.stem),
                    }
                )
    events.sort(key=lambda event: (event["type"], event["priority"], event["source"]))
    return events

