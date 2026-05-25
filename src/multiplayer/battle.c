#include "global.h"
#include "battle_session.h"
#include "battle_setup.h"
#include "main.h"
#include "multiplayer/battle.h"
#include "multiplayer/commit.h"
#include "multiplayer/interaction_menu.h"
#include "multiplayer/overworld.h"
#include "multiplayer/session.h"
#include "multiplayer/transport.h"
#include "pokemon.h"
#include "constants/battle.h"
#include "constants/characters.h"
#include "constants/moves.h"
#include "constants/pokemon.h"
#include "constants/species.h"
#include "constants/trainers.h"
#include "constants/weather.h"

static const u8 sFallbackPartnerName[] = _("PARTNER");

#if FEATURE_MULTIPLAYER
static EWRAM_DATA bool8 sMultiplayerBattleActive = FALSE;
static EWRAM_DATA bool8 sPendingPvpBattle = FALSE;
static EWRAM_DATA bool8 sPendingPveBattle = FALSE;
static EWRAM_DATA u8 sPendingSubsessionId = NET_SUBSESSION_NONE;
static EWRAM_DATA u8 sActiveSubsessionId = NET_SUBSESSION_NONE;
static EWRAM_DATA u8 sStartedSubsessionId = NET_SUBSESSION_NONE;
static EWRAM_DATA u8 sPvePartnerPlayerId = NET_PLAYER_NONE;
static EWRAM_DATA u8 sPvePartnerGender = MALE;
static EWRAM_DATA u8 sPvePartnerName[PLAYER_NAME_LENGTH + 1] = {EOS};
static EWRAM_DATA struct SecretBase sPendingSecretBase = {0};

static bool8 SubsessionStateCanStartBattle(u8 state)
{
    return state == MULTIPLAYER_SUBSESSION_STATE_INVITING
        || state == MULTIPLAYER_SUBSESSION_STATE_READY
        || state == MULTIPLAYER_SUBSESSION_STATE_ACTIVE;
}

static void ResetPvePartner(void)
{
    sPvePartnerPlayerId = NET_PLAYER_NONE;
    sPvePartnerGender = MALE;
    sPvePartnerName[0] = EOS;
}

static bool8 BattleProfileMonIsUsable(const struct NetPlayerBattleProfile *profile, u8 battle_profile_party_index)
{
    u16 species;

    if (profile == NULL || battle_profile_party_index >= NET_PLAYER_PARTY_SNAPSHOT_SIZE)
        return FALSE;
    if (battle_profile_party_index >= profile->party_count)
        return FALSE;

    species = profile->party_species[battle_profile_party_index];
    return species != SPECIES_NONE
        && species != SPECIES_EGG
        && species < NUM_SPECIES
        && profile->party_levels[battle_profile_party_index] != 0
        && profile->party_levels[battle_profile_party_index] <= MAX_LEVEL
        && profile->party_hp[battle_profile_party_index] != 0;
}

static bool8 BattleProfileHasUsablePartnerParty(const struct NetPlayerBattleProfile *profile)
{
    u8 party_mon_index;

    if (profile == NULL || profile->party_count == 0)
        return FALSE;

    for (party_mon_index = 0; party_mon_index < NET_PLAYER_PARTY_SNAPSHOT_SIZE; party_mon_index++)
    {
        if (BattleProfileMonIsUsable(profile, party_mon_index))
            return TRUE;
    }

    return FALSE;
}

