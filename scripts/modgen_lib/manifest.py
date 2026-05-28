"""Manifest loading and mod ordering for modgen."""

from __future__ import annotations

import re
from pathlib import Path
from typing import Any, Dict, List

from .common import read_json, require_mod_id
from .errors import ModgenError


def load_state_budget(root: Path) -> int:
    header = root / "include" / "mod" / "state.h"
    if not header.exists():
        return 184

    text = header.read_text(encoding="utf-8")
    match = re.search(r"^\s*#define\s+MOD_SAVE_RESERVED_BYTES\s+\((\d+)\s+-\s*MOD_SAVE_BADGE_LEVEL_BYTES(?:\s+-\s*MOD_SAVE_STATE_VERSION_BYTES)?\)", text, re.MULTILINE)
    if not match:
        return 184

    badge_match = re.search(r"^\s*#define\s+MOD_SAVE_BADGE_LEVEL_COUNT\s+(\d+)", text, re.MULTILINE)
    version_match = re.search(r"^\s*#define\s+MOD_SAVE_MAX_STATE_BLOCKS\s+(\d+)", text, re.MULTILINE)
    badge_bytes = int(badge_match.group(1)) if badge_match else 8
    version_bytes = int(version_match.group(1)) * 2 if version_match else 0
    return int(match.group(1)) - badge_bytes - version_bytes


def load_enabled_mod_ids(root: Path) -> List[str] | None:
    path = root / "mods" / "enabled.json"
    if not path.exists():
        return None

    data = read_json(path)
    enabled = data.get("enabled") if isinstance(data, dict) else data
    if not isinstance(enabled, list):
        raise ModgenError(f"{path}: expected list or object with 'enabled' list")

    out: List[str] = []
    seen = set()
    for item in enabled:
        mod_id = require_mod_id(item, path)
        if mod_id in seen:
            raise ModgenError(f"{path}: duplicate enabled mod {mod_id!r}")
        seen.add(mod_id)
        out.append(mod_id)
    return out


def load_mods(root: Path) -> List[Dict[str, Any]]:
    mods_dir = root / "mods"
    mods: List[Dict[str, Any]] = []
    seen = set()

    if not mods_dir.exists():
        return mods

    enabled = load_enabled_mod_ids(root)
    enabled_set = set(enabled) if enabled is not None else None

    for manifest_path in sorted(mods_dir.glob("*/mod.json")):
        manifest = read_json(manifest_path)
        mod_id = require_mod_id(manifest.get("id"), manifest_path)
        if enabled_set is not None and mod_id not in enabled_set:
            continue
        if mod_id in seen:
            raise ModgenError(f"Duplicate mod id {mod_id!r}")
        seen.add(mod_id)
        mod_root = manifest_path.parent
        mods.append(
            {
                "id": mod_id,
                "name": str(manifest.get("name", mod_id)),
                "version": str(manifest.get("version", "0.0.0")),
                "priority": int(manifest.get("priority", 1000)),
                "features": int(manifest.get("featureFlags", manifest.get("features", 0)) or 0),
                "state_version": int(manifest.get("stateVersion", 0) or 0),
                "state_bytes": int(manifest.get("stateBytes", 0) or 0),
                "state_offset": 0,
                "dependencies": list(manifest.get("dependencies", [])),
                "root": mod_root,
                "repo_root": root,
                "manifest": manifest,
            }
        )

    if enabled_set is not None:
        missing = sorted(enabled_set - seen)
        if missing:
            raise ModgenError(f"{mods_dir / 'enabled.json'}: enabled mods missing manifests: {', '.join(missing)}")

    known = {mod["id"] for mod in mods}
    for mod in mods:
        for dep in mod["dependencies"]:
            if dep not in known:
                raise ModgenError(f"{mod['id']}: missing dependency {dep!r}")

    mods.sort(key=lambda mod: (mod["priority"], mod["id"]))
    state_budget = load_state_budget(root)
    offset = 0
    for index, mod in enumerate(mods):
        if index >= 16 and mod["state_bytes"]:
            raise ModgenError("mods with stateBytes are limited to the first 16 loaded mods")
        mod["state_offset"] = offset
        offset += mod["state_bytes"]
        if offset > state_budget:
            raise ModgenError(f"declared mod stateBytes {offset} exceeds budget {state_budget}")
    return mods
