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

fail_if_found "RemoveObjectEventByLocalIdAndMap\\|SpawnSpecialObjectEvent\\|TryMoveObjectEventToMapCoords\\|ObjectEventTurn" \
    "Remote multiplayer avatars must use the non-colliding virtual-object adapter." \
    src/multiplayer include/multiplayer

if grep -n 'FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT ?= $(FEATURE_MULTIPLAYER)' Makefile >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Emulator transport must remain opt-in; FEATURE_MULTIPLAYER=1 cannot imply raw bridge reads." >&2
    cat /tmp/architecture_guard_matches.txt >&2
    exit 1
fi

if grep -n '#define FEATURE_MULTIPLAYER_EMULATOR_TRANSPORT FEATURE_MULTIPLAYER' include/config/features.h >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Emulator transport must remain opt-in in include/config/features.h." >&2
    cat /tmp/architecture_guard_matches.txt >&2
    exit 1
fi

if grep -R -n "0x10000000" src/multiplayer include/multiplayer 2>/dev/null \
    | grep -v "include/multiplayer/constants.h" >/tmp/architecture_guard_matches.txt; then
    echo "Raw emulator bridge addresses must stay in multiplayer constants/transport only." >&2
    cat /tmp/architecture_guard_matches.txt >&2
    exit 1
fi

if grep -R -n "sBridge->players\\|sBridge->subsessions" src/multiplayer 2>/dev/null >/tmp/architecture_guard_matches.txt; then
    echo "Emulator transport must keep server view and client output in separate bridge lanes." >&2
    cat /tmp/architecture_guard_matches.txt >&2
    exit 1
fi

if ! grep -n '#define NET_PROTOCOL_VERSION 7' include/multiplayer/constants.h >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Multiplayer protocol must retain ack-gated generic barrier version 7 semantics." >&2
    exit 1
fi

if ! grep -n '#define NET_EMULATOR_BRIDGE_VERSION 7' include/multiplayer/constants.h >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Multiplayer bridge must retain server-config/ack/generic-barrier version 7 semantics." >&2
    exit 1
fi

if ! grep -n '#define NET_TRANSPORT_MODE_SERVER_BRIDGE 1' include/multiplayer/constants.h >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Multiplayer transport must advertise the authoritative server bridge mode." >&2
    exit 1
fi

if ! grep -n '#define NET_COMMIT_LOG_SIZE 32' include/multiplayer/constants.h >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Multiplayer commit log must retain enough slots for idempotent retries." >&2
    exit 1
fi

if ! grep -n '#define NET_RELIABLE_QUEUE_SIZE 16' include/multiplayer/constants.h >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Multiplayer reliable action lane must retain a bounded ring buffer." >&2
    exit 1
fi

if ! grep -R -n "sessionEpoch" include/multiplayer src/multiplayer >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Multiplayer packets and snapshots must carry a session epoch for save-state/rewind rejection." >&2
    exit 1
fi

if ! grep -R -n "NET_PACKET_CLIENT_HELLO\\|NET_PACKET_SERVER_HELLO_ACK\\|NET_PACKET_HEARTBEAT\\|NET_PACKET_SERVER_CLOCK\\|NET_PACKET_INTERACTION_LOCK_RESULT" include/multiplayer/protocol.h >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Multiplayer protocol must expose acked handshake, heartbeat, server-clock, and NPC lock packet types." >&2
    exit 1
fi

if ! grep -R -n "MOD_NPC_INTERACTION_SHARED_READONLY\\|MOD_NPC_INTERACTION_EXCLUSIVE\\|MOD_NPC_INTERACTION_DISABLED_ONLINE" include/mod src/mod scripts/modgen.py >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "NPC interaction policy must remain explicit for multiplayer locking." >&2
    exit 1
fi

if ! grep -R -n "struct NetServerConfig" include/multiplayer include/mod src/multiplayer >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Multiplayer server profiles must remain represented by NetServerConfig." >&2
    exit 1
fi

if ! grep -R -n "NetTransport_SetServerConfig" include/multiplayer src/multiplayer >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "ROM must publish the selected server profile to the emulator bridge." >&2
    exit 1
fi

if ! grep -R -n "actionSequence\\|sessionEpoch\\|subsessionId" include/multiplayer/protocol.h src/multiplayer/commit.c >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "CommitResult must carry enough full-key fields for idempotent reconnects." >&2
    exit 1
fi

if ! grep -R -n "OverworldInteraction_Preflight" include/multiplayer src/multiplayer src/field_control_avatar.c >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Online overworld actions must flow through the central interaction preflight." >&2
    exit 1
fi

if ! grep -R -n "MULTIPLAYER_RESOURCE_.*FLAG\\|MULTIPLAYER_RESOURCE_.*ITEM_REWARD\\|MULTIPLAYER_RESOURCE_.*MAP_TILE\\|MULTIPLAYER_RESOURCE_.*MENU" include/multiplayer src/multiplayer src/field_control_avatar.c >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Overworld interaction locks must retain typed resource keys beyond NPC locks." >&2
    exit 1
