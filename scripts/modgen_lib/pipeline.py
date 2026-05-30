"""High-level mod registry generation pipeline."""

from __future__ import annotations

from pathlib import Path

from .domains import (
    calc_catalog_hash,
    collect_badge_effects,
    collect_battle_moves,
    collect_battle_sprites,
    collect_catalog_entries,
    collect_encounters,
    collect_engines,
    collect_events,
    collect_fishing_actions,
    collect_flags,
    collect_followers,
    collect_items,
    collect_language_texts,
    collect_level_caps,
    collect_maps,
    collect_mod_sources,
    collect_npcs,
    collect_overworld_sprites,
    collect_pokeballs,
    collect_pokemon_data,
    collect_rewards,
    collect_shops,
    collect_sprite_assets,
    collect_time_segments,
    collect_trainers,
    collect_weather,
)
from .emit_c import write_header, write_make_fragment, write_source
from .manifest import load_mods


def generate(root: Path, out_header: str, out_source: str, out_make: str) -> None:
    mods = load_mods(root)
    flags = collect_flags(mods)
    events = collect_events(mods)
    weather = collect_weather(mods)
    time_segments = collect_time_segments(mods)
    badge_effects = collect_badge_effects(mods)
    fishing_actions = collect_fishing_actions(mods)
    encounters = collect_encounters(mods)
    shops = collect_shops(mods)
    items = collect_items(mods)
    rewards = collect_rewards(mods)
    pokemon_data = collect_pokemon_data(mods)
    battle_moves = collect_battle_moves(mods)
    trainers = collect_trainers(mods)
    level_caps = collect_level_caps(mods)
    sprite_assets = collect_sprite_assets(mods)
    overworld_sprites = collect_overworld_sprites(mods)
    battle_sprites = collect_battle_sprites(mods)
    followers = collect_followers(mods)
    language_texts = collect_language_texts(mods)
    pokeballs = collect_pokeballs(mods)
    engines = collect_engines(mods)
    npcs = collect_npcs(mods)
    maps = collect_maps(mods)
    catalog_entries = collect_catalog_entries(
        weather,
        sprite_assets,
        language_texts,
        engines,
        npcs,
        badge_effects,
        fishing_actions,
        encounters,
        shops,
        items,
        rewards,
        pokemon_data,
        battle_moves,
        trainers,
        level_caps,
    )
    catalog_hash = calc_catalog_hash(catalog_entries)
    sources = collect_mod_sources(mods)
    map_scripts = [map_def["script_path"] for map_def in maps if map_def["script_path"]]

    write_header(root / out_header)
    write_source(
        root / out_source,
        mods,
        flags,
        events,
        weather,
        time_segments,
        badge_effects,
        fishing_actions,
        encounters,
        shops,
        items,
        rewards,
        pokemon_data,
        battle_moves,
        trainers,
        level_caps,
        sprite_assets,
        overworld_sprites,
        battle_sprites,
        followers,
        language_texts,
        pokeballs,
        engines,
        npcs,
        maps,
        catalog_entries,
        catalog_hash,
    )
    write_make_fragment(root / out_make, sources, map_scripts)
