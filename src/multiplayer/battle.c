#include "global.h"
#include "battle.h"
#include "battle_setup.h"
#include "field_weather.h"
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

static bool8 SnapshotPartyMonIsUsable(const struct NetPlayerSnapshot *snapshot, u8 partyId)
{
    u16 species;

    if (snapshot == NULL || partyId >= NET_PLAYER_PARTY_SNAPSHOT_SIZE)
        return FALSE;
    if (partyId >= snapshot->partyCount)
        return FALSE;

    species = snapshot->partySpecies[partyId];
    return species != SPECIES_NONE
        && species != SPECIES_EGG
        && species < NUM_SPECIES
        && snapshot->partyLevels[partyId] != 0
        && snapshot->partyLevels[partyId] <= MAX_LEVEL
        && snapshot->partyHp[partyId] != 0;
}

static bool8 SnapshotHasUsablePartnerParty(const struct NetPlayerSnapshot *snapshot)
{
    u8 i;

    if (snapshot == NULL || snapshot->partyCount == 0)
        return FALSE;

    for (i = 0; i < NET_PLAYER_PARTY_SNAPSHOT_SIZE; i++)
    {
        if (SnapshotPartyMonIsUsable(snapshot, i))
            return TRUE;
    }

    return FALSE;
}

static void CopySnapshotPartnerName(const struct NetPlayerSnapshot *snapshot)
{
    u8 i;

    for (i = 0; i < PLAYER_NAME_LENGTH && snapshot->playerName[i] != EOS && snapshot->playerName[i] != 0; i++)
        sPvePartnerName[i] = snapshot->playerName[i];

    if (i == 0)
    {
        sPvePartnerName[0] = CHAR_P;
        sPvePartnerName[1] = CHAR_A;
        sPvePartnerName[2] = CHAR_R;
        sPvePartnerName[3] = CHAR_T;
        sPvePartnerName[4] = CHAR_N;
        sPvePartnerName[5] = CHAR_E;
        sPvePartnerName[6] = CHAR_R;
        i = PLAYER_NAME_LENGTH;
    }

    sPvePartnerName[i] = EOS;
}
#endif

#if FEATURE_MULTIPLAYER
static bool8 RequestHasUniquePlayers(const struct MultiplayerBattleRequest *request)
{
    u8 i;
    u8 j;

    for (i = 0; i < request->playerCount; i++)
    {
        if (request->players[i] >= MAX_NET_PLAYERS)
            return FALSE;

        for (j = i + 1; j < request->playerCount; j++)
        {
            if (request->players[i] == request->players[j])
                return FALSE;
        }
    }

    return TRUE;
}

static bool8 PlayerBlocksBattleRequest(u8 playerId)
{
    const struct MultiplayerSession *session;

    if (playerId >= MAX_NET_PLAYERS)
        return TRUE;
    if (MultiplayerSession_IsPlayerInSubsession(playerId))
        return TRUE;
    if (!MultiplayerSession_IsPlayerBusy(playerId))
        return FALSE;

    session = MultiplayerSession_Get();
    if (session == NULL || !session->players[playerId].active)
        return TRUE;

    return session->players[playerId].interactionState != MULTIPLAYER_INTERACTION_OPTIONS_MENU;
}
#endif

