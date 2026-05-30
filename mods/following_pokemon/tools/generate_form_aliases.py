#!/usr/bin/env python3
"""Generate form-aware follower aliases from imported base assets."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any


MOD_ROOT = Path(__file__).resolve().parents[1]
IMPORTED = MOD_ROOT / "followers" / "imported.json"
OUTPUT = MOD_ROOT / "followers" / "form_aliases.json"

CASTFORM_FORMS = [
    (1, "sunny"),
    (2, "rainy"),
    (3, "snowy"),
]
DEOXYS_FORMS = [
    (1, "attack"),
    (2, "defense"),
    (3, "speed"),
]
UNOWN_LETTERS = "bcdefghijklmnopqrstuvwxyz"


def load_imported_followers() -> list[dict[str, Any]]:
    data = json.loads(IMPORTED.read_text(encoding="utf-8"))
    followers = data.get("followers")
    if not isinstance(followers, list):
        raise ValueError("followers/imported.json has no followers list")
    return [item for item in followers if isinstance(item, dict)]


def find_by_id(items: list[dict[str, Any]], item_id: str) -> dict[str, Any]:
    for item in items:
        if item.get("id") == item_id:
            return item
    raise ValueError(f"missing imported follower {item_id}")


def alias_from(source: dict[str, Any], alias_id: str, species: str, form: int) -> dict[str, Any]:
    alias = dict(source)
    alias["id"] = alias_id
    alias["species"] = species
    alias["form"] = form
    alias["shiny"] = False
    return alias


def render() -> str:
    imported = load_imported_followers()
    aliases: list[dict[str, Any]] = []

    castform = find_by_id(imported, "imported_castform")
    for form, form_name in CASTFORM_FORMS:
        aliases.append(alias_from(castform, f"form_alias_castform_{form_name}", "SPECIES_CASTFORM", form))

    deoxys = find_by_id(imported, "imported_deoxys")
    for form, form_name in DEOXYS_FORMS:
        aliases.append(alias_from(deoxys, f"form_alias_deoxys_{form_name}", "SPECIES_DEOXYS", form))

    for form, letter in enumerate(UNOWN_LETTERS, start=1):
        source = find_by_id(imported, f"imported_unown_{letter}")
        aliases.append(alias_from(source, f"form_alias_unown_{letter}", "SPECIES_UNOWN", form))

    return json.dumps(
        {
            "_meta": {
                "generatedBy": "tools/generate_form_aliases.py",
                "source": "followers/imported.json",
                "note": "Aliases let rebuilt look up one species with form values while reusing imported assets.",
            },
            "followers": aliases,
        },
        indent=2,
        ensure_ascii=True,
    ) + "\n"


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    try:
        content = render()
    except ValueError as exc:
        print(f"following_pokemon form alias generation failed: {exc}", file=sys.stderr)
        return 1
    if args.check:
        existing = OUTPUT.read_text(encoding="utf-8") if OUTPUT.exists() else ""
        if existing != content:
            print(f"error: {OUTPUT} is out of date", file=sys.stderr)
            return 1
        print(f"{OUTPUT} is up to date")
        return 0
    OUTPUT.write_text(content, encoding="utf-8")
    print(f"wrote {OUTPUT}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
