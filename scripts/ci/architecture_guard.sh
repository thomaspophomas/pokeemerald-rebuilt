#!/usr/bin/env sh
set -eu

fail_if_found() {
    pattern="$1"
    message="$2"
    shift 2

    if grep -R -n "$pattern" "$@" >/tmp/architecture_guard_matches.txt 2>/dev/null; then
        echo "$message" >&2
        cat /tmp/architecture_guard_matches.txt >&2
        exit 1
    fi
}

fail_if_found "ReadConnectedByte" \
    "Do not couple multiplayer modules to legacy link byte checks." \
    src/multiplayer include/multiplayer

if grep -R -n "0x10000000" src/multiplayer include/multiplayer 2>/dev/null \
    | grep -v "include/multiplayer/constants.h" >/tmp/architecture_guard_matches.txt; then
    echo "Raw emulator bridge addresses must stay in multiplayer constants/transport only." >&2
    cat /tmp/architecture_guard_matches.txt >&2
    exit 1
fi

fail_if_found "gBattleTypeFlags\\|gBattleWeather\\|gLinkPlayers" \
    "Multiplayer modules must use battle/session ports instead of legacy battle/link globals." \
    src/multiplayer include/multiplayer

if grep -R -n "gObjectEvents" src/multiplayer include/multiplayer 2>/dev/null \
    | grep -v "src/multiplayer/overworld.c" >/tmp/architecture_guard_matches.txt; then
    echo "gObjectEvents access belongs in the multiplayer overworld adapter only." >&2
    cat /tmp/architecture_guard_matches.txt >&2
    exit 1
fi

echo "Architecture guard OK"