static void CopyBattleProfilePartnerName(const struct NetPlayerBattleProfile *profile)
{
    u8 name_char_index;

    for (name_char_index = 0; name_char_index < PLAYER_NAME_LENGTH && profile->player_name[name_char_index] != EOS && profile->player_name[name_char_index] != 0; name_char_index++)
        sPvePartnerName[name_char_index] = profile->player_name[name_char_index];

    if (name_char_index == 0)
    {
        sPvePartnerName[0] = CHAR_P;
        sPvePartnerName[1] = CHAR_A;
        sPvePartnerName[2] = CHAR_R;
        sPvePartnerName[3] = CHAR_T;
        sPvePartnerName[4] = CHAR_N;
        sPvePartnerName[5] = CHAR_E;
        sPvePartnerName[6] = CHAR_R;
        name_char_index = PLAYER_NAME_LENGTH;
    }

    sPvePartnerName[name_char_index] = EOS;
}
#endif

#if FEATURE_MULTIPLAYER
static bool8 RequestHasUniquePlayers(const struct MultiplayerBattleRequest *request)
{
    u8 player_index_in_request;
    u8 duplicate_check_index;

    for (player_index_in_request = 0; player_index_in_request < request->player_count; player_index_in_request++)
    {
        if (request->players[player_index_in_request] >= MAX_NET_PLAYERS)
            return FALSE;

        for (duplicate_check_index = player_index_in_request + 1; duplicate_check_index < request->player_count; duplicate_check_index++)
        {
            if (request->players[player_index_in_request] == request->players[duplicate_check_index])
                return FALSE;
        }
    }

    return TRUE;
}

static bool8 PlayerBlocksBattleRequest(u8 player_id)
{
    const struct MultiplayerSession *session;

    if (player_id >= MAX_NET_PLAYERS)
        return TRUE;
    if (MultiplayerSession_IsPlayerInSubsession(player_id))
        return TRUE;
    if (!MultiplayerSession_IsPlayerBusy(player_id))
        return FALSE;

    session = MultiplayerSession_Get();
    if (session == NULL || !session->players[player_id].active)
        return TRUE;

    return session->players[player_id].interaction_state != MULTIPLAYER_INTERACTION_OPTIONS_MENU;
}
#endif

bool8 MultiplayerBattle_CanStart(const struct MultiplayerBattleRequest *request)
{
#if FEATURE_MULTIPLAYER
    u8 player_index_in_request;

    if (request == NULL)
        return FALSE;
    if (!MultiplayerSession_IsOnline())
        return FALSE;
    if (request->player_count == 0 || request->player_count > MAX_NET_BATTLE_PLAYERS)
        return FALSE;
    if (!RequestHasUniquePlayers(request))
        return FALSE;
    for (player_index_in_request = 0; player_index_in_request < request->player_count; player_index_in_request++)
    {
        if (PlayerBlocksBattleRequest(request->players[player_index_in_request]))
            return FALSE;
    }

    switch (request->type)
    {
    case MULTIPLAYER_SUBSESSION_PVE_BATTLE:
        return request->player_count > 0
            && request->player_count <= MAX_NET_PVE_PLAYERS
            && MultiplayerSession_ArePlayersWithinRange(request->player_count, request->players, NET_PLAYER_PVE_BATTLE_RANGE_TILES);
    case MULTIPLAYER_SUBSESSION_PVP_BATTLE:
        return request->player_count == MAX_NET_PVP_PLAYERS
            && MultiplayerSession_ArePlayersWithinRange(request->player_count, request->players, NET_PLAYER_INTERACTION_RANGE_TILES);
    default:
        return FALSE;
    }
#else
    (void)request;
    return FALSE;
#endif
}

bool8 MultiplayerBattle_Start(const struct MultiplayerBattleRequest *request)
{
#if FEATURE_MULTIPLAYER
    if (!MultiplayerBattle_CanStart(request))
        return FALSE;

    return MultiplayerSession_StartSubsession(request->type, request->player_count, request->players);
#else
    return FALSE;
#endif
}

#if FEATURE_MULTIPLAYER
static bool8 SubsessionIncludesPlayer(const struct MultiplayerSubsession *subsession, u8 player_id)
{
    u8 player_index_in_subsession;

    if (subsession == NULL || !subsession->active)
        return FALSE;
    if (player_id >= MAX_NET_PLAYERS)
        return FALSE;

    for (player_index_in_subsession = 0; player_index_in_subsession < subsession->player_count; player_index_in_subsession++)
    {
        if (subsession->players[player_index_in_subsession] == player_id)
            return TRUE;
    }

    return FALSE;
}

