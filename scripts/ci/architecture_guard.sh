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

if grep -R -n "\\<slot\\>" src/mod src/engine src/multiplayer include/mod include/engine include/multiplayer 2>/dev/null >/tmp/architecture_guard_matches.txt; then
    echo "Beginner-facing mod/multiplayer code must use context-specific names instead of bare 'slot'." >&2
    cat /tmp/architecture_guard_matches.txt >&2
    exit 1
fi

if grep -R -n -E "^[[:space:]]*(u8|u16|u32|s32)[[:space:]]+[ij];|for[[:space:]]*\\((i|j)[[:space:]]*=" src/mod src/engine src/multiplayer 2>/dev/null >/tmp/architecture_guard_matches.txt; then
    echo "Beginner-facing mod/multiplayer code must use descriptive iterator names instead of bare i/j." >&2
    cat /tmp/architecture_guard_matches.txt >&2
    exit 1
fi

if grep -R -n -E "\\<(src|dst|dest|temp|partyId|packet|result|id)\\>" src/mod src/engine src/multiplayer include/mod include/engine include/multiplayer 2>/dev/null >/tmp/architecture_guard_matches.txt; then
    echo "Beginner-facing mod/multiplayer code must use domain-specific names instead of bare src/dst/dest/temp/packet/result/id." >&2
    cat /tmp/architecture_guard_matches.txt >&2
    exit 1
fi

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

if grep -R -n "0x10000000\\|NET_EMULATOR_SHARED_BASE" src/multiplayer include/multiplayer 2>/dev/null >/tmp/architecture_guard_matches.txt; then
    echo "Raw emulator bridge addresses must not be used; bridge adapters must resolve the EWRAM mailbox symbol." >&2
    cat /tmp/architecture_guard_matches.txt >&2
    exit 1
fi

if ! grep -n "gNetEmulatorBridgeMailbox" include/multiplayer/bridge_mailbox.h src/multiplayer/transport_emulator.c >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Emulator bridge mailbox must remain a stable exported EWRAM symbol." >&2
    exit 1
fi

if grep -R -n "sBridge->players\\|sBridge->subsessions" src/multiplayer 2>/dev/null >/tmp/architecture_guard_matches.txt; then
    echo "Emulator transport must keep server view and client output in separate bridge lanes." >&2
    cat /tmp/architecture_guard_matches.txt >&2
    exit 1
fi

if ! grep -n '#define NET_PROTOCOL_VERSION 2' include/multiplayer/constants.h >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Multiplayer protocol must retain epoch-aware version 2 semantics." >&2
    exit 1
fi

if ! grep -n '#define NET_EMULATOR_BRIDGE_VERSION 10' include/multiplayer/constants.h >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Multiplayer bridge must retain compact hot-snapshot version 10 semantics." >&2
    exit 1
fi

if ! grep -n '#define NET_TRANSPORT_MODE_SERVER_BRIDGE 1' include/multiplayer/constants.h >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Multiplayer transport must advertise the authoritative server bridge mode." >&2
    exit 1
fi

if ! grep -n '#define NET_COMMIT_LOG_SIZE 16' include/multiplayer/constants.h >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Multiplayer commit log must retain a bounded retry cache within the EWRAM budget." >&2
    exit 1
fi

if ! grep -n '#define NET_RELIABLE_QUEUE_SIZE 8' include/multiplayer/constants.h >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Multiplayer reliable action lane must retain a bounded ring buffer." >&2
    exit 1
fi

if ! grep -n '#define NET_EMULATOR_MAILBOX_MAX_SIZE 4096' include/multiplayer/bridge_mailbox.h >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Emulator bridge mailbox must stay within the compact 4 KiB EWRAM budget." >&2
    exit 1
fi

if ! grep -n '#define NET_TRANSPORT_PACKET_PAYLOAD_SIZE 96' include/multiplayer/transport.h >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Multiplayer packet payload size must stay within the EWRAM mailbox budget." >&2
    exit 1