bool8 MultiplayerBattle_CanStart(const struct MultiplayerBattleRequest *request)
{
#if FEATURE_MULTIPLAYER
    u8 i;

    if (request == NULL)
        return FALSE;
    if (!MultiplayerSession_IsOnline())
        return FALSE;
    if (request->playerCount == 0 || request->playerCount > MAX_NET_BATTLE_PLAYERS)
        return FALSE;
    if (!RequestHasUniquePlayers(request))
        return FALSE;
    for (i = 0; i < request->playerCount; i++)
    {
        if (PlayerBlocksBattleRequest(request->players[i]))
            return FALSE;
    }

    switch (request->type)
    {
    case MULTIPLAYER_SUBSESSION_PVE_BATTLE:
        return request->playerCount > 0
            && request->playerCount <= MAX_NET_PVE_PLAYERS
            && MultiplayerSession_ArePlayersWithinRange(request->playerCount, request->players, NET_PLAYER_PVE_BATTLE_RANGE_TILES);
    case MULTIPLAYER_SUBSESSION_PVP_BATTLE:
        return request->playerCount == MAX_NET_PVP_PLAYERS
            && MultiplayerSession_ArePlayersWithinRange(request->playerCount, request->players, NET_PLAYER_INTERACTION_RANGE_TILES);
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

    return MultiplayerSession_StartSubsession(request->type, request->playerCount, request->players);
#else
    return FALSE;
#endif
}

#if FEATURE_MULTIPLAYER
static bool8 SubsessionIncludesPlayer(const struct MultiplayerSubsession *subsession, u8 playerId)
{
    u8 i;

    if (subsession == NULL || !subsession->active)
        return FALSE;
    if (playerId >= MAX_NET_PLAYERS)
        return FALSE;

    for (i = 0; i < subsession->playerCount; i++)
    {
        if (subsession->players[i] == playerId)
            return TRUE;
    }

    return FALSE;
}

static const struct MultiplayerSubsession *FindLocalBattleSubsessionByType(const struct MultiplayerSession *session, u8 type)
{
    u8 i;

    if (session == NULL || session->localPlayerId >= MAX_NET_PLAYERS)
        return NULL;

    for (i = 0; i < MAX_NET_SUBSESSIONS; i++)
    {
        const struct MultiplayerSubsession *subsession = &session->subsessions[i];

        if (!subsession->active)
            continue;
        if (subsession->type != type)
            continue;
        if (!SubsessionStateCanStartBattle(subsession->state))
            continue;
        if (!SubsessionIncludesPlayer(subsession, session->localPlayerId))
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
    u8 i;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        u32 species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES_OR_EGG, NULL);

        if (species != SPECIES_NONE
         && species != SPECIES_EGG
         && GetMonData(&gPlayerParty[i], MON_DATA_HP, NULL) != 0)
            return TRUE;
    }

    return FALSE;
}

bool8 MultiplayerBattle_LocalPlayerCanFight(void)
{
    return PlayerHasUsableParty();
}

bool8 MultiplayerBattle_RemotePlayerCanPartner(u8 partnerPlayerId)
{
#if FEATURE_MULTIPLAYER
    const struct MultiplayerSession *session;

    if (!MultiplayerSession_IsPlayerActive(partnerPlayerId))
        return FALSE;

    session = MultiplayerSession_Get();
    if (session == NULL)
        return FALSE;

    return SnapshotHasUsablePartnerParty(&session->players[partnerPlayerId]);
#else
    (void)partnerPlayerId;
    return FALSE;
#endif
}

bool8 MultiplayerBattle_PrepareTrainerPvePartnerParty(u8 partnerPlayerId)
{
#if FEATURE_MULTIPLAYER
    const struct MultiplayerSession *session;
    const struct NetPlayerSnapshot *snapshot;
    u8 i;
    u8 moveSlot;
    u8 dst = 0;

    if (!MultiplayerBattle_RemotePlayerCanPartner(partnerPlayerId))
        return FALSE;

    session = MultiplayerSession_Get();
    if (session == NULL)
        return FALSE;
    snapshot = &session->players[partnerPlayerId];

    for (i = 0; i < NET_PLAYER_PARTY_SNAPSHOT_SIZE; i++)
        ZeroMonData(&gPlayerParty[MULTI_PARTY_SIZE + i]);

    for (i = 0; i < NET_PLAYER_PARTY_SNAPSHOT_SIZE && dst < NET_PLAYER_PARTY_SNAPSHOT_SIZE; i++)
    {
        u32 maxHp;
        u32 hp;
        u32 otGender;

        if (!SnapshotPartyMonIsUsable(snapshot, i))
            continue;

        CreateMon(&gPlayerParty[MULTI_PARTY_SIZE + dst],
                  snapshot->partySpecies[i],
                  snapshot->partyLevels[i],
                  USE_RANDOM_IVS,
                  FALSE,
                  0,
                  OT_ID_RANDOM_NO_SHINY,
                  0);

        SetMonData(&gPlayerParty[MULTI_PARTY_SIZE + dst], MON_DATA_HELD_ITEM, &snapshot->partyHeldItems[i]);
        for (moveSlot = 0; moveSlot < MAX_MON_MOVES; moveSlot++)
        {
            u16 move = snapshot->partyMoves[i][moveSlot];

            if (move != MOVE_NONE && move < MOVES_COUNT)
                SetMonMoveSlot(&gPlayerParty[MULTI_PARTY_SIZE + dst], move, moveSlot);
        }

        maxHp = GetMonData(&gPlayerParty[MULTI_PARTY_SIZE + dst], MON_DATA_MAX_HP, NULL);
        hp = min((u32)snapshot->partyHp[i], maxHp);
        SetMonData(&gPlayerParty[MULTI_PARTY_SIZE + dst], MON_DATA_HP, &hp);
        SetMonData(&gPlayerParty[MULTI_PARTY_SIZE + dst], MON_DATA_OT_NAME, snapshot->playerName);
        otGender = snapshot->trainerGender;
        SetMonData(&gPlayerParty[MULTI_PARTY_SIZE + dst], MON_DATA_OT_GENDER, &otGender);
        CalculateMonStats(&gPlayerParty[MULTI_PARTY_SIZE + dst]);
        dst++;
    }

    if (dst == 0)
        return FALSE;

    sPvePartnerPlayerId = partnerPlayerId;
    sPvePartnerGender = snapshot->trainerGender;
    CopySnapshotPartnerName(snapshot);
    return TRUE;
#else
    (void)partnerPlayerId;
    return FALSE;
#endif
}