static const struct MultiplayerSubsession *FindLocalBattleSubsessionByType(const struct MultiplayerSession *session, u8 type)
{
    u8 subsession_index;

    if (session == NULL || session->local_player_id >= MAX_NET_PLAYERS)
        return NULL;

    for (subsession_index = 0; subsession_index < MAX_NET_SUBSESSIONS; subsession_index++)
    {
        const struct MultiplayerSubsession *subsession = &session->subsessions[subsession_index];

        if (!subsession->active)
            continue;
        if (subsession->type != type)
            continue;
        if (!SubsessionStateCanStartBattle(subsession->state))
            continue;
        if (!SubsessionIncludesPlayer(subsession, session->local_player_id))
            continue;

        return subsession;
    }

    return NULL;
}

static const struct MultiplayerSubsession *FindLocalPvpSubsession(const struct MultiplayerSession *session)
{
    return FindLocalBattleSubsessionByType(session, MULTIPLAYER_SUBSESSION_PVP_BATTLE);
}

static bool8 PlayerHasUsableParty(void)
{
    u8 party_mon_index;

    for (party_mon_index = 0; party_mon_index < PARTY_SIZE; party_mon_index++)
    {
        u32 species = GetMonData(&gPlayerParty[party_mon_index], MON_DATA_SPECIES_OR_EGG, NULL);

        if (species != SPECIES_NONE
         && species != SPECIES_EGG
         && GetMonData(&gPlayerParty[party_mon_index], MON_DATA_HP, NULL) != 0)
            return TRUE;
    }

    return FALSE;
}

bool8 MultiplayerBattle_LocalPlayerCanFight(void)
{
    return PlayerHasUsableParty();
}

bool8 MultiplayerBattle_RemotePlayerCanPartner(u8 partner_player_id)
{
#if FEATURE_MULTIPLAYER
    const struct NetPlayerBattleProfile *profile;

    if (!MultiplayerSession_IsPlayerActive(partner_player_id))
        return FALSE;

    profile = MultiplayerSession_GetPlayerBattleProfile(partner_player_id);

    return BattleProfileHasUsablePartnerParty(profile);
#else
    (void)partner_player_id;
    return FALSE;
#endif
}