fi

if ! grep -n '#define NET_TRANSPORT_PACKET_HEADER_SIZE 22' include/multiplayer/transport.h >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Multiplayer packet slots must retain the compact mailbox header." >&2
    exit 1
fi

if awk '
    /^struct NetTransportPacketSlot$/ { inside = 1 }
    inside && /NetPacketEnvelope/ { found = 1 }
    inside && /^}.*;/ { inside = 0 }
    END { exit found ? 0 : 1 }
' include/multiplayer/transport.h; then
    echo "Mailbox packet slots must not store full NetPacketEnvelope headers." >&2
    exit 1
fi

if ! grep -n '#define NET_PROFILE_CHUNK_DATA_SIZE 80' include/multiplayer/constants.h >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Server runtime-profile chunks must remain small enough for the compact packet payload." >&2
    exit 1
fi

if ! grep -n '#define NET_CATALOG_CHUNK_ENTRY_COUNT 7' include/multiplayer/constants.h >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Client mod-catalog chunks must remain small enough for the compact packet payload." >&2
    exit 1
fi

if ! grep -n '#define NET_PLAYER_SNAPSHOT_IDLE_REPUBLISH_FRAMES 15' include/multiplayer/constants.h >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Idle overworld snapshots must stay paced so unchanged avatars do not write the mailbox every frame." >&2
    exit 1
fi

if ! grep -n "NET_PACKET_PLAYER_BATTLE_PROFILE" include/multiplayer/protocol.h src/multiplayer/session.c >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Multiplayer party/battle profiles must use the reliable packet lane, not the overworld snapshot lane." >&2
    exit 1
fi

if awk '
    /^[[:space:]]*struct NetPlayerSnapshot[[:space:]]*$/ { inside = 1 }
    inside && /trainerGender|partyCount|partySpecies|partyMoves|playerName/ { found = 1 }
    inside && /^[[:space:]]*};/ { inside = 0 }
    END { exit found ? 0 : 1 }
' include/multiplayer/types.h; then
    echo "NetPlayerSnapshot must remain overworld-only; party/battle profile fields belong in NetPlayerBattleProfile." >&2
    exit 1
fi

if awk '
    /^[[:space:]]*struct NetPlayerSnapshot[[:space:]]*$/ { inside = 1 }
    inside && /client_frame|player_token|join_nonce|trust_flags/ { found = 1 }
    inside && /^[[:space:]]*};/ { inside = 0 }
    END { exit found ? 0 : 1 }
' include/multiplayer/types.h; then
    echo "NetPlayerSnapshot must stay hot-path only; client identity belongs in hello/heartbeat/session state." >&2
    exit 1
fi

if ! grep -R -n "session_epoch" include/multiplayer src/multiplayer >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Multiplayer packets and snapshots must carry a session epoch for save-state/rewind rejection." >&2
    exit 1
fi

if ! grep -R -n "NET_PACKET_CLIENT_HELLO\\|NET_PACKET_HEARTBEAT\\|NET_PACKET_SERVER_CLOCK" include/multiplayer/protocol.h >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Multiplayer protocol must expose handshake, heartbeat, and server-clock packet types." >&2
    exit 1
fi

if grep -R -n "NET_PACKET_[A-Z0-9_]*MOD\\|MODPACK_NEGOTIATION\\|MOD_NEGOTIATION" include/multiplayer src/multiplayer 2>/dev/null >/tmp/architecture_guard_matches.txt; then
    echo "Online mods must use the bounded server runtime-profile lane, not ad-hoc mod negotiation packets." >&2
    cat /tmp/architecture_guard_matches.txt >&2
    exit 1
fi

fail_if_found "generated/mod_registry.h\\|mod/runtime_profile.h" \
    "Multiplayer modules must use engine/extension_profile instead of depending on generated mod registries or mod runtime internals." \
    src/multiplayer include/multiplayer

