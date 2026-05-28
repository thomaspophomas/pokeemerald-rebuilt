"""Fishing action domain collector."""

from __future__ import annotations

from typing import Any, Dict, List, Tuple

from ..common import *
from ..errors import ModgenError
from .keys import key_for

def collect_fishing_actions(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    actions = []
    seen = set()
    for mod in mods:
        for path in iter_json_files(mod["root"] / "fishing"):
            for index, item in enumerate(read_domain_list(path, "actions")):
                key = key_for(mod["id"], item, f"action_{index}")
                if key in seen:
                    raise ModgenError(f"Duplicate fishing action key {key!r}")
                seen.add(key)

                hook = c_func(item.get("hook", item.get("hookSymbol", item.get("hook_symbol"))))
                if hook == "NULL":
                    raise ModgenError(f"{key}: fishing action hook is required")
                hook_key = str(item.get("hookKey", item.get("hook_key", hook)))
                if hook_key == "" or len(hook_key) > 31:
                    raise ModgenError(f"{key}: hookKey must be non-empty and <= 31 bytes")
                prompt_key = item.get("promptKey", item.get("prompt_key"))
                if prompt_key is not None:
                    prompt_key = str(prompt_key)
                    if ":" not in prompt_key:
                        prompt_key = f"{mod['id']}:{prompt_key}"
                    if len(prompt_key) > 31:
                        raise ModgenError(f"{key}: promptKey must be <= 31 bytes")

                rod_mask = parse_fishing_rod_mask(item)
                phase_mask = parse_fishing_phase_mask(item)
                button_mask = parse_button_mask(item.get("buttonMask", item.get("button_mask")), 0)
                timeout = int(item.get("timeoutFrames", item.get("timeout_frames", 0)))
                if timeout < 0 or timeout > 32767:
                    raise ModgenError(f"{key}: timeoutFrames must be in [0, 32767]")
                params = parse_s16_params(item.get("params"), key)
                raw_flags = item.get("flags", 0)
                if isinstance(raw_flags, str):
                    if not re.fullmatch(r"0x[0-9A-Fa-f]+|[0-9]+", raw_flags.strip()):
                        raise ModgenError(f"{key}: flags must be an integer")
                    flags_value = int(raw_flags, 0)
                else:
                    flags_value = int(raw_flags or 0)
                if flags_value < 0 or flags_value > 0xFFFF:
                    raise ModgenError(f"{key}: flags must be in [0, 65535]")
                priority = int(item.get("priority", 1000))
                if priority < -32768 or priority > 32767:
                    raise ModgenError(f"{key}: priority must be in signed 16-bit range")

                actions.append(
                    {
                        "key": key,
                        "hook": hook,
                        "hook_key": hook_key,
                        "rod_mask": rod_mask,
                        "phase_mask": phase_mask,
                        "priority": priority,
                        "flags": flags_value,
                        "prompt_key": prompt_key,
                        "button_mask": button_mask,
                        "timeout": timeout,
                        "success_outcome": parse_fishing_outcome(item.get("successOutcome", item.get("success_outcome")), "CONTINUE"),
                        "failure_outcome": parse_fishing_outcome(item.get("failureOutcome", item.get("failure_outcome")), "GOT_AWAY"),
                        "params": params,
                    }
                )
    actions.sort(key=lambda action: (action["priority"], action["key"]))
    return actions