bool8 MultiplayerBattle_StartTrainerPveBattle(u8 partnerPlayerId, u16 trainerId)
{
#if FEATURE_MULTIPLAYER
    struct MultiplayerBattleRequest request;
    const struct MultiplayerSession *session;
    const struct MultiplayerSubsession *subsession;

    if (sMultiplayerBattleActive || sPendingPvpBattle || sPendingPveBattle)
        return FALSE;

    session = MultiplayerSession_Get();
    if (session == NULL || session->localPlayerId >= MAX_NET_PLAYERS || partnerPlayerId >= MAX_NET_PLAYERS)
        return FALSE;
    if (!MultiplayerBattle_RemotePlayerCanPartner(partnerPlayerId))
        return FALSE;

    memset(&request, 0, sizeof(request));
    request.type = MULTIPLAYER_SUBSESSION_PVE_BATTLE;
    request.playerCount = MAX_NET_PVE_PLAYERS;
    request.players[0] = session->localPlayerId;
    request.players[1] = partnerPlayerId;
    request.battleTypeFlags = BATTLE_TYPE_DOUBLE | BATTLE_TYPE_TRAINER;
    request.trainerA = trainerId;
    if (!MultiplayerBattle_Start(&request))
        return FALSE;

    subsession = FindLocalBattleSubsessionByType(MultiplayerSession_Get(), MULTIPLAYER_SUBSESSION_PVE_BATTLE);
    if (subsession == NULL)
        return FALSE;

    sPendingPveBattle = TRUE;
    sPendingSubsessionId = subsession->id;
    sStartedSubsessionId = subsession->id;
    sPvePartnerPlayerId = partnerPlayerId;
    return TRUE;
#else
    (void)partnerPlayerId;
    (void)trainerId;
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
        && !(gBattleTypeFlags & BATTLE_TYPE_SECRET_BASE);
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

static u16 GetCurrentFieldBattleWeather(void)
{
    switch (GetCurrentWeather())
    {
    case WEATHER_RAIN:
    case WEATHER_RAIN_THUNDERSTORM:
    case WEATHER_DOWNPOUR:
        return B_WEATHER_RAIN_PERMANENT;
    case WEATHER_SANDSTORM:
    case WEATHER_VOLCANIC_ASH:
        return B_WEATHER_SANDSTORM_PERMANENT;
    case WEATHER_SUNNY:
    case WEATHER_DROUGHT:
        return B_WEATHER_SUN_PERMANENT;
    case WEATHER_SNOW:
        return B_WEATHER_HAIL_TEMPORARY;
    default:
        return 0;
    }
}

static u8 GetAveragePartyMonEvs(struct Pokemon *mon)
{
    u8 statId;
    u16 sum = 0;

    for (statId = 0; statId < NUM_STATS; statId++)
        sum += GetMonData(mon, MON_DATA_HP_EV + statId, NULL);

    return sum / NUM_STATS;
}

static bool8 BuildMirrorSecretBaseBattle(void)
{
    u8 i;
    u8 j;
    u8 partyId = 0;

    if (gSaveBlock2Ptr == NULL)
        return FALSE;

    memset(&sPendingSecretBase, 0, sizeof(sPendingSecretBase));
    sPendingSecretBase.secretBaseId = 1;
    sPendingSecretBase.gender = gSaveBlock2Ptr->playerGender;
    sPendingSecretBase.language = gGameLanguage;
    memcpy(sPendingSecretBase.trainerName, gSaveBlock2Ptr->playerName, PLAYER_NAME_LENGTH);
    memcpy(sPendingSecretBase.trainerId, gSaveBlock2Ptr->playerTrainerId, TRAINER_ID_LENGTH);

    for (i = 0; i < PARTY_SIZE && partyId < PARTY_SIZE; i++)
    {
        u32 species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES_OR_EGG, NULL);

        if (species == SPECIES_NONE || species == SPECIES_EGG)
            continue;

        sPendingSecretBase.party.species[partyId] = species;
        sPendingSecretBase.party.heldItems[partyId] = GetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM, NULL);
        sPendingSecretBase.party.levels[partyId] = GetMonData(&gPlayerParty[i], MON_DATA_LEVEL, NULL);
        sPendingSecretBase.party.personality[partyId] = GetMonData(&gPlayerParty[i], MON_DATA_PERSONALITY, NULL);
        sPendingSecretBase.party.EVs[partyId] = GetAveragePartyMonEvs(&gPlayerParty[i]);

        for (j = 0; j < MAX_MON_MOVES; j++)
            sPendingSecretBase.party.moves[partyId * MAX_MON_MOVES + j] = GetMonData(&gPlayerParty[i], MON_DATA_MOVE1 + j, NULL);

        partyId++;
    }

    return partyId != 0;
}

