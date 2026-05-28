"""Pokemon data domain collector."""

from __future__ import annotations

from typing import Any, Dict, List, Tuple

from ..common import *
from ..errors import ModgenError
from .keys import key_for

def collect_pokemon_data(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    pokemon = []
    seen = set()
    for mod in mods:
        for path in iter_json_files(mod["root"] / "pokemon"):
            for index, item in enumerate(read_domain_list(path, "pokemon")):
                key = key_for(mod["id"], item, f"pokemon_{index}")
                if key in seen:
                    raise ModgenError(f"Duplicate pokemon data key {key!r}")
                seen.add(key)
                inferred = 0
                info = item.get("info", item)
                base = info.get("baseStats", info.get("base_stats"))
                if base is not None:
                    inferred |= POKEMON_OVERRIDE_FLAGS["BASE_STATS"]
                types = info.get("types")
                if types is not None:
                    inferred |= POKEMON_OVERRIDE_FLAGS["TYPES"]
                if "catchRate" in info or "catch_rate" in info:
                    inferred |= POKEMON_OVERRIDE_FLAGS["CATCH_RATE"]
                if "expYield" in info or "exp_yield" in info:
                    inferred |= POKEMON_OVERRIDE_FLAGS["EXP_YIELD"]
                ev_yields = info.get("evYields", info.get("ev_yields"))
                if ev_yields is not None:
                    inferred |= POKEMON_OVERRIDE_FLAGS["EV_YIELD"]
                if "items" in info or "itemCommon" in info or "item_common" in info:
                    inferred |= POKEMON_OVERRIDE_FLAGS["ITEMS"]
                if "genderRatio" in info or "gender_ratio" in info:
                    inferred |= POKEMON_OVERRIDE_FLAGS["GENDER"]
                if "eggCycles" in info or "egg_cycles" in info:
                    inferred |= POKEMON_OVERRIDE_FLAGS["EGG_CYCLES"]
                if "friendship" in info:
                    inferred |= POKEMON_OVERRIDE_FLAGS["FRIENDSHIP"]
                if "growthRate" in info or "growth_rate" in info:
                    inferred |= POKEMON_OVERRIDE_FLAGS["GROWTH_RATE"]
                if "eggGroups" in info or "egg_groups" in info:
                    inferred |= POKEMON_OVERRIDE_FLAGS["EGG_GROUPS"]
                if "abilities" in info:
                    inferred |= POKEMON_OVERRIDE_FLAGS["ABILITIES"]
                if "safariZoneFleeRate" in info or "safari_zone_flee_rate" in info:
                    inferred |= POKEMON_OVERRIDE_FLAGS["SAFARI_FLEE"]
                if "bodyColor" in info or "body_color" in info:
                    inferred |= POKEMON_OVERRIDE_FLAGS["BODY_COLOR"]
                level_up = item.get("levelUpMoves", item.get("level_up_moves", []))
                if level_up:
                    inferred |= POKEMON_OVERRIDE_FLAGS["LEVEL_UP"]
                evolutions = item.get("evolutions", [])
                if evolutions:
                    inferred |= POKEMON_OVERRIDE_FLAGS["EVOLUTIONS"]
                flags = parse_u16_flags(item.get("flags"), POKEMON_OVERRIDE_FLAGS, "pokemon data", inferred)
                base_values = base or [0, 0, 0, 0, 0, 0]
                if len(base_values) != 6:
                    raise ModgenError(f"{key}: baseStats must contain six values")
                type_values = types or [0, 0]
                if len(type_values) != 2:
                    raise ModgenError(f"{key}: types must contain two values")
                ev_values = ev_yields or [0, 0, 0, 0, 0, 0]
                if len(ev_values) != 6:
                    raise ModgenError(f"{key}: evYields must contain six values")
                egg_groups = info.get("eggGroups", info.get("egg_groups", [0, 0]))
                abilities = info.get("abilities", [0, 0])
                if len(egg_groups) != 2 or len(abilities) != 2:
                    raise ModgenError(f"{key}: eggGroups and abilities must contain two values")
                level_moves = []
                for move in level_up:
                    level = int(move.get("level", 1))
                    if level < 1 or level > 100:
                        raise ModgenError(f"{key}: level up move level must be in [1, 100]")
                    level_moves.append({"level": level, "move": c_int_or_token(move.get("move"), "MOVE_NONE")})
                if len(level_moves) > 32:
                    raise ModgenError(f"{key}: levelUpMoves may contain at most 32 entries")
                while len(level_moves) < 32:
                    level_moves.append({"level": 0, "move": "MOVE_NONE"})
                evo_values = []
                for evo in evolutions:
                    evo_values.append(
                        {
                            "method": c_int_or_token(evo.get("method"), "EVO_NONE"),
                            "param": c_int_or_token(evo.get("param"), "0"),
                            "target": c_int_or_token(evo.get("targetSpecies", evo.get("target_species")), "SPECIES_NONE"),
                        }
                    )
                if len(evo_values) > 5:
                    raise ModgenError(f"{key}: evolutions may contain at most 5 entries")
                while len(evo_values) < 5:
                    evo_values.append({"method": "EVO_NONE", "param": "0", "target": "SPECIES_NONE"})
                pokemon.append(
                    {
                        "key": key,
                        "species": c_int_or_token(item.get("species"), "SPECIES_NONE"),
                        "priority": int(item.get("priority", 1000)),
                        "flags": flags,
                        "base": [c_int_or_token(v, "0") for v in base_values],
                        "types": [c_int_or_token(v, "0") for v in type_values],
                        "catch_rate": c_int_or_token(info.get("catchRate", info.get("catch_rate")), "0"),
                        "exp_yield": c_int_or_token(info.get("expYield", info.get("exp_yield")), "0"),
                        "ev_yields": [c_int_or_token(v, "0") for v in ev_values],
                        "item_common": c_int_or_token(info.get("itemCommon", info.get("item_common", (info.get("items") or [None, None])[0] if info.get("items") else None)), "ITEM_NONE"),
                        "item_rare": c_int_or_token(info.get("itemRare", info.get("item_rare", (info.get("items") or [None, None])[1] if info.get("items") else None)), "ITEM_NONE"),
                        "gender": c_int_or_token(info.get("genderRatio", info.get("gender_ratio")), "0"),
                        "egg_cycles": c_int_or_token(info.get("eggCycles", info.get("egg_cycles")), "0"),
                        "friendship": c_int_or_token(info.get("friendship"), "0"),
                        "growth_rate": c_int_or_token(info.get("growthRate", info.get("growth_rate")), "0"),
                        "egg_groups": [c_int_or_token(v, "0") for v in egg_groups],
                        "abilities": [c_int_or_token(v, "0") for v in abilities],
                        "safari_flee": c_int_or_token(info.get("safariZoneFleeRate", info.get("safari_zone_flee_rate")), "0"),
                        "body_color": c_int_or_token(info.get("bodyColor", info.get("body_color")), "0"),
                        "no_flip": c_bool(info.get("noFlip", info.get("no_flip", False))),
                        "level_moves": level_moves,
                        "level_move_count": len(level_up),
                        "evolutions": evo_values,
                        "evolution_count": len(evolutions),
                    }
                )
    pokemon.sort(key=lambda data: (data["priority"], data["key"]))
    return pokemon

