"""Domain collectors for build-time mod data."""

from .badges import collect_badge_effects
from .battle import collect_battle_moves
from .catalog import add_catalog_entry, calc_catalog_hash, collect_catalog_entries
from .encounters import collect_encounters
from .engines import collect_engines
from .events import collect_events
from .fishing import collect_fishing_actions
from .flags import collect_flags
from .items import collect_items
from .keys import key_for
from .language import collect_language_texts
from .maps import collect_maps
from .npcs import collect_npcs
from .pokeballs import collect_pokeballs
from .pokemon import collect_pokemon_data
from .rewards import collect_rewards, normalize_reward_source
from .shops import collect_shops
from .sources import collect_mod_sources
from .sprites import collect_battle_sprites, collect_followers, collect_overworld_sprites, collect_sprite_assets
from .time import collect_time_segments
from .trainers import collect_trainers
from .weather import collect_weather

__all__ = [
    "add_catalog_entry",
    "calc_catalog_hash",
    "collect_badge_effects",
    "collect_battle_moves",
    "collect_battle_sprites",
    "collect_catalog_entries",
    "collect_encounters",
    "collect_engines",
    "collect_events",
    "collect_fishing_actions",
    "collect_flags",
    "collect_followers",
    "collect_items",
    "collect_language_texts",
    "collect_maps",
    "collect_mod_sources",
    "collect_npcs",
    "collect_overworld_sprites",
    "collect_pokeballs",
    "collect_pokemon_data",
    "collect_rewards",
    "collect_shops",
    "collect_sprite_assets",
    "collect_time_segments",
    "collect_trainers",
    "collect_weather",
    "key_for",
    "normalize_reward_source",
]
