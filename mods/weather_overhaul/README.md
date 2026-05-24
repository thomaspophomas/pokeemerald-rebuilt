# Weather Overhaul

This mod reserves composable weather layers without assigning final gameplay
effects yet.

Supported primary layers:

- rain
- sandstorm
- hail/snow
- sun

Supported overlay layers:

- darkness
- wind
- thunder
- fog
- ash
- extreme silence

Primary layers are mutually exclusive, so rain plus sun normalizes to one
primary weather. Overlay layers can stack with a primary weather; wind plus rain
and wind plus sun are valid. Extreme silence removes wind and thunder.

Final triggers, battle effects, map placement, and text can be added later on
top of the same layer API.