bool8 MultiplayerBattle_PrepareTrainerPvePartnerParty(u8 partner_player_id)
{
#if FEATURE_MULTIPLAYER
    const struct NetPlayerBattleProfile *profile;
    u8 party_mon_index;
    u8 move_index;
    u8 destination_party_index = 0;

    if (!MultiplayerBattle_RemotePlayerCanPartner(partner_player_id))
        return FALSE;

    profile = MultiplayerSession_GetPlayerBattleProfile(partner_player_id);
    if (profile == NULL)
        return FALSE;

    for (party_mon_index = 0; party_mon_index < NET_PLAYER_PARTY_SNAPSHOT_SIZE; party_mon_index++)
        ZeroMonData(&gPlayerParty[MULTI_PARTY_SIZE + party_mon_index]);

    for (party_mon_index = 0; party_mon_index < NET_PLAYER_PARTY_SNAPSHOT_SIZE && destination_party_index < NET_PLAYER_PARTY_SNAPSHOT_SIZE; party_mon_index++)
    {
        u32 maxHp;
        u32 hp;
        u32 otGender;

        if (!BattleProfileMonIsUsable(profile, party_mon_index))
            continue;

        CreateMon(&gPlayerParty[MULTI_PARTY_SIZE + destination_party_index],
                  profile->party_species[party_mon_index],
                  profile->party_levels[party_mon_index],
                  USE_RANDOM_IVS,
                  FALSE,
                  0,
                  OT_ID_RANDOM_NO_SHINY,
                  0);

        MultiplayerCommit_WriteMonData(&gPlayerParty[MULTI_PARTY_SIZE + destination_party_index], MON_DATA_HELD_ITEM, &profile->party_held_items[party_mon_index]);
        for (move_index = 0; move_index < MAX_MON_MOVES; move_index++)
        {
            u16 move = profile->party_moves[party_mon_index][move_index];

            if (move != MOVE_NONE && move < MOVES_COUNT)
                SetMonMoveSlot(&gPlayerParty[MULTI_PARTY_SIZE + destination_party_index], move, move_index);
        }

        maxHp = GetMonData(&gPlayerParty[MULTI_PARTY_SIZE + destination_party_index], MON_DATA_MAX_HP, NULL);
        hp = min((u32)profile->party_hp[party_mon_index], maxHp);
        MultiplayerCommit_WriteMonData(&gPlayerParty[MULTI_PARTY_SIZE + destination_party_index], MON_DATA_HP, &hp);
        MultiplayerCommit_WriteMonData(&gPlayerParty[MULTI_PARTY_SIZE + destination_party_index], MON_DATA_OT_NAME, profile->player_name);
        otGender = profile->trainer_gender;
        MultiplayerCommit_WriteMonData(&gPlayerParty[MULTI_PARTY_SIZE + destination_party_index], MON_DATA_OT_GENDER, &otGender);
        CalculateMonStats(&gPlayerParty[MULTI_PARTY_SIZE + destination_party_index]);
        destination_party_index++;
    }

    if (destination_party_index == 0)
        return FALSE;

    sPvePartnerPlayerId = partner_player_id;
    sPvePartnerGender = profile->trainer_gender;
    CopyBattleProfilePartnerName(profile);
    return TRUE;
#else
    (void)partner_player_id;
    return FALSE;
#endif
}

bool8 MultiplayerBattle_StartTrainerPveBattle(u8 partner_player_id, u16 trainer_id)
{
#if FEATURE_MULTIPLAYER
    struct MultiplayerBattleRequest request;
    const struct MultiplayerSession *session;
    const struct MultiplayerSubsession *subsession;

    if (sMultiplayerBattleActive || sPendingPvpBattle || sPendingPveBattle)
        return FALSE;

    session = MultiplayerSession_Get();
    if (session == NULL || session->local_player_id >= MAX_NET_PLAYERS || partner_player_id >= MAX_NET_PLAYERS)
        return FALSE;
    if (!MultiplayerBattle_RemotePlayerCanPartner(partner_player_id))
        return FALSE;

    memset(&request, 0, sizeof(request));
    request.type = MULTIPLAYER_SUBSESSION_PVE_BATTLE;
    request.player_count = MAX_NET_PVE_PLAYERS;
    request.players[0] = session->local_player_id;
    request.players[1] = partner_player_id;
    request.battle_type_flags = BATTLE_TYPE_DOUBLE | BATTLE_TYPE_TRAINER;
    request.trainer_a = trainer_id;
    if (!MultiplayerBattle_Start(&request))
        return FALSE;

    subsession = FindLocalBattleSubsessionByType(MultiplayerSession_Get(), MULTIPLAYER_SUBSESSION_PVE_BATTLE);
    if (subsession == NULL)
        return FALSE;

    sPendingPveBattle = TRUE;
    sPendingSubsessionId = subsession->subsession_id;
    sStartedSubsessionId = subsession->subsession_id;
    sPvePartnerPlayerId = partner_player_id;
    return TRUE;
#else
    (void)partner_player_id;
    (void)trainer_id;
    return FALSE;
#endif
}

