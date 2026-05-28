# Map Script Mod

Place map metadata and script include together:

```text
mods/my_map/maps/Route101/map.json
mods/my_map/maps/Route101/scripts.inc
```

`map.json`:

```json
{
  "id": "route101_script",
  "map_group": 1,
  "map_num": 1,
  "script": "scripts.inc",
  "mode": "observe"
}
```

`modgen` registers the script path in `MOD_MAP_SCRIPT_INCS` and
`gModMapDefinitions[].script_path` for downstream map/script plumbing.
