"""C and Makefile emission for generated mod registries."""

from __future__ import annotations

from pathlib import Path
from typing import Any, Dict, List

from .common import LEVEL_CAP_MAX_STAGES, c_bool, c_string, c_symbol, c_u8_string

def c_symbol_ptr(symbol: str) -> str:
    if symbol == "NULL":
        return "NULL"
    return f"&{symbol}"

def write_header(path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(
        """#ifndef GUARD_GENERATED_MOD_REGISTRY_H
#define GUARD_GENERATED_MOD_REGISTRY_H

#include "global.h"
#include "mod/badge.h"
#include "mod/battle_data.h"
#include "mod/catalog.h"
#include "mod/battle_sprite.h"
#include "mod/encounter.h"
#include "mod/engine.h"
#include "mod/event.h"
#include "mod/fishing.h"
#include "mod/flags.h"
#include "mod/item.h"
#include "mod/language.h"
#include "mod/level_cap.h"
#include "mod/map.h"
#include "mod/npc.h"
#include "mod/overworld_sprite.h"
#include "mod/pokemon_data.h"
#include "mod/pokeball.h"
#include "mod/reward.h"
#include "mod/shop.h"
#include "mod/sprite_asset.h"
#include "mod/time.h"
#include "mod/trainer.h"
#include "mod/types.h"
#include "mod/weather.h"

extern const struct ModManifest gModManifests[];
extern const u16 gModManifestCount;
extern const struct ModFlagDefinition gModFlagDefinitions[];
extern const u16 gModFlagDefinitionCount;
extern const struct ModEventSubscription gModEventSubscriptions[];
extern const u16 gModEventSubscriptionCount;
extern const struct ModWeatherProvider gModWeatherProviders[];
extern const u16 gModWeatherProviderCount;
extern const struct ModTimeSegmentDefinition gModTimeSegments[];
extern const u16 gModTimeSegmentCount;
extern const struct ModSpriteAssetDefinition gModSpriteAssets[];
extern const u16 gModSpriteAssetCount;
extern const struct ModOverworldSpriteDefinition gModOverworldSprites[];
extern const u16 gModOverworldSpriteCount;
extern const struct ModBattleSpriteDefinition gModBattleSprites[];
extern const u16 gModBattleSpriteCount;
extern const struct ModFollowerSpriteDefinition gModFollowerSprites[];
extern const u16 gModFollowerSpriteCount;
extern const struct ModLanguageText gModLanguageTexts[];
extern const u16 gModLanguageTextCount;
extern const struct ModPokeBallDefinition gModPokeBallDefinitions[];
extern const u16 gModPokeBallDefinitionCount;
extern const struct ModBadgeEffectDefinition gModBadgeEffects[];
extern const u16 gModBadgeEffectCount;
extern const struct EngineRuleset gModEngineRulesets[];
extern const u16 gModEngineRulesetCount;
extern const struct ModNpcDefinition gModNpcDefinitions[];
extern const u16 gModNpcDefinitionCount;
extern const struct ModMapDefinition gModMapDefinitions[];
extern const u16 gModMapDefinitionCount;
extern const struct FishingActionDefinition gModFishingActions[];
extern const u16 gModFishingActionCount;
extern const struct ModEncounterDefinition gModEncounterDefinitions[];
extern const u16 gModEncounterDefinitionCount;
extern const struct ModShopDefinition gModShopDefinitions[];
extern const u16 gModShopDefinitionCount;
extern const struct ModItemDefinition gModItemDefinitions[];
extern const u16 gModItemDefinitionCount;
extern const struct ModRewardDefinition gModRewardDefinitions[];
extern const u16 gModRewardDefinitionCount;
extern const struct ModPokemonDataDefinition gModPokemonDataDefinitions[];
extern const u16 gModPokemonDataDefinitionCount;
extern const struct ModBattleMoveDefinition gModBattleMoveDefinitions[];
extern const u16 gModBattleMoveDefinitionCount;
extern const struct ModTrainerDefinition gModTrainerDefinitions[];
extern const u16 gModTrainerDefinitionCount;
extern const struct ModLevelCapDefinition gModLevelCaps[];
extern const u16 gModLevelCapCount;
extern const struct ModCatalogEntry gModCatalogEntries[];
extern const u16 gModCatalogEntryCount;
extern const u32 gModCatalogHash;

#endif // GUARD_GENERATED_MOD_REGISTRY_H
""",
        encoding="utf-8",
    )


def write_source(path: Path, mods: List[Dict[str, Any]], flags: List[Dict[str, Any]], events: List[Dict[str, Any]], weather: List[Dict[str, Any]], time_segments: List[Dict[str, Any]], badge_effects: List[Dict[str, Any]], fishing_actions: List[Dict[str, Any]], encounters: List[Dict[str, Any]], shops: List[Dict[str, Any]], items: List[Dict[str, Any]], rewards: List[Dict[str, Any]], pokemon_data: List[Dict[str, Any]], battle_moves: List[Dict[str, Any]], trainers: List[Dict[str, Any]], level_caps: List[Dict[str, Any]], sprite_assets: List[Dict[str, Any]], overworld_sprites: List[Dict[str, Any]], battle_sprites: List[Dict[str, Any]], followers: List[Dict[str, Any]], language_texts: List[Dict[str, Any]], pokeballs: List[Dict[str, Any]], engines: List[Dict[str, Any]], npcs: List[Dict[str, Any]], maps: List[Dict[str, Any]], catalog_entries: List[Dict[str, int]], catalog_hash: int) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    event_handlers = sorted({event["handler"] for event in events if event["handler"] != "NULL"})
    weather_handlers = sorted({provider["handler"] for provider in weather if provider["handler"] != "NULL"})
    capture_hooks = sorted({engine["capture"] for engine in engines if engine["capture"] != "NULL"})
    battle_weather_hooks = sorted({engine["battle_weather"] for engine in engines if engine["battle_weather"] != "NULL"})
    npc_scripts = sorted({npc["script"] for npc in npcs if npc["script"] != "NULL"})
    sheet_symbols = sorted({asset["sheet"] for asset in sprite_assets if asset["sheet"] != "NULL"})
    compressed_sheet_symbols = sorted({asset["compressed_sheet"] for asset in sprite_assets if asset["compressed_sheet"] != "NULL"})
    palette_symbols = sorted({asset["palette"] for asset in sprite_assets if asset["palette"] != "NULL"})
    compressed_palette_symbols = sorted({asset["compressed_palette"] for asset in sprite_assets if asset["compressed_palette"] != "NULL"})
    template_symbols = sorted({asset["template"] for asset in sprite_assets if asset["template"] != "NULL"})
    follower_graphics_infos = sorted({follower["graphics_info"] for follower in followers if follower["graphics_info"] != "NULL"})
    pokeball_modifier_hooks = sorted({ball["modifier_hook"] for ball in pokeballs if ball["modifier_hook"] != "NULL"})
    pokeball_commit_hooks = sorted({ball["commit_hook"] for ball in pokeballs if ball["commit_hook"] != "NULL"})
    pokeball_scripts = sorted({ball["battle_script"] for ball in pokeballs if ball["battle_script"] != "NULL"})
    fishing_hooks = sorted({action["hook"] for action in fishing_actions if action["hook"] != "NULL" and action["hook"] != "FishingApi_RequestConfiguredAction"})
    encounter_hooks = sorted({encounter["hook"] for encounter in encounters if encounter["hook"] != "NULL"})
    item_hooks = sorted({item["field_hook"] for item in items if item["field_hook"] != "NULL"} | {item["battle_hook"] for item in items if item["battle_hook"] != "NULL"})
    reward_hooks = sorted({reward["hook"] for reward in rewards if reward["hook"] != "NULL"})

    lines = [
        "/* Auto-generated by scripts/modgen.py. */",
        '#include "global.h"',
        '#include "constants/decorations.h"',
        '#include "constants/flags.h"',
        '#include "constants/items.h"',
        '#include "constants/maps.h"',
        '#include "constants/moves.h"',
        '#include "constants/pokemon.h"',
        '#include "constants/species.h"',
        '#include "constants/trainers.h"',
        '#include "constants/event_object_movement.h"',
        '#include "constants/event_objects.h"',
        '#include "generated/mod_registry.h"',
        '#include "pokeball.h"',
        "",
    ]

    for handler in event_handlers:
        lines.append(f"extern s8 {handler}(const struct ModEvent *event);")
    for handler in weather_handlers:
        lines.append(f"extern bool8 {handler}(struct ModWeatherDisplay *display);")
    for hook in capture_hooks:
        lines.append(f"extern u8 {hook}(const struct EngineRuleset *ruleset, u16 ball_item_id);")
    for hook in battle_weather_hooks:
        lines.append(f"extern u32 {hook}(const struct EngineRuleset *ruleset, u16 weather_layers);")
    for script in npc_scripts:
        lines.append(f"extern const u8 {script}[];")
    for symbol in sheet_symbols:
        lines.append(f"extern const struct SpriteSheet {symbol};")
    for symbol in compressed_sheet_symbols:
        lines.append(f"extern const struct CompressedSpriteSheet {symbol};")
    for symbol in palette_symbols:
        lines.append(f"extern const struct SpritePalette {symbol};")
    for symbol in compressed_palette_symbols:
        lines.append(f"extern const struct CompressedSpritePalette {symbol};")
    for symbol in template_symbols:
        lines.append(f"extern const struct SpriteTemplate {symbol};")
    for symbol in follower_graphics_infos:
        lines.append(f"extern const struct ObjectEventGraphicsInfo {symbol};")
    for hook in pokeball_modifier_hooks:
        lines.append(f"extern u8 {hook}(const struct PokeBallCatchContext *context);")
    for hook in pokeball_commit_hooks:
        lines.append(f"extern void {hook}(const struct PokeBallCatchContext *context, const struct PokeBallThrowResult *result);")
    for script in pokeball_scripts:
        lines.append(f"extern const u8 {script}[];")
    for hook in fishing_hooks:
        lines.append(f"extern u8 {hook}(const struct FishingActionDefinition *definition, struct FishingContext *context, struct FishingActionRequest *request);")
    for hook in encounter_hooks:
        lines.append(f"extern u8 {hook}(const struct ModEncounterDefinition *definition, struct ModEncounterContext *context);")
    for hook in item_hooks:
        lines.append(f"extern void {hook}(u8 task_id);")
    for hook in reward_hooks:
        lines.append(f"extern u8 {hook}(const struct ModRewardDefinition *definition, struct ModRewardContext *context);")
    if event_handlers or weather_handlers or capture_hooks or battle_weather_hooks or npc_scripts or sheet_symbols or compressed_sheet_symbols or palette_symbols or compressed_palette_symbols or template_symbols or follower_graphics_infos or pokeball_modifier_hooks or pokeball_commit_hooks or pokeball_scripts or fishing_hooks or encounter_hooks or item_hooks or reward_hooks:
        lines.append("")

    lines.append("const struct ModManifest gModManifests[] =")
    lines.append("{")
    if mods:
        for mod in mods:
            lines.append(f"    {{ .mod_id = {c_string(mod['id'])}, .name = {c_string(mod['name'])}, .version = {c_string(mod['version'])}, .priority = {mod['priority']}, .feature_flags = {mod['features']}, .state_version = {mod['state_version']}, .state_bytes = {mod['state_bytes']}, .state_offset = {mod['state_offset']} }},")
    else:
        lines.append("    { .mod_id = NULL, .name = NULL, .version = NULL, .priority = 0, .feature_flags = 0, .state_version = 0, .state_bytes = 0, .state_offset = 0 },")
    lines.append("};")
    lines.append(f"const u16 gModManifestCount = {len(mods)};")
    lines.append("")

    lines.append("const struct ModFlagDefinition gModFlagDefinitions[] =")
    lines.append("{")
    if flags:
        for flag in flags:
            lines.append(f"    {{ .key = {c_string(flag['key'])}, .mod_flag_id = {flag['id']}, .initial_value = {c_bool(flag['initial'])} }},")
    else:
        lines.append("    { .key = NULL, .mod_flag_id = 0, .initial_value = FALSE },")
    lines.append("};")
    lines.append(f"const u16 gModFlagDefinitionCount = {len(flags)};")
    lines.append("")

    lines.append("const struct ModEventSubscription gModEventSubscriptions[] =")
    lines.append("{")
    if events:
        for event in events:
            lines.append(f"    {{ {event['type']}, {event['priority']}, {event['handler']}, {c_string(event['source'])} }},")
    else:
        lines.append("    { 0, 0, NULL, NULL },")
    lines.append("};")
    lines.append(f"const u16 gModEventSubscriptionCount = {len(events)};")
    lines.append("")

    lines.append("const struct ModWeatherProvider gModWeatherProviders[] =")
    lines.append("{")
    if weather:
        for provider in weather:
            lines.append(f"    {{ .weather_provider_id = {c_string(provider['id'])}, .priority = {provider['priority']}, .resolve = {provider['handler']} }},")
    else:
        lines.append("    { .weather_provider_id = NULL, .priority = 0, .resolve = NULL },")
    lines.append("};")
    lines.append(f"const u16 gModWeatherProviderCount = {len(weather)};")
    lines.append("")

    lines.append("const struct ModTimeSegmentDefinition gModTimeSegments[] =")
    lines.append("{")
    if time_segments:
        for segment in time_segments:
            lines.append(f"    {{ {c_string(segment['key'])}, {segment['start']}, {segment['end']}, {segment['segment']} }},")
    else:
        lines.append("    { NULL, 0, 0, 0 },")
    lines.append("};")
    lines.append(f"const u16 gModTimeSegmentCount = {len(time_segments)};")
    lines.append("")

    lines.append("const struct ModSpriteAssetDefinition gModSpriteAssets[] =")
    lines.append("{")
    if sprite_assets:
        for asset in sprite_assets:
            lines.append(f"    {{ {c_string(asset['key'])}, {c_symbol_ptr(asset['sheet'])}, {c_symbol_ptr(asset['compressed_sheet'])}, {c_symbol_ptr(asset['palette'])}, {c_symbol_ptr(asset['compressed_palette'])}, {c_symbol_ptr(asset['template'])}, {asset['tile_tag']}, {asset['palette_tag']} }},")
    else:
        lines.append("    { NULL, NULL, NULL, NULL, NULL, NULL, TAG_NONE, TAG_NONE },")
    lines.append("};")
    lines.append(f"const u16 gModSpriteAssetCount = {len(sprite_assets)};")
    lines.append("")

    lines.append("const struct ModOverworldSpriteDefinition gModOverworldSprites[] =")
    lines.append("{")
    if overworld_sprites:
        for sprite in overworld_sprites:
            lines.append(f"    {{ {c_string(sprite['key'])}, {c_string(sprite['asset_key'])}, {sprite['graphics']}, {sprite['revision']} }},")
    else:
        lines.append("    { NULL, NULL, 0, 0 },")
    lines.append("};")
    lines.append(f"const u16 gModOverworldSpriteCount = {len(overworld_sprites)};")
    lines.append("")

    lines.append("const struct ModBattleSpriteDefinition gModBattleSprites[] =")
    lines.append("{")
    if battle_sprites:
        for sprite in battle_sprites:
            lines.append(f"    {{ {c_string(sprite['key'])}, {c_string(sprite['asset_key'])}, {sprite['species']}, {sprite['form']}, {sprite['side']}, {sprite['flags']} }},")
    else:
        lines.append("    { NULL, NULL, 0, 0, 0, 0 },")
    lines.append("};")
    lines.append(f"const u16 gModBattleSpriteCount = {len(battle_sprites)};")
    lines.append("")

    lines.append("const struct ModFollowerSpriteDefinition gModFollowerSprites[] =")
    lines.append("{")
    if followers:
        for follower in followers:
            lines.append(f"    {{ {c_string(follower['key'])}, {follower['species']}, {follower['form']}, {follower['shiny']}, {follower['graphics']}, {c_string(follower['asset_key'])}, {follower['revision']}, {c_symbol_ptr(follower['graphics_info'])} }},")
    else:
        lines.append("    { NULL, 0, 0, FALSE, 0, NULL, 0, NULL },")
    lines.append("};")
    lines.append(f"const u16 gModFollowerSpriteCount = {len(followers)};")
    lines.append("")

    lines.append("const struct ModLanguageText gModLanguageTexts[] =")
    lines.append("{")
    if language_texts:
        for text in language_texts:
            lines.append(f"    {{ {c_string(text['language'])}, {c_string(text['key'])}, {c_u8_string(text['text'])} }},")
    else:
        lines.append("    { NULL, NULL, NULL },")
    lines.append("};")
    lines.append(f"const u16 gModLanguageTextCount = {len(language_texts)};")
    lines.append("")

    lines.append("const struct ModPokeBallDefinition gModPokeBallDefinitions[] =")
    lines.append("{")
    if pokeballs:
        for ball in pokeballs:
            lines.append(f"    {{ {c_string(ball['key'])}, {ball['item']}, {ball['ball']}, {ball['modifier']}, {ball['flags']}, {ball['modifier_hook']}, {ball['battle_script']}, {ball['commit_hook']} }},")
    else:
        lines.append("    { NULL, 0, BALL_POKE, 10, 0, NULL, NULL, NULL },")
    lines.append("};")
    lines.append(f"const u16 gModPokeBallDefinitionCount = {len(pokeballs)};")
    lines.append("")

    lines.append("const struct ModBadgeEffectDefinition gModBadgeEffects[] =")
    lines.append("{")
    if badge_effects:
        for effect in badge_effects:
            lines.append(f"    {{ {c_string(effect['key'])}, {effect['badge']}, {effect['effect']}, {effect['target']}, {effect['percent']}, {effect['max_level']}, {effect['flags']} }},")
    else:
        lines.append("    { NULL, 0, 0, 0, 0, 0, 0 },")
    lines.append("};")
    lines.append(f"const u16 gModBadgeEffectCount = {len(badge_effects)};")
    lines.append("")

    lines.append("const struct EngineRuleset gModEngineRulesets[] =")
    lines.append("{")
    for engine in engines:
        lines.append(f"    {{ .ruleset_id = {c_string(engine['id'])}, .name = {c_string(engine['name'])}, .version = {engine['version']}, .flags = {engine['flags']}, .get_capture_modifier = {engine['capture']}, .get_battle_weather_mask = {engine['battle_weather']} }},")
    lines.append("};")
    lines.append(f"const u16 gModEngineRulesetCount = {len(engines)};")
    lines.append("")

    lines.append("const struct ModNpcDefinition gModNpcDefinitions[] =")
    lines.append("{")
    if npcs:
        for npc in npcs:
            lines.append(f"    {{ .key = {c_string(npc['key'])}, .npc_definition_id = {npc['id']}, .graphics_id = {npc['graphics']}, .movement_type = {npc['movement']}, .local_id = {npc['local']}, .elevation = {npc['elevation']}, .flag_id = {npc['flag']}, .script = {npc['script']} }},")
    else:
        lines.append("    { .key = NULL, .npc_definition_id = 0, .graphics_id = 0, .movement_type = 0, .local_id = 0, .elevation = 0, .flag_id = 0, .script = NULL },")
    lines.append("};")
    lines.append(f"const u16 gModNpcDefinitionCount = {len(npcs)};")
    lines.append("")

    lines.append("const struct ModMapDefinition gModMapDefinitions[] =")
    lines.append("{")
    if maps:
        for map_def in maps:
            lines.append(f"    {{ .key = {c_string(map_def['key'])}, .map_id = {map_def['id']}, .map_group = {map_def['group']}, .map_num = {map_def['num']}, .json_path = {c_string(map_def['path'])}, .script_path = {c_string(map_def['script_path'])} }},")
    else:
        lines.append("    { .key = NULL, .map_id = 0, .map_group = 0, .map_num = 0, .json_path = NULL, .script_path = NULL },")
    lines.append("};")
    lines.append(f"const u16 gModMapDefinitionCount = {len(maps)};")
    lines.append("")

    lines.append("const struct FishingActionDefinition gModFishingActions[] =")
    lines.append("{")
    if fishing_actions:
        for action in fishing_actions:
            params = ", ".join(str(param) for param in action["params"])
            lines.append(
                f"    {{ {c_string(action['key'])}, {c_string(action['hook_key'])}, {action['rod_mask']}, {action['phase_mask']}, {action['priority']}, {action['flags']}, {action['hook']}, {c_string(action['prompt_key'])}, {action['button_mask']}, {action['timeout']}, {action['success_outcome']}, {action['failure_outcome']}, {{ {params} }} }},"
            )
    else:
        lines.append("    { NULL, NULL, 0, 0, 0, 0, NULL, NULL, 0, 0, 0, 0, { 0, 0, 0, 0 } },")
    lines.append("};")
    lines.append(f"const u16 gModFishingActionCount = {len(fishing_actions)};")
    lines.append("")

    lines.append("const struct ModEncounterDefinition gModEncounterDefinitions[] =")
    lines.append("{")
    if encounters:
        for encounter in encounters:
            lines.append("    {")
            lines.append(f"        {c_string(encounter['key'])}, {c_string(encounter['hook_key'])}, {encounter['group']}, {encounter['num']}, {encounter['area']}, {encounter['rod_mask']}, {encounter['encounter_rate']}, {encounter['slot_count']}, {encounter['priority']}, {encounter['flags']}, {encounter['hook']},")
            lines.append("        {")
            for slot in encounter["slots"]:
                lines.append(f"            {{ {slot['min']}, {slot['max']}, {slot['species']}, {slot['weight']}, {slot['rod_mask']}, {slot['flags']} }},")
            lines.append("        }")
            lines.append("    },")
    else:
        lines.append("    { NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, NULL, { { 0, 0, SPECIES_NONE, 0, 0, 0 } } },")
    lines.append("};")
    lines.append(f"const u16 gModEncounterDefinitionCount = {len(encounters)};")
    lines.append("")

    lines.append("const struct ModShopDefinition gModShopDefinitions[] =")
    lines.append("{")
    if shops:
        for shop in shops:
            shop_items = ", ".join(str(item) for item in shop["items"])
            lines.append(f"    {{ {c_string(shop['key'])}, {shop['group']}, {shop['num']}, {shop['mart_type']}, {shop['item_count']}, {shop['priority']}, {shop['flags']}, {{ {shop_items} }} }},")
    else:
        lines.append("    { NULL, 0, 0, 0, 0, 0, 0, { ITEM_NONE } },")
    lines.append("};")
    lines.append(f"const u16 gModShopDefinitionCount = {len(shops)};")
    lines.append("")

    lines.append("const struct ModItemDefinition gModItemDefinitions[] =")
    lines.append("{")
    if items:
        for item in items:
            lines.append(
                f"    {{ {c_string(item['key'])}, {c_string(item['name_key'])}, {c_string(item['description_key'])}, {c_string(item['field_hook_key'])}, {c_string(item['battle_hook_key'])}, {item['item']}, {item['price']}, {item['flags']}, {item['hold_effect']}, {item['hold_effect_param']}, {item['importance']}, {item['pocket']}, {item['type']}, {item['battle_usage']}, {item['secondary_id']}, {item['priority']}, {item['field_hook']}, {item['battle_hook']} }},"
            )
    else:
        lines.append("    { NULL, NULL, NULL, NULL, NULL, ITEM_NONE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL },")
    lines.append("};")
    lines.append(f"const u16 gModItemDefinitionCount = {len(items)};")
    lines.append("")

    lines.append("const struct ModRewardDefinition gModRewardDefinitions[] =")
    lines.append("{")
    if rewards:
        for reward in rewards:
            lines.append(f"    {{ {c_string(reward['key'])}, {c_string(reward['hook_key'])}, {reward['source']}, {reward['min_level']}, {reward['max_level']}, {reward['priority']}, {reward['flags']}, {reward['item']}, {reward['quantity']}, {reward['hook']} }},")
    else:
        lines.append("    { NULL, NULL, 0, 0, 0, 0, 0, ITEM_NONE, 0, NULL },")
    lines.append("};")
    lines.append(f"const u16 gModRewardDefinitionCount = {len(rewards)};")
    lines.append("")

    lines.append("const struct ModPokemonDataDefinition gModPokemonDataDefinitions[] =")
    lines.append("{")
    if pokemon_data:
        for data in pokemon_data:
            lines.append("    {")
            lines.append(f"        {c_string(data['key'])}, {data['species']}, {data['priority']}, {data['flags']},")
            lines.append(
                f"        {{ {', '.join(data['base'])}, {{ {', '.join(data['types'])} }}, {data['catch_rate']}, {data['exp_yield']}, {{ {', '.join(data['ev_yields'])} }}, {data['item_common']}, {data['item_rare']}, {data['gender']}, {data['egg_cycles']}, {data['friendship']}, {data['growth_rate']}, {{ {', '.join(data['egg_groups'])} }}, {{ {', '.join(data['abilities'])} }}, {data['safari_flee']}, {data['body_color']}, {data['no_flip']} }},"
            )
            lines.append(f"        {data['level_move_count']},")
            lines.append("        {")
            for move in data["level_moves"]:
                lines.append(f"            {{ {move['level']}, {move['move']} }},")
            lines.append("        },")
            lines.append(f"        {data['evolution_count']},")
            lines.append("        {")
            for evo in data["evolutions"]:
                lines.append(f"            {{ {evo['method']}, {evo['param']}, {evo['target']} }},")
            lines.append("        }")
            lines.append("    },")
    else:
        lines.append("    { NULL, SPECIES_NONE, 0, 0, { 0 }, 0, { { 0, MOVE_NONE } }, 0, { { 0, 0, SPECIES_NONE } } },")
    lines.append("};")
    lines.append(f"const u16 gModPokemonDataDefinitionCount = {len(pokemon_data)};")
    lines.append("")

    lines.append("const struct ModBattleMoveDefinition gModBattleMoveDefinitions[] =")
    lines.append("{")
    if battle_moves:
        for move in battle_moves:
            lines.append(f"    {{ {c_string(move['key'])}, {move['move']}, {move['priority']}, {move['override_flags']}, {{ {move['effect']}, {move['power']}, {move['type']}, {move['accuracy']}, {move['pp']}, {move['secondary']}, {move['target']}, {move['move_priority']}, {move['move_flags']} }} }},")
    else:
        lines.append("    { NULL, MOVE_NONE, 0, 0, { 0 } },")
    lines.append("};")
    lines.append(f"const u16 gModBattleMoveDefinitionCount = {len(battle_moves)};")
    lines.append("")

    lines.append("const struct ModTrainerDefinition gModTrainerDefinitions[] =")
    lines.append("{")
    if trainers:
        for trainer in trainers:
            lines.append("    {")
            lines.append(f"        {c_string(trainer['key'])}, {trainer['trainer']}, {trainer['priority']}, {trainer['flags']}, {trainer['class']}, {trainer['music_gender']}, {trainer['pic']}, {{ {', '.join(trainer['items'])} }}, {trainer['double']}, {trainer['ai']}, {trainer['party_count']},")
            lines.append("        {")
            for mon in trainer["party"]:
                lines.append(f"            {{ {mon['iv']}, {mon['level']}, {mon['species']}, {mon['item']}, {{ {', '.join(mon['moves'])} }} }},")
            lines.append("        }")
            lines.append("    },")
    else:
        lines.append("    { NULL, 0, 0, 0, 0, 0, 0, { ITEM_NONE }, FALSE, 0, 0, { { 0, 0, SPECIES_NONE, ITEM_NONE, { MOVE_NONE } } } },")
    lines.append("};")
    lines.append(f"const u16 gModTrainerDefinitionCount = {len(trainers)};")
    lines.append("")

    lines.append("const struct ModLevelCapDefinition gModLevelCaps[] =")
    lines.append("{")
    if level_caps:
        for cap in level_caps:
            stages = [f"{{ {stage['flag']}, {stage['level']}, 0 }}" for stage in cap["stages"]]
            stages.extend(["{ MOD_LEVEL_CAP_FLAG_ALWAYS, 0, 0 }"] * (LEVEL_CAP_MAX_STAGES - len(stages)))
            lines.append(f"    {{ {c_string(cap['key'])}, {cap['mode']}, {cap['stage_count']}, {cap['rare_candy']}, 0, {cap['priority']}, {cap['flags']}, {{ {', '.join(stages)} }} }},")
    else:
        lines.append("    { NULL, 0, 0, 0, 0, 0, 0, { { MOD_LEVEL_CAP_FLAG_ALWAYS, 0, 0 } } },")
    lines.append("};")
    lines.append(f"const u16 gModLevelCapCount = {len(level_caps)};")
    lines.append("")

    lines.append("const struct ModCatalogEntry gModCatalogEntries[] =")
    lines.append("{")
    if catalog_entries:
        for entry in catalog_entries:
            lines.append(f"    {{ {entry['type']}, {entry.get('flags', 0)}, 0, 0x{entry['key_hash']:08X}, 0x{entry['content_hash']:08X} }},")
    else:
        lines.append("    { 0, 0, 0, 0, 0 },")
    lines.append("};")
    lines.append(f"const u16 gModCatalogEntryCount = {len(catalog_entries)};")
    lines.append(f"const u32 gModCatalogHash = 0x{catalog_hash:08X};")
    lines.append("")

    path.write_text("\n".join(lines), encoding="utf-8")


def write_make_fragment(path: Path, sources: List[str], map_scripts: List[str] | None = None) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    lines = ["# Auto-generated by scripts/modgen.py.", "MOD_C_SRCS :="]
    for source in sources:
        lines.append(f"MOD_C_SRCS += {source}")
    lines.append("MOD_MAP_SCRIPT_INCS :=")
    for script in map_scripts or []:
        lines.append(f"MOD_MAP_SCRIPT_INCS += {script}")
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")