void MultiplayerBattle_CancelPendingTrainerPveBattle(void)
{
#if FEATURE_MULTIPLAYER
    if (sPendingPveBattle && sPendingSubsessionId != NET_SUBSESSION_NONE)
        MultiplayerSession_EndSubsession(sPendingSubsessionId, MULTIPLAYER_SUBSESSION_STATE_ERROR);

    sPendingPveBattle = FALSE;
    sPendingSubsessionId = NET_SUBSESSION_NONE;
    ResetPvePartner();
#endif
}

bool8 MultiplayerBattle_IsTrainerPvePartnerBattle(void)
{
#if FEATURE_MULTIPLAYER
    return sPvePartnerPlayerId != NET_PLAYER_NONE
        && (sPendingPveBattle || sMultiplayerBattleActive)
        && !BattleSession_IsSecretBaseBattle();
#else
    return FALSE;
#endif
}

u8 MultiplayerBattle_GetPartnerTrainerBackPicId(void)
{
#if FEATURE_MULTIPLAYER
    if (sPvePartnerGender == FEMALE)
        return TRAINER_BACK_PIC_MAY;
    return TRAINER_BACK_PIC_BRENDAN;
#else
    return TRAINER_BACK_PIC_BRENDAN;
#endif
}

const u8 *MultiplayerBattle_GetPartnerName(void)
{
#if FEATURE_MULTIPLAYER
    if (sPvePartnerName[0] != EOS)
        return sPvePartnerName;
#endif
    return sFallbackPartnerName;
}

static u8 GetAveragePartyMonEvs(struct Pokemon *party_mon)
{
    u8 statId;
    u16 sum = 0;

    for (statId = 0; statId < NUM_STATS; statId++)
        sum += GetMonData(party_mon, MON_DATA_HP_EV + statId, NULL);

    return sum / NUM_STATS;
}

static bool8 BuildMirrorSecretBaseBattle(void)
{
    u8 source_party_index;
    u8 move_index;
    u8 secret_base_party_index = 0;

    if (gSaveBlock2Ptr == NULL)
        return FALSE;

    memset(&sPendingSecretBase, 0, sizeof(sPendingSecretBase));
    sPendingSecretBase.secretBaseId = 1;
    sPendingSecretBase.gender = gSaveBlock2Ptr->playerGender;
    sPendingSecretBase.language = gGameLanguage;
    memcpy(sPendingSecretBase.trainerName, gSaveBlock2Ptr->playerName, PLAYER_NAME_LENGTH);
    memcpy(sPendingSecretBase.trainerId, gSaveBlock2Ptr->playerTrainerId, TRAINER_ID_LENGTH);

    for (source_party_index = 0; source_party_index < PARTY_SIZE && secret_base_party_index < PARTY_SIZE; source_party_index++)
    {
        u32 species = GetMonData(&gPlayerParty[source_party_index], MON_DATA_SPECIES_OR_EGG, NULL);

        if (species == SPECIES_NONE || species == SPECIES_EGG)
            continue;

        sPendingSecretBase.party.species[secret_base_party_index] = species;
        sPendingSecretBase.party.heldItems[secret_base_party_index] = GetMonData(&gPlayerParty[source_party_index], MON_DATA_HELD_ITEM, NULL);
        sPendingSecretBase.party.levels[secret_base_party_index] = GetMonData(&gPlayerParty[source_party_index], MON_DATA_LEVEL, NULL);
        sPendingSecretBase.party.personality[secret_base_party_index] = GetMonData(&gPlayerParty[source_party_index], MON_DATA_PERSONALITY, NULL);
        sPendingSecretBase.party.EVs[secret_base_party_index] = GetAveragePartyMonEvs(&gPlayerParty[source_party_index]);

        for (move_index = 0; move_index < MAX_MON_MOVES; move_index++)
            sPendingSecretBase.party.moves[secret_base_party_index * MAX_MON_MOVES + move_index] = GetMonData(&gPlayerParty[source_party_index], MON_DATA_MOVE1 + move_index, NULL);

        secret_base_party_index++;
    }

    return secret_base_party_index != 0;
}

