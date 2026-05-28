# Shop Mod

```json
{
  "shops": [
    {
      "id": "route_kiosk",
      "mapGroup": 1,
      "mapNum": 1,
      "type": "NORMAL",
      "mode": "claim",
      "items": ["ITEM_POTION", "ITEM_ANTIDOTE"]
    }
  ]
}
```

Use `mode: "claim"` when this mod owns the shop resource and should conflict
with another enabled mod claiming the same shop key.