static void TryStartLocalBattleForSubsession(const struct MultiplayerSubsession *subsession)
{
    if (subsession == NULL)
        return;
    if (sMultiplayerBattleActive || sPendingPvpBattle || sPendingPveBattle)
        return;
    if (sStartedSubsessionId == subsession->id)
        return;
    if (!MultiplayerOverworld_CanTick())
        return;
    if (!PlayerHasUsableParty() || !BuildMirrorSecretBaseBattle())
    {
        sStartedSubsessionId = subsession->id;
        MultiplayerSession_EndSubsession(subsession->id, MULTIPLAYER_SUBSESSION_STATE_ERROR);
        return;
    }

    sPendingPvpBattle = TRUE;
    sPendingSubsessionId = subsession->id;
    sStartedSubsessionId = subsession->id;
    MultiplayerInteractionMenu_Reset();
    BattleSetup_StartMultiplayerPvpBattle();
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
    if (!(gBattleTypeFlags & BATTLE_TYPE_SECRET_BASE))
        return;

    gBattleWeather = GetCurrentFieldBattleWeather();
#endif
}

bool8 MultiplayerBattle_SendAction(u8 subsessionId, u8 battlerSlot, u8 action, u8 target, u16 parameter)
{
#if FEATURE_MULTIPLAYER
    struct NetBattleAction packet;
    struct MultiplayerTransactionKey key;
    const struct MultiplayerSession *session;
    u32 actionSequence;

    if (!MultiplayerSession_IsOnline())
        return FALSE;
    if (subsessionId == NET_SUBSESSION_NONE || subsessionId > MAX_NET_SUBSESSIONS)
        return FALSE;

    session = MultiplayerSession_Get();
    if (session == NULL)
        return FALSE;

    memset(&packet, 0, sizeof(packet));
    actionSequence = MultiplayerSession_NextActionSequence();
    if (!MultiplayerSession_BuildTransactionKey(&key, NET_PACKET_BATTLE_ACTION, subsessionId, actionSequence))
        return FALSE;
    packet.header.clientFrame = session->localClientFrame;
    packet.header.serverTickSeen = session->bridgeTick;
    packet.header.actionSequence = actionSequence;
    packet.header.transactionId = MultiplayerCommit_GetTransactionId(&key);
    packet.subsessionId = subsessionId;
    packet.battlerSlot = battlerSlot;
    packet.action = action;
    packet.target = target;
    packet.parameter = parameter;
    MultiplayerCommit_Prepare(&key, MULTIPLAYER_COMMIT_BATTLE, &packet, sizeof(packet), NULL);
    if (!NetTransport_SendPacket(NET_PACKET_BATTLE_ACTION, &packet, sizeof(packet)))
    {
        MultiplayerCommit_Rollback(&key, MULTIPLAYER_COMMIT_BATTLE, &packet, sizeof(packet), NULL);
        return FALSE;
    }

    return TRUE;
#else
    return FALSE;
#endif
}

void MultiplayerBattle_OnBattleStart(u32 battleTypeFlags)
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
    if (!(battleTypeFlags & BATTLE_TYPE_TRAINER))
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
    (void)battleTypeFlags;
#endif
}

void MultiplayerBattle_OnBattleEnd(u32 battleOutcome)
{
#if FEATURE_MULTIPLAYER
    (void)battleOutcome;

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
    (void)battleOutcome;
#endif
}
