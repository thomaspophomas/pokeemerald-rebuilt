"""Catalog entry collection and hashing."""

from __future__ import annotations

from typing import Any, Dict, List, Tuple

from ..common import *
from ..errors import ModgenError

def add_catalog_entry(entries: List[Dict[str, int]], type_name: str, key: str, *content: Any, flags: int = 0) -> None:
    entries.append(
        {
            "type": type_name,
            "flags": flags,
            "key_hash": hash_text(key),
            "content_hash": hash_parts(key, *content),
        }
    )


def collect_catalog_entries(
    weather: List[Dict[str, Any]],
    sprite_assets: List[Dict[str, Any]],
    language_texts: List[Dict[str, Any]],
    engines: List[Dict[str, Any]],
    npcs: List[Dict[str, Any]],
    badge_effects: List[Dict[str, Any]],
    fishing_actions: List[Dict[str, Any]],
    encounters: List[Dict[str, Any]],
    shops: List[Dict[str, Any]],
    items: List[Dict[str, Any]],
    rewards: List[Dict[str, Any]],
    pokemon_data: List[Dict[str, Any]],
    battle_moves: List[Dict[str, Any]],
    trainers: List[Dict[str, Any]],
) -> List[Dict[str, int]]:
    entries: List[Dict[str, int]] = []

    for text in language_texts:
        add_catalog_entry(
            entries,
            "MOD_CATALOG_ENTRY_TEXT",
            f"{text['language']}:{text['key']}",
            text["language"],
            text["text"],
        )
    for provider in weather:
        add_catalog_entry(
            entries,
            "MOD_CATALOG_ENTRY_WEATHER",
            provider["id"],
            provider["priority"],
            provider["handler"],
        )
    for engine in engines:
        add_catalog_entry(
            entries,
            "MOD_CATALOG_ENTRY_ENGINE",
            engine["id"],
            engine["name"],
            engine["version"],
            engine["flags"],
            engine["capture"],
            engine["battle_weather"],
        )
    for npc in npcs:
        add_catalog_entry(
            entries,
            "MOD_CATALOG_ENTRY_NPC",
            npc["key"],
            npc["graphics"],
            npc["movement"],
            npc["local"],
            npc["elevation"],
            npc["flag"],
            npc["script"],
        )
    for asset in sprite_assets:
        add_catalog_entry(
            entries,
            "MOD_CATALOG_ENTRY_SPRITE_ASSET",
            asset["key"],
            asset["sheet"],
            asset["compressed_sheet"],
            asset["palette"],
            asset["compressed_palette"],
            asset["template"],
            asset["tile_tag"],
            asset["palette_tag"],
        )
    for effect in badge_effects:
        add_catalog_entry(
            entries,
            "MOD_CATALOG_ENTRY_BADGE_EFFECT",
            effect["key"],
            effect["badge"],
            effect["effect"],
            effect["target"],
            effect["percent"],
            effect["max_level"],
            effect["flags"],
        )
    for action in fishing_actions:
        add_catalog_entry(
            entries,
            "MOD_CATALOG_ENTRY_FISHING_ACTION",
            action["key"],
            action["hook_key"],
            action["rod_mask"],
            action["phase_mask"],
            action["priority"],
            action["flags"],
            action["prompt_key"] or "",
            action["button_mask"],
            action["timeout"],
            action["success_outcome"],
            action["failure_outcome"],
            *action["params"],
        )
        add_catalog_entry(
            entries,
            "MOD_CATALOG_ENTRY_FISHING_ACTION",
            f"{action['key']}:{action['hook_key']}",
            "hook_ref",
            action["key"],
            action["hook_key"],
            flags=1,
        )
    for encounter in encounters:
        slot_parts: List[Any] = []
        for slot in encounter["slots"][: encounter["slot_count"]]:
            slot_parts.extend([slot["min"], slot["max"], slot["species"], slot["weight"], slot["rod_mask"], slot["flags"]])
        add_catalog_entry(
            entries,
            "MOD_CATALOG_ENTRY_ENCOUNTER",
            encounter["key"],
            encounter["hook_key"] or "",
            encounter["group"],
            encounter["num"],
            encounter["area"],
            encounter["rod_mask"],
            encounter["encounter_rate"],
            encounter["slot_count"],
            encounter["priority"],
            encounter["flags"],
            *slot_parts,
        )
        if encounter["hook_key"]:
            add_catalog_entry(
                entries,
                "MOD_CATALOG_ENTRY_ENCOUNTER",
                f"{encounter['key']}:{encounter['hook_key']}",
                "hook_ref",
                encounter["key"],
                encounter["hook_key"],
                flags=1,
            )
    for shop in shops:
        add_catalog_entry(
            entries,
            "MOD_CATALOG_ENTRY_SHOP",
            shop["key"],
            shop["group"],
            shop["num"],
            shop["mart_type"],
            shop["item_count"],
            shop["priority"],
            shop["flags"],
            *shop["items"][: shop["item_count"]],
        )
    for item in items:
        add_catalog_entry(
            entries,
            "MOD_CATALOG_ENTRY_ITEM",
            item["key"],
            item["item"],
            item["name_key"] or "",
            item["description_key"] or "",
            item["field_hook_key"] or "",
            item["battle_hook_key"] or "",
            item["price"],
            item["flags"],
            item["hold_effect"],
            item["hold_effect_param"],
            item["importance"],
            item["pocket"],
            item["type"],
            item["battle_usage"],
            item["secondary_id"],
            item["priority"],
        )
        for hook_key in (item["field_hook_key"], item["battle_hook_key"]):
            if hook_key:
                add_catalog_entry(
                    entries,
                    "MOD_CATALOG_ENTRY_ITEM",
                    f"{item['key']}:{hook_key}",
                    "hook_ref",
                    item["key"],
                    hook_key,
                    flags=1,
                )
    for reward in rewards:
        add_catalog_entry(
            entries,
            "MOD_CATALOG_ENTRY_REWARD",
            reward["key"],
            reward["hook_key"] or "",
            reward["source"],
            reward["min_level"],
            reward["max_level"],
            reward["priority"],
            reward["flags"],
            reward["item"],
            reward["quantity"],
        )
        if reward["hook_key"]:
            add_catalog_entry(
                entries,
                "MOD_CATALOG_ENTRY_REWARD",
                f"{reward['key']}:{reward['hook_key']}",
                "hook_ref",
                reward["key"],
                reward["hook_key"],
                flags=1,
            )
    for data in pokemon_data:
        add_catalog_entry(
            entries,
            "MOD_CATALOG_ENTRY_POKEMON_DATA",
            data["key"],
            data["species"],
            data["priority"],
            data["flags"],
            data["level_move_count"],
            data["evolution_count"],
        )
    for move in battle_moves:
        add_catalog_entry(
            entries,
            "MOD_CATALOG_ENTRY_BATTLE_MOVE",
            move["key"],
            move["move"],
            move["priority"],
            move["override_flags"],
            move["effect"],
            move["power"],
            move["type"],
            move["accuracy"],
            move["pp"],
            move["secondary"],
            move["target"],
            move["move_priority"],
            move["move_flags"],
        )
    for trainer in trainers:
        add_catalog_entry(
            entries,
            "MOD_CATALOG_ENTRY_TRAINER",
            trainer["key"],
            trainer["trainer"],
            trainer["priority"],
            trainer["flags"],
            trainer["party_count"],
        )

    entries.sort(key=lambda entry: (entry["type"], entry["flags"], entry["key_hash"], entry["content_hash"]))
    return entries


def calc_catalog_hash(entries: List[Dict[str, int]]) -> int:
    data = bytearray()
    for entry in entries:
        data.extend(str(entry["type"]).encode("ascii"))
        data.append(0)
        data.append(entry.get("flags", 0) & 0xFF)
        data.extend(entry["key_hash"].to_bytes(4, "little"))
        data.extend(entry["content_hash"].to_bytes(4, "little"))
    return fnv1a(bytes(data))

