"""Time segment domain collector."""

from __future__ import annotations

from typing import Any, Dict, List, Tuple

from ..common import *
from ..errors import ModgenError
from .keys import key_for

def collect_time_segments(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    segments = []
    seen = set()
    for mod in mods:
        for path in iter_json_files(mod["root"] / "time"):
            for item in read_domain_list(path, "segments"):
                key = key_for(mod["id"], item, path.stem)
                if key in seen:
                    raise ModgenError(f"Duplicate time segment key {key!r}")
                seen.add(key)
                start = int(item.get("startMinute", item.get("start_minute", 0)))
                end = int(item.get("endMinute", item.get("end_minute", 0)))
                if start < 0 or start >= 24 * 60 or end < 0 or end >= 24 * 60:
                    raise ModgenError(f"{key}: time segment minutes must be in [0, 1439]")
                segments.append(
                    {
                        "key": key,
                        "start": start,
                        "end": end,
                        "segment": normalize_time_segment(item.get("segment", "DAY")),
                    }
                )
    segments.sort(key=lambda segment: (segment["start"], segment["key"]))
    return segments