fi

if ! grep -n '#define NET_REMOTE_PLAYER_VIRTUAL_ID_BASE 248' include/multiplayer/constants.h >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Remote virtual object IDs must stay in the reserved high-ID range." >&2
    exit 1
fi

if ! grep -n 'NET_REMOTE_PLAYER_VIRTUAL_ID_END > 254' include/multiplayer/constants.h >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Remote virtual object ID range must retain a compile-time overflow guard." >&2
    exit 1
fi

fail_if_found "gBattleTypeFlags\\|gBattleWeather\\|gLinkPlayers" \
    "Multiplayer modules must use battle/session ports instead of legacy battle/link globals." \
    src/multiplayer include/multiplayer

fail_if_found "CreateVirtualObject(" \
    "Multiplayer remote avatars must use CreateOrUpdateVirtualObject for idempotent sprite ownership." \
    src/multiplayer include/multiplayer

if grep -R -n "AddBagItem\\|RemoveBagItem\\|GiveMon\\|SetMonData\\|FlagSet\\|FlagClear\\|VarSet\\|AddMoney\\|RemoveMoney" src/multiplayer include/multiplayer 2>/dev/null \
    | grep -v "src/multiplayer/commit.c" >/tmp/architecture_guard_matches.txt; then
    echo "Online gameplay state mutations must go through multiplayer commit APIs." >&2
    cat /tmp/architecture_guard_matches.txt >&2
    exit 1
fi

fail_if_found "NetTransport_" \
    "UI code must switch multiplayer through runtime/session ports, not transport adapters." \
    src/start_menu.c src/option_menu.c

if ! grep -R -n "EngineRuntimeState_IsMultiplayerOnlineEnabled" src/multiplayer/session.c >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Multiplayer session ticks must remain gated by the runtime Solo/Online mode." >&2
    exit 1
fi

if ! grep -n "SaveBlock2SizeChanged" src/save.c >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "SaveBlock2 size must remain locked when multiplayer options use padding bits." >&2
    exit 1
fi

if ! grep -n "scripts/modgen.py" Makefile >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Mod registries must be generated through scripts/modgen.py before source scanning." >&2
    exit 1
fi

if ! grep -R -n "ModApi_Init" src/engine include/engine >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "The mod API must attach through engine/module_registry." >&2
    exit 1
fi

if ! grep -n "PokeBallApi_CalculateThrowResult" src/battle_script_commands.c >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Pokeball catch calculation must go through PokeBallApi_CalculateThrowResult." >&2
    exit 1
fi

if ! grep -n "PokeBallApi_GetBattleScript" src/battle_util.c >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Pokeball battle scripts must go through PokeBallApi_GetBattleScript." >&2
    exit 1
fi

if ! grep -n "BattleSpriteApi_GetBallSprite" src/battle_anim_throw.c >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Battle ball graphics must go through BattleSpriteApi_GetBallSprite." >&2
    exit 1
fi

if grep -R -n "gBallSpriteTemplates\\|gBallSpriteSheets\\|gBallSpritePalettes" src/mod include/mod 2>/dev/null \
    | grep -v "src/mod/battle_sprite.c" >/tmp/architecture_guard_matches.txt; then
    echo "Mod sprite code must use sprite asset/battle sprite ports instead of raw ball sprite tables." >&2
    cat /tmp/architecture_guard_matches.txt >&2
    exit 1
fi

if grep -R -n "gText_" src/mod include/mod 2>/dev/null \
    | grep -v "src/mod/language.c" >/tmp/architecture_guard_matches.txt; then
    echo "Mod-facing text should use LanguageApi_* instead of hard-coded gText_* references." >&2
    cat /tmp/architecture_guard_matches.txt >&2
    exit 1
fi

if grep -R -n "gWeather" src/mod include/mod 2>/dev/null \
    | grep -v "src/mod/weather.c" >/tmp/architecture_guard_matches.txt; then
    echo "Mod code must query weather through mod/weather; raw gWeather belongs only in the weather adapter." >&2
    cat /tmp/architecture_guard_matches.txt >&2
    exit 1
fi

if grep -R -n "FlagSet\\|FlagClear\\|FlagGet" src/mod include/mod 2>/dev/null \
    | grep -v "src/mod/flags.c" >/tmp/architecture_guard_matches.txt; then
    echo "Mod code must use ModFlag_* instead of raw script flags outside the flag adapter." >&2
    cat /tmp/architecture_guard_matches.txt >&2
    exit 1
fi

if grep -R -n "gObjectEvents" src/multiplayer include/multiplayer 2>/dev/null \
    | grep -v "src/multiplayer/overworld.c" >/tmp/architecture_guard_matches.txt; then
    echo "gObjectEvents access belongs in the multiplayer overworld adapter only." >&2
    cat /tmp/architecture_guard_matches.txt >&2
    exit 1
fi

echo "Architecture guard OK"
