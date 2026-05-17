#!/usr/bin/env python3
"""Validate JSON files used by the mod data pipeline."""

from __future__ import annotations

import json
import sys
from pathlib import Path


def main() -> int:
    root = Path(__file__).resolve().parents[2]
    json_roots = [root / "mods", root / "docs" / "mod_api_schemas"]
    errors = []
    checked = 0

    for json_root in json_roots:
        if not json_root.exists():
            errors.append(f"missing JSON root: {json_root.relative_to(root).as_posix()}")
            continue
        for path in sorted(json_root.rglob("*.json")):
            checked += 1
            try:
                with path.open("r", encoding="utf-8") as handle:
                    json.load(handle)
            except json.JSONDecodeError as exc:
                errors.append(f"{path.relative_to(root).as_posix()}: {exc}")

    vanilla_root = root / "mods" / "vanilla"
    summary_path = vanilla_root / "extraction_summary.json"
    if summary_path.exists():
        summary = json.loads(summary_path.read_text(encoding="utf-8"))
        for domain in summary.get("domains", []):
            domain_root = vanilla_root / domain
            schema_path = root / "docs" / "mod_api_schemas" / f"{domain}.schema.json"
            expectation_path = vanilla_root / "expectations" / domain / "baseline.expected.json"
            if not domain_root.is_dir():
                errors.append(f"missing vanilla domain folder: mods/vanilla/{domain}")
            if not schema_path.is_file():
                errors.append(f"missing schema: docs/mod_api_schemas/{domain}.schema.json")
            if not expectation_path.is_file():
                errors.append(f"missing expectation: mods/vanilla/expectations/{domain}/baseline.expected.json")

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print(f"OK: parsed {checked} mod/schema JSON files")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
