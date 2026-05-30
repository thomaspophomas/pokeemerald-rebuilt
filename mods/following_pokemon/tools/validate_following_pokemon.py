#!/usr/bin/env python3
"""Validate the source-only following_pokemon mod."""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from pathlib import Path
from typing import Any


MOD_ROOT = Path(__file__).resolve().parents[1]
REPO_ROOT = MOD_ROOT.parents[1]
ARTIFACT_SUFFIXES = {".4bpp", ".gbapal", ".gba", ".elf", ".sym", ".map", ".pyc"}
ARTIFACT_DIR_NAMES = {"__pycache__"}
PNG_SIGNATURE = b"\x89PNG\r\n\x1a\n"
SHINY_GENERATOR = MOD_ROOT / "tools" / "generate_shiny_fallbacks.py"
FORM_ALIAS_GENERATOR = MOD_ROOT / "tools" / "generate_form_aliases.py"


def load_json(path: Path) -> dict[str, Any]:
    with path.open("r", encoding="utf-8") as handle:
        return json.load(handle)


def parse_int(value: Any) -> int:
    if isinstance(value, int):
        return value
    if isinstance(value, str):
        return int(value, 0)
    raise TypeError(f"cannot parse integer from {value!r}")


def png_size(path: Path) -> tuple[int, int]:
    data = path.read_bytes()[:24]
    if len(data) < 24 or data[:8] != PNG_SIGNATURE or data[12:16] != b"IHDR":
        raise ValueError(f"{path} is not a valid PNG")
    return int.from_bytes(data[16:20], "big"), int.from_bytes(data[20:24], "big")


def source_path_for_incbin(path_text: str, replacement_suffix: str) -> Path:
    source = path_text
    if source.endswith(".4bpp"):
        source = source[:-5] + replacement_suffix
    elif source.endswith(".gbapal"):
        source = source[:-7] + replacement_suffix
    return REPO_ROOT / source


def collect_followers(mod_root: Path) -> list[tuple[Path, dict[str, Any]]]:
    followers: list[tuple[Path, dict[str, Any]]] = []
    for path in sorted((mod_root / "followers").glob("*.json")):
        data = load_json(path)
        for item in data.get("followers", []):
            followers.append((path, item))
    return followers


def collect_assets(mod_root: Path) -> list[tuple[Path, dict[str, Any]]]:
    assets: list[tuple[Path, dict[str, Any]]] = []
    for path in sorted((mod_root / "sprites" / "assets").glob("*.json")):
        data = load_json(path)
        for item in data.get("assets", []):
            assets.append((path, item))
    return assets


