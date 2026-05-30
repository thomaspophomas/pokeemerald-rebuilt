#!/usr/bin/env python3
"""Estimate converted follower asset footprint from PNG dimensions and PAL files."""

from __future__ import annotations

from pathlib import Path


MOD_ROOT = Path(__file__).resolve().parents[1]
GRAPHICS = MOD_ROOT / "graphics" / "followers"
PNG_SIGNATURE = b"\x89PNG\r\n\x1a\n"


def png_size(path: Path) -> tuple[int, int]:
    data = path.read_bytes()[:24]
    if len(data) < 24 or data[:8] != PNG_SIGNATURE or data[12:16] != b"IHDR":
        raise SystemExit(f"{path} is not a valid PNG")
    return int.from_bytes(data[16:20], "big"), int.from_bytes(data[20:24], "big")


def main() -> int:
    pngs = sorted(GRAPHICS.glob("*.png"))
    pals = sorted(GRAPHICS.glob("*.pal"))
    source_png_bytes = sum(path.stat().st_size for path in pngs)
    source_pal_bytes = sum(path.stat().st_size for path in pals)
    converted_4bpp_bytes = 0
    largest: list[tuple[int, str, int, int]] = []
    for path in pngs:
        width, height = png_size(path)
        size = width * height // 2
        converted_4bpp_bytes += size
        largest.append((size, path.name, width, height))
    converted_pal_bytes = len(pals) * 32

    print(f"pngFiles={len(pngs)}")
    print(f"palFiles={len(pals)}")
    print(f"sourcePngBytes={source_png_bytes}")
    print(f"sourcePalBytes={source_pal_bytes}")
    print(f"estimated4bppBytes={converted_4bpp_bytes}")
    print(f"estimatedGbapalBytes={converted_pal_bytes}")
    print(f"estimatedConvertedTotalBytes={converted_4bpp_bytes + converted_pal_bytes}")
    for size, name, width, height in sorted(largest, reverse=True)[:10]:
        print(f"largeAsset={name},{width}x{height},{size}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
