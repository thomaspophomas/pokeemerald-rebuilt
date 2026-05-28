# Example Mods

These folders are copyable examples. They are nested under `mods/examples`, so
the normal `mods/*/mod.json` build scan does not compile them automatically.

To try one:

```bash
cp -R mods/examples/minimal_weather mods/minimal_weather
make mod-check
make modern FEATURE_MODS=1
```

For a fresh starter folder, use:

```bash
make mod-new ID=my_mod TYPE=event
```

Examples:

- `minimal_weather`: C hook plus weather provider.
- `guide_npc`: data-only NPC definition.
- `kiosk_items`: item text/price override plus shop/reward data.
- `map_script`: map metadata plus a colocated `scripts.inc`.
