#!/usr/bin/env python3
"""Smoke-test the mod generator with a synthetic mod tree."""

from __future__ import annotations

import json
import subprocess
import sys
import tempfile
from pathlib import Path


def write_json(path: Path, data) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(data, indent=2), encoding="utf-8")


def run_modgen(modgen: Path, root: Path) -> subprocess.CompletedProcess:
    return subprocess.run(
        [
            sys.executable,
            str(modgen),
            "--root",
            str(root),
            "--out-header",
            "include/generated/mod_registry.h",
            "--out-source",
            "src/generated/mod_registry.c",
            "--out-make",
            "build/generated/mod_sources.mk",
        ],
        check=False,
        text=True,
        capture_output=True,
    )


def main() -> int:
    repo = Path(__file__).resolve().parents[2]
    modgen = repo / "scripts" / "modgen.py"

    with tempfile.TemporaryDirectory() as temp_dir:
        root = Path(temp_dir)
        mod_root = root / "mods" / "demo"
        write_json(
            mod_root / "mod.json",
            {
                "id": "demo",
                "name": "Demo Mod",
                "version": "1.0.0",
                "priority": 10,
                "dependencies": [],
            },
        )
        write_json(mod_root / "flags" / "story.json", {"flags": [{"id": "badge_1", "initial": False}]})
        write_json(
            mod_root / "events" / "hooks.json",
            {"subscriptions": [{"type": "FLAG_CHANGED", "handler": "Demo_OnFlagChanged", "priority": 10}]},
        )
        write_json(
            mod_root / "weather" / "providers.json",
            {"providers": [{"id": "dark_rain", "handler": "Demo_Weather", "priority": 5}]},
        )
        write_json(
            mod_root / "sprites" / "assets" / "assets.json",
            {
                "assets": [
                    {
                        "id": "guide_asset",
                        "compressedSheetSymbol": "Demo_GuideSheet",
                        "compressedPaletteSymbol": "Demo_GuidePalette",
                        "tileTag": "0xF100",
                        "paletteTag": "0xF101",
                    }
                ]
            },
        )
        write_json(
            mod_root / "sprites" / "overworld" / "sprites.json",
            {"sprites": [{"id": "guide_ow", "asset": "guide_asset", "graphicsId": "OBJ_EVENT_GFX_BOY_1"}]},
        )
        write_json(
            mod_root / "sprites" / "battle" / "sprites.json",
            {"sprites": [{"id": "demo_ball_anim", "asset": "guide_asset", "species": 1, "side": 0}]},
        )
        write_json(
            mod_root / "followers" / "followers.json",
            {"followers": [{"id": "treecko", "species": "SPECIES_TREECKO", "form": 0, "graphicsId": "OBJ_EVENT_GFX_BOY_1"}]},
        )
        write_json(
            mod_root / "lang" / "en.json",
            {"strings": {"npc_intro": "Hello!", "potion_name": "Demo Potion", "potion_desc": "A demo item."}},
        )
        write_json(mod_root / "lang" / "de.json", {"strings": {"npc_intro": "Hallo!"}})
        write_json(
            mod_root / "pokeballs" / "balls.json",
            {
                "pokeballs": [
                    {
                        "id": "story_ball",
                        "itemId": "ITEM_POKE_BALL",
                        "ballId": "BALL_PREMIER",
                        "catchModifier": 20,
                        "catchModifierHook": "Demo_BallModifier",
                    }
                ]
            },
        )
        write_json(
            mod_root / "time" / "segments.json",
            {"segments": [{"id": "deep_night", "startMinute": 1320, "endMinute": 299, "segment": "NIGHT"}]},
        )
        write_json(
            mod_root / "badges" / "effects.json",
            {
                "effects": [
                    {
                        "id": "stone_ground_resist",
                        "badge": "STONE",
                        "effect": "RESISTANCE_PERCENT",
                        "target": "TYPE_GROUND",
                        "percentPerLevel": 1,
                        "maxLevel": 10,
                    }
                ]
            },
        )
        write_json(
            mod_root / "fishing" / "actions.json",
            {
                "actions": [
                    {
                        "id": "extra_reel",
                        "hook": "FishingApi_RequestConfiguredAction",
                        "hookKey": "configured_button",
                        "rods": ["GOOD", "SUPER"],
                        "phases": ["INPUT_WINDOW"],
                        "buttonMask": "B_BUTTON",
                        "timeoutFrames": 24,
                        "successOutcome": "CONTINUE",
                        "failureOutcome": "GOT_AWAY",
                        "promptKey": "fishing_prompt",
                        "params": [1, 2],
                    }
                ]
            },
        )
        write_json(
            mod_root / "encounters" / "route101.json",
            {
                "encounters": [
                    {
                        "id": "route101_land",
                        "mapGroup": 0,
                        "mapNum": 16,
                        "area": "LAND",
                        "encounterRate": 20,
                        "priority": 10,
                        "hook": "Demo_EncounterHook",
                        "hookKey": "scale_level",
                        "slots": [
                            {"species": "SPECIES_POOCHYENA", "minLevel": 2, "maxLevel": 4, "weight": 100}
                        ],
                    }
                ]
            },
        )
        write_json(
            mod_root / "shops" / "oldale.json",
            {
                "shops": [
                    {
                        "id": "oldale_basic",
                        "mapGroup": 0,
                        "mapNum": 21,
                        "type": "NORMAL",
                        "items": ["ITEM_POKE_BALL", "ITEM_POTION"],
                    }
                ]
            },
        )
        write_json(
            mod_root / "items" / "items.json",
            {
                "items": [
                    {
                        "id": "potion_tweak",
                        "itemId": "ITEM_POTION",
                        "nameKey": "potion_name",
                        "descriptionKey": "potion_desc",
                        "price": 250,
                    }
                ]
            },
        )
        write_json(
            mod_root / "rewards" / "pickup.json",
            {"rewards": [{"id": "pickup_bonus", "source": "PICKUP_COMMON", "minLevel": 1, "maxLevel": 100, "itemId": "ITEM_POTION"}]},
        )
        write_json(
            mod_root / "pokemon" / "poochyena.json",
            {
                "pokemon": [
                    {
                        "id": "poochyena_data",
                        "species": "SPECIES_POOCHYENA",
                        "baseStats": [35, 55, 35, 35, 30, 30],
                        "types": ["TYPE_DARK", "TYPE_DARK"],
                        "abilities": ["ABILITY_RUN_AWAY", "ABILITY_QUICK_FEET"],
                        "levelUpMoves": [{"level": 5, "move": "MOVE_TACKLE"}],
                    }
                ]
            },
        )
        write_json(
            mod_root / "battle" / "moves" / "moves.json",
            {"moves": [{"id": "tackle_power", "move": "MOVE_TACKLE", "power": 45}]},
        )
        write_json(
            mod_root / "trainers" / "trainers.json",
            {
                "trainers": [
                    {
                        "id": "youngster_party",
                        "trainerId": 1,
                        "party": [{"species": "SPECIES_POOCHYENA", "level": 5, "moves": ["MOVE_TACKLE"]}],
                    }
                ]
            },
        )
        write_json(
            mod_root / "engines" / "rulesets.json",
            {"engines": [{"id": "demo_engine", "name": "Demo Engine", "version": 1, "saveCompatible": True}]},
        )
        write_json(
            mod_root / "npcs" / "npcs.json",
            {
                "npcs": [
                    {
                        "id": "guide",
                        "graphicsId": "OBJ_EVENT_GFX_BOY_1",
                        "movementType": "MOVEMENT_TYPE_FACE_DOWN",
                        "localId": 208,
                        "elevation": 3,
                    }
                ]
            },
        )
        write_json(
            mod_root / "maps" / "DemoTown" / "map.json",
            {"id": "demo_town", "name": "DemoTown", "map_group": 0, "map_num": 0},
        )
        (mod_root / "src").mkdir(parents=True, exist_ok=True)
        (mod_root / "src" / "demo.c").write_text("/* demo */\n", encoding="utf-8")

        result = run_modgen(modgen, root)
        if result.returncode != 0:
            raise AssertionError(result.stderr)

        header = (root / "include" / "generated" / "mod_registry.h").read_text(encoding="utf-8")
        source = (root / "src" / "generated" / "mod_registry.c").read_text(encoding="utf-8")
        make = (root / "build" / "generated" / "mod_sources.mk").read_text(encoding="utf-8")

        required = [
            "gModFlagDefinitionCount = 1",
            "Demo_OnFlagChanged",
            "Demo_Weather",
            "demo:deep_night",
            "demo:guide_asset",
            "demo:guide_ow",
            "demo:demo_ball_anim",
            "demo:npc_intro",
            "demo:story_ball",
            "Demo_BallModifier",
            "gModBadgeEffects",
            "demo:stone_ground_resist",
            "gModFishingActions",
            "demo:extra_reel",
            "configured_button",
            "MOD_CATALOG_ENTRY_FISHING_ACTION",
            "gModEncounterDefinitions",
            "demo:route101_land",
            "Demo_EncounterHook",
            "MOD_CATALOG_ENTRY_ENCOUNTER",
            "gModShopDefinitions",
            "demo:oldale_basic",
            "MOD_CATALOG_ENTRY_SHOP",
            "gModItemDefinitions",
            "demo:potion_tweak",
            "MOD_CATALOG_ENTRY_ITEM",
            "gModRewardDefinitions",
            "demo:pickup_bonus",
            "MOD_CATALOG_ENTRY_REWARD",
            "gModPokemonDataDefinitions",
            "demo:poochyena_data",
            "MOD_CATALOG_ENTRY_POKEMON_DATA",
            "gModBattleMoveDefinitions",
            "demo:tackle_power",
            "MOD_CATALOG_ENTRY_BATTLE_MOVE",
            "gModTrainerDefinitions",
            "demo:youngster_party",
            "MOD_CATALOG_ENTRY_TRAINER",
            "demo:guide",
            "demo:demo_town",
            "demo:demo_engine",
            "gModCatalogEntryCount",
            "gModCatalogHash",
            "MOD_CATALOG_ENTRY_TEXT",
            "MOD_CATALOG_ENTRY_SPRITE_ASSET",
            "MOD_CATALOG_ENTRY_BADGE_EFFECT",
        ]
        for needle in required:
            if needle not in source:
                raise AssertionError(f"generated source missing {needle}")
        if "extern const struct ModManifest gModManifests[]" not in header:
            raise AssertionError("generated header missing manifest declaration")
        if "extern const struct ModCatalogEntry gModCatalogEntries[]" not in header:
            raise AssertionError("generated header missing catalog declaration")
        if "extern const struct ModBadgeEffectDefinition gModBadgeEffects[]" not in header:
            raise AssertionError("generated header missing badge effect declaration")
        if "extern const struct FishingActionDefinition gModFishingActions[]" not in header:
            raise AssertionError("generated header missing fishing action declaration")
        if "extern const struct ModEncounterDefinition gModEncounterDefinitions[]" not in header:
            raise AssertionError("generated header missing encounter declaration")
        if "extern const struct ModShopDefinition gModShopDefinitions[]" not in header:
            raise AssertionError("generated header missing shop declaration")
        if "extern const struct ModItemDefinition gModItemDefinitions[]" not in header:
            raise AssertionError("generated header missing item declaration")
        if "extern const struct ModRewardDefinition gModRewardDefinitions[]" not in header:
            raise AssertionError("generated header missing reward declaration")
        if "extern const struct ModPokemonDataDefinition gModPokemonDataDefinitions[]" not in header:
            raise AssertionError("generated header missing pokemon data declaration")
        if "extern const struct ModBattleMoveDefinition gModBattleMoveDefinitions[]" not in header:
            raise AssertionError("generated header missing battle move declaration")
        if "extern const struct ModTrainerDefinition gModTrainerDefinitions[]" not in header:
            raise AssertionError("generated header missing trainer declaration")
        if "MOD_C_SRCS += mods/demo/src/demo.c" not in make:
            raise AssertionError("generated make fragment missing mod C source")

        valid_effect = {
            "id": "stone_ground_resist",
            "badge": "STONE",
            "effect": "RESISTANCE_PERCENT",
            "target": "TYPE_GROUND",
            "percentPerLevel": 1,
            "maxLevel": 10,
        }
        invalid_badge_cases = {
            "type target outside range": [{**valid_effect, "target": 18}],
            "TYPE_NONE target": [{**valid_effect, "target": "TYPE_NONE"}],
            "stat target outside range": [{**valid_effect, "effect": "STAT_PERCENT", "target": 8}],
            "NONE effect": [{**valid_effect, "effect": "NONE"}],
            "maxLevel zero": [{**valid_effect, "maxLevel": 0}],
            "badge outside range": [{**valid_effect, "badge": 8}],
            "duplicate badge key": [valid_effect, valid_effect],
        }
        for label, effects in invalid_badge_cases.items():
            write_json(mod_root / "badges" / "effects.json", {"effects": effects})
            result = run_modgen(modgen, root)
            if result.returncode == 0:
                raise AssertionError(f"modgen accepted invalid badge effect: {label}")
        write_json(mod_root / "badges" / "effects.json", {"effects": [valid_effect]})

        valid_action = {
            "id": "extra_reel",
            "hook": "FishingApi_RequestConfiguredAction",
            "hookKey": "configured_button",
            "rodMask": 2,
            "phases": ["INPUT_WINDOW"],
            "buttonMask": "B_BUTTON",
            "timeoutFrames": 24,
            "successOutcome": "CONTINUE",
            "failureOutcome": "GOT_AWAY",
            "params": [1],
        }
        invalid_fishing_cases = {
            "duplicate key": [valid_action, valid_action],
            "empty hook": [{**valid_action, "hook": ""}],
            "invalid rod mask": [{**valid_action, "rodMask": 8}],
            "invalid phase": [{**valid_action, "phases": ["AFTER_LUNCH"]}],
            "invalid outcome": [{**valid_action, "successOutcome": "SPLASH"}],
            "param outside range": [{**valid_action, "params": [40000]}],
        }
        for label, actions in invalid_fishing_cases.items():
            write_json(mod_root / "fishing" / "actions.json", {"actions": actions})
            result = run_modgen(modgen, root)
            if result.returncode == 0:
                raise AssertionError(f"modgen accepted invalid fishing action: {label}")
        write_json(mod_root / "fishing" / "actions.json", {"actions": [valid_action]})

        valid_encounter = {
            "id": "route101_land",
            "mapGroup": 0,
            "mapNum": 16,
            "area": "LAND",
            "encounterRate": 20,
            "slots": [{"species": "SPECIES_POOCHYENA", "minLevel": 2, "maxLevel": 4, "weight": 100}],
        }
        invalid_encounter_cases = {
            "duplicate key": [valid_encounter, valid_encounter],
            "missing map": [{key: value for key, value in valid_encounter.items() if key not in ("mapGroup", "mapNum")}],
            "invalid area": [{**valid_encounter, "area": "SKY"}],
            "too many slots": [{**valid_encounter, "slots": valid_encounter["slots"] * 13}],
            "level outside range": [{**valid_encounter, "slots": [{**valid_encounter["slots"][0], "minLevel": 0}]}],
            "weight outside range": [{**valid_encounter, "slots": [{**valid_encounter["slots"][0], "weight": 0}]}],
            "empty without hook": [{**valid_encounter, "slots": []}],
        }
        for label, encounters in invalid_encounter_cases.items():
            write_json(mod_root / "encounters" / "route101.json", {"encounters": encounters})
            result = run_modgen(modgen, root)
            if result.returncode == 0:
                raise AssertionError(f"modgen accepted invalid encounter: {label}")
        write_json(mod_root / "encounters" / "route101.json", {"encounters": [valid_encounter]})

        valid_shop = {
            "id": "oldale_basic",
            "mapGroup": 0,
            "mapNum": 21,
            "type": "NORMAL",
            "items": ["ITEM_POKE_BALL", "ITEM_POTION"],
        }
        invalid_shop_cases = {
            "duplicate key": [valid_shop, valid_shop],
            "missing map": [{key: value for key, value in valid_shop.items() if key not in ("mapGroup", "mapNum")}],
            "invalid type": [{**valid_shop, "type": "BLACK_MARKET"}],
            "empty items": [{**valid_shop, "items": []}],
            "too many items": [{**valid_shop, "items": ["ITEM_POTION"] * 33}],
        }
        for label, shops in invalid_shop_cases.items():
            write_json(mod_root / "shops" / "oldale.json", {"shops": shops})
            result = run_modgen(modgen, root)
            if result.returncode == 0:
                raise AssertionError(f"modgen accepted invalid shop: {label}")

        valid_item = {"id": "potion_tweak", "itemId": "ITEM_POTION", "nameKey": "potion_name", "price": 250}
        invalid_item_cases = {
            "duplicate key": [valid_item, valid_item],
            "empty field hook key": [{**valid_item, "fieldUseHook": "Demo_ItemUse", "fieldUseHookKey": ""}],
        }
        for label, items in invalid_item_cases.items():
            write_json(mod_root / "items" / "items.json", {"items": items})
            result = run_modgen(modgen, root)
            if result.returncode == 0:
                raise AssertionError(f"modgen accepted invalid item: {label}")
        write_json(mod_root / "items" / "items.json", {"items": [valid_item]})

        valid_reward = {"id": "pickup_bonus", "source": "PICKUP_COMMON", "minLevel": 1, "maxLevel": 100, "itemId": "ITEM_POTION"}
        invalid_reward_cases = {
            "duplicate key": [valid_reward, valid_reward],
            "invalid source": [{**valid_reward, "source": "LOTTERY"}],
            "invalid level": [{**valid_reward, "minLevel": 0}],
        }
        for label, rewards in invalid_reward_cases.items():
            write_json(mod_root / "rewards" / "pickup.json", {"rewards": rewards})
            result = run_modgen(modgen, root)
            if result.returncode == 0:
                raise AssertionError(f"modgen accepted invalid reward: {label}")
        write_json(mod_root / "rewards" / "pickup.json", {"rewards": [valid_reward]})

        valid_pokemon = {
            "id": "poochyena_data",
            "species": "SPECIES_POOCHYENA",
            "baseStats": [35, 55, 35, 35, 30, 30],
            "levelUpMoves": [{"level": 5, "move": "MOVE_TACKLE"}],
        }
        invalid_pokemon_cases = {
            "duplicate key": [valid_pokemon, valid_pokemon],
            "bad base stats": [{**valid_pokemon, "baseStats": [1, 2]}],
            "too many moves": [{**valid_pokemon, "levelUpMoves": [{"level": 5, "move": "MOVE_TACKLE"}] * 33}],
        }
        for label, pokemon in invalid_pokemon_cases.items():
            write_json(mod_root / "pokemon" / "poochyena.json", {"pokemon": pokemon})
            result = run_modgen(modgen, root)
            if result.returncode == 0:
                raise AssertionError(f"modgen accepted invalid pokemon data: {label}")
        write_json(mod_root / "pokemon" / "poochyena.json", {"pokemon": [valid_pokemon]})

        valid_move = {"id": "tackle_power", "move": "MOVE_TACKLE", "power": 45}
        invalid_move_cases = {
            "duplicate key": [valid_move, valid_move],
            "empty override": [{"id": "tackle_power", "move": "MOVE_TACKLE", "overrideFlags": 0}],
        }
        for label, moves in invalid_move_cases.items():
            write_json(mod_root / "battle" / "moves" / "moves.json", {"moves": moves})
            result = run_modgen(modgen, root)
            if result.returncode == 0:
                raise AssertionError(f"modgen accepted invalid battle move: {label}")
        write_json(mod_root / "battle" / "moves" / "moves.json", {"moves": [valid_move]})

        valid_trainer = {"id": "youngster_party", "trainerId": 1, "party": [{"species": "SPECIES_POOCHYENA", "level": 5}]}
        invalid_trainer_cases = {
            "duplicate key": [valid_trainer, valid_trainer],
            "too many mons": [{**valid_trainer, "party": valid_trainer["party"] * 7}],
            "invalid level": [{**valid_trainer, "party": [{"species": "SPECIES_POOCHYENA", "level": 0}]}],
        }
        for label, trainers in invalid_trainer_cases.items():
            write_json(mod_root / "trainers" / "trainers.json", {"trainers": trainers})
            result = run_modgen(modgen, root)
            if result.returncode == 0:
                raise AssertionError(f"modgen accepted invalid trainer: {label}")

    print("modgen smoke OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