def parse_c_assets(path: Path) -> dict[str, Any]:
    text = path.read_text(encoding="utf-8")
    pic_defs = dict(
        re.findall(
            r"static const u32\s+(\w+)\[\]\s*=\s*INCBIN_U32\(\"([^\"]+)\"\);",
            text,
        )
    )
    pal_defs = dict(
        re.findall(
            r"static const u16\s+(\w+)\[\]\s*=\s*INCBIN_U16\(\"([^\"]+)\"\);",
            text,
        )
    )
    palette_defs = {
        symbol: (pal_var, parse_int(tag))
        for symbol, pal_var, tag in re.findall(
            r"const struct SpritePalette\s+(\w+)\s*=\s*\{\s*(\w+)\s*,\s*(0x[0-9A-Fa-f]+|\d+)\s*\};",
            text,
        )
    }
    graphics_infos = {
        symbol: body
        for symbol, body in re.findall(
            r"const struct ObjectEventGraphicsInfo\s+(\w+)\s*=\s*\{(.*?)\n\};",
            text,
            re.S,
        )
    }

    pic_tables: dict[str, list[tuple[str, int, int, int]]] = {}
    for table_name, body in re.findall(
        r"static const struct SpriteFrameImage\s+(\w+)\[\]\s*=\s*\{(.*?)\n\};",
        text,
        re.S,
    ):
        frames = [
            (pic, int(tile_w), int(tile_h), int(frame))
            for pic, tile_w, tile_h, frame in re.findall(
                r"overworld_frame\((\w+),\s*(\d+),\s*(\d+),\s*(\d+)\)",
                body,
            )
        ]
        pic_tables[table_name] = frames

    anim_cmds: dict[str, int] = {}
    for anim_name, body in re.findall(
        r"static const union AnimCmd\s+(\w+)\[\]\s*=\s*\{(.*?)\n\};",
        text,
        re.S,
    ):
        frames = [int(match) for match in re.findall(r"ANIMCMD_FRAME\((\d+),", body)]
        anim_cmds[anim_name] = max(frames) if frames else -1

    anim_tables: dict[str, int] = {}
    for table_name, body in re.findall(
        r"static const union AnimCmd \*const\s+(\w+)\[\]\s*=\s*\{(.*?)\n\};",
        text,
        re.S,
    ):
        entries = re.findall(r"\b(sFollowerAnim\w+)\b", body)
        anim_tables[table_name] = max((anim_cmds.get(entry, -1) for entry in entries), default=-1)

    return {
        "text": text,
        "pic_defs": pic_defs,
        "pal_defs": pal_defs,
        "palette_defs": palette_defs,
        "graphics_infos": graphics_infos,
        "pic_tables": pic_tables,
        "anim_tables": anim_tables,
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--mod-root", type=Path, default=MOD_ROOT)
    args = parser.parse_args()

    mod_root = args.mod_root.resolve()
    errors: list[str] = []

    requirements_path = mod_root / "engine_requirements.json"
    requirements = load_json(requirements_path)
    expected = requirements.get("expectedCounts", {})

    manifest = load_json(mod_root / "mod.json")
    if manifest.get("id") != "following_pokemon":
        errors.append("mod.json id must be following_pokemon")

    artifacts = [
        path
        for path in mod_root.rglob("*")
        if (path.is_file() and path.suffix in ARTIFACT_SUFFIXES)
        or (path.is_dir() and path.name in ARTIFACT_DIR_NAMES)
    ]
    if artifacts:
        errors.append("build artifacts found: " + ", ".join(str(path.relative_to(mod_root)) for path in artifacts[:10]))

    pngs = sorted((mod_root / "graphics" / "followers").glob("*.png"))
    pals = sorted((mod_root / "graphics" / "followers").glob("*.pal"))
    png_stems = {path.stem for path in pngs}
    pal_stems = {path.stem for path in pals}
    if png_stems != pal_stems:
        errors.append(f"PNG/PAL stem mismatch: png-only={sorted(png_stems - pal_stems)[:10]}, pal-only={sorted(pal_stems - png_stems)[:10]}")
    if expected.get("pngFiles") is not None and len(pngs) != expected["pngFiles"]:
        errors.append(f"expected {expected['pngFiles']} PNG files, found {len(pngs)}")
    if expected.get("palFiles") is not None and len(pals) != expected["palFiles"]:
        errors.append(f"expected {expected['palFiles']} PAL files, found {len(pals)}")

    followers = collect_followers(mod_root)
    expected_followers = expected.get("totalFollowerMappings", expected.get("importedFollowerMappings"))
    if expected_followers is not None and len(followers) != expected_followers:
        errors.append(f"expected {expected_followers} follower mappings, found {len(followers)}")

    follower_ids: set[str] = set()
    follower_tuples: dict[tuple[Any, Any, Any], str] = {}
    asset_refs: set[str] = set()
    graphics_info_refs: set[str] = set()
    for path, item in followers:
        item_id = item.get("id")
        if not item_id:
            errors.append(f"{path.relative_to(mod_root)} follower missing id")
            continue
        if item_id in follower_ids:
            errors.append(f"duplicate follower id {item_id}")
        follower_ids.add(item_id)
        key = (item.get("species"), item.get("form", 0), item.get("shiny", False))
        if key in follower_tuples:
            errors.append(f"duplicate follower tuple {key}: {follower_tuples[key]} and {item_id}")
        follower_tuples[key] = item_id
        if "asset" in item:
            asset_refs.add(item["asset"])
        if "graphicsInfoSymbol" in item:
            graphics_info_refs.add(item["graphicsInfoSymbol"])

    assets = collect_assets(mod_root)
    if expected.get("importedAssets") is not None and len(assets) != expected["importedAssets"]:
        errors.append(f"expected {expected['importedAssets']} assets, found {len(assets)}")

    asset_ids: set[str] = set()
    palette_tags: dict[int, str] = {}
    palette_symbols: dict[str, int] = {}
    for path, item in assets:
        asset_id = item.get("id") or item.get("key")
        if not asset_id:
            errors.append(f"{path.relative_to(mod_root)} asset missing id")
            continue
        if asset_id in asset_ids:
            errors.append(f"duplicate asset id {asset_id}")
        asset_ids.add(asset_id)
        palette_symbol = item.get("paletteSymbol")
        palette_tag = item.get("paletteTag")
        if palette_symbol is None or palette_tag is None:
            errors.append(f"asset {asset_id} must define paletteSymbol and paletteTag")
            continue
        palette_tag_int = parse_int(palette_tag)
        if palette_tag_int in palette_tags:
            errors.append(f"duplicate paletteTag {palette_tag_int}: {palette_tags[palette_tag_int]} and {asset_id}")
        palette_tags[palette_tag_int] = asset_id
        palette_symbols[palette_symbol] = palette_tag_int

    missing_assets = sorted(asset_refs - asset_ids)
    if missing_assets:
        errors.append("missing asset records: " + ", ".join(missing_assets[:10]))

    c = parse_c_assets(mod_root / "src" / "follower_assets.c")
    missing_graphics = sorted(graphics_info_refs - set(c["graphics_infos"]))
    if missing_graphics:
        errors.append("missing ObjectEventGraphicsInfo symbols: " + ", ".join(missing_graphics[:10]))

    for symbol, expected_tag in sorted(palette_symbols.items()):
        if symbol not in c["palette_defs"]:
            errors.append(f"missing SpritePalette symbol {symbol}")
            continue
        pal_var, c_tag = c["palette_defs"][symbol]
        if c_tag != expected_tag:
            errors.append(f"palette tag mismatch for {symbol}: json={expected_tag}, c={c_tag}")
        if pal_var not in c["pal_defs"]:
            errors.append(f"palette symbol {symbol} references missing palette data {pal_var}")
            continue
        pal_path = source_path_for_incbin(c["pal_defs"][pal_var], ".pal")
        if not pal_path.exists():
            errors.append(f"missing PAL source for {symbol}: {pal_path.relative_to(REPO_ROOT)}")

    for pic_var, incbin_path in c["pic_defs"].items():
        png_path = source_path_for_incbin(incbin_path, ".png")
        if not png_path.exists():
            errors.append(f"missing PNG source for {pic_var}: {png_path.relative_to(REPO_ROOT)}")

    for table_name, frames in sorted(c["pic_tables"].items()):
        if not frames:
            errors.append(f"{table_name} has no frame entries")
            continue
        pic_vars = {frame[0] for frame in frames}
        tile_dims = {(frame[1], frame[2]) for frame in frames}
        if len(pic_vars) != 1:
            errors.append(f"{table_name} mixes pic variables: {sorted(pic_vars)}")
            continue
        if len(tile_dims) != 1:
            errors.append(f"{table_name} mixes frame dimensions: {sorted(tile_dims)}")
            continue
        pic_var = next(iter(pic_vars))
        tile_w, tile_h = next(iter(tile_dims))
        incbin_path = c["pic_defs"].get(pic_var)
        if incbin_path is None:
            errors.append(f"{table_name} references missing pic variable {pic_var}")
            continue
        png_path = source_path_for_incbin(incbin_path, ".png")
        if not png_path.exists():
            continue
        try:
            width, height = png_size(png_path)
        except ValueError as exc:
            errors.append(str(exc))
            continue
        frame_w = tile_w * 8
        frame_h = tile_h * 8
        if width % frame_w != 0 or height % frame_h != 0:
            errors.append(f"{png_path.relative_to(REPO_ROOT)} dimensions {width}x{height} do not fit {frame_w}x{frame_h} frames")
            continue
        capacity = (width // frame_w) * (height // frame_h)
        max_declared = max(frame[3] for frame in frames)
        if max_declared >= capacity:
            errors.append(f"{table_name} declares frame {max_declared}, but {png_path.name} only has {capacity} frames")

    for symbol, body in sorted(c["graphics_infos"].items()):
        images_match = re.search(r"\.images\s*=\s*(\w+)", body)
        anims_match = re.search(r"\.anims\s*=\s*(\w+)", body)
        if not images_match or not anims_match:
            errors.append(f"{symbol} must define .images and .anims")
            continue
        image_table = images_match.group(1)
        anim_table = anims_match.group(1)
        frames = c["pic_tables"].get(image_table)
        if frames is None:
            errors.append(f"{symbol} references missing image table {image_table}")
            continue
        if anim_table not in c["anim_tables"]:
            errors.append(f"{symbol} references missing anim table {anim_table}")
            continue
        image_count = max(frame[3] for frame in frames) + 1
        max_anim_frame = c["anim_tables"][anim_table]
        if max_anim_frame >= image_count:
            errors.append(f"{symbol} anim table {anim_table} references frame {max_anim_frame}, but {image_table} has {image_count} declared frames")

    if errors:
        for error in errors:
            print(f"error: {error}", file=sys.stderr)
        return 1

    for generator in (FORM_ALIAS_GENERATOR, SHINY_GENERATOR):
        result = subprocess.run([sys.executable, str(generator), "--check"], cwd=mod_root, check=False)
        if result.returncode != 0:
            return result.returncode

    print(
        "following_pokemon validation OK: "
        f"{len(assets)} assets, {len(followers)} follower mappings, "
        f"{len(pngs)} PNG, {len(pals)} PAL"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