static void TryStartLocalBattleForSubsession(const struct MultiplayerSubsession *subsession)
{
    if (subsession == NULL)
        return;
    if (sMultiplayerBattleActive || sPendingPvpBattle || sPendingPveBattle)
        return;
    if (sStartedSubsessionId == subsession->subsession_id)
        return;
    if (!MultiplayerOverworld_CanTick())
        return;
    if (!PlayerHasUsableParty() || !BuildMirrorSecretBaseBattle())
    {
        sStartedSubsessionId = subsession->subsession_id;
        MultiplayerSession_EndSubsession(subsession->subsession_id, MULTIPLAYER_SUBSESSION_STATE_ERROR);
        return;
    }

    sPendingPvpBattle = TRUE;
    sPendingSubsessionId = subsession->subsession_id;
    sStartedSubsessionId = subsession->subsession_id;
    MultiplayerInteractionMenu_Reset();
    BattleSetup_StartMultiplayerPvpBattle();
}
#endif

#if !FEATURE_MULTIPLAYER
bool8 MultiplayerBattle_LocalPlayerCanFight(void)
{
    return FALSE;
}

bool8 MultiplayerBattle_RemotePlayerCanPartner(u8 partner_player_id)
{
    (void)partner_player_id;
    return FALSE;
}

bool8 MultiplayerBattle_PrepareTrainerPvePartnerParty(u8 partner_player_id)
{
    (void)partner_player_id;
    return FALSE;
}

bool8 MultiplayerBattle_StartTrainerPveBattle(u8 partner_player_id, u16 trainer_id)
{
    (void)partner_player_id;
    (void)trainer_id;
    return FALSE;
}

void MultiplayerBattle_CancelPendingTrainerPveBattle(void)
{
}

bool8 MultiplayerBattle_IsTrainerPvePartnerBattle(void)
{
    return FALSE;
}

u8 MultiplayerBattle_GetPartnerTrainerBackPicId(void)
{
    return TRAINER_BACK_PIC_BRENDAN;
}

const u8 *MultiplayerBattle_GetPartnerName(void)
{
    return sFallbackPartnerName;
}
#endif

void MultiplayerBattle_Tick(const struct MultiplayerSession *session)
{
#if FEATURE_MULTIPLAYER
    const struct MultiplayerSubsession *subsession;

    if (!MultiplayerSession_IsOnline())
        return;
    if (sMultiplayerBattleActive || sPendingPvpBattle || sPendingPveBattle)
        return;

    subsession = FindLocalPvpSubsession(session);
    if (subsession == NULL)
    {
        sStartedSubsessionId = NET_SUBSESSION_NONE;
        return;
    }

    TryStartLocalBattleForSubsession(subsession);
#else
    (void)session;
#endif
}

bool8 MultiplayerBattle_TryCreateSecretBaseEnemyParty(void)
{
#if FEATURE_MULTIPLAYER
    if (!sPendingPvpBattle)
        return FALSE;

    CreateSecretBaseEnemyParty(&sPendingSecretBase);
    return TRUE;
#else
    return FALSE;
#endif
}

void MultiplayerBattle_ApplyCurrentWeather(void)
{
#if FEATURE_MULTIPLAYER
    if (!sMultiplayerBattleActive)
        return;
    if (!BattleSession_IsSecretBaseBattle())
        return;

    BattleSession_SetWeather(BattleSession_GetCurrentFieldBattleWeather());
#endif
}