if ! grep -n "engine/extension_profile.h" src/multiplayer/session.c >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Multiplayer session must consume runtime-profile/catalog state through engine/extension_profile." >&2
    exit 1
fi

if ! grep -n "EngineExtensionProfile_GetCapabilityHash" include/engine/extension_profile.h src/engine/extension_profile.c >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Engine extension-profile facade must expose multiplayer-safe catalog/profile metadata." >&2
    exit 1
fi

if ! grep -n 'FEATURE_MODS ?= 0' Makefile >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "FEATURE_MODS must default off so the base game plus multiplayer builds without drop-in mods." >&2
    exit 1
fi

if ! awk '
    /^ifeq \(\$\(FEATURE_MODS\),1\)$/ { in_mods = 1 }
    in_mods && /-include build\/generated\/mod_sources\.mk/ { found = 1 }
    in_mods && /^endif$/ { in_mods = 0 }
    END { exit found ? 0 : 1 }
' Makefile; then
    echo "Generated mod source lists must only be included for FEATURE_MODS=1 builds." >&2
    exit 1
fi

if ! grep -n 'FEATURE_MODS 0' include/config/features.h >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "include/config/features.h must keep FEATURE_MODS off by default." >&2
    exit 1
fi

if ! grep -n "profile_capability_hash" include/multiplayer/protocol.h >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Multiplayer protocol must expose the server runtime-profile capability hash." >&2
    exit 1
fi

if ! grep -n "hello.profile_capability_hash" src/multiplayer/session.c >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Multiplayer hello must advertise the server runtime-profile capability hash." >&2
    exit 1
fi

if ! grep -n "hello.mod_catalog_hash" src/multiplayer/session.c >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Multiplayer hello must advertise the generated ROM mod catalog hash." >&2
    exit 1
fi

if ! grep -n "NET_PACKET_SERVER_PROFILE_BEGIN" include/multiplayer/protocol.h >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Multiplayer protocol must keep the server runtime-profile begin packet." >&2
    exit 1
fi

if ! grep -n "case NET_PACKET_SERVER_PROFILE_BEGIN" src/multiplayer/session.c >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Multiplayer session must handle server runtime-profile begin packets." >&2
    exit 1
fi

if ! grep -n "NET_PACKET_SERVER_PROFILE_ACK" include/multiplayer/protocol.h >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Multiplayer protocol must ACK server runtime-profile results." >&2
    exit 1
fi

if ! grep -n "NetTransport_SendPacket(NET_PACKET_SERVER_PROFILE_ACK" src/multiplayer/session.c >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Multiplayer session must send server runtime-profile ACK packets." >&2
    exit 1
fi

if ! grep -n "NET_PACKET_SERVER_CATALOG_REQUEST" include/multiplayer/protocol.h >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Multiplayer protocol must keep the server mod-catalog request packet." >&2
    exit 1
fi

if ! grep -n "NET_PACKET_CLIENT_CATALOG_CHUNK" include/multiplayer/protocol.h >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Multiplayer protocol must keep the client mod-catalog chunk packet." >&2
    exit 1
fi

if ! grep -n "case NET_PACKET_SERVER_CATALOG_REQUEST" src/multiplayer/session.c >/tmp/architecture_guard_matches.txt 2>/dev/null; then
    echo "Multiplayer session must respond to server mod-catalog requests." >&2
    exit 1
fi

if grep -R -n "cargo run -p net-server\\|cargo run -p net-bridge\\|cargo test -p net-integration" README.md AGENTS.md CLAUDE.md docs 2>/dev/null >/tmp/architecture_guard_matches.txt; then
    echo "Rust host scaffold commands must not be documented as the current ROM-repo multiplayer path." >&2
    cat /tmp/architecture_guard_matches.txt >&2
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
    | grep -v "src/mod/flags.c" \
    | grep -v "src/mod/badge.c" >/tmp/architecture_guard_matches.txt; then
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
