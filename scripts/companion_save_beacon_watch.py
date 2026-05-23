#!/usr/bin/env python3
import argparse
import os
import struct
import time

MAGIC = 0x3142434E
SECTOR_SIZE = 4096
RECORDED_BATTLE_SECTOR = 31
BEACON_OFFSET = RECORDED_BATTLE_SECTOR * SECTOR_SIZE + 4
BEACON_STRUCT = struct.Struct("<IHHIIHBBBBBBBBhhBBBBHH")
CHECKSUM_OFFSET = 16


def calc_checksum(payload):
    checksum = 0
    for byte in payload:
        checksum = ((checksum << 1) & 0xFFFF) ^ (checksum >> 15) ^ byte
    return checksum


def read_beacon(path):
    with open(path, "rb") as save_file:
        save_file.seek(BEACON_OFFSET)
        payload = bytearray(save_file.read(BEACON_STRUCT.size))

    if len(payload) != BEACON_STRUCT.size:
        return None

    fields = BEACON_STRUCT.unpack(payload)
    if fields[0] != MAGIC:
        return None

    expected_checksum = fields[5]
    payload[CHECKSUM_OFFSET] = 0
    payload[CHECKSUM_OFFSET + 1] = 0
    actual_checksum = calc_checksum(payload)

    return {
        "version": fields[1],
        "size": fields[2],
        "sequence": fields[3],
        "tick": fields[4],
        "checksum_ok": expected_checksum == actual_checksum,
        "session_state": fields[6],
        "health_state": fields[7],
        "local_player_id": fields[8],
        "player_count": fields[9],
        "active": fields[10],
        "map_group": fields[11],
        "map_num": fields[12],
        "elevation": fields[13],
        "x": fields[14],
        "y": fields[15],
        "facing": fields[16],
        "movement": fields[17],
        "avatar": fields[18],
        "graphics_revision": fields[20],
        "flags": fields[21],
    }


def format_beacon(beacon, mtime):
    status = "ok" if beacon["checksum_ok"] else "bad-checksum"
    return (
        f"{time.strftime('%H:%M:%S')} mtime={mtime:.0f} seq={beacon['sequence']} "
        f"tick={beacon['tick']} map={beacon['map_group']}.{beacon['map_num']} "
        f"xy=({beacon['x']},{beacon['y']}) facing={beacon['facing']} "
        f"state={beacon['session_state']} health={beacon['health_state']} "
        f"players={beacon['player_count']} checksum={status}"
    )


def main():
    parser = argparse.ArgumentParser(description="Watch the experimental Pokemon Emerald companion save beacon.")
    parser.add_argument("save_path", help="Path to the .sav file produced by the emulator")
    parser.add_argument("--interval", type=float, default=1.0, help="Polling interval in seconds")
    args = parser.parse_args()

    last_sequence = None
    while True:
        try:
            stat_result = os.stat(args.save_path)
            beacon = read_beacon(args.save_path)
        except OSError as exc:
            print(f"{time.strftime('%H:%M:%S')} waiting for save: {exc}")
            time.sleep(args.interval)
            continue

        if beacon is None:
            print(f"{time.strftime('%H:%M:%S')} no beacon at offset {BEACON_OFFSET}")
        elif beacon["sequence"] != last_sequence:
            print(format_beacon(beacon, stat_result.st_mtime), flush=True)
            last_sequence = beacon["sequence"]

        time.sleep(args.interval)


if __name__ == "__main__":
    main()
