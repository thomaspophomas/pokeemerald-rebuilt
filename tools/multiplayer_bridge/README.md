# Multiplayer Bridge Reference

This is a small reference bridge for the v1 multiplayer contract.

The ROM does not open sockets. It writes local state and server preferences into
an emulator-exposed bridge buffer. The bridge owns the TCP connection to the
authoritative server, including Tailscale addresses such as `100.x.y.z:7777`.

The reference bridge intentionally supports a file-backed buffer for tests and
development. Emulator-specific memory adapters can replace `BridgeMemory`
without changing the TCP framing rules.

```sh
python tools/multiplayer_bridge/bridge.py --memory-file bridge.bin --server 100.64.0.1:7777
```

`--server` is only the startup default. Once the ROM publishes an active
save-backed server profile, the bridge reconnects to that IPv4/port. Use
`--force-server` only for debugging when the in-game profile should be ignored.

TCP frames are length-prefixed big-endian payloads. Each payload starts with a
one-byte bridge frame type. Frames larger than 64 KiB and unknown frame types
are dropped by the reference bridge.

- `1` bridge hello, sent after TCP connect.
- `2` ROM/server packet slot, used for reliable ROM packets and server replies.
- `3` authoritative server view, written into `serverPlayers[]` and
  `serverSubsessions[]`.
- `4` disconnect.
- `5` server config, sent by the bridge when the ROM changes the selected
  in-game profile.
- `6` server session header, used by the server to assign slot, epoch, token,
  join nonce, tick and clock.

The bridge acknowledges reliable ROM packets only after it has handed the packet
to TCP. If the server config changes while online, the bridge forwards the new
save-backed profile, updates the ROM-visible config ack sequence, and reconnects
to the newly selected server.
