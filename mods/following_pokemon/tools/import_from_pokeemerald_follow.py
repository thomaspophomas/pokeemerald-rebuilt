#!/usr/bin/env python3
"""Reimport pinned follower PNG/PAL source assets from pokeemerald-follow."""

from __future__ import annotations

import argparse
import re
import subprocess
import sys
from pathlib import Path


MOD_ROOT = Path(__file__).resolve().parents[1]
DEFAULT_COMMIT = "435a4f84e"
PIC_PREFIX = "graphics/object_events/pics/pokemon/followers/"
PAL_PREFIX = "graphics/object_events/palettes/followers/"


def run_git(reference: Path, args: list[str], *, bytes_output: bool = False) -> bytes | str:
    result = subprocess.run(
        ["git", "-C", str(reference), *args],
        check=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    if bytes_output:
        return result.stdout
    return result.stdout.decode("utf-8")


def list_source_files(reference: Path, commit: str) -> list[str]:
    output = run_git(reference, ["ls-tree", "-r", "--name-only", commit, PIC_PREFIX, PAL_PREFIX])
    assert isinstance(output, str)
    paths = []
    for line in output.splitlines():
        if line.endswith(".png") or line.endswith(".pal"):
            paths.append(line)
    return sorted(paths)


def target_name(source_path: str) -> str:
    if source_path.startswith(PIC_PREFIX):
        return Path(source_path).name
    if source_path.startswith(PAL_PREFIX):
        return Path(source_path).name
    raise ValueError(f"unsupported source path {source_path}")


def registered_asset_stems(mod_root: Path) -> set[str]:
    source = (mod_root / "src" / "follower_assets.c").read_text(encoding="utf-8")
    return set(
        re.findall(
            r'INCBIN_U(?:32|16)\("mods/following_pokemon/graphics/followers/([^".]+)\.(?:4bpp|gbapal)"\)',
            source,
        )
    )


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--reference", type=Path, required=True, help="Local clone of cedricwaltercastroson/pokeemerald-follow")
    parser.add_argument("--commit", default=DEFAULT_COMMIT, help="Source commit to import from")
    parser.add_argument("--mod-root", type=Path, default=MOD_ROOT)
    parser.add_argument("--delete", action="store_true", help="Delete existing PNG/PAL files that are not in the source commit")
    parser.add_argument("--all-source-assets", action="store_true", help="Import every source follower PNG/PAL instead of the registered mod subset")
    parser.add_argument("--no-validate", action="store_true", help="Skip validate_following_pokemon.py after import")
    args = parser.parse_args()

    reference = args.reference.resolve()
    mod_root = args.mod_root.resolve()
    target_dir = mod_root / "graphics" / "followers"

    if not (reference / ".git").exists():
        print(f"error: {reference} is not a git checkout", file=sys.stderr)
        return 2

    try:
        resolved = run_git(reference, ["rev-parse", "--verify", f"{args.commit}^{{commit}}"]).strip()
    except subprocess.CalledProcessError as exc:
        print(exc.stderr.decode("utf-8"), file=sys.stderr)
        return 2

    paths = list_source_files(reference, args.commit)
    if not paths:
        print(f"error: no follower PNG/PAL assets found at {args.commit}", file=sys.stderr)
        return 1

    allowed_stems = registered_asset_stems(mod_root)
    if not args.all_source_assets:
        paths = [path for path in paths if Path(target_name(path)).stem in allowed_stems]

    target_dir.mkdir(parents=True, exist_ok=True)
    expected_names = {target_name(path) for path in paths}

    if args.delete:
        for existing in target_dir.iterdir():
            if existing.is_file() and existing.suffix in {".png", ".pal"} and existing.name not in expected_names:
                existing.unlink()

    for source_path in paths:
        content = run_git(reference, ["show", f"{args.commit}:{source_path}"], bytes_output=True)
        assert isinstance(content, bytes)
        (target_dir / target_name(source_path)).write_bytes(content)

    png_count = len([name for name in expected_names if name.endswith(".png")])
    pal_count = len([name for name in expected_names if name.endswith(".pal")])
    skipped = len(list_source_files(reference, args.commit)) - len(paths)
    print(f"imported {png_count} PNG and {pal_count} PAL assets from {resolved[:12]}")
    if skipped and not args.all_source_assets:
        print(f"skipped {skipped} source assets that are not registered by this mod")

    if not args.no_validate:
        validator = mod_root / "tools" / "validate_following_pokemon.py"
        subprocess.run([sys.executable, str(validator), "--mod-root", str(mod_root)], check=True)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
