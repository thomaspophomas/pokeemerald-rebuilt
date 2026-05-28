"""Sprite and follower domain collectors."""

from __future__ import annotations

from typing import Any, Dict, List, Tuple

from ..common import *
from ..errors import ModgenError
from .keys import key_for

def collect_sprite_assets(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    assets = []
    seen = set()
    for mod in mods:
        for path in iter_json_files(mod["root"] / "sprites" / "assets"):
            for item in read_domain_list(path, "assets"):
                key = key_for(mod["id"], item, path.stem)
                if key in seen:
                    raise ModgenError(f"Duplicate sprite asset key {key!r}")
                seen.add(key)
                assets.append(
                    {
                        "key": key,
                        "sheet": c_symbol(item.get("sheetSymbol", item.get("sheet"))),
                        "compressed_sheet": c_symbol(item.get("compressedSheetSymbol", item.get("compressed_sheet"))),
                        "palette": c_symbol(item.get("paletteSymbol", item.get("palette"))),
                        "compressed_palette": c_symbol(item.get("compressedPaletteSymbol", item.get("compressed_palette"))),
                        "template": c_symbol(item.get("templateSymbol", item.get("template"))),
                        "tile_tag": c_int_or_token(item.get("tileTag", item.get("tile_tag")), "TAG_NONE"),
                        "palette_tag": c_int_or_token(item.get("paletteTag", item.get("palette_tag")), "TAG_NONE"),
                    }
                )
    return assets


def collect_overworld_sprites(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    sprites = []
    seen = set()
    for mod in mods:
        roots = [
            (mod["root"] / "sprites" / "overworld", "sprites"),
            (mod["root"] / "outfits", "outfits"),
        ]
        for root, list_key in roots:
            for path in iter_json_files(root):
                for item in read_domain_list(path, list_key):
                    key = key_for(mod["id"], item, path.stem)
                    if key in seen:
                        raise ModgenError(f"Duplicate overworld sprite key {key!r}")
                    seen.add(key)
                    asset_key = item.get("assetKey", item.get("asset"))
                    if asset_key is not None and ":" not in str(asset_key):
                        asset_key = f"{mod['id']}:{asset_key}"
                    sprites.append(
                        {
                            "key": key,
                            "asset_key": asset_key,
                            "graphics": c_int_or_token(item.get("graphicsId", item.get("graphics_id")), "0"),
                            "revision": c_int_or_token(item.get("graphicsRevision", item.get("graphics_revision")), "1"),
                        }
                    )
    return sprites


def collect_battle_sprites(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    sprites = []
    seen = set()
    for mod in mods:
        for path in iter_json_files(mod["root"] / "sprites" / "battle"):
            for item in read_domain_list(path, "sprites"):
                key = key_for(mod["id"], item, path.stem)
                if key in seen:
                    raise ModgenError(f"Duplicate battle sprite key {key!r}")
                seen.add(key)
                asset_key = item.get("assetKey", item.get("asset"))
                if asset_key is not None and ":" not in str(asset_key):
                    asset_key = f"{mod['id']}:{asset_key}"
                sprites.append(
                    {
                        "key": key,
                        "asset_key": asset_key,
                        "species": c_int_or_token(item.get("species", item.get("trainerId")), "0"),
                        "form": c_int_or_token(item.get("form"), "0"),
                        "side": c_int_or_token(item.get("side"), "0"),
                        "flags": c_int_or_token(item.get("flags"), "0"),
                    }
                )
    return sprites


def collect_followers(mods: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    followers = []
    seen = set()
    for mod in mods:
        for path in iter_json_files(mod["root"] / "followers"):
            for item in read_domain_list(path, "followers"):
                key = key_for(mod["id"], item, path.stem)
                if key in seen:
                    raise ModgenError(f"Duplicate follower sprite key {key!r}")
                seen.add(key)
                followers.append(
                    {
                        "key": key,
                        "species": c_int_or_token(item.get("species"), "0"),
                        "form": c_int_or_token(item.get("form"), "0"),
                        "shiny": c_bool(item.get("shiny", False)),
                        "graphics": c_int_or_token(item.get("graphicsId", item.get("graphics_id")), "0"),
                    }
                )
    return followers

