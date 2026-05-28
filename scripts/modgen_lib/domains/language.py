"""Language text domain collector."""

from __future__ import annotations

from typing import Any, Dict, List, Tuple

from ..common import *
from ..errors import ModgenError
from .keys import key_for

def collect_language_texts(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    texts = []
    seen = set()
    for mod in mods:
        lang_root = mod["root"] / "lang"
        for path in iter_json_files(lang_root):
            data = read_json(path)
            language = str(data.get("language", path.stem)) if isinstance(data, dict) else path.stem
            entries = data.get("strings", data) if isinstance(data, dict) else data
            if not isinstance(entries, dict):
                raise ModgenError(f"{path}: language file must be an object or contain a 'strings' object")
            for raw_key, value in sorted(entries.items()):
                key = raw_key if ":" in raw_key else f"{mod['id']}:{raw_key}"
                seen_key = (language, key)
                if seen_key in seen:
                    raise ModgenError(f"Duplicate language key {language}:{key}")
                seen.add(seen_key)
                texts.append({"language": language, "key": key, "text": str(value)})
    texts.sort(key=lambda text: (text["key"], text["language"]))
    return texts