bool8 MultiplayerBattle_SendAction(u8 subsession_id, u8 battler_slot, u8 action, u8 target, u16 parameter)
{
#if FEATURE_MULTIPLAYER
    struct NetBattleAction battle_action_packet;
    struct MultiplayerTransactionKey transaction_key;
    const struct MultiplayerSession *session;
    u32 action_sequence;

    if (!MultiplayerSession_IsOnline())
        return FALSE;
    if (subsession_id == NET_SUBSESSION_NONE || subsession_id > MAX_NET_SUBSESSIONS)
        return FALSE;

    session = MultiplayerSession_Get();
    if (session == NULL)
        return FALSE;

    memset(&battle_action_packet, 0, sizeof(battle_action_packet));
    action_sequence = MultiplayerSession_NextActionSequence();
    if (!MultiplayerSession_BuildTransactionKey(&transaction_key, NET_PACKET_BATTLE_ACTION, subsession_id, action_sequence))
        return FALSE;
    battle_action_packet.header.client_frame = session->local_client_frame;
    battle_action_packet.header.server_tick_seen = session->bridge_tick;
    battle_action_packet.header.action_sequence = action_sequence;
    battle_action_packet.header.transaction_id = MultiplayerCommit_GetTransactionId(&transaction_key);
    battle_action_packet.subsession_id = subsession_id;
    battle_action_packet.battler_slot = battler_slot;
    battle_action_packet.action = action;
    battle_action_packet.target = target;
    battle_action_packet.parameter = parameter;
    MultiplayerCommit_Prepare(&transaction_key, MULTIPLAYER_COMMIT_BATTLE, &battle_action_packet, sizeof(battle_action_packet), NULL);
    if (!NetTransport_SendPacket(NET_PACKET_BATTLE_ACTION, &battle_action_packet, sizeof(battle_action_packet)))
    {
        MultiplayerCommit_Rollback(&transaction_key, MULTIPLAYER_COMMIT_BATTLE, &battle_action_packet, sizeof(battle_action_packet), NULL);
        return FALSE;
    }

    return TRUE;
#else
    return FALSE;
#endif
}

void MultiplayerBattle_OnBattleStart(u32 battle_type_flags)
{
#if FEATURE_MULTIPLAYER
    if (!MultiplayerSession_IsOnline())
        return;
    if (sMultiplayerBattleActive)
        return;
    if (sPendingPvpBattle)
    {
        sMultiplayerBattleActive = TRUE;
        sActiveSubsessionId = sPendingSubsessionId;
        sPendingSubsessionId = NET_SUBSESSION_NONE;
        sPendingPvpBattle = FALSE;
        ResetPvePartner();
        return;
    }
    if (!sPendingPveBattle)
        return;
    if (!(battle_type_flags & BATTLE_TYPE_TRAINER))
    {
        if (sPendingSubsessionId != NET_SUBSESSION_NONE)
            MultiplayerSession_EndSubsession(sPendingSubsessionId, MULTIPLAYER_SUBSESSION_STATE_ERROR);
        sPendingPveBattle = FALSE;
        sPendingSubsessionId = NET_SUBSESSION_NONE;
        ResetPvePartner();
        return;
    }

    sMultiplayerBattleActive = TRUE;
    sActiveSubsessionId = sPendingSubsessionId;
    sPendingSubsessionId = NET_SUBSESSION_NONE;
    sPendingPveBattle = FALSE;
#else
    (void)battle_type_flags;
#endif
}

void MultiplayerBattle_OnBattleEnd(u32 battle_outcome)
{
#if FEATURE_MULTIPLAYER
    (void)battle_outcome;

    if (!sMultiplayerBattleActive)
        return;

    if (sActiveSubsessionId != NET_SUBSESSION_NONE)
        MultiplayerSession_EndSubsession(sActiveSubsessionId, MULTIPLAYER_SUBSESSION_STATE_ENDING);
    sMultiplayerBattleActive = FALSE;
    sPendingPvpBattle = FALSE;
    sPendingPveBattle = FALSE;
    sPendingSubsessionId = NET_SUBSESSION_NONE;
    sActiveSubsessionId = NET_SUBSESSION_NONE;
    sStartedSubsessionId = NET_SUBSESSION_NONE;
    ResetPvePartner();
#else
    (void)battle_outcome;
#endif
}
