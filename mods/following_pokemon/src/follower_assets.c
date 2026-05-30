// Generated follower graphics data for mods/following_pokemon.
// Reimport source assets with tools/import_from_pokeemerald_follow.py and
// verify registry consistency with tools/validate_following_pokemon.py.

#include "mod_sdk.h"
#include "constants/event_objects.h"
#include "event_object_movement.h"
#include "global.fieldmap.h"
#include "sprite.h"

extern const struct OamData gObjectEventBaseOam_64x64;

#define MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE 0x11FF

static const union AnimCmd sFollowerAnimFaceSouth[] = {
    ANIMCMD_FRAME(0, 14),
    ANIMCMD_FRAME(0, 14),
    ANIMCMD_FRAME(1, 14),
    ANIMCMD_FRAME(1, 14),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimFaceNorth[] = {
    ANIMCMD_FRAME(2, 14),
    ANIMCMD_FRAME(2, 14),
    ANIMCMD_FRAME(3, 14),
    ANIMCMD_FRAME(3, 14),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimFaceWest[] = {
    ANIMCMD_FRAME(4, 14),
    ANIMCMD_FRAME(4, 14),
    ANIMCMD_FRAME(5, 14),
    ANIMCMD_FRAME(5, 14),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimFaceEast[] = {
    ANIMCMD_FRAME(4, 14, .hFlip = TRUE),
    ANIMCMD_FRAME(4, 14, .hFlip = TRUE),
    ANIMCMD_FRAME(5, 14, .hFlip = TRUE),
    ANIMCMD_FRAME(5, 14, .hFlip = TRUE),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimWalkSouth[] = {
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_FRAME(1, 8),
    ANIMCMD_FRAME(1, 8),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimWalkNorth[] = {
    ANIMCMD_FRAME(2, 8),
    ANIMCMD_FRAME(2, 8),
    ANIMCMD_FRAME(3, 8),
    ANIMCMD_FRAME(3, 8),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimWalkWest[] = {
    ANIMCMD_FRAME(4, 8),
    ANIMCMD_FRAME(4, 8),
    ANIMCMD_FRAME(5, 8),
    ANIMCMD_FRAME(5, 8),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimWalkEast[] = {
    ANIMCMD_FRAME(4, 8, .hFlip = TRUE),
    ANIMCMD_FRAME(4, 8, .hFlip = TRUE),
    ANIMCMD_FRAME(5, 8, .hFlip = TRUE),
    ANIMCMD_FRAME(5, 8, .hFlip = TRUE),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimWalkSouthFast[] = {
    ANIMCMD_FRAME(0, 6),
    ANIMCMD_FRAME(0, 6),
    ANIMCMD_FRAME(1, 6),
    ANIMCMD_FRAME(1, 6),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimWalkNorthFast[] = {
    ANIMCMD_FRAME(2, 6),
    ANIMCMD_FRAME(2, 6),
    ANIMCMD_FRAME(3, 6),
    ANIMCMD_FRAME(3, 6),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimWalkWestFast[] = {
    ANIMCMD_FRAME(4, 6),
    ANIMCMD_FRAME(4, 6),
    ANIMCMD_FRAME(5, 6),
    ANIMCMD_FRAME(5, 6),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimWalkEastFast[] = {
    ANIMCMD_FRAME(4, 6, .hFlip = TRUE),
    ANIMCMD_FRAME(4, 6, .hFlip = TRUE),
    ANIMCMD_FRAME(5, 6, .hFlip = TRUE),
    ANIMCMD_FRAME(5, 6, .hFlip = TRUE),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimFaceEastUnique[] = {
    ANIMCMD_FRAME(6, 14),
    ANIMCMD_FRAME(6, 14),
    ANIMCMD_FRAME(7, 14),
    ANIMCMD_FRAME(7, 14),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimWalkEastUnique[] = {
    ANIMCMD_FRAME(6, 8),
    ANIMCMD_FRAME(6, 8),
    ANIMCMD_FRAME(7, 8),
    ANIMCMD_FRAME(7, 8),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimWalkEastFastUnique[] = {
    ANIMCMD_FRAME(6, 6),
    ANIMCMD_FRAME(6, 6),
    ANIMCMD_FRAME(7, 6),
    ANIMCMD_FRAME(7, 6),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimFaceNorthDouble[] = {
    ANIMCMD_FRAME(0, 14, .hFlip = TRUE),
    ANIMCMD_FRAME(0, 14, .hFlip = TRUE),
    ANIMCMD_FRAME(1, 14, .hFlip = TRUE),
    ANIMCMD_FRAME(1, 14, .hFlip = TRUE),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimFaceWestDouble[] = {
    ANIMCMD_FRAME(2, 14),
    ANIMCMD_FRAME(2, 14),
    ANIMCMD_FRAME(3, 14),
    ANIMCMD_FRAME(3, 14),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimFaceEastUniqueDouble[] = {
    ANIMCMD_FRAME(4, 14),
    ANIMCMD_FRAME(4, 14),
    ANIMCMD_FRAME(5, 14),
    ANIMCMD_FRAME(5, 14),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimWalkNorthDouble[] = {
    ANIMCMD_FRAME(0, 8, .hFlip = TRUE),
    ANIMCMD_FRAME(0, 8, .hFlip = TRUE),
    ANIMCMD_FRAME(1, 8, .hFlip = TRUE),
    ANIMCMD_FRAME(1, 8, .hFlip = TRUE),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimWalkWestDouble[] = {
    ANIMCMD_FRAME(2, 8),
    ANIMCMD_FRAME(2, 8),
    ANIMCMD_FRAME(3, 8),
    ANIMCMD_FRAME(3, 8),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimWalkEastUniqueDouble[] = {
    ANIMCMD_FRAME(4, 8),
    ANIMCMD_FRAME(4, 8),
    ANIMCMD_FRAME(5, 8),
    ANIMCMD_FRAME(5, 8),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimWalkNorthFastDouble[] = {
    ANIMCMD_FRAME(0, 6, .hFlip = TRUE),
    ANIMCMD_FRAME(0, 6, .hFlip = TRUE),
    ANIMCMD_FRAME(1, 6, .hFlip = TRUE),
    ANIMCMD_FRAME(1, 6, .hFlip = TRUE),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimWalkWestFastDouble[] = {
    ANIMCMD_FRAME(2, 6),
    ANIMCMD_FRAME(2, 6),
    ANIMCMD_FRAME(3, 6),
    ANIMCMD_FRAME(3, 6),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimWalkEastFastUniqueDouble[] = {
    ANIMCMD_FRAME(4, 6),
    ANIMCMD_FRAME(4, 6),
    ANIMCMD_FRAME(5, 6),
    ANIMCMD_FRAME(5, 6),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimRayquazaFaceSouth[] = {
    ANIMCMD_FRAME(0, 28),
    ANIMCMD_FRAME(1, 28),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimRayquazaFaceNorth[] = {
    ANIMCMD_FRAME(2, 28),
    ANIMCMD_FRAME(3, 28),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimRayquazaFaceWest[] = {
    ANIMCMD_FRAME(4, 28),
    ANIMCMD_FRAME(5, 28),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimRayquazaFaceEast[] = {
    ANIMCMD_FRAME(4, 28, .hFlip = TRUE),
    ANIMCMD_FRAME(5, 28, .hFlip = TRUE),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimRayquazaWalkSouth[] = {
    ANIMCMD_FRAME(0, 16),
    ANIMCMD_FRAME(1, 16),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimRayquazaWalkNorth[] = {
    ANIMCMD_FRAME(2, 16),
    ANIMCMD_FRAME(3, 16),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimRayquazaWalkWest[] = {
    ANIMCMD_FRAME(4, 16),
    ANIMCMD_FRAME(5, 16),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimRayquazaWalkEast[] = {
    ANIMCMD_FRAME(4, 16, .hFlip = TRUE),
    ANIMCMD_FRAME(5, 16, .hFlip = TRUE),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimRayquazaWalkSouthFast[] = {
    ANIMCMD_FRAME(0, 12),
    ANIMCMD_FRAME(1, 12),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimRayquazaWalkNorthFast[] = {
    ANIMCMD_FRAME(2, 12),
    ANIMCMD_FRAME(3, 12),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimRayquazaWalkWestFast[] = {
    ANIMCMD_FRAME(4, 12),
    ANIMCMD_FRAME(5, 12),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sFollowerAnimRayquazaWalkEastFast[] = {
    ANIMCMD_FRAME(4, 12, .hFlip = TRUE),
    ANIMCMD_FRAME(5, 12, .hFlip = TRUE),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd *const sFollowerAnimTableStandard[] = {
    sFollowerAnimFaceSouth,
    sFollowerAnimFaceNorth,
    sFollowerAnimFaceWest,
    sFollowerAnimFaceEast,
    sFollowerAnimWalkSouth,
    sFollowerAnimWalkNorth,
    sFollowerAnimWalkWest,
    sFollowerAnimWalkEast,
    sFollowerAnimWalkSouthFast,
    sFollowerAnimWalkNorthFast,
    sFollowerAnimWalkWestFast,
    sFollowerAnimWalkEastFast,
    sFollowerAnimWalkSouthFast,
    sFollowerAnimWalkNorthFast,
    sFollowerAnimWalkWestFast,
    sFollowerAnimWalkEastFast,
    sFollowerAnimWalkSouthFast,
    sFollowerAnimWalkNorthFast,
    sFollowerAnimWalkWestFast,
    sFollowerAnimWalkEastFast,
    sFollowerAnimWalkSouthFast,
    sFollowerAnimWalkNorthFast,
    sFollowerAnimWalkWestFast,
    sFollowerAnimWalkEastFast,
};

static const union AnimCmd *const sFollowerAnimTableUniqueEast[] = {
    sFollowerAnimFaceSouth,
    sFollowerAnimFaceNorth,
    sFollowerAnimFaceWest,
    sFollowerAnimFaceEastUnique,
    sFollowerAnimWalkSouth,
    sFollowerAnimWalkNorth,
    sFollowerAnimWalkWest,
    sFollowerAnimWalkEastUnique,
    sFollowerAnimWalkSouthFast,
    sFollowerAnimWalkNorthFast,
    sFollowerAnimWalkWestFast,
    sFollowerAnimWalkEastFastUnique,
    sFollowerAnimWalkSouthFast,
    sFollowerAnimWalkNorthFast,
    sFollowerAnimWalkWestFast,
    sFollowerAnimWalkEastFastUnique,
    sFollowerAnimWalkSouthFast,
    sFollowerAnimWalkNorthFast,
    sFollowerAnimWalkWestFast,
    sFollowerAnimWalkEastFastUnique,
    sFollowerAnimWalkSouthFast,
    sFollowerAnimWalkNorthFast,
    sFollowerAnimWalkWestFast,
    sFollowerAnimWalkEastFastUnique,
};

static const union AnimCmd *const sFollowerAnimTableUniqueEastDoubleNorth[] = {
    sFollowerAnimFaceSouth,
    sFollowerAnimFaceNorthDouble,
    sFollowerAnimFaceWestDouble,
    sFollowerAnimFaceEastUniqueDouble,
    sFollowerAnimWalkSouth,
    sFollowerAnimWalkNorthDouble,
    sFollowerAnimWalkWestDouble,
    sFollowerAnimWalkEastUniqueDouble,
    sFollowerAnimWalkSouthFast,
    sFollowerAnimWalkNorthFastDouble,
    sFollowerAnimWalkWestFastDouble,
    sFollowerAnimWalkEastFastUniqueDouble,
    sFollowerAnimWalkSouthFast,
    sFollowerAnimWalkNorthFastDouble,
    sFollowerAnimWalkWestFastDouble,
    sFollowerAnimWalkEastFastUniqueDouble,
    sFollowerAnimWalkSouthFast,
    sFollowerAnimWalkNorthFastDouble,
    sFollowerAnimWalkWestFastDouble,
    sFollowerAnimWalkEastFastUniqueDouble,
    sFollowerAnimWalkSouthFast,
    sFollowerAnimWalkNorthFastDouble,
    sFollowerAnimWalkWestFastDouble,
    sFollowerAnimWalkEastFastUniqueDouble,
};

static const union AnimCmd *const sFollowerAnimTableRayquaza[] = {
    sFollowerAnimRayquazaFaceSouth,
    sFollowerAnimRayquazaFaceNorth,
    sFollowerAnimRayquazaFaceWest,
    sFollowerAnimRayquazaFaceEast,
    sFollowerAnimRayquazaWalkSouth,
    sFollowerAnimRayquazaWalkNorth,
    sFollowerAnimRayquazaWalkWest,
    sFollowerAnimRayquazaWalkEast,
    sFollowerAnimRayquazaWalkSouthFast,
    sFollowerAnimRayquazaWalkNorthFast,
    sFollowerAnimRayquazaWalkWestFast,
    sFollowerAnimRayquazaWalkEastFast,
    sFollowerAnimRayquazaWalkSouthFast,
    sFollowerAnimRayquazaWalkNorthFast,
    sFollowerAnimRayquazaWalkWestFast,
    sFollowerAnimRayquazaWalkEastFast,
    sFollowerAnimRayquazaWalkSouthFast,
    sFollowerAnimRayquazaWalkNorthFast,
    sFollowerAnimRayquazaWalkWestFast,
    sFollowerAnimRayquazaWalkEastFast,
    sFollowerAnimRayquazaWalkSouthFast,
    sFollowerAnimRayquazaWalkNorthFast,
    sFollowerAnimRayquazaWalkWestFast,
    sFollowerAnimRayquazaWalkEastFast,
};

static const u32 sFollowerPicBulbasaur[] = INCBIN_U32("mods/following_pokemon/graphics/followers/bulbasaur.4bpp");
static const u16 sFollowerPalBulbasaur[] = INCBIN_U16("mods/following_pokemon/graphics/followers/bulbasaur.gbapal");
const struct SpritePalette gFollowingPokemonPaletteBulbasaur = { sFollowerPalBulbasaur, 0xF000 };
static const struct SpriteFrameImage sFollowerPicTableBulbasaur[] =
{
    overworld_frame(sFollowerPicBulbasaur, 4, 4, 0),
    overworld_frame(sFollowerPicBulbasaur, 4, 4, 1),
    overworld_frame(sFollowerPicBulbasaur, 4, 4, 2),
    overworld_frame(sFollowerPicBulbasaur, 4, 4, 3),
    overworld_frame(sFollowerPicBulbasaur, 4, 4, 4),
    overworld_frame(sFollowerPicBulbasaur, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoBulbasaur =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF000,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableBulbasaur,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicIvysaur[] = INCBIN_U32("mods/following_pokemon/graphics/followers/ivysaur.4bpp");
static const u16 sFollowerPalIvysaur[] = INCBIN_U16("mods/following_pokemon/graphics/followers/ivysaur.gbapal");
const struct SpritePalette gFollowingPokemonPaletteIvysaur = { sFollowerPalIvysaur, 0xF001 };
static const struct SpriteFrameImage sFollowerPicTableIvysaur[] =
{
    overworld_frame(sFollowerPicIvysaur, 4, 4, 0),
    overworld_frame(sFollowerPicIvysaur, 4, 4, 1),
    overworld_frame(sFollowerPicIvysaur, 4, 4, 2),
    overworld_frame(sFollowerPicIvysaur, 4, 4, 3),
    overworld_frame(sFollowerPicIvysaur, 4, 4, 4),
    overworld_frame(sFollowerPicIvysaur, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoIvysaur =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF001,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableIvysaur,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicVenusaur[] = INCBIN_U32("mods/following_pokemon/graphics/followers/venusaur.4bpp");
static const u16 sFollowerPalVenusaur[] = INCBIN_U16("mods/following_pokemon/graphics/followers/venusaur.gbapal");
const struct SpritePalette gFollowingPokemonPaletteVenusaur = { sFollowerPalVenusaur, 0xF002 };
static const struct SpriteFrameImage sFollowerPicTableVenusaur[] =
{
    overworld_frame(sFollowerPicVenusaur, 4, 4, 0),
    overworld_frame(sFollowerPicVenusaur, 4, 4, 1),
    overworld_frame(sFollowerPicVenusaur, 4, 4, 2),
    overworld_frame(sFollowerPicVenusaur, 4, 4, 3),
    overworld_frame(sFollowerPicVenusaur, 4, 4, 4),
    overworld_frame(sFollowerPicVenusaur, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoVenusaur =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF002,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableVenusaur,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicCharmander[] = INCBIN_U32("mods/following_pokemon/graphics/followers/charmander.4bpp");
static const u16 sFollowerPalCharmander[] = INCBIN_U16("mods/following_pokemon/graphics/followers/charmander.gbapal");
const struct SpritePalette gFollowingPokemonPaletteCharmander = { sFollowerPalCharmander, 0xF003 };
static const struct SpriteFrameImage sFollowerPicTableCharmander[] =
{
    overworld_frame(sFollowerPicCharmander, 4, 4, 0),
    overworld_frame(sFollowerPicCharmander, 4, 4, 1),
    overworld_frame(sFollowerPicCharmander, 4, 4, 2),
    overworld_frame(sFollowerPicCharmander, 4, 4, 3),
    overworld_frame(sFollowerPicCharmander, 4, 4, 4),
    overworld_frame(sFollowerPicCharmander, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoCharmander =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF003,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableCharmander,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicCharmeleon[] = INCBIN_U32("mods/following_pokemon/graphics/followers/charmeleon.4bpp");
static const u16 sFollowerPalCharmeleon[] = INCBIN_U16("mods/following_pokemon/graphics/followers/charmeleon.gbapal");
const struct SpritePalette gFollowingPokemonPaletteCharmeleon = { sFollowerPalCharmeleon, 0xF004 };
static const struct SpriteFrameImage sFollowerPicTableCharmeleon[] =
{
    overworld_frame(sFollowerPicCharmeleon, 4, 4, 0),
    overworld_frame(sFollowerPicCharmeleon, 4, 4, 1),
    overworld_frame(sFollowerPicCharmeleon, 4, 4, 2),
    overworld_frame(sFollowerPicCharmeleon, 4, 4, 3),
    overworld_frame(sFollowerPicCharmeleon, 4, 4, 4),
    overworld_frame(sFollowerPicCharmeleon, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoCharmeleon =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF004,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableCharmeleon,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicCharizard[] = INCBIN_U32("mods/following_pokemon/graphics/followers/charizard.4bpp");
static const u16 sFollowerPalCharizard[] = INCBIN_U16("mods/following_pokemon/graphics/followers/charizard.gbapal");
const struct SpritePalette gFollowingPokemonPaletteCharizard = { sFollowerPalCharizard, 0xF005 };
static const struct SpriteFrameImage sFollowerPicTableCharizard[] =
{
    overworld_frame(sFollowerPicCharizard, 4, 4, 0),
    overworld_frame(sFollowerPicCharizard, 4, 4, 1),
    overworld_frame(sFollowerPicCharizard, 4, 4, 2),
    overworld_frame(sFollowerPicCharizard, 4, 4, 3),
    overworld_frame(sFollowerPicCharizard, 4, 4, 4),
    overworld_frame(sFollowerPicCharizard, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoCharizard =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF005,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableCharizard,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSquirtle[] = INCBIN_U32("mods/following_pokemon/graphics/followers/squirtle.4bpp");
static const u16 sFollowerPalSquirtle[] = INCBIN_U16("mods/following_pokemon/graphics/followers/squirtle.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSquirtle = { sFollowerPalSquirtle, 0xF006 };
static const struct SpriteFrameImage sFollowerPicTableSquirtle[] =
{
    overworld_frame(sFollowerPicSquirtle, 4, 4, 0),
    overworld_frame(sFollowerPicSquirtle, 4, 4, 1),
    overworld_frame(sFollowerPicSquirtle, 4, 4, 2),
    overworld_frame(sFollowerPicSquirtle, 4, 4, 3),
    overworld_frame(sFollowerPicSquirtle, 4, 4, 4),
    overworld_frame(sFollowerPicSquirtle, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSquirtle =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF006,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSquirtle,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicWartortle[] = INCBIN_U32("mods/following_pokemon/graphics/followers/wartortle.4bpp");
static const u16 sFollowerPalWartortle[] = INCBIN_U16("mods/following_pokemon/graphics/followers/wartortle.gbapal");
const struct SpritePalette gFollowingPokemonPaletteWartortle = { sFollowerPalWartortle, 0xF007 };
static const struct SpriteFrameImage sFollowerPicTableWartortle[] =
{
    overworld_frame(sFollowerPicWartortle, 4, 4, 0),
    overworld_frame(sFollowerPicWartortle, 4, 4, 1),
    overworld_frame(sFollowerPicWartortle, 4, 4, 2),
    overworld_frame(sFollowerPicWartortle, 4, 4, 3),
    overworld_frame(sFollowerPicWartortle, 4, 4, 4),
    overworld_frame(sFollowerPicWartortle, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoWartortle =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF007,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableWartortle,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicBlastoise[] = INCBIN_U32("mods/following_pokemon/graphics/followers/blastoise.4bpp");
static const u16 sFollowerPalBlastoise[] = INCBIN_U16("mods/following_pokemon/graphics/followers/blastoise.gbapal");
const struct SpritePalette gFollowingPokemonPaletteBlastoise = { sFollowerPalBlastoise, 0xF008 };
static const struct SpriteFrameImage sFollowerPicTableBlastoise[] =
{
    overworld_frame(sFollowerPicBlastoise, 4, 4, 0),
    overworld_frame(sFollowerPicBlastoise, 4, 4, 1),
    overworld_frame(sFollowerPicBlastoise, 4, 4, 2),
    overworld_frame(sFollowerPicBlastoise, 4, 4, 3),
    overworld_frame(sFollowerPicBlastoise, 4, 4, 4),
    overworld_frame(sFollowerPicBlastoise, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoBlastoise =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF008,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableBlastoise,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicCaterpie[] = INCBIN_U32("mods/following_pokemon/graphics/followers/caterpie.4bpp");
static const u16 sFollowerPalCaterpie[] = INCBIN_U16("mods/following_pokemon/graphics/followers/caterpie.gbapal");
const struct SpritePalette gFollowingPokemonPaletteCaterpie = { sFollowerPalCaterpie, 0xF009 };
static const struct SpriteFrameImage sFollowerPicTableCaterpie[] =
{
    overworld_frame(sFollowerPicCaterpie, 4, 4, 0),
    overworld_frame(sFollowerPicCaterpie, 4, 4, 1),
    overworld_frame(sFollowerPicCaterpie, 4, 4, 2),
    overworld_frame(sFollowerPicCaterpie, 4, 4, 3),
    overworld_frame(sFollowerPicCaterpie, 4, 4, 4),
    overworld_frame(sFollowerPicCaterpie, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoCaterpie =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF009,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableCaterpie,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMetapod[] = INCBIN_U32("mods/following_pokemon/graphics/followers/metapod.4bpp");
static const u16 sFollowerPalMetapod[] = INCBIN_U16("mods/following_pokemon/graphics/followers/metapod.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMetapod = { sFollowerPalMetapod, 0xF00A };
static const struct SpriteFrameImage sFollowerPicTableMetapod[] =
{
    overworld_frame(sFollowerPicMetapod, 4, 4, 0),
    overworld_frame(sFollowerPicMetapod, 4, 4, 1),
    overworld_frame(sFollowerPicMetapod, 4, 4, 2),
    overworld_frame(sFollowerPicMetapod, 4, 4, 3),
    overworld_frame(sFollowerPicMetapod, 4, 4, 4),
    overworld_frame(sFollowerPicMetapod, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMetapod =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF00A,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMetapod,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicButterfree[] = INCBIN_U32("mods/following_pokemon/graphics/followers/butterfree.4bpp");
static const u16 sFollowerPalButterfree[] = INCBIN_U16("mods/following_pokemon/graphics/followers/butterfree.gbapal");
const struct SpritePalette gFollowingPokemonPaletteButterfree = { sFollowerPalButterfree, 0xF00B };
static const struct SpriteFrameImage sFollowerPicTableButterfree[] =
{
    overworld_frame(sFollowerPicButterfree, 4, 4, 0),
    overworld_frame(sFollowerPicButterfree, 4, 4, 1),
    overworld_frame(sFollowerPicButterfree, 4, 4, 2),
    overworld_frame(sFollowerPicButterfree, 4, 4, 3),
    overworld_frame(sFollowerPicButterfree, 4, 4, 4),
    overworld_frame(sFollowerPicButterfree, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoButterfree =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF00B,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableButterfree,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicWeedle[] = INCBIN_U32("mods/following_pokemon/graphics/followers/weedle.4bpp");
static const u16 sFollowerPalWeedle[] = INCBIN_U16("mods/following_pokemon/graphics/followers/weedle.gbapal");
const struct SpritePalette gFollowingPokemonPaletteWeedle = { sFollowerPalWeedle, 0xF00C };
static const struct SpriteFrameImage sFollowerPicTableWeedle[] =
{
    overworld_frame(sFollowerPicWeedle, 4, 4, 0),
    overworld_frame(sFollowerPicWeedle, 4, 4, 1),
    overworld_frame(sFollowerPicWeedle, 4, 4, 2),
    overworld_frame(sFollowerPicWeedle, 4, 4, 3),
    overworld_frame(sFollowerPicWeedle, 4, 4, 4),
    overworld_frame(sFollowerPicWeedle, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoWeedle =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF00C,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableWeedle,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicKakuna[] = INCBIN_U32("mods/following_pokemon/graphics/followers/kakuna.4bpp");
static const u16 sFollowerPalKakuna[] = INCBIN_U16("mods/following_pokemon/graphics/followers/kakuna.gbapal");
const struct SpritePalette gFollowingPokemonPaletteKakuna = { sFollowerPalKakuna, 0xF00D };
static const struct SpriteFrameImage sFollowerPicTableKakuna[] =
{
    overworld_frame(sFollowerPicKakuna, 4, 4, 0),
    overworld_frame(sFollowerPicKakuna, 4, 4, 1),
    overworld_frame(sFollowerPicKakuna, 4, 4, 2),
    overworld_frame(sFollowerPicKakuna, 4, 4, 3),
    overworld_frame(sFollowerPicKakuna, 4, 4, 4),
    overworld_frame(sFollowerPicKakuna, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoKakuna =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF00D,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableKakuna,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicBeedrill[] = INCBIN_U32("mods/following_pokemon/graphics/followers/beedrill.4bpp");
static const u16 sFollowerPalBeedrill[] = INCBIN_U16("mods/following_pokemon/graphics/followers/beedrill.gbapal");
const struct SpritePalette gFollowingPokemonPaletteBeedrill = { sFollowerPalBeedrill, 0xF00E };
static const struct SpriteFrameImage sFollowerPicTableBeedrill[] =
{
    overworld_frame(sFollowerPicBeedrill, 4, 4, 0),
    overworld_frame(sFollowerPicBeedrill, 4, 4, 1),
    overworld_frame(sFollowerPicBeedrill, 4, 4, 2),
    overworld_frame(sFollowerPicBeedrill, 4, 4, 3),
    overworld_frame(sFollowerPicBeedrill, 4, 4, 4),
    overworld_frame(sFollowerPicBeedrill, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoBeedrill =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF00E,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableBeedrill,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicPidgey[] = INCBIN_U32("mods/following_pokemon/graphics/followers/pidgey.4bpp");
static const u16 sFollowerPalPidgey[] = INCBIN_U16("mods/following_pokemon/graphics/followers/pidgey.gbapal");
const struct SpritePalette gFollowingPokemonPalettePidgey = { sFollowerPalPidgey, 0xF00F };
static const struct SpriteFrameImage sFollowerPicTablePidgey[] =
{
    overworld_frame(sFollowerPicPidgey, 4, 4, 0),
    overworld_frame(sFollowerPicPidgey, 4, 4, 1),
    overworld_frame(sFollowerPicPidgey, 4, 4, 2),
    overworld_frame(sFollowerPicPidgey, 4, 4, 3),
    overworld_frame(sFollowerPicPidgey, 4, 4, 4),
    overworld_frame(sFollowerPicPidgey, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoPidgey =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF00F,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTablePidgey,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicPidgeotto[] = INCBIN_U32("mods/following_pokemon/graphics/followers/pidgeotto.4bpp");
static const u16 sFollowerPalPidgeotto[] = INCBIN_U16("mods/following_pokemon/graphics/followers/pidgeotto.gbapal");
const struct SpritePalette gFollowingPokemonPalettePidgeotto = { sFollowerPalPidgeotto, 0xF010 };
static const struct SpriteFrameImage sFollowerPicTablePidgeotto[] =
{
    overworld_frame(sFollowerPicPidgeotto, 4, 4, 0),
    overworld_frame(sFollowerPicPidgeotto, 4, 4, 1),
    overworld_frame(sFollowerPicPidgeotto, 4, 4, 2),
    overworld_frame(sFollowerPicPidgeotto, 4, 4, 3),
    overworld_frame(sFollowerPicPidgeotto, 4, 4, 4),
    overworld_frame(sFollowerPicPidgeotto, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoPidgeotto =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF010,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTablePidgeotto,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicPidgeot[] = INCBIN_U32("mods/following_pokemon/graphics/followers/pidgeot.4bpp");
static const u16 sFollowerPalPidgeot[] = INCBIN_U16("mods/following_pokemon/graphics/followers/pidgeot.gbapal");
const struct SpritePalette gFollowingPokemonPalettePidgeot = { sFollowerPalPidgeot, 0xF011 };
static const struct SpriteFrameImage sFollowerPicTablePidgeot[] =
{
    overworld_frame(sFollowerPicPidgeot, 4, 4, 0),
    overworld_frame(sFollowerPicPidgeot, 4, 4, 1),
    overworld_frame(sFollowerPicPidgeot, 4, 4, 2),
    overworld_frame(sFollowerPicPidgeot, 4, 4, 3),
    overworld_frame(sFollowerPicPidgeot, 4, 4, 4),
    overworld_frame(sFollowerPicPidgeot, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoPidgeot =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF011,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTablePidgeot,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicRattata[] = INCBIN_U32("mods/following_pokemon/graphics/followers/rattata.4bpp");
static const u16 sFollowerPalRattata[] = INCBIN_U16("mods/following_pokemon/graphics/followers/rattata.gbapal");
const struct SpritePalette gFollowingPokemonPaletteRattata = { sFollowerPalRattata, 0xF012 };
static const struct SpriteFrameImage sFollowerPicTableRattata[] =
{
    overworld_frame(sFollowerPicRattata, 4, 4, 0),
    overworld_frame(sFollowerPicRattata, 4, 4, 1),
    overworld_frame(sFollowerPicRattata, 4, 4, 2),
    overworld_frame(sFollowerPicRattata, 4, 4, 3),
    overworld_frame(sFollowerPicRattata, 4, 4, 4),
    overworld_frame(sFollowerPicRattata, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoRattata =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF012,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableRattata,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicRaticate[] = INCBIN_U32("mods/following_pokemon/graphics/followers/raticate.4bpp");
static const u16 sFollowerPalRaticate[] = INCBIN_U16("mods/following_pokemon/graphics/followers/raticate.gbapal");
const struct SpritePalette gFollowingPokemonPaletteRaticate = { sFollowerPalRaticate, 0xF013 };
static const struct SpriteFrameImage sFollowerPicTableRaticate[] =
{
    overworld_frame(sFollowerPicRaticate, 4, 4, 0),
    overworld_frame(sFollowerPicRaticate, 4, 4, 1),
    overworld_frame(sFollowerPicRaticate, 4, 4, 2),
    overworld_frame(sFollowerPicRaticate, 4, 4, 3),
    overworld_frame(sFollowerPicRaticate, 4, 4, 4),
    overworld_frame(sFollowerPicRaticate, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoRaticate =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF013,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableRaticate,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSpearow[] = INCBIN_U32("mods/following_pokemon/graphics/followers/spearow.4bpp");
static const u16 sFollowerPalSpearow[] = INCBIN_U16("mods/following_pokemon/graphics/followers/spearow.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSpearow = { sFollowerPalSpearow, 0xF014 };
static const struct SpriteFrameImage sFollowerPicTableSpearow[] =
{
    overworld_frame(sFollowerPicSpearow, 4, 4, 0),
    overworld_frame(sFollowerPicSpearow, 4, 4, 1),
    overworld_frame(sFollowerPicSpearow, 4, 4, 2),
    overworld_frame(sFollowerPicSpearow, 4, 4, 3),
    overworld_frame(sFollowerPicSpearow, 4, 4, 4),
    overworld_frame(sFollowerPicSpearow, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSpearow =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF014,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSpearow,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicFearow[] = INCBIN_U32("mods/following_pokemon/graphics/followers/fearow.4bpp");
static const u16 sFollowerPalFearow[] = INCBIN_U16("mods/following_pokemon/graphics/followers/fearow.gbapal");
const struct SpritePalette gFollowingPokemonPaletteFearow = { sFollowerPalFearow, 0xF015 };
static const struct SpriteFrameImage sFollowerPicTableFearow[] =
{
    overworld_frame(sFollowerPicFearow, 4, 4, 0),
    overworld_frame(sFollowerPicFearow, 4, 4, 1),
    overworld_frame(sFollowerPicFearow, 4, 4, 2),
    overworld_frame(sFollowerPicFearow, 4, 4, 3),
    overworld_frame(sFollowerPicFearow, 4, 4, 4),
    overworld_frame(sFollowerPicFearow, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoFearow =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF015,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableFearow,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicEkans[] = INCBIN_U32("mods/following_pokemon/graphics/followers/ekans.4bpp");
static const u16 sFollowerPalEkans[] = INCBIN_U16("mods/following_pokemon/graphics/followers/ekans.gbapal");
const struct SpritePalette gFollowingPokemonPaletteEkans = { sFollowerPalEkans, 0xF016 };
static const struct SpriteFrameImage sFollowerPicTableEkans[] =
{
    overworld_frame(sFollowerPicEkans, 4, 4, 0),
    overworld_frame(sFollowerPicEkans, 4, 4, 1),
    overworld_frame(sFollowerPicEkans, 4, 4, 2),
    overworld_frame(sFollowerPicEkans, 4, 4, 3),
    overworld_frame(sFollowerPicEkans, 4, 4, 4),
    overworld_frame(sFollowerPicEkans, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoEkans =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF016,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableEkans,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicArbok[] = INCBIN_U32("mods/following_pokemon/graphics/followers/arbok.4bpp");
static const u16 sFollowerPalArbok[] = INCBIN_U16("mods/following_pokemon/graphics/followers/arbok.gbapal");
const struct SpritePalette gFollowingPokemonPaletteArbok = { sFollowerPalArbok, 0xF017 };
static const struct SpriteFrameImage sFollowerPicTableArbok[] =
{
    overworld_frame(sFollowerPicArbok, 4, 4, 0),
    overworld_frame(sFollowerPicArbok, 4, 4, 1),
    overworld_frame(sFollowerPicArbok, 4, 4, 2),
    overworld_frame(sFollowerPicArbok, 4, 4, 3),
    overworld_frame(sFollowerPicArbok, 4, 4, 4),
    overworld_frame(sFollowerPicArbok, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoArbok =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF017,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableArbok,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicPikachu[] = INCBIN_U32("mods/following_pokemon/graphics/followers/pikachu.4bpp");
static const u16 sFollowerPalPikachu[] = INCBIN_U16("mods/following_pokemon/graphics/followers/pikachu.gbapal");
const struct SpritePalette gFollowingPokemonPalettePikachu = { sFollowerPalPikachu, 0xF018 };
static const struct SpriteFrameImage sFollowerPicTablePikachu[] =
{
    overworld_frame(sFollowerPicPikachu, 4, 4, 0),
    overworld_frame(sFollowerPicPikachu, 4, 4, 1),
    overworld_frame(sFollowerPicPikachu, 4, 4, 2),
    overworld_frame(sFollowerPicPikachu, 4, 4, 3),
    overworld_frame(sFollowerPicPikachu, 4, 4, 4),
    overworld_frame(sFollowerPicPikachu, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoPikachu =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF018,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTablePikachu,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicRaichu[] = INCBIN_U32("mods/following_pokemon/graphics/followers/raichu.4bpp");
static const u16 sFollowerPalRaichu[] = INCBIN_U16("mods/following_pokemon/graphics/followers/raichu.gbapal");
const struct SpritePalette gFollowingPokemonPaletteRaichu = { sFollowerPalRaichu, 0xF019 };
static const struct SpriteFrameImage sFollowerPicTableRaichu[] =
{
    overworld_frame(sFollowerPicRaichu, 4, 4, 0),
    overworld_frame(sFollowerPicRaichu, 4, 4, 1),
    overworld_frame(sFollowerPicRaichu, 4, 4, 2),
    overworld_frame(sFollowerPicRaichu, 4, 4, 3),
    overworld_frame(sFollowerPicRaichu, 4, 4, 4),
    overworld_frame(sFollowerPicRaichu, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoRaichu =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF019,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableRaichu,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSandshrew[] = INCBIN_U32("mods/following_pokemon/graphics/followers/sandshrew.4bpp");
static const u16 sFollowerPalSandshrew[] = INCBIN_U16("mods/following_pokemon/graphics/followers/sandshrew.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSandshrew = { sFollowerPalSandshrew, 0xF01A };
static const struct SpriteFrameImage sFollowerPicTableSandshrew[] =
{
    overworld_frame(sFollowerPicSandshrew, 4, 4, 0),
    overworld_frame(sFollowerPicSandshrew, 4, 4, 1),
    overworld_frame(sFollowerPicSandshrew, 4, 4, 2),
    overworld_frame(sFollowerPicSandshrew, 4, 4, 3),
    overworld_frame(sFollowerPicSandshrew, 4, 4, 4),
    overworld_frame(sFollowerPicSandshrew, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSandshrew =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF01A,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSandshrew,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSandslash[] = INCBIN_U32("mods/following_pokemon/graphics/followers/sandslash.4bpp");
static const u16 sFollowerPalSandslash[] = INCBIN_U16("mods/following_pokemon/graphics/followers/sandslash.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSandslash = { sFollowerPalSandslash, 0xF01B };
static const struct SpriteFrameImage sFollowerPicTableSandslash[] =
{
    overworld_frame(sFollowerPicSandslash, 4, 4, 0),
    overworld_frame(sFollowerPicSandslash, 4, 4, 1),
    overworld_frame(sFollowerPicSandslash, 4, 4, 2),
    overworld_frame(sFollowerPicSandslash, 4, 4, 3),
    overworld_frame(sFollowerPicSandslash, 4, 4, 4),
    overworld_frame(sFollowerPicSandslash, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSandslash =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF01B,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSandslash,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicNidoranF[] = INCBIN_U32("mods/following_pokemon/graphics/followers/nidoran_f.4bpp");
static const u16 sFollowerPalNidoranF[] = INCBIN_U16("mods/following_pokemon/graphics/followers/nidoran_f.gbapal");
const struct SpritePalette gFollowingPokemonPaletteNidoranF = { sFollowerPalNidoranF, 0xF01C };
static const struct SpriteFrameImage sFollowerPicTableNidoranF[] =
{
    overworld_frame(sFollowerPicNidoranF, 4, 4, 0),
    overworld_frame(sFollowerPicNidoranF, 4, 4, 1),
    overworld_frame(sFollowerPicNidoranF, 4, 4, 2),
    overworld_frame(sFollowerPicNidoranF, 4, 4, 3),
    overworld_frame(sFollowerPicNidoranF, 4, 4, 4),
    overworld_frame(sFollowerPicNidoranF, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoNidoranF =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF01C,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableNidoranF,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicNidorina[] = INCBIN_U32("mods/following_pokemon/graphics/followers/nidorina.4bpp");
static const u16 sFollowerPalNidorina[] = INCBIN_U16("mods/following_pokemon/graphics/followers/nidorina.gbapal");
const struct SpritePalette gFollowingPokemonPaletteNidorina = { sFollowerPalNidorina, 0xF01D };
static const struct SpriteFrameImage sFollowerPicTableNidorina[] =
{
    overworld_frame(sFollowerPicNidorina, 4, 4, 0),
    overworld_frame(sFollowerPicNidorina, 4, 4, 1),
    overworld_frame(sFollowerPicNidorina, 4, 4, 2),
    overworld_frame(sFollowerPicNidorina, 4, 4, 3),
    overworld_frame(sFollowerPicNidorina, 4, 4, 4),
    overworld_frame(sFollowerPicNidorina, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoNidorina =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF01D,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableNidorina,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicNidoqueen[] = INCBIN_U32("mods/following_pokemon/graphics/followers/nidoqueen.4bpp");
static const u16 sFollowerPalNidoqueen[] = INCBIN_U16("mods/following_pokemon/graphics/followers/nidoqueen.gbapal");
const struct SpritePalette gFollowingPokemonPaletteNidoqueen = { sFollowerPalNidoqueen, 0xF01E };
static const struct SpriteFrameImage sFollowerPicTableNidoqueen[] =
{
    overworld_frame(sFollowerPicNidoqueen, 4, 4, 0),
    overworld_frame(sFollowerPicNidoqueen, 4, 4, 1),
    overworld_frame(sFollowerPicNidoqueen, 4, 4, 2),
    overworld_frame(sFollowerPicNidoqueen, 4, 4, 3),
    overworld_frame(sFollowerPicNidoqueen, 4, 4, 4),
    overworld_frame(sFollowerPicNidoqueen, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoNidoqueen =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF01E,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableNidoqueen,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicNidoranM[] = INCBIN_U32("mods/following_pokemon/graphics/followers/nidoran_m.4bpp");
static const u16 sFollowerPalNidoranM[] = INCBIN_U16("mods/following_pokemon/graphics/followers/nidoran_m.gbapal");
const struct SpritePalette gFollowingPokemonPaletteNidoranM = { sFollowerPalNidoranM, 0xF01F };
static const struct SpriteFrameImage sFollowerPicTableNidoranM[] =
{
    overworld_frame(sFollowerPicNidoranM, 4, 4, 0),
    overworld_frame(sFollowerPicNidoranM, 4, 4, 1),
    overworld_frame(sFollowerPicNidoranM, 4, 4, 2),
    overworld_frame(sFollowerPicNidoranM, 4, 4, 3),
    overworld_frame(sFollowerPicNidoranM, 4, 4, 4),
    overworld_frame(sFollowerPicNidoranM, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoNidoranM =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF01F,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableNidoranM,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicNidorino[] = INCBIN_U32("mods/following_pokemon/graphics/followers/nidorino.4bpp");
static const u16 sFollowerPalNidorino[] = INCBIN_U16("mods/following_pokemon/graphics/followers/nidorino.gbapal");
const struct SpritePalette gFollowingPokemonPaletteNidorino = { sFollowerPalNidorino, 0xF020 };
static const struct SpriteFrameImage sFollowerPicTableNidorino[] =
{
    overworld_frame(sFollowerPicNidorino, 4, 4, 0),
    overworld_frame(sFollowerPicNidorino, 4, 4, 1),
    overworld_frame(sFollowerPicNidorino, 4, 4, 2),
    overworld_frame(sFollowerPicNidorino, 4, 4, 3),
    overworld_frame(sFollowerPicNidorino, 4, 4, 4),
    overworld_frame(sFollowerPicNidorino, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoNidorino =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF020,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableNidorino,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicNidoking[] = INCBIN_U32("mods/following_pokemon/graphics/followers/nidoking.4bpp");
static const u16 sFollowerPalNidoking[] = INCBIN_U16("mods/following_pokemon/graphics/followers/nidoking.gbapal");
const struct SpritePalette gFollowingPokemonPaletteNidoking = { sFollowerPalNidoking, 0xF021 };
static const struct SpriteFrameImage sFollowerPicTableNidoking[] =
{
    overworld_frame(sFollowerPicNidoking, 4, 4, 0),
    overworld_frame(sFollowerPicNidoking, 4, 4, 1),
    overworld_frame(sFollowerPicNidoking, 4, 4, 2),
    overworld_frame(sFollowerPicNidoking, 4, 4, 3),
    overworld_frame(sFollowerPicNidoking, 4, 4, 4),
    overworld_frame(sFollowerPicNidoking, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoNidoking =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF021,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableNidoking,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicClefairy[] = INCBIN_U32("mods/following_pokemon/graphics/followers/clefairy.4bpp");
static const u16 sFollowerPalClefairy[] = INCBIN_U16("mods/following_pokemon/graphics/followers/clefairy.gbapal");
const struct SpritePalette gFollowingPokemonPaletteClefairy = { sFollowerPalClefairy, 0xF022 };
static const struct SpriteFrameImage sFollowerPicTableClefairy[] =
{
    overworld_frame(sFollowerPicClefairy, 4, 4, 0),
    overworld_frame(sFollowerPicClefairy, 4, 4, 1),
    overworld_frame(sFollowerPicClefairy, 4, 4, 2),
    overworld_frame(sFollowerPicClefairy, 4, 4, 3),
    overworld_frame(sFollowerPicClefairy, 4, 4, 4),
    overworld_frame(sFollowerPicClefairy, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoClefairy =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF022,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableClefairy,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicClefable[] = INCBIN_U32("mods/following_pokemon/graphics/followers/clefable.4bpp");
static const u16 sFollowerPalClefable[] = INCBIN_U16("mods/following_pokemon/graphics/followers/clefable.gbapal");
const struct SpritePalette gFollowingPokemonPaletteClefable = { sFollowerPalClefable, 0xF023 };
static const struct SpriteFrameImage sFollowerPicTableClefable[] =
{
    overworld_frame(sFollowerPicClefable, 4, 4, 0),
    overworld_frame(sFollowerPicClefable, 4, 4, 1),
    overworld_frame(sFollowerPicClefable, 4, 4, 2),
    overworld_frame(sFollowerPicClefable, 4, 4, 3),
    overworld_frame(sFollowerPicClefable, 4, 4, 4),
    overworld_frame(sFollowerPicClefable, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoClefable =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF023,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableClefable,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicVulpix[] = INCBIN_U32("mods/following_pokemon/graphics/followers/vulpix.4bpp");
static const u16 sFollowerPalVulpix[] = INCBIN_U16("mods/following_pokemon/graphics/followers/vulpix.gbapal");
const struct SpritePalette gFollowingPokemonPaletteVulpix = { sFollowerPalVulpix, 0xF024 };
static const struct SpriteFrameImage sFollowerPicTableVulpix[] =
{
    overworld_frame(sFollowerPicVulpix, 4, 4, 0),
    overworld_frame(sFollowerPicVulpix, 4, 4, 1),
    overworld_frame(sFollowerPicVulpix, 4, 4, 2),
    overworld_frame(sFollowerPicVulpix, 4, 4, 3),
    overworld_frame(sFollowerPicVulpix, 4, 4, 4),
    overworld_frame(sFollowerPicVulpix, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoVulpix =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF024,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableVulpix,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicNinetales[] = INCBIN_U32("mods/following_pokemon/graphics/followers/ninetales.4bpp");
static const u16 sFollowerPalNinetales[] = INCBIN_U16("mods/following_pokemon/graphics/followers/ninetales.gbapal");
const struct SpritePalette gFollowingPokemonPaletteNinetales = { sFollowerPalNinetales, 0xF025 };
static const struct SpriteFrameImage sFollowerPicTableNinetales[] =
{
    overworld_frame(sFollowerPicNinetales, 4, 4, 0),
    overworld_frame(sFollowerPicNinetales, 4, 4, 1),
    overworld_frame(sFollowerPicNinetales, 4, 4, 2),
    overworld_frame(sFollowerPicNinetales, 4, 4, 3),
    overworld_frame(sFollowerPicNinetales, 4, 4, 4),
    overworld_frame(sFollowerPicNinetales, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoNinetales =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF025,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableNinetales,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicJigglypuff[] = INCBIN_U32("mods/following_pokemon/graphics/followers/jigglypuff.4bpp");
static const u16 sFollowerPalJigglypuff[] = INCBIN_U16("mods/following_pokemon/graphics/followers/jigglypuff.gbapal");
const struct SpritePalette gFollowingPokemonPaletteJigglypuff = { sFollowerPalJigglypuff, 0xF026 };
static const struct SpriteFrameImage sFollowerPicTableJigglypuff[] =
{
    overworld_frame(sFollowerPicJigglypuff, 4, 4, 0),
    overworld_frame(sFollowerPicJigglypuff, 4, 4, 1),
    overworld_frame(sFollowerPicJigglypuff, 4, 4, 2),
    overworld_frame(sFollowerPicJigglypuff, 4, 4, 3),
    overworld_frame(sFollowerPicJigglypuff, 4, 4, 4),
    overworld_frame(sFollowerPicJigglypuff, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoJigglypuff =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF026,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableJigglypuff,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicWigglytuff[] = INCBIN_U32("mods/following_pokemon/graphics/followers/wigglytuff.4bpp");
static const u16 sFollowerPalWigglytuff[] = INCBIN_U16("mods/following_pokemon/graphics/followers/wigglytuff.gbapal");
const struct SpritePalette gFollowingPokemonPaletteWigglytuff = { sFollowerPalWigglytuff, 0xF027 };
static const struct SpriteFrameImage sFollowerPicTableWigglytuff[] =
{
    overworld_frame(sFollowerPicWigglytuff, 4, 4, 0),
    overworld_frame(sFollowerPicWigglytuff, 4, 4, 1),
    overworld_frame(sFollowerPicWigglytuff, 4, 4, 2),
    overworld_frame(sFollowerPicWigglytuff, 4, 4, 3),
    overworld_frame(sFollowerPicWigglytuff, 4, 4, 4),
    overworld_frame(sFollowerPicWigglytuff, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoWigglytuff =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF027,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableWigglytuff,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicZubat[] = INCBIN_U32("mods/following_pokemon/graphics/followers/zubat.4bpp");
static const u16 sFollowerPalZubat[] = INCBIN_U16("mods/following_pokemon/graphics/followers/zubat.gbapal");
const struct SpritePalette gFollowingPokemonPaletteZubat = { sFollowerPalZubat, 0xF028 };
static const struct SpriteFrameImage sFollowerPicTableZubat[] =
{
    overworld_frame(sFollowerPicZubat, 4, 4, 0),
    overworld_frame(sFollowerPicZubat, 4, 4, 1),
    overworld_frame(sFollowerPicZubat, 4, 4, 2),
    overworld_frame(sFollowerPicZubat, 4, 4, 3),
    overworld_frame(sFollowerPicZubat, 4, 4, 4),
    overworld_frame(sFollowerPicZubat, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoZubat =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF028,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableZubat,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicGolbat[] = INCBIN_U32("mods/following_pokemon/graphics/followers/golbat.4bpp");
static const u16 sFollowerPalGolbat[] = INCBIN_U16("mods/following_pokemon/graphics/followers/golbat.gbapal");
const struct SpritePalette gFollowingPokemonPaletteGolbat = { sFollowerPalGolbat, 0xF029 };
static const struct SpriteFrameImage sFollowerPicTableGolbat[] =
{
    overworld_frame(sFollowerPicGolbat, 4, 4, 0),
    overworld_frame(sFollowerPicGolbat, 4, 4, 1),
    overworld_frame(sFollowerPicGolbat, 4, 4, 2),
    overworld_frame(sFollowerPicGolbat, 4, 4, 3),
    overworld_frame(sFollowerPicGolbat, 4, 4, 4),
    overworld_frame(sFollowerPicGolbat, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoGolbat =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF029,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableGolbat,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicOddish[] = INCBIN_U32("mods/following_pokemon/graphics/followers/oddish.4bpp");
static const u16 sFollowerPalOddish[] = INCBIN_U16("mods/following_pokemon/graphics/followers/oddish.gbapal");
const struct SpritePalette gFollowingPokemonPaletteOddish = { sFollowerPalOddish, 0xF02A };
static const struct SpriteFrameImage sFollowerPicTableOddish[] =
{
    overworld_frame(sFollowerPicOddish, 4, 4, 0),
    overworld_frame(sFollowerPicOddish, 4, 4, 1),
    overworld_frame(sFollowerPicOddish, 4, 4, 2),
    overworld_frame(sFollowerPicOddish, 4, 4, 3),
    overworld_frame(sFollowerPicOddish, 4, 4, 4),
    overworld_frame(sFollowerPicOddish, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoOddish =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF02A,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableOddish,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicGloom[] = INCBIN_U32("mods/following_pokemon/graphics/followers/gloom.4bpp");
static const u16 sFollowerPalGloom[] = INCBIN_U16("mods/following_pokemon/graphics/followers/gloom.gbapal");
const struct SpritePalette gFollowingPokemonPaletteGloom = { sFollowerPalGloom, 0xF02B };
static const struct SpriteFrameImage sFollowerPicTableGloom[] =
{
    overworld_frame(sFollowerPicGloom, 4, 4, 0),
    overworld_frame(sFollowerPicGloom, 4, 4, 1),
    overworld_frame(sFollowerPicGloom, 4, 4, 2),
    overworld_frame(sFollowerPicGloom, 4, 4, 3),
    overworld_frame(sFollowerPicGloom, 4, 4, 4),
    overworld_frame(sFollowerPicGloom, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoGloom =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF02B,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableGloom,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicVileplume[] = INCBIN_U32("mods/following_pokemon/graphics/followers/vileplume.4bpp");
static const u16 sFollowerPalVileplume[] = INCBIN_U16("mods/following_pokemon/graphics/followers/vileplume.gbapal");
const struct SpritePalette gFollowingPokemonPaletteVileplume = { sFollowerPalVileplume, 0xF02C };
static const struct SpriteFrameImage sFollowerPicTableVileplume[] =
{
    overworld_frame(sFollowerPicVileplume, 4, 4, 0),
    overworld_frame(sFollowerPicVileplume, 4, 4, 1),
    overworld_frame(sFollowerPicVileplume, 4, 4, 2),
    overworld_frame(sFollowerPicVileplume, 4, 4, 3),
    overworld_frame(sFollowerPicVileplume, 4, 4, 4),
    overworld_frame(sFollowerPicVileplume, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoVileplume =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF02C,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableVileplume,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicParas[] = INCBIN_U32("mods/following_pokemon/graphics/followers/paras.4bpp");
static const u16 sFollowerPalParas[] = INCBIN_U16("mods/following_pokemon/graphics/followers/paras.gbapal");
const struct SpritePalette gFollowingPokemonPaletteParas = { sFollowerPalParas, 0xF02D };
static const struct SpriteFrameImage sFollowerPicTableParas[] =
{
    overworld_frame(sFollowerPicParas, 4, 4, 0),
    overworld_frame(sFollowerPicParas, 4, 4, 1),
    overworld_frame(sFollowerPicParas, 4, 4, 2),
    overworld_frame(sFollowerPicParas, 4, 4, 3),
    overworld_frame(sFollowerPicParas, 4, 4, 4),
    overworld_frame(sFollowerPicParas, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoParas =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF02D,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableParas,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicParasect[] = INCBIN_U32("mods/following_pokemon/graphics/followers/parasect.4bpp");
static const u16 sFollowerPalParasect[] = INCBIN_U16("mods/following_pokemon/graphics/followers/parasect.gbapal");
const struct SpritePalette gFollowingPokemonPaletteParasect = { sFollowerPalParasect, 0xF02E };
static const struct SpriteFrameImage sFollowerPicTableParasect[] =
{
    overworld_frame(sFollowerPicParasect, 4, 4, 0),
    overworld_frame(sFollowerPicParasect, 4, 4, 1),
    overworld_frame(sFollowerPicParasect, 4, 4, 2),
    overworld_frame(sFollowerPicParasect, 4, 4, 3),
    overworld_frame(sFollowerPicParasect, 4, 4, 4),
    overworld_frame(sFollowerPicParasect, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoParasect =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF02E,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableParasect,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicVenonat[] = INCBIN_U32("mods/following_pokemon/graphics/followers/venonat.4bpp");
static const u16 sFollowerPalVenonat[] = INCBIN_U16("mods/following_pokemon/graphics/followers/venonat.gbapal");
const struct SpritePalette gFollowingPokemonPaletteVenonat = { sFollowerPalVenonat, 0xF02F };
static const struct SpriteFrameImage sFollowerPicTableVenonat[] =
{
    overworld_frame(sFollowerPicVenonat, 4, 4, 0),
    overworld_frame(sFollowerPicVenonat, 4, 4, 1),
    overworld_frame(sFollowerPicVenonat, 4, 4, 2),
    overworld_frame(sFollowerPicVenonat, 4, 4, 3),
    overworld_frame(sFollowerPicVenonat, 4, 4, 4),
    overworld_frame(sFollowerPicVenonat, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoVenonat =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF02F,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableVenonat,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicVenomoth[] = INCBIN_U32("mods/following_pokemon/graphics/followers/venomoth.4bpp");
static const u16 sFollowerPalVenomoth[] = INCBIN_U16("mods/following_pokemon/graphics/followers/venomoth.gbapal");
const struct SpritePalette gFollowingPokemonPaletteVenomoth = { sFollowerPalVenomoth, 0xF030 };
static const struct SpriteFrameImage sFollowerPicTableVenomoth[] =
{
    overworld_frame(sFollowerPicVenomoth, 4, 4, 0),
    overworld_frame(sFollowerPicVenomoth, 4, 4, 1),
    overworld_frame(sFollowerPicVenomoth, 4, 4, 2),
    overworld_frame(sFollowerPicVenomoth, 4, 4, 3),
    overworld_frame(sFollowerPicVenomoth, 4, 4, 4),
    overworld_frame(sFollowerPicVenomoth, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoVenomoth =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF030,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableVenomoth,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicDiglett[] = INCBIN_U32("mods/following_pokemon/graphics/followers/diglett.4bpp");
static const u16 sFollowerPalDiglett[] = INCBIN_U16("mods/following_pokemon/graphics/followers/diglett.gbapal");
const struct SpritePalette gFollowingPokemonPaletteDiglett = { sFollowerPalDiglett, 0xF031 };
static const struct SpriteFrameImage sFollowerPicTableDiglett[] =
{
    overworld_frame(sFollowerPicDiglett, 4, 4, 0),
    overworld_frame(sFollowerPicDiglett, 4, 4, 1),
    overworld_frame(sFollowerPicDiglett, 4, 4, 2),
    overworld_frame(sFollowerPicDiglett, 4, 4, 3),
    overworld_frame(sFollowerPicDiglett, 4, 4, 4),
    overworld_frame(sFollowerPicDiglett, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoDiglett =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF031,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableDiglett,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicDugtrio[] = INCBIN_U32("mods/following_pokemon/graphics/followers/dugtrio.4bpp");
static const u16 sFollowerPalDugtrio[] = INCBIN_U16("mods/following_pokemon/graphics/followers/dugtrio.gbapal");
const struct SpritePalette gFollowingPokemonPaletteDugtrio = { sFollowerPalDugtrio, 0xF032 };
static const struct SpriteFrameImage sFollowerPicTableDugtrio[] =
{
    overworld_frame(sFollowerPicDugtrio, 4, 4, 0),
    overworld_frame(sFollowerPicDugtrio, 4, 4, 1),
    overworld_frame(sFollowerPicDugtrio, 4, 4, 2),
    overworld_frame(sFollowerPicDugtrio, 4, 4, 3),
    overworld_frame(sFollowerPicDugtrio, 4, 4, 4),
    overworld_frame(sFollowerPicDugtrio, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoDugtrio =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF032,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableDugtrio,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMeowth[] = INCBIN_U32("mods/following_pokemon/graphics/followers/meowth.4bpp");
static const u16 sFollowerPalMeowth[] = INCBIN_U16("mods/following_pokemon/graphics/followers/meowth.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMeowth = { sFollowerPalMeowth, 0xF033 };
static const struct SpriteFrameImage sFollowerPicTableMeowth[] =
{
    overworld_frame(sFollowerPicMeowth, 4, 4, 0),
    overworld_frame(sFollowerPicMeowth, 4, 4, 1),
    overworld_frame(sFollowerPicMeowth, 4, 4, 2),
    overworld_frame(sFollowerPicMeowth, 4, 4, 3),
    overworld_frame(sFollowerPicMeowth, 4, 4, 4),
    overworld_frame(sFollowerPicMeowth, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMeowth =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF033,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMeowth,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicPersian[] = INCBIN_U32("mods/following_pokemon/graphics/followers/persian.4bpp");
static const u16 sFollowerPalPersian[] = INCBIN_U16("mods/following_pokemon/graphics/followers/persian.gbapal");
const struct SpritePalette gFollowingPokemonPalettePersian = { sFollowerPalPersian, 0xF034 };
static const struct SpriteFrameImage sFollowerPicTablePersian[] =
{
    overworld_frame(sFollowerPicPersian, 4, 4, 0),
    overworld_frame(sFollowerPicPersian, 4, 4, 1),
    overworld_frame(sFollowerPicPersian, 4, 4, 2),
    overworld_frame(sFollowerPicPersian, 4, 4, 3),
    overworld_frame(sFollowerPicPersian, 4, 4, 4),
    overworld_frame(sFollowerPicPersian, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoPersian =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF034,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTablePersian,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicPsyduck[] = INCBIN_U32("mods/following_pokemon/graphics/followers/psyduck.4bpp");
static const u16 sFollowerPalPsyduck[] = INCBIN_U16("mods/following_pokemon/graphics/followers/psyduck.gbapal");
const struct SpritePalette gFollowingPokemonPalettePsyduck = { sFollowerPalPsyduck, 0xF035 };
static const struct SpriteFrameImage sFollowerPicTablePsyduck[] =
{
    overworld_frame(sFollowerPicPsyduck, 4, 4, 0),
    overworld_frame(sFollowerPicPsyduck, 4, 4, 1),
    overworld_frame(sFollowerPicPsyduck, 4, 4, 2),
    overworld_frame(sFollowerPicPsyduck, 4, 4, 3),
    overworld_frame(sFollowerPicPsyduck, 4, 4, 4),
    overworld_frame(sFollowerPicPsyduck, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoPsyduck =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF035,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTablePsyduck,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicGolduck[] = INCBIN_U32("mods/following_pokemon/graphics/followers/golduck.4bpp");
static const u16 sFollowerPalGolduck[] = INCBIN_U16("mods/following_pokemon/graphics/followers/golduck.gbapal");
const struct SpritePalette gFollowingPokemonPaletteGolduck = { sFollowerPalGolduck, 0xF036 };
static const struct SpriteFrameImage sFollowerPicTableGolduck[] =
{
    overworld_frame(sFollowerPicGolduck, 4, 4, 0),
    overworld_frame(sFollowerPicGolduck, 4, 4, 1),
    overworld_frame(sFollowerPicGolduck, 4, 4, 2),
    overworld_frame(sFollowerPicGolduck, 4, 4, 3),
    overworld_frame(sFollowerPicGolduck, 4, 4, 4),
    overworld_frame(sFollowerPicGolduck, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoGolduck =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF036,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableGolduck,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMankey[] = INCBIN_U32("mods/following_pokemon/graphics/followers/mankey.4bpp");
static const u16 sFollowerPalMankey[] = INCBIN_U16("mods/following_pokemon/graphics/followers/mankey.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMankey = { sFollowerPalMankey, 0xF037 };
static const struct SpriteFrameImage sFollowerPicTableMankey[] =
{
    overworld_frame(sFollowerPicMankey, 4, 4, 0),
    overworld_frame(sFollowerPicMankey, 4, 4, 1),
    overworld_frame(sFollowerPicMankey, 4, 4, 2),
    overworld_frame(sFollowerPicMankey, 4, 4, 3),
    overworld_frame(sFollowerPicMankey, 4, 4, 4),
    overworld_frame(sFollowerPicMankey, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMankey =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF037,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMankey,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicPrimeape[] = INCBIN_U32("mods/following_pokemon/graphics/followers/primeape.4bpp");
static const u16 sFollowerPalPrimeape[] = INCBIN_U16("mods/following_pokemon/graphics/followers/primeape.gbapal");
const struct SpritePalette gFollowingPokemonPalettePrimeape = { sFollowerPalPrimeape, 0xF038 };
static const struct SpriteFrameImage sFollowerPicTablePrimeape[] =
{
    overworld_frame(sFollowerPicPrimeape, 4, 4, 0),
    overworld_frame(sFollowerPicPrimeape, 4, 4, 1),
    overworld_frame(sFollowerPicPrimeape, 4, 4, 2),
    overworld_frame(sFollowerPicPrimeape, 4, 4, 3),
    overworld_frame(sFollowerPicPrimeape, 4, 4, 4),
    overworld_frame(sFollowerPicPrimeape, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoPrimeape =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF038,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTablePrimeape,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicGrowlithe[] = INCBIN_U32("mods/following_pokemon/graphics/followers/growlithe.4bpp");
static const u16 sFollowerPalGrowlithe[] = INCBIN_U16("mods/following_pokemon/graphics/followers/growlithe.gbapal");
const struct SpritePalette gFollowingPokemonPaletteGrowlithe = { sFollowerPalGrowlithe, 0xF039 };
static const struct SpriteFrameImage sFollowerPicTableGrowlithe[] =
{
    overworld_frame(sFollowerPicGrowlithe, 4, 4, 0),
    overworld_frame(sFollowerPicGrowlithe, 4, 4, 1),
    overworld_frame(sFollowerPicGrowlithe, 4, 4, 2),
    overworld_frame(sFollowerPicGrowlithe, 4, 4, 3),
    overworld_frame(sFollowerPicGrowlithe, 4, 4, 4),
    overworld_frame(sFollowerPicGrowlithe, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoGrowlithe =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF039,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableGrowlithe,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicArcanine[] = INCBIN_U32("mods/following_pokemon/graphics/followers/arcanine.4bpp");
static const u16 sFollowerPalArcanine[] = INCBIN_U16("mods/following_pokemon/graphics/followers/arcanine.gbapal");
const struct SpritePalette gFollowingPokemonPaletteArcanine = { sFollowerPalArcanine, 0xF03A };
static const struct SpriteFrameImage sFollowerPicTableArcanine[] =
{
    overworld_frame(sFollowerPicArcanine, 4, 4, 0),
    overworld_frame(sFollowerPicArcanine, 4, 4, 1),
    overworld_frame(sFollowerPicArcanine, 4, 4, 2),
    overworld_frame(sFollowerPicArcanine, 4, 4, 3),
    overworld_frame(sFollowerPicArcanine, 4, 4, 4),
    overworld_frame(sFollowerPicArcanine, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoArcanine =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF03A,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableArcanine,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicPoliwag[] = INCBIN_U32("mods/following_pokemon/graphics/followers/poliwag.4bpp");
static const u16 sFollowerPalPoliwag[] = INCBIN_U16("mods/following_pokemon/graphics/followers/poliwag.gbapal");
const struct SpritePalette gFollowingPokemonPalettePoliwag = { sFollowerPalPoliwag, 0xF03B };
static const struct SpriteFrameImage sFollowerPicTablePoliwag[] =
{
    overworld_frame(sFollowerPicPoliwag, 4, 4, 0),
    overworld_frame(sFollowerPicPoliwag, 4, 4, 1),
    overworld_frame(sFollowerPicPoliwag, 4, 4, 2),
    overworld_frame(sFollowerPicPoliwag, 4, 4, 3),
    overworld_frame(sFollowerPicPoliwag, 4, 4, 4),
    overworld_frame(sFollowerPicPoliwag, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoPoliwag =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF03B,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTablePoliwag,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicPoliwhirl[] = INCBIN_U32("mods/following_pokemon/graphics/followers/poliwhirl.4bpp");
static const u16 sFollowerPalPoliwhirl[] = INCBIN_U16("mods/following_pokemon/graphics/followers/poliwhirl.gbapal");
const struct SpritePalette gFollowingPokemonPalettePoliwhirl = { sFollowerPalPoliwhirl, 0xF03C };
static const struct SpriteFrameImage sFollowerPicTablePoliwhirl[] =
{
    overworld_frame(sFollowerPicPoliwhirl, 4, 4, 0),
    overworld_frame(sFollowerPicPoliwhirl, 4, 4, 1),
    overworld_frame(sFollowerPicPoliwhirl, 4, 4, 2),
    overworld_frame(sFollowerPicPoliwhirl, 4, 4, 3),
    overworld_frame(sFollowerPicPoliwhirl, 4, 4, 4),
    overworld_frame(sFollowerPicPoliwhirl, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoPoliwhirl =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF03C,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTablePoliwhirl,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicPoliwrath[] = INCBIN_U32("mods/following_pokemon/graphics/followers/poliwrath.4bpp");
static const u16 sFollowerPalPoliwrath[] = INCBIN_U16("mods/following_pokemon/graphics/followers/poliwrath.gbapal");
const struct SpritePalette gFollowingPokemonPalettePoliwrath = { sFollowerPalPoliwrath, 0xF03D };
static const struct SpriteFrameImage sFollowerPicTablePoliwrath[] =
{
    overworld_frame(sFollowerPicPoliwrath, 4, 4, 0),
    overworld_frame(sFollowerPicPoliwrath, 4, 4, 1),
    overworld_frame(sFollowerPicPoliwrath, 4, 4, 2),
    overworld_frame(sFollowerPicPoliwrath, 4, 4, 3),
    overworld_frame(sFollowerPicPoliwrath, 4, 4, 4),
    overworld_frame(sFollowerPicPoliwrath, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoPoliwrath =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF03D,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTablePoliwrath,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicAbra[] = INCBIN_U32("mods/following_pokemon/graphics/followers/abra.4bpp");
static const u16 sFollowerPalAbra[] = INCBIN_U16("mods/following_pokemon/graphics/followers/abra.gbapal");
const struct SpritePalette gFollowingPokemonPaletteAbra = { sFollowerPalAbra, 0xF03E };
static const struct SpriteFrameImage sFollowerPicTableAbra[] =
{
    overworld_frame(sFollowerPicAbra, 4, 4, 0),
    overworld_frame(sFollowerPicAbra, 4, 4, 1),
    overworld_frame(sFollowerPicAbra, 4, 4, 2),
    overworld_frame(sFollowerPicAbra, 4, 4, 3),
    overworld_frame(sFollowerPicAbra, 4, 4, 4),
    overworld_frame(sFollowerPicAbra, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoAbra =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF03E,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableAbra,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicKadabra[] = INCBIN_U32("mods/following_pokemon/graphics/followers/kadabra.4bpp");
static const u16 sFollowerPalKadabra[] = INCBIN_U16("mods/following_pokemon/graphics/followers/kadabra.gbapal");
const struct SpritePalette gFollowingPokemonPaletteKadabra = { sFollowerPalKadabra, 0xF03F };
static const struct SpriteFrameImage sFollowerPicTableKadabra[] =
{
    overworld_frame(sFollowerPicKadabra, 4, 4, 0),
    overworld_frame(sFollowerPicKadabra, 4, 4, 1),
    overworld_frame(sFollowerPicKadabra, 4, 4, 2),
    overworld_frame(sFollowerPicKadabra, 4, 4, 3),
    overworld_frame(sFollowerPicKadabra, 4, 4, 4),
    overworld_frame(sFollowerPicKadabra, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoKadabra =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF03F,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableKadabra,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicAlakazam[] = INCBIN_U32("mods/following_pokemon/graphics/followers/alakazam.4bpp");
static const u16 sFollowerPalAlakazam[] = INCBIN_U16("mods/following_pokemon/graphics/followers/alakazam.gbapal");
const struct SpritePalette gFollowingPokemonPaletteAlakazam = { sFollowerPalAlakazam, 0xF040 };
static const struct SpriteFrameImage sFollowerPicTableAlakazam[] =
{
    overworld_frame(sFollowerPicAlakazam, 4, 4, 0),
    overworld_frame(sFollowerPicAlakazam, 4, 4, 1),
    overworld_frame(sFollowerPicAlakazam, 4, 4, 2),
    overworld_frame(sFollowerPicAlakazam, 4, 4, 3),
    overworld_frame(sFollowerPicAlakazam, 4, 4, 4),
    overworld_frame(sFollowerPicAlakazam, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoAlakazam =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF040,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableAlakazam,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMachop[] = INCBIN_U32("mods/following_pokemon/graphics/followers/machop.4bpp");
static const u16 sFollowerPalMachop[] = INCBIN_U16("mods/following_pokemon/graphics/followers/machop.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMachop = { sFollowerPalMachop, 0xF041 };
static const struct SpriteFrameImage sFollowerPicTableMachop[] =
{
    overworld_frame(sFollowerPicMachop, 4, 4, 0),
    overworld_frame(sFollowerPicMachop, 4, 4, 1),
    overworld_frame(sFollowerPicMachop, 4, 4, 2),
    overworld_frame(sFollowerPicMachop, 4, 4, 3),
    overworld_frame(sFollowerPicMachop, 4, 4, 4),
    overworld_frame(sFollowerPicMachop, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMachop =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF041,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMachop,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMachoke[] = INCBIN_U32("mods/following_pokemon/graphics/followers/machoke.4bpp");
static const u16 sFollowerPalMachoke[] = INCBIN_U16("mods/following_pokemon/graphics/followers/machoke.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMachoke = { sFollowerPalMachoke, 0xF042 };
static const struct SpriteFrameImage sFollowerPicTableMachoke[] =
{
    overworld_frame(sFollowerPicMachoke, 4, 4, 0),
    overworld_frame(sFollowerPicMachoke, 4, 4, 1),
    overworld_frame(sFollowerPicMachoke, 4, 4, 2),
    overworld_frame(sFollowerPicMachoke, 4, 4, 3),
    overworld_frame(sFollowerPicMachoke, 4, 4, 4),
    overworld_frame(sFollowerPicMachoke, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMachoke =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF042,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMachoke,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMachamp[] = INCBIN_U32("mods/following_pokemon/graphics/followers/machamp.4bpp");
static const u16 sFollowerPalMachamp[] = INCBIN_U16("mods/following_pokemon/graphics/followers/machamp.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMachamp = { sFollowerPalMachamp, 0xF043 };
static const struct SpriteFrameImage sFollowerPicTableMachamp[] =
{
    overworld_frame(sFollowerPicMachamp, 4, 4, 0),
    overworld_frame(sFollowerPicMachamp, 4, 4, 1),
    overworld_frame(sFollowerPicMachamp, 4, 4, 2),
    overworld_frame(sFollowerPicMachamp, 4, 4, 3),
    overworld_frame(sFollowerPicMachamp, 4, 4, 4),
    overworld_frame(sFollowerPicMachamp, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMachamp =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF043,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMachamp,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicBellsprout[] = INCBIN_U32("mods/following_pokemon/graphics/followers/bellsprout.4bpp");
static const u16 sFollowerPalBellsprout[] = INCBIN_U16("mods/following_pokemon/graphics/followers/bellsprout.gbapal");
const struct SpritePalette gFollowingPokemonPaletteBellsprout = { sFollowerPalBellsprout, 0xF044 };
static const struct SpriteFrameImage sFollowerPicTableBellsprout[] =
{
    overworld_frame(sFollowerPicBellsprout, 4, 4, 0),
    overworld_frame(sFollowerPicBellsprout, 4, 4, 1),
    overworld_frame(sFollowerPicBellsprout, 4, 4, 2),
    overworld_frame(sFollowerPicBellsprout, 4, 4, 3),
    overworld_frame(sFollowerPicBellsprout, 4, 4, 4),
    overworld_frame(sFollowerPicBellsprout, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoBellsprout =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF044,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableBellsprout,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicWeepinbell[] = INCBIN_U32("mods/following_pokemon/graphics/followers/weepinbell.4bpp");
static const u16 sFollowerPalWeepinbell[] = INCBIN_U16("mods/following_pokemon/graphics/followers/weepinbell.gbapal");
const struct SpritePalette gFollowingPokemonPaletteWeepinbell = { sFollowerPalWeepinbell, 0xF045 };
static const struct SpriteFrameImage sFollowerPicTableWeepinbell[] =
{
    overworld_frame(sFollowerPicWeepinbell, 4, 4, 0),
    overworld_frame(sFollowerPicWeepinbell, 4, 4, 1),
    overworld_frame(sFollowerPicWeepinbell, 4, 4, 2),
    overworld_frame(sFollowerPicWeepinbell, 4, 4, 3),
    overworld_frame(sFollowerPicWeepinbell, 4, 4, 4),
    overworld_frame(sFollowerPicWeepinbell, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoWeepinbell =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF045,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableWeepinbell,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicVictreebel[] = INCBIN_U32("mods/following_pokemon/graphics/followers/victreebel.4bpp");
static const u16 sFollowerPalVictreebel[] = INCBIN_U16("mods/following_pokemon/graphics/followers/victreebel.gbapal");
const struct SpritePalette gFollowingPokemonPaletteVictreebel = { sFollowerPalVictreebel, 0xF046 };
static const struct SpriteFrameImage sFollowerPicTableVictreebel[] =
{
    overworld_frame(sFollowerPicVictreebel, 4, 4, 0),
    overworld_frame(sFollowerPicVictreebel, 4, 4, 1),
    overworld_frame(sFollowerPicVictreebel, 4, 4, 2),
    overworld_frame(sFollowerPicVictreebel, 4, 4, 3),
    overworld_frame(sFollowerPicVictreebel, 4, 4, 4),
    overworld_frame(sFollowerPicVictreebel, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoVictreebel =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF046,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableVictreebel,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicTentacool[] = INCBIN_U32("mods/following_pokemon/graphics/followers/tentacool.4bpp");
static const u16 sFollowerPalTentacool[] = INCBIN_U16("mods/following_pokemon/graphics/followers/tentacool.gbapal");
const struct SpritePalette gFollowingPokemonPaletteTentacool = { sFollowerPalTentacool, 0xF047 };
static const struct SpriteFrameImage sFollowerPicTableTentacool[] =
{
    overworld_frame(sFollowerPicTentacool, 4, 4, 0),
    overworld_frame(sFollowerPicTentacool, 4, 4, 1),
    overworld_frame(sFollowerPicTentacool, 4, 4, 2),
    overworld_frame(sFollowerPicTentacool, 4, 4, 3),
    overworld_frame(sFollowerPicTentacool, 4, 4, 4),
    overworld_frame(sFollowerPicTentacool, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoTentacool =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF047,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableTentacool,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicTentacruel[] = INCBIN_U32("mods/following_pokemon/graphics/followers/tentacruel.4bpp");
static const u16 sFollowerPalTentacruel[] = INCBIN_U16("mods/following_pokemon/graphics/followers/tentacruel.gbapal");
const struct SpritePalette gFollowingPokemonPaletteTentacruel = { sFollowerPalTentacruel, 0xF048 };
static const struct SpriteFrameImage sFollowerPicTableTentacruel[] =
{
    overworld_frame(sFollowerPicTentacruel, 4, 4, 0),
    overworld_frame(sFollowerPicTentacruel, 4, 4, 1),
    overworld_frame(sFollowerPicTentacruel, 4, 4, 2),
    overworld_frame(sFollowerPicTentacruel, 4, 4, 3),
    overworld_frame(sFollowerPicTentacruel, 4, 4, 4),
    overworld_frame(sFollowerPicTentacruel, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoTentacruel =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF048,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableTentacruel,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicGeodude[] = INCBIN_U32("mods/following_pokemon/graphics/followers/geodude.4bpp");
static const u16 sFollowerPalGeodude[] = INCBIN_U16("mods/following_pokemon/graphics/followers/geodude.gbapal");
const struct SpritePalette gFollowingPokemonPaletteGeodude = { sFollowerPalGeodude, 0xF049 };
static const struct SpriteFrameImage sFollowerPicTableGeodude[] =
{
    overworld_frame(sFollowerPicGeodude, 4, 4, 0),
    overworld_frame(sFollowerPicGeodude, 4, 4, 1),
    overworld_frame(sFollowerPicGeodude, 4, 4, 2),
    overworld_frame(sFollowerPicGeodude, 4, 4, 3),
    overworld_frame(sFollowerPicGeodude, 4, 4, 4),
    overworld_frame(sFollowerPicGeodude, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoGeodude =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF049,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableGeodude,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicGraveler[] = INCBIN_U32("mods/following_pokemon/graphics/followers/graveler.4bpp");
static const u16 sFollowerPalGraveler[] = INCBIN_U16("mods/following_pokemon/graphics/followers/graveler.gbapal");
const struct SpritePalette gFollowingPokemonPaletteGraveler = { sFollowerPalGraveler, 0xF04A };
static const struct SpriteFrameImage sFollowerPicTableGraveler[] =
{
    overworld_frame(sFollowerPicGraveler, 4, 4, 0),
    overworld_frame(sFollowerPicGraveler, 4, 4, 1),
    overworld_frame(sFollowerPicGraveler, 4, 4, 2),
    overworld_frame(sFollowerPicGraveler, 4, 4, 3),
    overworld_frame(sFollowerPicGraveler, 4, 4, 4),
    overworld_frame(sFollowerPicGraveler, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoGraveler =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF04A,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableGraveler,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicGolem[] = INCBIN_U32("mods/following_pokemon/graphics/followers/golem.4bpp");
static const u16 sFollowerPalGolem[] = INCBIN_U16("mods/following_pokemon/graphics/followers/golem.gbapal");
const struct SpritePalette gFollowingPokemonPaletteGolem = { sFollowerPalGolem, 0xF04B };
static const struct SpriteFrameImage sFollowerPicTableGolem[] =
{
    overworld_frame(sFollowerPicGolem, 4, 4, 0),
    overworld_frame(sFollowerPicGolem, 4, 4, 1),
    overworld_frame(sFollowerPicGolem, 4, 4, 2),
    overworld_frame(sFollowerPicGolem, 4, 4, 3),
    overworld_frame(sFollowerPicGolem, 4, 4, 4),
    overworld_frame(sFollowerPicGolem, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoGolem =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF04B,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableGolem,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicPonyta[] = INCBIN_U32("mods/following_pokemon/graphics/followers/ponyta.4bpp");
static const u16 sFollowerPalPonyta[] = INCBIN_U16("mods/following_pokemon/graphics/followers/ponyta.gbapal");
const struct SpritePalette gFollowingPokemonPalettePonyta = { sFollowerPalPonyta, 0xF04C };
static const struct SpriteFrameImage sFollowerPicTablePonyta[] =
{
    overworld_frame(sFollowerPicPonyta, 4, 4, 0),
    overworld_frame(sFollowerPicPonyta, 4, 4, 1),
    overworld_frame(sFollowerPicPonyta, 4, 4, 2),
    overworld_frame(sFollowerPicPonyta, 4, 4, 3),
    overworld_frame(sFollowerPicPonyta, 4, 4, 4),
    overworld_frame(sFollowerPicPonyta, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoPonyta =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF04C,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTablePonyta,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicRapidash[] = INCBIN_U32("mods/following_pokemon/graphics/followers/rapidash.4bpp");
static const u16 sFollowerPalRapidash[] = INCBIN_U16("mods/following_pokemon/graphics/followers/rapidash.gbapal");
const struct SpritePalette gFollowingPokemonPaletteRapidash = { sFollowerPalRapidash, 0xF04D };
static const struct SpriteFrameImage sFollowerPicTableRapidash[] =
{
    overworld_frame(sFollowerPicRapidash, 4, 4, 0),
    overworld_frame(sFollowerPicRapidash, 4, 4, 1),
    overworld_frame(sFollowerPicRapidash, 4, 4, 2),
    overworld_frame(sFollowerPicRapidash, 4, 4, 3),
    overworld_frame(sFollowerPicRapidash, 4, 4, 4),
    overworld_frame(sFollowerPicRapidash, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoRapidash =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF04D,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableRapidash,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSlowpoke[] = INCBIN_U32("mods/following_pokemon/graphics/followers/slowpoke.4bpp");
static const u16 sFollowerPalSlowpoke[] = INCBIN_U16("mods/following_pokemon/graphics/followers/slowpoke.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSlowpoke = { sFollowerPalSlowpoke, 0xF04E };
static const struct SpriteFrameImage sFollowerPicTableSlowpoke[] =
{
    overworld_frame(sFollowerPicSlowpoke, 4, 4, 0),
    overworld_frame(sFollowerPicSlowpoke, 4, 4, 1),
    overworld_frame(sFollowerPicSlowpoke, 4, 4, 2),
    overworld_frame(sFollowerPicSlowpoke, 4, 4, 3),
    overworld_frame(sFollowerPicSlowpoke, 4, 4, 4),
    overworld_frame(sFollowerPicSlowpoke, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSlowpoke =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF04E,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSlowpoke,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSlowbro[] = INCBIN_U32("mods/following_pokemon/graphics/followers/slowbro.4bpp");
static const u16 sFollowerPalSlowbro[] = INCBIN_U16("mods/following_pokemon/graphics/followers/slowbro.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSlowbro = { sFollowerPalSlowbro, 0xF04F };
static const struct SpriteFrameImage sFollowerPicTableSlowbro[] =
{
    overworld_frame(sFollowerPicSlowbro, 4, 4, 0),
    overworld_frame(sFollowerPicSlowbro, 4, 4, 1),
    overworld_frame(sFollowerPicSlowbro, 4, 4, 2),
    overworld_frame(sFollowerPicSlowbro, 4, 4, 3),
    overworld_frame(sFollowerPicSlowbro, 4, 4, 4),
    overworld_frame(sFollowerPicSlowbro, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSlowbro =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF04F,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSlowbro,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMagnemite[] = INCBIN_U32("mods/following_pokemon/graphics/followers/magnemite.4bpp");
static const u16 sFollowerPalMagnemite[] = INCBIN_U16("mods/following_pokemon/graphics/followers/magnemite.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMagnemite = { sFollowerPalMagnemite, 0xF050 };
static const struct SpriteFrameImage sFollowerPicTableMagnemite[] =
{
    overworld_frame(sFollowerPicMagnemite, 4, 4, 0),
    overworld_frame(sFollowerPicMagnemite, 4, 4, 1),
    overworld_frame(sFollowerPicMagnemite, 4, 4, 2),
    overworld_frame(sFollowerPicMagnemite, 4, 4, 3),
    overworld_frame(sFollowerPicMagnemite, 4, 4, 4),
    overworld_frame(sFollowerPicMagnemite, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMagnemite =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF050,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMagnemite,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMagneton[] = INCBIN_U32("mods/following_pokemon/graphics/followers/magneton.4bpp");
static const u16 sFollowerPalMagneton[] = INCBIN_U16("mods/following_pokemon/graphics/followers/magneton.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMagneton = { sFollowerPalMagneton, 0xF051 };
static const struct SpriteFrameImage sFollowerPicTableMagneton[] =
{
    overworld_frame(sFollowerPicMagneton, 4, 4, 0),
    overworld_frame(sFollowerPicMagneton, 4, 4, 1),
    overworld_frame(sFollowerPicMagneton, 4, 4, 2),
    overworld_frame(sFollowerPicMagneton, 4, 4, 3),
    overworld_frame(sFollowerPicMagneton, 4, 4, 4),
    overworld_frame(sFollowerPicMagneton, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMagneton =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF051,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMagneton,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicFarfetchd[] = INCBIN_U32("mods/following_pokemon/graphics/followers/farfetchd.4bpp");
static const u16 sFollowerPalFarfetchd[] = INCBIN_U16("mods/following_pokemon/graphics/followers/farfetchd.gbapal");
const struct SpritePalette gFollowingPokemonPaletteFarfetchd = { sFollowerPalFarfetchd, 0xF052 };
static const struct SpriteFrameImage sFollowerPicTableFarfetchd[] =
{
    overworld_frame(sFollowerPicFarfetchd, 4, 4, 0),
    overworld_frame(sFollowerPicFarfetchd, 4, 4, 1),
    overworld_frame(sFollowerPicFarfetchd, 4, 4, 2),
    overworld_frame(sFollowerPicFarfetchd, 4, 4, 3),
    overworld_frame(sFollowerPicFarfetchd, 4, 4, 4),
    overworld_frame(sFollowerPicFarfetchd, 4, 4, 5),
    overworld_frame(sFollowerPicFarfetchd, 4, 4, 6),
    overworld_frame(sFollowerPicFarfetchd, 4, 4, 7),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoFarfetchd =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF052,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableUniqueEast,
    .images = sFollowerPicTableFarfetchd,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicDoduo[] = INCBIN_U32("mods/following_pokemon/graphics/followers/doduo.4bpp");
static const u16 sFollowerPalDoduo[] = INCBIN_U16("mods/following_pokemon/graphics/followers/doduo.gbapal");
const struct SpritePalette gFollowingPokemonPaletteDoduo = { sFollowerPalDoduo, 0xF053 };
static const struct SpriteFrameImage sFollowerPicTableDoduo[] =
{
    overworld_frame(sFollowerPicDoduo, 4, 4, 0),
    overworld_frame(sFollowerPicDoduo, 4, 4, 1),
    overworld_frame(sFollowerPicDoduo, 4, 4, 2),
    overworld_frame(sFollowerPicDoduo, 4, 4, 3),
    overworld_frame(sFollowerPicDoduo, 4, 4, 4),
    overworld_frame(sFollowerPicDoduo, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoDoduo =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF053,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableDoduo,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicDodrio[] = INCBIN_U32("mods/following_pokemon/graphics/followers/dodrio.4bpp");
static const u16 sFollowerPalDodrio[] = INCBIN_U16("mods/following_pokemon/graphics/followers/dodrio.gbapal");
const struct SpritePalette gFollowingPokemonPaletteDodrio = { sFollowerPalDodrio, 0xF054 };
static const struct SpriteFrameImage sFollowerPicTableDodrio[] =
{
    overworld_frame(sFollowerPicDodrio, 4, 4, 0),
    overworld_frame(sFollowerPicDodrio, 4, 4, 1),
    overworld_frame(sFollowerPicDodrio, 4, 4, 2),
    overworld_frame(sFollowerPicDodrio, 4, 4, 3),
    overworld_frame(sFollowerPicDodrio, 4, 4, 4),
    overworld_frame(sFollowerPicDodrio, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoDodrio =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF054,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableDodrio,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSeel[] = INCBIN_U32("mods/following_pokemon/graphics/followers/seel.4bpp");
static const u16 sFollowerPalSeel[] = INCBIN_U16("mods/following_pokemon/graphics/followers/seel.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSeel = { sFollowerPalSeel, 0xF055 };
static const struct SpriteFrameImage sFollowerPicTableSeel[] =
{
    overworld_frame(sFollowerPicSeel, 4, 4, 0),
    overworld_frame(sFollowerPicSeel, 4, 4, 1),
    overworld_frame(sFollowerPicSeel, 4, 4, 2),
    overworld_frame(sFollowerPicSeel, 4, 4, 3),
    overworld_frame(sFollowerPicSeel, 4, 4, 4),
    overworld_frame(sFollowerPicSeel, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSeel =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF055,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSeel,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicDewgong[] = INCBIN_U32("mods/following_pokemon/graphics/followers/dewgong.4bpp");
static const u16 sFollowerPalDewgong[] = INCBIN_U16("mods/following_pokemon/graphics/followers/dewgong.gbapal");
const struct SpritePalette gFollowingPokemonPaletteDewgong = { sFollowerPalDewgong, 0xF056 };
static const struct SpriteFrameImage sFollowerPicTableDewgong[] =
{
    overworld_frame(sFollowerPicDewgong, 4, 4, 0),
    overworld_frame(sFollowerPicDewgong, 4, 4, 1),
    overworld_frame(sFollowerPicDewgong, 4, 4, 2),
    overworld_frame(sFollowerPicDewgong, 4, 4, 3),
    overworld_frame(sFollowerPicDewgong, 4, 4, 4),
    overworld_frame(sFollowerPicDewgong, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoDewgong =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF056,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableDewgong,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicGrimer[] = INCBIN_U32("mods/following_pokemon/graphics/followers/grimer.4bpp");
static const u16 sFollowerPalGrimer[] = INCBIN_U16("mods/following_pokemon/graphics/followers/grimer.gbapal");
const struct SpritePalette gFollowingPokemonPaletteGrimer = { sFollowerPalGrimer, 0xF057 };
static const struct SpriteFrameImage sFollowerPicTableGrimer[] =
{
    overworld_frame(sFollowerPicGrimer, 4, 4, 0),
    overworld_frame(sFollowerPicGrimer, 4, 4, 1),
    overworld_frame(sFollowerPicGrimer, 4, 4, 2),
    overworld_frame(sFollowerPicGrimer, 4, 4, 3),
    overworld_frame(sFollowerPicGrimer, 4, 4, 4),
    overworld_frame(sFollowerPicGrimer, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoGrimer =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF057,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableGrimer,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMuk[] = INCBIN_U32("mods/following_pokemon/graphics/followers/muk.4bpp");
static const u16 sFollowerPalMuk[] = INCBIN_U16("mods/following_pokemon/graphics/followers/muk.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMuk = { sFollowerPalMuk, 0xF058 };
static const struct SpriteFrameImage sFollowerPicTableMuk[] =
{
    overworld_frame(sFollowerPicMuk, 4, 4, 0),
    overworld_frame(sFollowerPicMuk, 4, 4, 1),
    overworld_frame(sFollowerPicMuk, 4, 4, 2),
    overworld_frame(sFollowerPicMuk, 4, 4, 3),
    overworld_frame(sFollowerPicMuk, 4, 4, 4),
    overworld_frame(sFollowerPicMuk, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMuk =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF058,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMuk,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicShellder[] = INCBIN_U32("mods/following_pokemon/graphics/followers/shellder.4bpp");
static const u16 sFollowerPalShellder[] = INCBIN_U16("mods/following_pokemon/graphics/followers/shellder.gbapal");
const struct SpritePalette gFollowingPokemonPaletteShellder = { sFollowerPalShellder, 0xF059 };
static const struct SpriteFrameImage sFollowerPicTableShellder[] =
{
    overworld_frame(sFollowerPicShellder, 4, 4, 0),
    overworld_frame(sFollowerPicShellder, 4, 4, 1),
    overworld_frame(sFollowerPicShellder, 4, 4, 2),
    overworld_frame(sFollowerPicShellder, 4, 4, 3),
    overworld_frame(sFollowerPicShellder, 4, 4, 4),
    overworld_frame(sFollowerPicShellder, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoShellder =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF059,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableShellder,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicCloyster[] = INCBIN_U32("mods/following_pokemon/graphics/followers/cloyster.4bpp");
static const u16 sFollowerPalCloyster[] = INCBIN_U16("mods/following_pokemon/graphics/followers/cloyster.gbapal");
const struct SpritePalette gFollowingPokemonPaletteCloyster = { sFollowerPalCloyster, 0xF05A };
static const struct SpriteFrameImage sFollowerPicTableCloyster[] =
{
    overworld_frame(sFollowerPicCloyster, 4, 4, 0),
    overworld_frame(sFollowerPicCloyster, 4, 4, 1),
    overworld_frame(sFollowerPicCloyster, 4, 4, 2),
    overworld_frame(sFollowerPicCloyster, 4, 4, 3),
    overworld_frame(sFollowerPicCloyster, 4, 4, 4),
    overworld_frame(sFollowerPicCloyster, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoCloyster =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF05A,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableCloyster,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicGastly[] = INCBIN_U32("mods/following_pokemon/graphics/followers/gastly.4bpp");
static const u16 sFollowerPalGastly[] = INCBIN_U16("mods/following_pokemon/graphics/followers/gastly.gbapal");
const struct SpritePalette gFollowingPokemonPaletteGastly = { sFollowerPalGastly, 0xF05B };
static const struct SpriteFrameImage sFollowerPicTableGastly[] =
{
    overworld_frame(sFollowerPicGastly, 4, 4, 0),
    overworld_frame(sFollowerPicGastly, 4, 4, 1),
    overworld_frame(sFollowerPicGastly, 4, 4, 2),
    overworld_frame(sFollowerPicGastly, 4, 4, 3),
    overworld_frame(sFollowerPicGastly, 4, 4, 4),
    overworld_frame(sFollowerPicGastly, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoGastly =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF05B,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableGastly,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicHaunter[] = INCBIN_U32("mods/following_pokemon/graphics/followers/haunter.4bpp");
static const u16 sFollowerPalHaunter[] = INCBIN_U16("mods/following_pokemon/graphics/followers/haunter.gbapal");
const struct SpritePalette gFollowingPokemonPaletteHaunter = { sFollowerPalHaunter, 0xF05C };
static const struct SpriteFrameImage sFollowerPicTableHaunter[] =
{
    overworld_frame(sFollowerPicHaunter, 4, 4, 0),
    overworld_frame(sFollowerPicHaunter, 4, 4, 1),
    overworld_frame(sFollowerPicHaunter, 4, 4, 2),
    overworld_frame(sFollowerPicHaunter, 4, 4, 3),
    overworld_frame(sFollowerPicHaunter, 4, 4, 4),
    overworld_frame(sFollowerPicHaunter, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoHaunter =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF05C,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableHaunter,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicGengar[] = INCBIN_U32("mods/following_pokemon/graphics/followers/gengar.4bpp");
static const u16 sFollowerPalGengar[] = INCBIN_U16("mods/following_pokemon/graphics/followers/gengar.gbapal");
const struct SpritePalette gFollowingPokemonPaletteGengar = { sFollowerPalGengar, 0xF05D };
static const struct SpriteFrameImage sFollowerPicTableGengar[] =
{
    overworld_frame(sFollowerPicGengar, 4, 4, 0),
    overworld_frame(sFollowerPicGengar, 4, 4, 1),
    overworld_frame(sFollowerPicGengar, 4, 4, 2),
    overworld_frame(sFollowerPicGengar, 4, 4, 3),
    overworld_frame(sFollowerPicGengar, 4, 4, 4),
    overworld_frame(sFollowerPicGengar, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoGengar =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF05D,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableGengar,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicOnix[] = INCBIN_U32("mods/following_pokemon/graphics/followers/onix.4bpp");
static const u16 sFollowerPalOnix[] = INCBIN_U16("mods/following_pokemon/graphics/followers/onix.gbapal");
const struct SpritePalette gFollowingPokemonPaletteOnix = { sFollowerPalOnix, 0xF05E };
static const struct SpriteFrameImage sFollowerPicTableOnix[] =
{
    overworld_frame(sFollowerPicOnix, 4, 4, 0),
    overworld_frame(sFollowerPicOnix, 4, 4, 1),
    overworld_frame(sFollowerPicOnix, 4, 4, 2),
    overworld_frame(sFollowerPicOnix, 4, 4, 3),
    overworld_frame(sFollowerPicOnix, 4, 4, 4),
    overworld_frame(sFollowerPicOnix, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoOnix =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF05E,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableOnix,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicDrowzee[] = INCBIN_U32("mods/following_pokemon/graphics/followers/drowzee.4bpp");
static const u16 sFollowerPalDrowzee[] = INCBIN_U16("mods/following_pokemon/graphics/followers/drowzee.gbapal");
const struct SpritePalette gFollowingPokemonPaletteDrowzee = { sFollowerPalDrowzee, 0xF05F };
static const struct SpriteFrameImage sFollowerPicTableDrowzee[] =
{
    overworld_frame(sFollowerPicDrowzee, 4, 4, 0),
    overworld_frame(sFollowerPicDrowzee, 4, 4, 1),
    overworld_frame(sFollowerPicDrowzee, 4, 4, 2),
    overworld_frame(sFollowerPicDrowzee, 4, 4, 3),
    overworld_frame(sFollowerPicDrowzee, 4, 4, 4),
    overworld_frame(sFollowerPicDrowzee, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoDrowzee =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF05F,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableDrowzee,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicHypno[] = INCBIN_U32("mods/following_pokemon/graphics/followers/hypno.4bpp");
static const u16 sFollowerPalHypno[] = INCBIN_U16("mods/following_pokemon/graphics/followers/hypno.gbapal");
const struct SpritePalette gFollowingPokemonPaletteHypno = { sFollowerPalHypno, 0xF060 };
static const struct SpriteFrameImage sFollowerPicTableHypno[] =
{
    overworld_frame(sFollowerPicHypno, 4, 4, 0),
    overworld_frame(sFollowerPicHypno, 4, 4, 1),
    overworld_frame(sFollowerPicHypno, 4, 4, 2),
    overworld_frame(sFollowerPicHypno, 4, 4, 3),
    overworld_frame(sFollowerPicHypno, 4, 4, 4),
    overworld_frame(sFollowerPicHypno, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoHypno =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF060,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableHypno,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicKrabby[] = INCBIN_U32("mods/following_pokemon/graphics/followers/krabby.4bpp");
static const u16 sFollowerPalKrabby[] = INCBIN_U16("mods/following_pokemon/graphics/followers/krabby.gbapal");
const struct SpritePalette gFollowingPokemonPaletteKrabby = { sFollowerPalKrabby, 0xF061 };
static const struct SpriteFrameImage sFollowerPicTableKrabby[] =
{
    overworld_frame(sFollowerPicKrabby, 4, 4, 0),
    overworld_frame(sFollowerPicKrabby, 4, 4, 1),
    overworld_frame(sFollowerPicKrabby, 4, 4, 2),
    overworld_frame(sFollowerPicKrabby, 4, 4, 3),
    overworld_frame(sFollowerPicKrabby, 4, 4, 4),
    overworld_frame(sFollowerPicKrabby, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoKrabby =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF061,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableUniqueEastDoubleNorth,
    .images = sFollowerPicTableKrabby,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicKingler[] = INCBIN_U32("mods/following_pokemon/graphics/followers/kingler.4bpp");
static const u16 sFollowerPalKingler[] = INCBIN_U16("mods/following_pokemon/graphics/followers/kingler.gbapal");
const struct SpritePalette gFollowingPokemonPaletteKingler = { sFollowerPalKingler, 0xF062 };
static const struct SpriteFrameImage sFollowerPicTableKingler[] =
{
    overworld_frame(sFollowerPicKingler, 4, 4, 0),
    overworld_frame(sFollowerPicKingler, 4, 4, 1),
    overworld_frame(sFollowerPicKingler, 4, 4, 2),
    overworld_frame(sFollowerPicKingler, 4, 4, 3),
    overworld_frame(sFollowerPicKingler, 4, 4, 4),
    overworld_frame(sFollowerPicKingler, 4, 4, 5),
    overworld_frame(sFollowerPicKingler, 4, 4, 6),
    overworld_frame(sFollowerPicKingler, 4, 4, 7),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoKingler =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF062,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableUniqueEast,
    .images = sFollowerPicTableKingler,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicVoltorb[] = INCBIN_U32("mods/following_pokemon/graphics/followers/voltorb.4bpp");
static const u16 sFollowerPalVoltorb[] = INCBIN_U16("mods/following_pokemon/graphics/followers/voltorb.gbapal");
const struct SpritePalette gFollowingPokemonPaletteVoltorb = { sFollowerPalVoltorb, 0xF063 };
static const struct SpriteFrameImage sFollowerPicTableVoltorb[] =
{
    overworld_frame(sFollowerPicVoltorb, 4, 4, 0),
    overworld_frame(sFollowerPicVoltorb, 4, 4, 1),
    overworld_frame(sFollowerPicVoltorb, 4, 4, 2),
    overworld_frame(sFollowerPicVoltorb, 4, 4, 3),
    overworld_frame(sFollowerPicVoltorb, 4, 4, 4),
    overworld_frame(sFollowerPicVoltorb, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoVoltorb =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF063,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableVoltorb,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicElectrode[] = INCBIN_U32("mods/following_pokemon/graphics/followers/electrode.4bpp");
static const u16 sFollowerPalElectrode[] = INCBIN_U16("mods/following_pokemon/graphics/followers/electrode.gbapal");
const struct SpritePalette gFollowingPokemonPaletteElectrode = { sFollowerPalElectrode, 0xF064 };
static const struct SpriteFrameImage sFollowerPicTableElectrode[] =
{
    overworld_frame(sFollowerPicElectrode, 4, 4, 0),
    overworld_frame(sFollowerPicElectrode, 4, 4, 1),
    overworld_frame(sFollowerPicElectrode, 4, 4, 2),
    overworld_frame(sFollowerPicElectrode, 4, 4, 3),
    overworld_frame(sFollowerPicElectrode, 4, 4, 4),
    overworld_frame(sFollowerPicElectrode, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoElectrode =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF064,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableElectrode,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicExeggcute[] = INCBIN_U32("mods/following_pokemon/graphics/followers/exeggcute.4bpp");
static const u16 sFollowerPalExeggcute[] = INCBIN_U16("mods/following_pokemon/graphics/followers/exeggcute.gbapal");
const struct SpritePalette gFollowingPokemonPaletteExeggcute = { sFollowerPalExeggcute, 0xF065 };
static const struct SpriteFrameImage sFollowerPicTableExeggcute[] =
{
    overworld_frame(sFollowerPicExeggcute, 4, 4, 0),
    overworld_frame(sFollowerPicExeggcute, 4, 4, 1),
    overworld_frame(sFollowerPicExeggcute, 4, 4, 2),
    overworld_frame(sFollowerPicExeggcute, 4, 4, 3),
    overworld_frame(sFollowerPicExeggcute, 4, 4, 4),
    overworld_frame(sFollowerPicExeggcute, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoExeggcute =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF065,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableExeggcute,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicExeggutor[] = INCBIN_U32("mods/following_pokemon/graphics/followers/exeggutor.4bpp");
static const u16 sFollowerPalExeggutor[] = INCBIN_U16("mods/following_pokemon/graphics/followers/exeggutor.gbapal");
const struct SpritePalette gFollowingPokemonPaletteExeggutor = { sFollowerPalExeggutor, 0xF066 };
static const struct SpriteFrameImage sFollowerPicTableExeggutor[] =
{
    overworld_frame(sFollowerPicExeggutor, 4, 4, 0),
    overworld_frame(sFollowerPicExeggutor, 4, 4, 1),
    overworld_frame(sFollowerPicExeggutor, 4, 4, 2),
    overworld_frame(sFollowerPicExeggutor, 4, 4, 3),
    overworld_frame(sFollowerPicExeggutor, 4, 4, 4),
    overworld_frame(sFollowerPicExeggutor, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoExeggutor =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF066,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableExeggutor,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicCubone[] = INCBIN_U32("mods/following_pokemon/graphics/followers/cubone.4bpp");
static const u16 sFollowerPalCubone[] = INCBIN_U16("mods/following_pokemon/graphics/followers/cubone.gbapal");
const struct SpritePalette gFollowingPokemonPaletteCubone = { sFollowerPalCubone, 0xF067 };
static const struct SpriteFrameImage sFollowerPicTableCubone[] =
{
    overworld_frame(sFollowerPicCubone, 4, 4, 0),
    overworld_frame(sFollowerPicCubone, 4, 4, 1),
    overworld_frame(sFollowerPicCubone, 4, 4, 2),
    overworld_frame(sFollowerPicCubone, 4, 4, 3),
    overworld_frame(sFollowerPicCubone, 4, 4, 4),
    overworld_frame(sFollowerPicCubone, 4, 4, 5),
    overworld_frame(sFollowerPicCubone, 4, 4, 6),
    overworld_frame(sFollowerPicCubone, 4, 4, 7),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoCubone =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF067,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableUniqueEast,
    .images = sFollowerPicTableCubone,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMarowak[] = INCBIN_U32("mods/following_pokemon/graphics/followers/marowak.4bpp");
static const u16 sFollowerPalMarowak[] = INCBIN_U16("mods/following_pokemon/graphics/followers/marowak.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMarowak = { sFollowerPalMarowak, 0xF068 };
static const struct SpriteFrameImage sFollowerPicTableMarowak[] =
{
    overworld_frame(sFollowerPicMarowak, 4, 4, 0),
    overworld_frame(sFollowerPicMarowak, 4, 4, 1),
    overworld_frame(sFollowerPicMarowak, 4, 4, 2),
    overworld_frame(sFollowerPicMarowak, 4, 4, 3),
    overworld_frame(sFollowerPicMarowak, 4, 4, 4),
    overworld_frame(sFollowerPicMarowak, 4, 4, 5),
    overworld_frame(sFollowerPicMarowak, 4, 4, 6),
    overworld_frame(sFollowerPicMarowak, 4, 4, 7),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMarowak =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF068,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableUniqueEast,
    .images = sFollowerPicTableMarowak,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicHitmonlee[] = INCBIN_U32("mods/following_pokemon/graphics/followers/hitmonlee.4bpp");
static const u16 sFollowerPalHitmonlee[] = INCBIN_U16("mods/following_pokemon/graphics/followers/hitmonlee.gbapal");
const struct SpritePalette gFollowingPokemonPaletteHitmonlee = { sFollowerPalHitmonlee, 0xF069 };
static const struct SpriteFrameImage sFollowerPicTableHitmonlee[] =
{
    overworld_frame(sFollowerPicHitmonlee, 4, 4, 0),
    overworld_frame(sFollowerPicHitmonlee, 4, 4, 1),
    overworld_frame(sFollowerPicHitmonlee, 4, 4, 2),
    overworld_frame(sFollowerPicHitmonlee, 4, 4, 3),
    overworld_frame(sFollowerPicHitmonlee, 4, 4, 4),
    overworld_frame(sFollowerPicHitmonlee, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoHitmonlee =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF069,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableHitmonlee,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicHitmonchan[] = INCBIN_U32("mods/following_pokemon/graphics/followers/hitmonchan.4bpp");
static const u16 sFollowerPalHitmonchan[] = INCBIN_U16("mods/following_pokemon/graphics/followers/hitmonchan.gbapal");
const struct SpritePalette gFollowingPokemonPaletteHitmonchan = { sFollowerPalHitmonchan, 0xF06A };
static const struct SpriteFrameImage sFollowerPicTableHitmonchan[] =
{
    overworld_frame(sFollowerPicHitmonchan, 4, 4, 0),
    overworld_frame(sFollowerPicHitmonchan, 4, 4, 1),
    overworld_frame(sFollowerPicHitmonchan, 4, 4, 2),
    overworld_frame(sFollowerPicHitmonchan, 4, 4, 3),
    overworld_frame(sFollowerPicHitmonchan, 4, 4, 4),
    overworld_frame(sFollowerPicHitmonchan, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoHitmonchan =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF06A,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableHitmonchan,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicLickitung[] = INCBIN_U32("mods/following_pokemon/graphics/followers/lickitung.4bpp");
static const u16 sFollowerPalLickitung[] = INCBIN_U16("mods/following_pokemon/graphics/followers/lickitung.gbapal");
const struct SpritePalette gFollowingPokemonPaletteLickitung = { sFollowerPalLickitung, 0xF06B };
static const struct SpriteFrameImage sFollowerPicTableLickitung[] =
{
    overworld_frame(sFollowerPicLickitung, 4, 4, 0),
    overworld_frame(sFollowerPicLickitung, 4, 4, 1),
    overworld_frame(sFollowerPicLickitung, 4, 4, 2),
    overworld_frame(sFollowerPicLickitung, 4, 4, 3),
    overworld_frame(sFollowerPicLickitung, 4, 4, 4),
    overworld_frame(sFollowerPicLickitung, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoLickitung =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF06B,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableLickitung,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicKoffing[] = INCBIN_U32("mods/following_pokemon/graphics/followers/koffing.4bpp");
static const u16 sFollowerPalKoffing[] = INCBIN_U16("mods/following_pokemon/graphics/followers/koffing.gbapal");
const struct SpritePalette gFollowingPokemonPaletteKoffing = { sFollowerPalKoffing, 0xF06C };
static const struct SpriteFrameImage sFollowerPicTableKoffing[] =
{
    overworld_frame(sFollowerPicKoffing, 4, 4, 0),
    overworld_frame(sFollowerPicKoffing, 4, 4, 1),
    overworld_frame(sFollowerPicKoffing, 4, 4, 2),
    overworld_frame(sFollowerPicKoffing, 4, 4, 3),
    overworld_frame(sFollowerPicKoffing, 4, 4, 4),
    overworld_frame(sFollowerPicKoffing, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoKoffing =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF06C,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableKoffing,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicWeezing[] = INCBIN_U32("mods/following_pokemon/graphics/followers/weezing.4bpp");
static const u16 sFollowerPalWeezing[] = INCBIN_U16("mods/following_pokemon/graphics/followers/weezing.gbapal");
const struct SpritePalette gFollowingPokemonPaletteWeezing = { sFollowerPalWeezing, 0xF06D };
static const struct SpriteFrameImage sFollowerPicTableWeezing[] =
{
    overworld_frame(sFollowerPicWeezing, 4, 4, 0),
    overworld_frame(sFollowerPicWeezing, 4, 4, 1),
    overworld_frame(sFollowerPicWeezing, 4, 4, 2),
    overworld_frame(sFollowerPicWeezing, 4, 4, 3),
    overworld_frame(sFollowerPicWeezing, 4, 4, 4),
    overworld_frame(sFollowerPicWeezing, 4, 4, 5),
    overworld_frame(sFollowerPicWeezing, 4, 4, 6),
    overworld_frame(sFollowerPicWeezing, 4, 4, 7),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoWeezing =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF06D,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableUniqueEast,
    .images = sFollowerPicTableWeezing,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicRhyhorn[] = INCBIN_U32("mods/following_pokemon/graphics/followers/rhyhorn.4bpp");
static const u16 sFollowerPalRhyhorn[] = INCBIN_U16("mods/following_pokemon/graphics/followers/rhyhorn.gbapal");
const struct SpritePalette gFollowingPokemonPaletteRhyhorn = { sFollowerPalRhyhorn, 0xF06E };
static const struct SpriteFrameImage sFollowerPicTableRhyhorn[] =
{
    overworld_frame(sFollowerPicRhyhorn, 4, 4, 0),
    overworld_frame(sFollowerPicRhyhorn, 4, 4, 1),
    overworld_frame(sFollowerPicRhyhorn, 4, 4, 2),
    overworld_frame(sFollowerPicRhyhorn, 4, 4, 3),
    overworld_frame(sFollowerPicRhyhorn, 4, 4, 4),
    overworld_frame(sFollowerPicRhyhorn, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoRhyhorn =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF06E,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableRhyhorn,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicRhydon[] = INCBIN_U32("mods/following_pokemon/graphics/followers/rhydon.4bpp");
static const u16 sFollowerPalRhydon[] = INCBIN_U16("mods/following_pokemon/graphics/followers/rhydon.gbapal");
const struct SpritePalette gFollowingPokemonPaletteRhydon = { sFollowerPalRhydon, 0xF06F };
static const struct SpriteFrameImage sFollowerPicTableRhydon[] =
{
    overworld_frame(sFollowerPicRhydon, 4, 4, 0),
    overworld_frame(sFollowerPicRhydon, 4, 4, 1),
    overworld_frame(sFollowerPicRhydon, 4, 4, 2),
    overworld_frame(sFollowerPicRhydon, 4, 4, 3),
    overworld_frame(sFollowerPicRhydon, 4, 4, 4),
    overworld_frame(sFollowerPicRhydon, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoRhydon =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF06F,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableRhydon,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicChansey[] = INCBIN_U32("mods/following_pokemon/graphics/followers/chansey.4bpp");
static const u16 sFollowerPalChansey[] = INCBIN_U16("mods/following_pokemon/graphics/followers/chansey.gbapal");
const struct SpritePalette gFollowingPokemonPaletteChansey = { sFollowerPalChansey, 0xF070 };
static const struct SpriteFrameImage sFollowerPicTableChansey[] =
{
    overworld_frame(sFollowerPicChansey, 4, 4, 0),
    overworld_frame(sFollowerPicChansey, 4, 4, 1),
    overworld_frame(sFollowerPicChansey, 4, 4, 2),
    overworld_frame(sFollowerPicChansey, 4, 4, 3),
    overworld_frame(sFollowerPicChansey, 4, 4, 4),
    overworld_frame(sFollowerPicChansey, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoChansey =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF070,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableChansey,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicTangela[] = INCBIN_U32("mods/following_pokemon/graphics/followers/tangela.4bpp");
static const u16 sFollowerPalTangela[] = INCBIN_U16("mods/following_pokemon/graphics/followers/tangela.gbapal");
const struct SpritePalette gFollowingPokemonPaletteTangela = { sFollowerPalTangela, 0xF071 };
static const struct SpriteFrameImage sFollowerPicTableTangela[] =
{
    overworld_frame(sFollowerPicTangela, 4, 4, 0),
    overworld_frame(sFollowerPicTangela, 4, 4, 1),
    overworld_frame(sFollowerPicTangela, 4, 4, 2),
    overworld_frame(sFollowerPicTangela, 4, 4, 3),
    overworld_frame(sFollowerPicTangela, 4, 4, 4),
    overworld_frame(sFollowerPicTangela, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoTangela =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF071,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableTangela,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicKangaskhan[] = INCBIN_U32("mods/following_pokemon/graphics/followers/kangaskhan.4bpp");
static const u16 sFollowerPalKangaskhan[] = INCBIN_U16("mods/following_pokemon/graphics/followers/kangaskhan.gbapal");
const struct SpritePalette gFollowingPokemonPaletteKangaskhan = { sFollowerPalKangaskhan, 0xF072 };
static const struct SpriteFrameImage sFollowerPicTableKangaskhan[] =
{
    overworld_frame(sFollowerPicKangaskhan, 4, 4, 0),
    overworld_frame(sFollowerPicKangaskhan, 4, 4, 1),
    overworld_frame(sFollowerPicKangaskhan, 4, 4, 2),
    overworld_frame(sFollowerPicKangaskhan, 4, 4, 3),
    overworld_frame(sFollowerPicKangaskhan, 4, 4, 4),
    overworld_frame(sFollowerPicKangaskhan, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoKangaskhan =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF072,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableKangaskhan,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicHorsea[] = INCBIN_U32("mods/following_pokemon/graphics/followers/horsea.4bpp");
static const u16 sFollowerPalHorsea[] = INCBIN_U16("mods/following_pokemon/graphics/followers/horsea.gbapal");
const struct SpritePalette gFollowingPokemonPaletteHorsea = { sFollowerPalHorsea, 0xF073 };
static const struct SpriteFrameImage sFollowerPicTableHorsea[] =
{
    overworld_frame(sFollowerPicHorsea, 4, 4, 0),
    overworld_frame(sFollowerPicHorsea, 4, 4, 1),
    overworld_frame(sFollowerPicHorsea, 4, 4, 2),
    overworld_frame(sFollowerPicHorsea, 4, 4, 3),
    overworld_frame(sFollowerPicHorsea, 4, 4, 4),
    overworld_frame(sFollowerPicHorsea, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoHorsea =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF073,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableHorsea,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSeadra[] = INCBIN_U32("mods/following_pokemon/graphics/followers/seadra.4bpp");
static const u16 sFollowerPalSeadra[] = INCBIN_U16("mods/following_pokemon/graphics/followers/seadra.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSeadra = { sFollowerPalSeadra, 0xF074 };
static const struct SpriteFrameImage sFollowerPicTableSeadra[] =
{
    overworld_frame(sFollowerPicSeadra, 4, 4, 0),
    overworld_frame(sFollowerPicSeadra, 4, 4, 1),
    overworld_frame(sFollowerPicSeadra, 4, 4, 2),
    overworld_frame(sFollowerPicSeadra, 4, 4, 3),
    overworld_frame(sFollowerPicSeadra, 4, 4, 4),
    overworld_frame(sFollowerPicSeadra, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSeadra =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF074,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSeadra,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicGoldeen[] = INCBIN_U32("mods/following_pokemon/graphics/followers/goldeen.4bpp");
static const u16 sFollowerPalGoldeen[] = INCBIN_U16("mods/following_pokemon/graphics/followers/goldeen.gbapal");
const struct SpritePalette gFollowingPokemonPaletteGoldeen = { sFollowerPalGoldeen, 0xF075 };
static const struct SpriteFrameImage sFollowerPicTableGoldeen[] =
{
    overworld_frame(sFollowerPicGoldeen, 4, 4, 0),
    overworld_frame(sFollowerPicGoldeen, 4, 4, 1),
    overworld_frame(sFollowerPicGoldeen, 4, 4, 2),
    overworld_frame(sFollowerPicGoldeen, 4, 4, 3),
    overworld_frame(sFollowerPicGoldeen, 4, 4, 4),
    overworld_frame(sFollowerPicGoldeen, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoGoldeen =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF075,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableGoldeen,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSeaking[] = INCBIN_U32("mods/following_pokemon/graphics/followers/seaking.4bpp");
static const u16 sFollowerPalSeaking[] = INCBIN_U16("mods/following_pokemon/graphics/followers/seaking.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSeaking = { sFollowerPalSeaking, 0xF076 };
static const struct SpriteFrameImage sFollowerPicTableSeaking[] =
{
    overworld_frame(sFollowerPicSeaking, 4, 4, 0),
    overworld_frame(sFollowerPicSeaking, 4, 4, 1),
    overworld_frame(sFollowerPicSeaking, 4, 4, 2),
    overworld_frame(sFollowerPicSeaking, 4, 4, 3),
    overworld_frame(sFollowerPicSeaking, 4, 4, 4),
    overworld_frame(sFollowerPicSeaking, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSeaking =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF076,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSeaking,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicStaryu[] = INCBIN_U32("mods/following_pokemon/graphics/followers/staryu.4bpp");
static const u16 sFollowerPalStaryu[] = INCBIN_U16("mods/following_pokemon/graphics/followers/staryu.gbapal");
const struct SpritePalette gFollowingPokemonPaletteStaryu = { sFollowerPalStaryu, 0xF077 };
static const struct SpriteFrameImage sFollowerPicTableStaryu[] =
{
    overworld_frame(sFollowerPicStaryu, 4, 4, 0),
    overworld_frame(sFollowerPicStaryu, 4, 4, 1),
    overworld_frame(sFollowerPicStaryu, 4, 4, 2),
    overworld_frame(sFollowerPicStaryu, 4, 4, 3),
    overworld_frame(sFollowerPicStaryu, 4, 4, 4),
    overworld_frame(sFollowerPicStaryu, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoStaryu =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF077,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableStaryu,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicStarmie[] = INCBIN_U32("mods/following_pokemon/graphics/followers/starmie.4bpp");
static const u16 sFollowerPalStarmie[] = INCBIN_U16("mods/following_pokemon/graphics/followers/starmie.gbapal");
const struct SpritePalette gFollowingPokemonPaletteStarmie = { sFollowerPalStarmie, 0xF078 };
static const struct SpriteFrameImage sFollowerPicTableStarmie[] =
{
    overworld_frame(sFollowerPicStarmie, 4, 4, 0),
    overworld_frame(sFollowerPicStarmie, 4, 4, 1),
    overworld_frame(sFollowerPicStarmie, 4, 4, 2),
    overworld_frame(sFollowerPicStarmie, 4, 4, 3),
    overworld_frame(sFollowerPicStarmie, 4, 4, 4),
    overworld_frame(sFollowerPicStarmie, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoStarmie =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF078,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableStarmie,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMrMime[] = INCBIN_U32("mods/following_pokemon/graphics/followers/mr_mime.4bpp");
static const u16 sFollowerPalMrMime[] = INCBIN_U16("mods/following_pokemon/graphics/followers/mr_mime.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMrMime = { sFollowerPalMrMime, 0xF079 };
static const struct SpriteFrameImage sFollowerPicTableMrMime[] =
{
    overworld_frame(sFollowerPicMrMime, 4, 4, 0),
    overworld_frame(sFollowerPicMrMime, 4, 4, 1),
    overworld_frame(sFollowerPicMrMime, 4, 4, 2),
    overworld_frame(sFollowerPicMrMime, 4, 4, 3),
    overworld_frame(sFollowerPicMrMime, 4, 4, 4),
    overworld_frame(sFollowerPicMrMime, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMrMime =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF079,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMrMime,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicScyther[] = INCBIN_U32("mods/following_pokemon/graphics/followers/scyther.4bpp");
static const u16 sFollowerPalScyther[] = INCBIN_U16("mods/following_pokemon/graphics/followers/scyther.gbapal");
const struct SpritePalette gFollowingPokemonPaletteScyther = { sFollowerPalScyther, 0xF07A };
static const struct SpriteFrameImage sFollowerPicTableScyther[] =
{
    overworld_frame(sFollowerPicScyther, 4, 4, 0),
    overworld_frame(sFollowerPicScyther, 4, 4, 1),
    overworld_frame(sFollowerPicScyther, 4, 4, 2),
    overworld_frame(sFollowerPicScyther, 4, 4, 3),
    overworld_frame(sFollowerPicScyther, 4, 4, 4),
    overworld_frame(sFollowerPicScyther, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoScyther =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF07A,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableScyther,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicJynx[] = INCBIN_U32("mods/following_pokemon/graphics/followers/jynx.4bpp");
static const u16 sFollowerPalJynx[] = INCBIN_U16("mods/following_pokemon/graphics/followers/jynx.gbapal");
const struct SpritePalette gFollowingPokemonPaletteJynx = { sFollowerPalJynx, 0xF07B };
static const struct SpriteFrameImage sFollowerPicTableJynx[] =
{
    overworld_frame(sFollowerPicJynx, 4, 4, 0),
    overworld_frame(sFollowerPicJynx, 4, 4, 1),
    overworld_frame(sFollowerPicJynx, 4, 4, 2),
    overworld_frame(sFollowerPicJynx, 4, 4, 3),
    overworld_frame(sFollowerPicJynx, 4, 4, 4),
    overworld_frame(sFollowerPicJynx, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoJynx =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF07B,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableJynx,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicElectabuzz[] = INCBIN_U32("mods/following_pokemon/graphics/followers/electabuzz.4bpp");
static const u16 sFollowerPalElectabuzz[] = INCBIN_U16("mods/following_pokemon/graphics/followers/electabuzz.gbapal");
const struct SpritePalette gFollowingPokemonPaletteElectabuzz = { sFollowerPalElectabuzz, 0xF07C };
static const struct SpriteFrameImage sFollowerPicTableElectabuzz[] =
{
    overworld_frame(sFollowerPicElectabuzz, 4, 4, 0),
    overworld_frame(sFollowerPicElectabuzz, 4, 4, 1),
    overworld_frame(sFollowerPicElectabuzz, 4, 4, 2),
    overworld_frame(sFollowerPicElectabuzz, 4, 4, 3),
    overworld_frame(sFollowerPicElectabuzz, 4, 4, 4),
    overworld_frame(sFollowerPicElectabuzz, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoElectabuzz =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF07C,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableElectabuzz,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMagmar[] = INCBIN_U32("mods/following_pokemon/graphics/followers/magmar.4bpp");
static const u16 sFollowerPalMagmar[] = INCBIN_U16("mods/following_pokemon/graphics/followers/magmar.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMagmar = { sFollowerPalMagmar, 0xF07D };
static const struct SpriteFrameImage sFollowerPicTableMagmar[] =
{
    overworld_frame(sFollowerPicMagmar, 4, 4, 0),
    overworld_frame(sFollowerPicMagmar, 4, 4, 1),
    overworld_frame(sFollowerPicMagmar, 4, 4, 2),
    overworld_frame(sFollowerPicMagmar, 4, 4, 3),
    overworld_frame(sFollowerPicMagmar, 4, 4, 4),
    overworld_frame(sFollowerPicMagmar, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMagmar =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF07D,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMagmar,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicPinsir[] = INCBIN_U32("mods/following_pokemon/graphics/followers/pinsir.4bpp");
static const u16 sFollowerPalPinsir[] = INCBIN_U16("mods/following_pokemon/graphics/followers/pinsir.gbapal");
const struct SpritePalette gFollowingPokemonPalettePinsir = { sFollowerPalPinsir, 0xF07E };
static const struct SpriteFrameImage sFollowerPicTablePinsir[] =
{
    overworld_frame(sFollowerPicPinsir, 4, 4, 0),
    overworld_frame(sFollowerPicPinsir, 4, 4, 1),
    overworld_frame(sFollowerPicPinsir, 4, 4, 2),
    overworld_frame(sFollowerPicPinsir, 4, 4, 3),
    overworld_frame(sFollowerPicPinsir, 4, 4, 4),
    overworld_frame(sFollowerPicPinsir, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoPinsir =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF07E,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTablePinsir,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicTauros[] = INCBIN_U32("mods/following_pokemon/graphics/followers/tauros.4bpp");
static const u16 sFollowerPalTauros[] = INCBIN_U16("mods/following_pokemon/graphics/followers/tauros.gbapal");
const struct SpritePalette gFollowingPokemonPaletteTauros = { sFollowerPalTauros, 0xF07F };
static const struct SpriteFrameImage sFollowerPicTableTauros[] =
{
    overworld_frame(sFollowerPicTauros, 4, 4, 0),
    overworld_frame(sFollowerPicTauros, 4, 4, 1),
    overworld_frame(sFollowerPicTauros, 4, 4, 2),
    overworld_frame(sFollowerPicTauros, 4, 4, 3),
    overworld_frame(sFollowerPicTauros, 4, 4, 4),
    overworld_frame(sFollowerPicTauros, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoTauros =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF07F,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableTauros,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMagikarp[] = INCBIN_U32("mods/following_pokemon/graphics/followers/magikarp.4bpp");
static const u16 sFollowerPalMagikarp[] = INCBIN_U16("mods/following_pokemon/graphics/followers/magikarp.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMagikarp = { sFollowerPalMagikarp, 0xF080 };
static const struct SpriteFrameImage sFollowerPicTableMagikarp[] =
{
    overworld_frame(sFollowerPicMagikarp, 4, 4, 0),
    overworld_frame(sFollowerPicMagikarp, 4, 4, 1),
    overworld_frame(sFollowerPicMagikarp, 4, 4, 2),
    overworld_frame(sFollowerPicMagikarp, 4, 4, 3),
    overworld_frame(sFollowerPicMagikarp, 4, 4, 4),
    overworld_frame(sFollowerPicMagikarp, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMagikarp =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF080,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMagikarp,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicGyarados[] = INCBIN_U32("mods/following_pokemon/graphics/followers/gyarados.4bpp");
static const u16 sFollowerPalGyarados[] = INCBIN_U16("mods/following_pokemon/graphics/followers/gyarados.gbapal");
const struct SpritePalette gFollowingPokemonPaletteGyarados = { sFollowerPalGyarados, 0xF081 };
static const struct SpriteFrameImage sFollowerPicTableGyarados[] =
{
    overworld_frame(sFollowerPicGyarados, 4, 4, 0),
    overworld_frame(sFollowerPicGyarados, 4, 4, 1),
    overworld_frame(sFollowerPicGyarados, 4, 4, 2),
    overworld_frame(sFollowerPicGyarados, 4, 4, 3),
    overworld_frame(sFollowerPicGyarados, 4, 4, 4),
    overworld_frame(sFollowerPicGyarados, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoGyarados =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF081,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableGyarados,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicLapras[] = INCBIN_U32("mods/following_pokemon/graphics/followers/lapras.4bpp");
static const u16 sFollowerPalLapras[] = INCBIN_U16("mods/following_pokemon/graphics/followers/lapras.gbapal");
const struct SpritePalette gFollowingPokemonPaletteLapras = { sFollowerPalLapras, 0xF082 };
static const struct SpriteFrameImage sFollowerPicTableLapras[] =
{
    overworld_frame(sFollowerPicLapras, 4, 4, 0),
    overworld_frame(sFollowerPicLapras, 4, 4, 1),
    overworld_frame(sFollowerPicLapras, 4, 4, 2),
    overworld_frame(sFollowerPicLapras, 4, 4, 3),
    overworld_frame(sFollowerPicLapras, 4, 4, 4),
    overworld_frame(sFollowerPicLapras, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoLapras =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF082,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableLapras,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicDitto[] = INCBIN_U32("mods/following_pokemon/graphics/followers/ditto.4bpp");
static const u16 sFollowerPalDitto[] = INCBIN_U16("mods/following_pokemon/graphics/followers/ditto.gbapal");
const struct SpritePalette gFollowingPokemonPaletteDitto = { sFollowerPalDitto, 0xF083 };
static const struct SpriteFrameImage sFollowerPicTableDitto[] =
{
    overworld_frame(sFollowerPicDitto, 4, 4, 0),
    overworld_frame(sFollowerPicDitto, 4, 4, 1),
    overworld_frame(sFollowerPicDitto, 4, 4, 2),
    overworld_frame(sFollowerPicDitto, 4, 4, 3),
    overworld_frame(sFollowerPicDitto, 4, 4, 4),
    overworld_frame(sFollowerPicDitto, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoDitto =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF083,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableDitto,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicEevee[] = INCBIN_U32("mods/following_pokemon/graphics/followers/eevee.4bpp");
static const u16 sFollowerPalEevee[] = INCBIN_U16("mods/following_pokemon/graphics/followers/eevee.gbapal");
const struct SpritePalette gFollowingPokemonPaletteEevee = { sFollowerPalEevee, 0xF084 };
static const struct SpriteFrameImage sFollowerPicTableEevee[] =
{
    overworld_frame(sFollowerPicEevee, 4, 4, 0),
    overworld_frame(sFollowerPicEevee, 4, 4, 1),
    overworld_frame(sFollowerPicEevee, 4, 4, 2),
    overworld_frame(sFollowerPicEevee, 4, 4, 3),
    overworld_frame(sFollowerPicEevee, 4, 4, 4),
    overworld_frame(sFollowerPicEevee, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoEevee =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF084,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableEevee,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicVaporeon[] = INCBIN_U32("mods/following_pokemon/graphics/followers/vaporeon.4bpp");
static const u16 sFollowerPalVaporeon[] = INCBIN_U16("mods/following_pokemon/graphics/followers/vaporeon.gbapal");
const struct SpritePalette gFollowingPokemonPaletteVaporeon = { sFollowerPalVaporeon, 0xF085 };
static const struct SpriteFrameImage sFollowerPicTableVaporeon[] =
{
    overworld_frame(sFollowerPicVaporeon, 4, 4, 0),
    overworld_frame(sFollowerPicVaporeon, 4, 4, 1),
    overworld_frame(sFollowerPicVaporeon, 4, 4, 2),
    overworld_frame(sFollowerPicVaporeon, 4, 4, 3),
    overworld_frame(sFollowerPicVaporeon, 4, 4, 4),
    overworld_frame(sFollowerPicVaporeon, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoVaporeon =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF085,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableVaporeon,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicJolteon[] = INCBIN_U32("mods/following_pokemon/graphics/followers/jolteon.4bpp");
static const u16 sFollowerPalJolteon[] = INCBIN_U16("mods/following_pokemon/graphics/followers/jolteon.gbapal");
const struct SpritePalette gFollowingPokemonPaletteJolteon = { sFollowerPalJolteon, 0xF086 };
static const struct SpriteFrameImage sFollowerPicTableJolteon[] =
{
    overworld_frame(sFollowerPicJolteon, 4, 4, 0),
    overworld_frame(sFollowerPicJolteon, 4, 4, 1),
    overworld_frame(sFollowerPicJolteon, 4, 4, 2),
    overworld_frame(sFollowerPicJolteon, 4, 4, 3),
    overworld_frame(sFollowerPicJolteon, 4, 4, 4),
    overworld_frame(sFollowerPicJolteon, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoJolteon =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF086,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableJolteon,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicFlareon[] = INCBIN_U32("mods/following_pokemon/graphics/followers/flareon.4bpp");
static const u16 sFollowerPalFlareon[] = INCBIN_U16("mods/following_pokemon/graphics/followers/flareon.gbapal");
const struct SpritePalette gFollowingPokemonPaletteFlareon = { sFollowerPalFlareon, 0xF087 };
static const struct SpriteFrameImage sFollowerPicTableFlareon[] =
{
    overworld_frame(sFollowerPicFlareon, 4, 4, 0),
    overworld_frame(sFollowerPicFlareon, 4, 4, 1),
    overworld_frame(sFollowerPicFlareon, 4, 4, 2),
    overworld_frame(sFollowerPicFlareon, 4, 4, 3),
    overworld_frame(sFollowerPicFlareon, 4, 4, 4),
    overworld_frame(sFollowerPicFlareon, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoFlareon =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF087,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableFlareon,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicPorygon[] = INCBIN_U32("mods/following_pokemon/graphics/followers/porygon.4bpp");
static const u16 sFollowerPalPorygon[] = INCBIN_U16("mods/following_pokemon/graphics/followers/porygon.gbapal");
const struct SpritePalette gFollowingPokemonPalettePorygon = { sFollowerPalPorygon, 0xF088 };
static const struct SpriteFrameImage sFollowerPicTablePorygon[] =
{
    overworld_frame(sFollowerPicPorygon, 4, 4, 0),
    overworld_frame(sFollowerPicPorygon, 4, 4, 1),
    overworld_frame(sFollowerPicPorygon, 4, 4, 2),
    overworld_frame(sFollowerPicPorygon, 4, 4, 3),
    overworld_frame(sFollowerPicPorygon, 4, 4, 4),
    overworld_frame(sFollowerPicPorygon, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoPorygon =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF088,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTablePorygon,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicOmanyte[] = INCBIN_U32("mods/following_pokemon/graphics/followers/omanyte.4bpp");
static const u16 sFollowerPalOmanyte[] = INCBIN_U16("mods/following_pokemon/graphics/followers/omanyte.gbapal");
const struct SpritePalette gFollowingPokemonPaletteOmanyte = { sFollowerPalOmanyte, 0xF089 };
static const struct SpriteFrameImage sFollowerPicTableOmanyte[] =
{
    overworld_frame(sFollowerPicOmanyte, 4, 4, 0),
    overworld_frame(sFollowerPicOmanyte, 4, 4, 1),
    overworld_frame(sFollowerPicOmanyte, 4, 4, 2),
    overworld_frame(sFollowerPicOmanyte, 4, 4, 3),
    overworld_frame(sFollowerPicOmanyte, 4, 4, 4),
    overworld_frame(sFollowerPicOmanyte, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoOmanyte =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF089,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableOmanyte,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicOmastar[] = INCBIN_U32("mods/following_pokemon/graphics/followers/omastar.4bpp");
static const u16 sFollowerPalOmastar[] = INCBIN_U16("mods/following_pokemon/graphics/followers/omastar.gbapal");
const struct SpritePalette gFollowingPokemonPaletteOmastar = { sFollowerPalOmastar, 0xF08A };
static const struct SpriteFrameImage sFollowerPicTableOmastar[] =
{
    overworld_frame(sFollowerPicOmastar, 4, 4, 0),
    overworld_frame(sFollowerPicOmastar, 4, 4, 1),
    overworld_frame(sFollowerPicOmastar, 4, 4, 2),
    overworld_frame(sFollowerPicOmastar, 4, 4, 3),
    overworld_frame(sFollowerPicOmastar, 4, 4, 4),
    overworld_frame(sFollowerPicOmastar, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoOmastar =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF08A,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableOmastar,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicKabuto[] = INCBIN_U32("mods/following_pokemon/graphics/followers/kabuto.4bpp");
static const u16 sFollowerPalKabuto[] = INCBIN_U16("mods/following_pokemon/graphics/followers/kabuto.gbapal");
const struct SpritePalette gFollowingPokemonPaletteKabuto = { sFollowerPalKabuto, 0xF08B };
static const struct SpriteFrameImage sFollowerPicTableKabuto[] =
{
    overworld_frame(sFollowerPicKabuto, 4, 4, 0),
    overworld_frame(sFollowerPicKabuto, 4, 4, 1),
    overworld_frame(sFollowerPicKabuto, 4, 4, 2),
    overworld_frame(sFollowerPicKabuto, 4, 4, 3),
    overworld_frame(sFollowerPicKabuto, 4, 4, 4),
    overworld_frame(sFollowerPicKabuto, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoKabuto =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF08B,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableKabuto,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicKabutops[] = INCBIN_U32("mods/following_pokemon/graphics/followers/kabutops.4bpp");
static const u16 sFollowerPalKabutops[] = INCBIN_U16("mods/following_pokemon/graphics/followers/kabutops.gbapal");
const struct SpritePalette gFollowingPokemonPaletteKabutops = { sFollowerPalKabutops, 0xF08C };
static const struct SpriteFrameImage sFollowerPicTableKabutops[] =
{
    overworld_frame(sFollowerPicKabutops, 4, 4, 0),
    overworld_frame(sFollowerPicKabutops, 4, 4, 1),
    overworld_frame(sFollowerPicKabutops, 4, 4, 2),
    overworld_frame(sFollowerPicKabutops, 4, 4, 3),
    overworld_frame(sFollowerPicKabutops, 4, 4, 4),
    overworld_frame(sFollowerPicKabutops, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoKabutops =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF08C,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableKabutops,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicAerodactyl[] = INCBIN_U32("mods/following_pokemon/graphics/followers/aerodactyl.4bpp");
static const u16 sFollowerPalAerodactyl[] = INCBIN_U16("mods/following_pokemon/graphics/followers/aerodactyl.gbapal");
const struct SpritePalette gFollowingPokemonPaletteAerodactyl = { sFollowerPalAerodactyl, 0xF08D };
static const struct SpriteFrameImage sFollowerPicTableAerodactyl[] =
{
    overworld_frame(sFollowerPicAerodactyl, 4, 4, 0),
    overworld_frame(sFollowerPicAerodactyl, 4, 4, 1),
    overworld_frame(sFollowerPicAerodactyl, 4, 4, 2),
    overworld_frame(sFollowerPicAerodactyl, 4, 4, 3),
    overworld_frame(sFollowerPicAerodactyl, 4, 4, 4),
    overworld_frame(sFollowerPicAerodactyl, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoAerodactyl =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF08D,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableAerodactyl,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSnorlax[] = INCBIN_U32("mods/following_pokemon/graphics/followers/snorlax.4bpp");
static const u16 sFollowerPalSnorlax[] = INCBIN_U16("mods/following_pokemon/graphics/followers/snorlax.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSnorlax = { sFollowerPalSnorlax, 0xF08E };
static const struct SpriteFrameImage sFollowerPicTableSnorlax[] =
{
    overworld_frame(sFollowerPicSnorlax, 4, 4, 0),
    overworld_frame(sFollowerPicSnorlax, 4, 4, 1),
    overworld_frame(sFollowerPicSnorlax, 4, 4, 2),
    overworld_frame(sFollowerPicSnorlax, 4, 4, 3),
    overworld_frame(sFollowerPicSnorlax, 4, 4, 4),
    overworld_frame(sFollowerPicSnorlax, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSnorlax =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF08E,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSnorlax,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicArticuno[] = INCBIN_U32("mods/following_pokemon/graphics/followers/articuno.4bpp");
static const u16 sFollowerPalArticuno[] = INCBIN_U16("mods/following_pokemon/graphics/followers/articuno.gbapal");
const struct SpritePalette gFollowingPokemonPaletteArticuno = { sFollowerPalArticuno, 0xF08F };
static const struct SpriteFrameImage sFollowerPicTableArticuno[] =
{
    overworld_frame(sFollowerPicArticuno, 4, 4, 0),
    overworld_frame(sFollowerPicArticuno, 4, 4, 1),
    overworld_frame(sFollowerPicArticuno, 4, 4, 2),
    overworld_frame(sFollowerPicArticuno, 4, 4, 3),
    overworld_frame(sFollowerPicArticuno, 4, 4, 4),
    overworld_frame(sFollowerPicArticuno, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoArticuno =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF08F,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableArticuno,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicZapdos[] = INCBIN_U32("mods/following_pokemon/graphics/followers/zapdos.4bpp");
static const u16 sFollowerPalZapdos[] = INCBIN_U16("mods/following_pokemon/graphics/followers/zapdos.gbapal");
const struct SpritePalette gFollowingPokemonPaletteZapdos = { sFollowerPalZapdos, 0xF090 };
static const struct SpriteFrameImage sFollowerPicTableZapdos[] =
{
    overworld_frame(sFollowerPicZapdos, 4, 4, 0),
    overworld_frame(sFollowerPicZapdos, 4, 4, 1),
    overworld_frame(sFollowerPicZapdos, 4, 4, 2),
    overworld_frame(sFollowerPicZapdos, 4, 4, 3),
    overworld_frame(sFollowerPicZapdos, 4, 4, 4),
    overworld_frame(sFollowerPicZapdos, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoZapdos =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF090,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableZapdos,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMoltres[] = INCBIN_U32("mods/following_pokemon/graphics/followers/moltres.4bpp");
static const u16 sFollowerPalMoltres[] = INCBIN_U16("mods/following_pokemon/graphics/followers/moltres.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMoltres = { sFollowerPalMoltres, 0xF091 };
static const struct SpriteFrameImage sFollowerPicTableMoltres[] =
{
    overworld_frame(sFollowerPicMoltres, 4, 4, 0),
    overworld_frame(sFollowerPicMoltres, 4, 4, 1),
    overworld_frame(sFollowerPicMoltres, 4, 4, 2),
    overworld_frame(sFollowerPicMoltres, 4, 4, 3),
    overworld_frame(sFollowerPicMoltres, 4, 4, 4),
    overworld_frame(sFollowerPicMoltres, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMoltres =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF091,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMoltres,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicDratini[] = INCBIN_U32("mods/following_pokemon/graphics/followers/dratini.4bpp");
static const u16 sFollowerPalDratini[] = INCBIN_U16("mods/following_pokemon/graphics/followers/dratini.gbapal");
const struct SpritePalette gFollowingPokemonPaletteDratini = { sFollowerPalDratini, 0xF092 };
static const struct SpriteFrameImage sFollowerPicTableDratini[] =
{
    overworld_frame(sFollowerPicDratini, 4, 4, 0),
    overworld_frame(sFollowerPicDratini, 4, 4, 1),
    overworld_frame(sFollowerPicDratini, 4, 4, 2),
    overworld_frame(sFollowerPicDratini, 4, 4, 3),
    overworld_frame(sFollowerPicDratini, 4, 4, 4),
    overworld_frame(sFollowerPicDratini, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoDratini =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF092,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableDratini,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicDragonair[] = INCBIN_U32("mods/following_pokemon/graphics/followers/dragonair.4bpp");
static const u16 sFollowerPalDragonair[] = INCBIN_U16("mods/following_pokemon/graphics/followers/dragonair.gbapal");
const struct SpritePalette gFollowingPokemonPaletteDragonair = { sFollowerPalDragonair, 0xF093 };
static const struct SpriteFrameImage sFollowerPicTableDragonair[] =
{
    overworld_frame(sFollowerPicDragonair, 4, 4, 0),
    overworld_frame(sFollowerPicDragonair, 4, 4, 1),
    overworld_frame(sFollowerPicDragonair, 4, 4, 2),
    overworld_frame(sFollowerPicDragonair, 4, 4, 3),
    overworld_frame(sFollowerPicDragonair, 4, 4, 4),
    overworld_frame(sFollowerPicDragonair, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoDragonair =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF093,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableDragonair,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicDragonite[] = INCBIN_U32("mods/following_pokemon/graphics/followers/dragonite.4bpp");
static const u16 sFollowerPalDragonite[] = INCBIN_U16("mods/following_pokemon/graphics/followers/dragonite.gbapal");
const struct SpritePalette gFollowingPokemonPaletteDragonite = { sFollowerPalDragonite, 0xF094 };
static const struct SpriteFrameImage sFollowerPicTableDragonite[] =
{
    overworld_frame(sFollowerPicDragonite, 4, 4, 0),
    overworld_frame(sFollowerPicDragonite, 4, 4, 1),
    overworld_frame(sFollowerPicDragonite, 4, 4, 2),
    overworld_frame(sFollowerPicDragonite, 4, 4, 3),
    overworld_frame(sFollowerPicDragonite, 4, 4, 4),
    overworld_frame(sFollowerPicDragonite, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoDragonite =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF094,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableDragonite,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMewtwo[] = INCBIN_U32("mods/following_pokemon/graphics/followers/mewtwo.4bpp");
static const u16 sFollowerPalMewtwo[] = INCBIN_U16("mods/following_pokemon/graphics/followers/mewtwo.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMewtwo = { sFollowerPalMewtwo, 0xF095 };
static const struct SpriteFrameImage sFollowerPicTableMewtwo[] =
{
    overworld_frame(sFollowerPicMewtwo, 4, 4, 0),
    overworld_frame(sFollowerPicMewtwo, 4, 4, 1),
    overworld_frame(sFollowerPicMewtwo, 4, 4, 2),
    overworld_frame(sFollowerPicMewtwo, 4, 4, 3),
    overworld_frame(sFollowerPicMewtwo, 4, 4, 4),
    overworld_frame(sFollowerPicMewtwo, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMewtwo =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF095,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMewtwo,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMew[] = INCBIN_U32("mods/following_pokemon/graphics/followers/mew.4bpp");
static const u16 sFollowerPalMew[] = INCBIN_U16("mods/following_pokemon/graphics/followers/mew.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMew = { sFollowerPalMew, 0xF096 };
static const struct SpriteFrameImage sFollowerPicTableMew[] =
{
    overworld_frame(sFollowerPicMew, 4, 4, 0),
    overworld_frame(sFollowerPicMew, 4, 4, 1),
    overworld_frame(sFollowerPicMew, 4, 4, 2),
    overworld_frame(sFollowerPicMew, 4, 4, 3),
    overworld_frame(sFollowerPicMew, 4, 4, 4),
    overworld_frame(sFollowerPicMew, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMew =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF096,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMew,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicChikorita[] = INCBIN_U32("mods/following_pokemon/graphics/followers/chikorita.4bpp");
static const u16 sFollowerPalChikorita[] = INCBIN_U16("mods/following_pokemon/graphics/followers/chikorita.gbapal");
const struct SpritePalette gFollowingPokemonPaletteChikorita = { sFollowerPalChikorita, 0xF097 };
static const struct SpriteFrameImage sFollowerPicTableChikorita[] =
{
    overworld_frame(sFollowerPicChikorita, 4, 4, 0),
    overworld_frame(sFollowerPicChikorita, 4, 4, 1),
    overworld_frame(sFollowerPicChikorita, 4, 4, 2),
    overworld_frame(sFollowerPicChikorita, 4, 4, 3),
    overworld_frame(sFollowerPicChikorita, 4, 4, 4),
    overworld_frame(sFollowerPicChikorita, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoChikorita =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF097,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableChikorita,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicBayleef[] = INCBIN_U32("mods/following_pokemon/graphics/followers/bayleef.4bpp");
static const u16 sFollowerPalBayleef[] = INCBIN_U16("mods/following_pokemon/graphics/followers/bayleef.gbapal");
const struct SpritePalette gFollowingPokemonPaletteBayleef = { sFollowerPalBayleef, 0xF098 };
static const struct SpriteFrameImage sFollowerPicTableBayleef[] =
{
    overworld_frame(sFollowerPicBayleef, 4, 4, 0),
    overworld_frame(sFollowerPicBayleef, 4, 4, 1),
    overworld_frame(sFollowerPicBayleef, 4, 4, 2),
    overworld_frame(sFollowerPicBayleef, 4, 4, 3),
    overworld_frame(sFollowerPicBayleef, 4, 4, 4),
    overworld_frame(sFollowerPicBayleef, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoBayleef =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF098,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableBayleef,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMeganium[] = INCBIN_U32("mods/following_pokemon/graphics/followers/meganium.4bpp");
static const u16 sFollowerPalMeganium[] = INCBIN_U16("mods/following_pokemon/graphics/followers/meganium.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMeganium = { sFollowerPalMeganium, 0xF099 };
static const struct SpriteFrameImage sFollowerPicTableMeganium[] =
{
    overworld_frame(sFollowerPicMeganium, 4, 4, 0),
    overworld_frame(sFollowerPicMeganium, 4, 4, 1),
    overworld_frame(sFollowerPicMeganium, 4, 4, 2),
    overworld_frame(sFollowerPicMeganium, 4, 4, 3),
    overworld_frame(sFollowerPicMeganium, 4, 4, 4),
    overworld_frame(sFollowerPicMeganium, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMeganium =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF099,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMeganium,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicCyndaquil[] = INCBIN_U32("mods/following_pokemon/graphics/followers/cyndaquil.4bpp");
static const u16 sFollowerPalCyndaquil[] = INCBIN_U16("mods/following_pokemon/graphics/followers/cyndaquil.gbapal");
const struct SpritePalette gFollowingPokemonPaletteCyndaquil = { sFollowerPalCyndaquil, 0xF09A };
static const struct SpriteFrameImage sFollowerPicTableCyndaquil[] =
{
    overworld_frame(sFollowerPicCyndaquil, 4, 4, 0),
    overworld_frame(sFollowerPicCyndaquil, 4, 4, 1),
    overworld_frame(sFollowerPicCyndaquil, 4, 4, 2),
    overworld_frame(sFollowerPicCyndaquil, 4, 4, 3),
    overworld_frame(sFollowerPicCyndaquil, 4, 4, 4),
    overworld_frame(sFollowerPicCyndaquil, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoCyndaquil =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF09A,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableCyndaquil,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicQuilava[] = INCBIN_U32("mods/following_pokemon/graphics/followers/quilava.4bpp");
static const u16 sFollowerPalQuilava[] = INCBIN_U16("mods/following_pokemon/graphics/followers/quilava.gbapal");
const struct SpritePalette gFollowingPokemonPaletteQuilava = { sFollowerPalQuilava, 0xF09B };
static const struct SpriteFrameImage sFollowerPicTableQuilava[] =
{
    overworld_frame(sFollowerPicQuilava, 4, 4, 0),
    overworld_frame(sFollowerPicQuilava, 4, 4, 1),
    overworld_frame(sFollowerPicQuilava, 4, 4, 2),
    overworld_frame(sFollowerPicQuilava, 4, 4, 3),
    overworld_frame(sFollowerPicQuilava, 4, 4, 4),
    overworld_frame(sFollowerPicQuilava, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoQuilava =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF09B,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableQuilava,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicTyphlosion[] = INCBIN_U32("mods/following_pokemon/graphics/followers/typhlosion.4bpp");
static const u16 sFollowerPalTyphlosion[] = INCBIN_U16("mods/following_pokemon/graphics/followers/typhlosion.gbapal");
const struct SpritePalette gFollowingPokemonPaletteTyphlosion = { sFollowerPalTyphlosion, 0xF09C };
static const struct SpriteFrameImage sFollowerPicTableTyphlosion[] =
{
    overworld_frame(sFollowerPicTyphlosion, 4, 4, 0),
    overworld_frame(sFollowerPicTyphlosion, 4, 4, 1),
    overworld_frame(sFollowerPicTyphlosion, 4, 4, 2),
    overworld_frame(sFollowerPicTyphlosion, 4, 4, 3),
    overworld_frame(sFollowerPicTyphlosion, 4, 4, 4),
    overworld_frame(sFollowerPicTyphlosion, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoTyphlosion =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF09C,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableTyphlosion,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicTotodile[] = INCBIN_U32("mods/following_pokemon/graphics/followers/totodile.4bpp");
static const u16 sFollowerPalTotodile[] = INCBIN_U16("mods/following_pokemon/graphics/followers/totodile.gbapal");
const struct SpritePalette gFollowingPokemonPaletteTotodile = { sFollowerPalTotodile, 0xF09D };
static const struct SpriteFrameImage sFollowerPicTableTotodile[] =
{
    overworld_frame(sFollowerPicTotodile, 4, 4, 0),
    overworld_frame(sFollowerPicTotodile, 4, 4, 1),
    overworld_frame(sFollowerPicTotodile, 4, 4, 2),
    overworld_frame(sFollowerPicTotodile, 4, 4, 3),
    overworld_frame(sFollowerPicTotodile, 4, 4, 4),
    overworld_frame(sFollowerPicTotodile, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoTotodile =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF09D,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableTotodile,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicCroconaw[] = INCBIN_U32("mods/following_pokemon/graphics/followers/croconaw.4bpp");
static const u16 sFollowerPalCroconaw[] = INCBIN_U16("mods/following_pokemon/graphics/followers/croconaw.gbapal");
const struct SpritePalette gFollowingPokemonPaletteCroconaw = { sFollowerPalCroconaw, 0xF09E };
static const struct SpriteFrameImage sFollowerPicTableCroconaw[] =
{
    overworld_frame(sFollowerPicCroconaw, 4, 4, 0),
    overworld_frame(sFollowerPicCroconaw, 4, 4, 1),
    overworld_frame(sFollowerPicCroconaw, 4, 4, 2),
    overworld_frame(sFollowerPicCroconaw, 4, 4, 3),
    overworld_frame(sFollowerPicCroconaw, 4, 4, 4),
    overworld_frame(sFollowerPicCroconaw, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoCroconaw =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF09E,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableCroconaw,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicFeraligatr[] = INCBIN_U32("mods/following_pokemon/graphics/followers/feraligatr.4bpp");
static const u16 sFollowerPalFeraligatr[] = INCBIN_U16("mods/following_pokemon/graphics/followers/feraligatr.gbapal");
const struct SpritePalette gFollowingPokemonPaletteFeraligatr = { sFollowerPalFeraligatr, 0xF09F };
static const struct SpriteFrameImage sFollowerPicTableFeraligatr[] =
{
    overworld_frame(sFollowerPicFeraligatr, 4, 4, 0),
    overworld_frame(sFollowerPicFeraligatr, 4, 4, 1),
    overworld_frame(sFollowerPicFeraligatr, 4, 4, 2),
    overworld_frame(sFollowerPicFeraligatr, 4, 4, 3),
    overworld_frame(sFollowerPicFeraligatr, 4, 4, 4),
    overworld_frame(sFollowerPicFeraligatr, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoFeraligatr =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF09F,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableFeraligatr,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSentret[] = INCBIN_U32("mods/following_pokemon/graphics/followers/sentret.4bpp");
static const u16 sFollowerPalSentret[] = INCBIN_U16("mods/following_pokemon/graphics/followers/sentret.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSentret = { sFollowerPalSentret, 0xF0A0 };
static const struct SpriteFrameImage sFollowerPicTableSentret[] =
{
    overworld_frame(sFollowerPicSentret, 4, 4, 0),
    overworld_frame(sFollowerPicSentret, 4, 4, 1),
    overworld_frame(sFollowerPicSentret, 4, 4, 2),
    overworld_frame(sFollowerPicSentret, 4, 4, 3),
    overworld_frame(sFollowerPicSentret, 4, 4, 4),
    overworld_frame(sFollowerPicSentret, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSentret =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0A0,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSentret,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicFurret[] = INCBIN_U32("mods/following_pokemon/graphics/followers/furret.4bpp");
static const u16 sFollowerPalFurret[] = INCBIN_U16("mods/following_pokemon/graphics/followers/furret.gbapal");
const struct SpritePalette gFollowingPokemonPaletteFurret = { sFollowerPalFurret, 0xF0A1 };
static const struct SpriteFrameImage sFollowerPicTableFurret[] =
{
    overworld_frame(sFollowerPicFurret, 4, 4, 0),
    overworld_frame(sFollowerPicFurret, 4, 4, 1),
    overworld_frame(sFollowerPicFurret, 4, 4, 2),
    overworld_frame(sFollowerPicFurret, 4, 4, 3),
    overworld_frame(sFollowerPicFurret, 4, 4, 4),
    overworld_frame(sFollowerPicFurret, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoFurret =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0A1,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableFurret,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicHoothoot[] = INCBIN_U32("mods/following_pokemon/graphics/followers/hoothoot.4bpp");
static const u16 sFollowerPalHoothoot[] = INCBIN_U16("mods/following_pokemon/graphics/followers/hoothoot.gbapal");
const struct SpritePalette gFollowingPokemonPaletteHoothoot = { sFollowerPalHoothoot, 0xF0A2 };
static const struct SpriteFrameImage sFollowerPicTableHoothoot[] =
{
    overworld_frame(sFollowerPicHoothoot, 4, 4, 0),
    overworld_frame(sFollowerPicHoothoot, 4, 4, 1),
    overworld_frame(sFollowerPicHoothoot, 4, 4, 2),
    overworld_frame(sFollowerPicHoothoot, 4, 4, 3),
    overworld_frame(sFollowerPicHoothoot, 4, 4, 4),
    overworld_frame(sFollowerPicHoothoot, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoHoothoot =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0A2,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableHoothoot,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicNoctowl[] = INCBIN_U32("mods/following_pokemon/graphics/followers/noctowl.4bpp");
static const u16 sFollowerPalNoctowl[] = INCBIN_U16("mods/following_pokemon/graphics/followers/noctowl.gbapal");
const struct SpritePalette gFollowingPokemonPaletteNoctowl = { sFollowerPalNoctowl, 0xF0A3 };
static const struct SpriteFrameImage sFollowerPicTableNoctowl[] =
{
    overworld_frame(sFollowerPicNoctowl, 4, 4, 0),
    overworld_frame(sFollowerPicNoctowl, 4, 4, 1),
    overworld_frame(sFollowerPicNoctowl, 4, 4, 2),
    overworld_frame(sFollowerPicNoctowl, 4, 4, 3),
    overworld_frame(sFollowerPicNoctowl, 4, 4, 4),
    overworld_frame(sFollowerPicNoctowl, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoNoctowl =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0A3,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableNoctowl,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicLedyba[] = INCBIN_U32("mods/following_pokemon/graphics/followers/ledyba.4bpp");
static const u16 sFollowerPalLedyba[] = INCBIN_U16("mods/following_pokemon/graphics/followers/ledyba.gbapal");
const struct SpritePalette gFollowingPokemonPaletteLedyba = { sFollowerPalLedyba, 0xF0A4 };
static const struct SpriteFrameImage sFollowerPicTableLedyba[] =
{
    overworld_frame(sFollowerPicLedyba, 4, 4, 0),
    overworld_frame(sFollowerPicLedyba, 4, 4, 1),
    overworld_frame(sFollowerPicLedyba, 4, 4, 2),
    overworld_frame(sFollowerPicLedyba, 4, 4, 3),
    overworld_frame(sFollowerPicLedyba, 4, 4, 4),
    overworld_frame(sFollowerPicLedyba, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoLedyba =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0A4,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableLedyba,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicLedian[] = INCBIN_U32("mods/following_pokemon/graphics/followers/ledian.4bpp");
static const u16 sFollowerPalLedian[] = INCBIN_U16("mods/following_pokemon/graphics/followers/ledian.gbapal");
const struct SpritePalette gFollowingPokemonPaletteLedian = { sFollowerPalLedian, 0xF0A5 };
static const struct SpriteFrameImage sFollowerPicTableLedian[] =
{
    overworld_frame(sFollowerPicLedian, 4, 4, 0),
    overworld_frame(sFollowerPicLedian, 4, 4, 1),
    overworld_frame(sFollowerPicLedian, 4, 4, 2),
    overworld_frame(sFollowerPicLedian, 4, 4, 3),
    overworld_frame(sFollowerPicLedian, 4, 4, 4),
    overworld_frame(sFollowerPicLedian, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoLedian =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0A5,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableLedian,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSpinarak[] = INCBIN_U32("mods/following_pokemon/graphics/followers/spinarak.4bpp");
static const u16 sFollowerPalSpinarak[] = INCBIN_U16("mods/following_pokemon/graphics/followers/spinarak.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSpinarak = { sFollowerPalSpinarak, 0xF0A6 };
static const struct SpriteFrameImage sFollowerPicTableSpinarak[] =
{
    overworld_frame(sFollowerPicSpinarak, 4, 4, 0),
    overworld_frame(sFollowerPicSpinarak, 4, 4, 1),
    overworld_frame(sFollowerPicSpinarak, 4, 4, 2),
    overworld_frame(sFollowerPicSpinarak, 4, 4, 3),
    overworld_frame(sFollowerPicSpinarak, 4, 4, 4),
    overworld_frame(sFollowerPicSpinarak, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSpinarak =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0A6,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSpinarak,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicAriados[] = INCBIN_U32("mods/following_pokemon/graphics/followers/ariados.4bpp");
static const u16 sFollowerPalAriados[] = INCBIN_U16("mods/following_pokemon/graphics/followers/ariados.gbapal");
const struct SpritePalette gFollowingPokemonPaletteAriados = { sFollowerPalAriados, 0xF0A7 };
static const struct SpriteFrameImage sFollowerPicTableAriados[] =
{
    overworld_frame(sFollowerPicAriados, 4, 4, 0),
    overworld_frame(sFollowerPicAriados, 4, 4, 1),
    overworld_frame(sFollowerPicAriados, 4, 4, 2),
    overworld_frame(sFollowerPicAriados, 4, 4, 3),
    overworld_frame(sFollowerPicAriados, 4, 4, 4),
    overworld_frame(sFollowerPicAriados, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoAriados =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0A7,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableAriados,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicCrobat[] = INCBIN_U32("mods/following_pokemon/graphics/followers/crobat.4bpp");
static const u16 sFollowerPalCrobat[] = INCBIN_U16("mods/following_pokemon/graphics/followers/crobat.gbapal");
const struct SpritePalette gFollowingPokemonPaletteCrobat = { sFollowerPalCrobat, 0xF0A8 };
static const struct SpriteFrameImage sFollowerPicTableCrobat[] =
{
    overworld_frame(sFollowerPicCrobat, 4, 4, 0),
    overworld_frame(sFollowerPicCrobat, 4, 4, 1),
    overworld_frame(sFollowerPicCrobat, 4, 4, 2),
    overworld_frame(sFollowerPicCrobat, 4, 4, 3),
    overworld_frame(sFollowerPicCrobat, 4, 4, 4),
    overworld_frame(sFollowerPicCrobat, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoCrobat =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0A8,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableCrobat,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicChinchou[] = INCBIN_U32("mods/following_pokemon/graphics/followers/chinchou.4bpp");
static const u16 sFollowerPalChinchou[] = INCBIN_U16("mods/following_pokemon/graphics/followers/chinchou.gbapal");
const struct SpritePalette gFollowingPokemonPaletteChinchou = { sFollowerPalChinchou, 0xF0A9 };
static const struct SpriteFrameImage sFollowerPicTableChinchou[] =
{
    overworld_frame(sFollowerPicChinchou, 4, 4, 0),
    overworld_frame(sFollowerPicChinchou, 4, 4, 1),
    overworld_frame(sFollowerPicChinchou, 4, 4, 2),
    overworld_frame(sFollowerPicChinchou, 4, 4, 3),
    overworld_frame(sFollowerPicChinchou, 4, 4, 4),
    overworld_frame(sFollowerPicChinchou, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoChinchou =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0A9,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableChinchou,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicLanturn[] = INCBIN_U32("mods/following_pokemon/graphics/followers/lanturn.4bpp");
static const u16 sFollowerPalLanturn[] = INCBIN_U16("mods/following_pokemon/graphics/followers/lanturn.gbapal");
const struct SpritePalette gFollowingPokemonPaletteLanturn = { sFollowerPalLanturn, 0xF0AA };
static const struct SpriteFrameImage sFollowerPicTableLanturn[] =
{
    overworld_frame(sFollowerPicLanturn, 4, 4, 0),
    overworld_frame(sFollowerPicLanturn, 4, 4, 1),
    overworld_frame(sFollowerPicLanturn, 4, 4, 2),
    overworld_frame(sFollowerPicLanturn, 4, 4, 3),
    overworld_frame(sFollowerPicLanturn, 4, 4, 4),
    overworld_frame(sFollowerPicLanturn, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoLanturn =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0AA,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableLanturn,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicPichu[] = INCBIN_U32("mods/following_pokemon/graphics/followers/pichu.4bpp");
static const u16 sFollowerPalPichu[] = INCBIN_U16("mods/following_pokemon/graphics/followers/pichu.gbapal");
const struct SpritePalette gFollowingPokemonPalettePichu = { sFollowerPalPichu, 0xF0AB };
static const struct SpriteFrameImage sFollowerPicTablePichu[] =
{
    overworld_frame(sFollowerPicPichu, 4, 4, 0),
    overworld_frame(sFollowerPicPichu, 4, 4, 1),
    overworld_frame(sFollowerPicPichu, 4, 4, 2),
    overworld_frame(sFollowerPicPichu, 4, 4, 3),
    overworld_frame(sFollowerPicPichu, 4, 4, 4),
    overworld_frame(sFollowerPicPichu, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoPichu =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0AB,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTablePichu,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicCleffa[] = INCBIN_U32("mods/following_pokemon/graphics/followers/cleffa.4bpp");
static const u16 sFollowerPalCleffa[] = INCBIN_U16("mods/following_pokemon/graphics/followers/cleffa.gbapal");
const struct SpritePalette gFollowingPokemonPaletteCleffa = { sFollowerPalCleffa, 0xF0AC };
static const struct SpriteFrameImage sFollowerPicTableCleffa[] =
{
    overworld_frame(sFollowerPicCleffa, 4, 4, 0),
    overworld_frame(sFollowerPicCleffa, 4, 4, 1),
    overworld_frame(sFollowerPicCleffa, 4, 4, 2),
    overworld_frame(sFollowerPicCleffa, 4, 4, 3),
    overworld_frame(sFollowerPicCleffa, 4, 4, 4),
    overworld_frame(sFollowerPicCleffa, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoCleffa =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0AC,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableCleffa,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicIgglybuff[] = INCBIN_U32("mods/following_pokemon/graphics/followers/igglybuff.4bpp");
static const u16 sFollowerPalIgglybuff[] = INCBIN_U16("mods/following_pokemon/graphics/followers/igglybuff.gbapal");
const struct SpritePalette gFollowingPokemonPaletteIgglybuff = { sFollowerPalIgglybuff, 0xF0AD };
static const struct SpriteFrameImage sFollowerPicTableIgglybuff[] =
{
    overworld_frame(sFollowerPicIgglybuff, 4, 4, 0),
    overworld_frame(sFollowerPicIgglybuff, 4, 4, 1),
    overworld_frame(sFollowerPicIgglybuff, 4, 4, 2),
    overworld_frame(sFollowerPicIgglybuff, 4, 4, 3),
    overworld_frame(sFollowerPicIgglybuff, 4, 4, 4),
    overworld_frame(sFollowerPicIgglybuff, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoIgglybuff =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0AD,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableIgglybuff,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicTogepi[] = INCBIN_U32("mods/following_pokemon/graphics/followers/togepi.4bpp");
static const u16 sFollowerPalTogepi[] = INCBIN_U16("mods/following_pokemon/graphics/followers/togepi.gbapal");
const struct SpritePalette gFollowingPokemonPaletteTogepi = { sFollowerPalTogepi, 0xF0AE };
static const struct SpriteFrameImage sFollowerPicTableTogepi[] =
{
    overworld_frame(sFollowerPicTogepi, 4, 4, 0),
    overworld_frame(sFollowerPicTogepi, 4, 4, 1),
    overworld_frame(sFollowerPicTogepi, 4, 4, 2),
    overworld_frame(sFollowerPicTogepi, 4, 4, 3),
    overworld_frame(sFollowerPicTogepi, 4, 4, 4),
    overworld_frame(sFollowerPicTogepi, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoTogepi =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0AE,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableTogepi,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicTogetic[] = INCBIN_U32("mods/following_pokemon/graphics/followers/togetic.4bpp");
static const u16 sFollowerPalTogetic[] = INCBIN_U16("mods/following_pokemon/graphics/followers/togetic.gbapal");
const struct SpritePalette gFollowingPokemonPaletteTogetic = { sFollowerPalTogetic, 0xF0AF };
static const struct SpriteFrameImage sFollowerPicTableTogetic[] =
{
    overworld_frame(sFollowerPicTogetic, 4, 4, 0),
    overworld_frame(sFollowerPicTogetic, 4, 4, 1),
    overworld_frame(sFollowerPicTogetic, 4, 4, 2),
    overworld_frame(sFollowerPicTogetic, 4, 4, 3),
    overworld_frame(sFollowerPicTogetic, 4, 4, 4),
    overworld_frame(sFollowerPicTogetic, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoTogetic =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0AF,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableTogetic,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicNatu[] = INCBIN_U32("mods/following_pokemon/graphics/followers/natu.4bpp");
static const u16 sFollowerPalNatu[] = INCBIN_U16("mods/following_pokemon/graphics/followers/natu.gbapal");
const struct SpritePalette gFollowingPokemonPaletteNatu = { sFollowerPalNatu, 0xF0B0 };
static const struct SpriteFrameImage sFollowerPicTableNatu[] =
{
    overworld_frame(sFollowerPicNatu, 4, 4, 0),
    overworld_frame(sFollowerPicNatu, 4, 4, 1),
    overworld_frame(sFollowerPicNatu, 4, 4, 2),
    overworld_frame(sFollowerPicNatu, 4, 4, 3),
    overworld_frame(sFollowerPicNatu, 4, 4, 4),
    overworld_frame(sFollowerPicNatu, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoNatu =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0B0,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableNatu,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicXatu[] = INCBIN_U32("mods/following_pokemon/graphics/followers/xatu.4bpp");
static const u16 sFollowerPalXatu[] = INCBIN_U16("mods/following_pokemon/graphics/followers/xatu.gbapal");
const struct SpritePalette gFollowingPokemonPaletteXatu = { sFollowerPalXatu, 0xF0B1 };
static const struct SpriteFrameImage sFollowerPicTableXatu[] =
{
    overworld_frame(sFollowerPicXatu, 4, 4, 0),
    overworld_frame(sFollowerPicXatu, 4, 4, 1),
    overworld_frame(sFollowerPicXatu, 4, 4, 2),
    overworld_frame(sFollowerPicXatu, 4, 4, 3),
    overworld_frame(sFollowerPicXatu, 4, 4, 4),
    overworld_frame(sFollowerPicXatu, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoXatu =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0B1,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableXatu,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMareep[] = INCBIN_U32("mods/following_pokemon/graphics/followers/mareep.4bpp");
static const u16 sFollowerPalMareep[] = INCBIN_U16("mods/following_pokemon/graphics/followers/mareep.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMareep = { sFollowerPalMareep, 0xF0B2 };
static const struct SpriteFrameImage sFollowerPicTableMareep[] =
{
    overworld_frame(sFollowerPicMareep, 4, 4, 0),
    overworld_frame(sFollowerPicMareep, 4, 4, 1),
    overworld_frame(sFollowerPicMareep, 4, 4, 2),
    overworld_frame(sFollowerPicMareep, 4, 4, 3),
    overworld_frame(sFollowerPicMareep, 4, 4, 4),
    overworld_frame(sFollowerPicMareep, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMareep =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0B2,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMareep,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicFlaaffy[] = INCBIN_U32("mods/following_pokemon/graphics/followers/flaaffy.4bpp");
static const u16 sFollowerPalFlaaffy[] = INCBIN_U16("mods/following_pokemon/graphics/followers/flaaffy.gbapal");
const struct SpritePalette gFollowingPokemonPaletteFlaaffy = { sFollowerPalFlaaffy, 0xF0B3 };
static const struct SpriteFrameImage sFollowerPicTableFlaaffy[] =
{
    overworld_frame(sFollowerPicFlaaffy, 4, 4, 0),
    overworld_frame(sFollowerPicFlaaffy, 4, 4, 1),
    overworld_frame(sFollowerPicFlaaffy, 4, 4, 2),
    overworld_frame(sFollowerPicFlaaffy, 4, 4, 3),
    overworld_frame(sFollowerPicFlaaffy, 4, 4, 4),
    overworld_frame(sFollowerPicFlaaffy, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoFlaaffy =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0B3,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableFlaaffy,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicAmpharos[] = INCBIN_U32("mods/following_pokemon/graphics/followers/ampharos.4bpp");
static const u16 sFollowerPalAmpharos[] = INCBIN_U16("mods/following_pokemon/graphics/followers/ampharos.gbapal");
const struct SpritePalette gFollowingPokemonPaletteAmpharos = { sFollowerPalAmpharos, 0xF0B4 };
static const struct SpriteFrameImage sFollowerPicTableAmpharos[] =
{
    overworld_frame(sFollowerPicAmpharos, 4, 4, 0),
    overworld_frame(sFollowerPicAmpharos, 4, 4, 1),
    overworld_frame(sFollowerPicAmpharos, 4, 4, 2),
    overworld_frame(sFollowerPicAmpharos, 4, 4, 3),
    overworld_frame(sFollowerPicAmpharos, 4, 4, 4),
    overworld_frame(sFollowerPicAmpharos, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoAmpharos =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0B4,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableAmpharos,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicBellossom[] = INCBIN_U32("mods/following_pokemon/graphics/followers/bellossom.4bpp");
static const u16 sFollowerPalBellossom[] = INCBIN_U16("mods/following_pokemon/graphics/followers/bellossom.gbapal");
const struct SpritePalette gFollowingPokemonPaletteBellossom = { sFollowerPalBellossom, 0xF0B5 };
static const struct SpriteFrameImage sFollowerPicTableBellossom[] =
{
    overworld_frame(sFollowerPicBellossom, 4, 4, 0),
    overworld_frame(sFollowerPicBellossom, 4, 4, 1),
    overworld_frame(sFollowerPicBellossom, 4, 4, 2),
    overworld_frame(sFollowerPicBellossom, 4, 4, 3),
    overworld_frame(sFollowerPicBellossom, 4, 4, 4),
    overworld_frame(sFollowerPicBellossom, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoBellossom =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0B5,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableBellossom,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMarill[] = INCBIN_U32("mods/following_pokemon/graphics/followers/marill.4bpp");
static const u16 sFollowerPalMarill[] = INCBIN_U16("mods/following_pokemon/graphics/followers/marill.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMarill = { sFollowerPalMarill, 0xF0B6 };
static const struct SpriteFrameImage sFollowerPicTableMarill[] =
{
    overworld_frame(sFollowerPicMarill, 4, 4, 0),
    overworld_frame(sFollowerPicMarill, 4, 4, 1),
    overworld_frame(sFollowerPicMarill, 4, 4, 2),
    overworld_frame(sFollowerPicMarill, 4, 4, 3),
    overworld_frame(sFollowerPicMarill, 4, 4, 4),
    overworld_frame(sFollowerPicMarill, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMarill =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0B6,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMarill,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicAzumarill[] = INCBIN_U32("mods/following_pokemon/graphics/followers/azumarill.4bpp");
static const u16 sFollowerPalAzumarill[] = INCBIN_U16("mods/following_pokemon/graphics/followers/azumarill.gbapal");
const struct SpritePalette gFollowingPokemonPaletteAzumarill = { sFollowerPalAzumarill, 0xF0B7 };
static const struct SpriteFrameImage sFollowerPicTableAzumarill[] =
{
    overworld_frame(sFollowerPicAzumarill, 4, 4, 0),
    overworld_frame(sFollowerPicAzumarill, 4, 4, 1),
    overworld_frame(sFollowerPicAzumarill, 4, 4, 2),
    overworld_frame(sFollowerPicAzumarill, 4, 4, 3),
    overworld_frame(sFollowerPicAzumarill, 4, 4, 4),
    overworld_frame(sFollowerPicAzumarill, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoAzumarill =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0B7,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableAzumarill,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSudowoodo[] = INCBIN_U32("mods/following_pokemon/graphics/followers/sudowoodo.4bpp");
static const u16 sFollowerPalSudowoodo[] = INCBIN_U16("mods/following_pokemon/graphics/followers/sudowoodo.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSudowoodo = { sFollowerPalSudowoodo, 0xF0B8 };
static const struct SpriteFrameImage sFollowerPicTableSudowoodo[] =
{
    overworld_frame(sFollowerPicSudowoodo, 4, 4, 0),
    overworld_frame(sFollowerPicSudowoodo, 4, 4, 1),
    overworld_frame(sFollowerPicSudowoodo, 4, 4, 2),
    overworld_frame(sFollowerPicSudowoodo, 4, 4, 3),
    overworld_frame(sFollowerPicSudowoodo, 4, 4, 4),
    overworld_frame(sFollowerPicSudowoodo, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSudowoodo =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0B8,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSudowoodo,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicPolitoed[] = INCBIN_U32("mods/following_pokemon/graphics/followers/politoed.4bpp");
static const u16 sFollowerPalPolitoed[] = INCBIN_U16("mods/following_pokemon/graphics/followers/politoed.gbapal");
const struct SpritePalette gFollowingPokemonPalettePolitoed = { sFollowerPalPolitoed, 0xF0B9 };
static const struct SpriteFrameImage sFollowerPicTablePolitoed[] =
{
    overworld_frame(sFollowerPicPolitoed, 4, 4, 0),
    overworld_frame(sFollowerPicPolitoed, 4, 4, 1),
    overworld_frame(sFollowerPicPolitoed, 4, 4, 2),
    overworld_frame(sFollowerPicPolitoed, 4, 4, 3),
    overworld_frame(sFollowerPicPolitoed, 4, 4, 4),
    overworld_frame(sFollowerPicPolitoed, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoPolitoed =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0B9,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTablePolitoed,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicHoppip[] = INCBIN_U32("mods/following_pokemon/graphics/followers/hoppip.4bpp");
static const u16 sFollowerPalHoppip[] = INCBIN_U16("mods/following_pokemon/graphics/followers/hoppip.gbapal");
const struct SpritePalette gFollowingPokemonPaletteHoppip = { sFollowerPalHoppip, 0xF0BA };
static const struct SpriteFrameImage sFollowerPicTableHoppip[] =
{
    overworld_frame(sFollowerPicHoppip, 4, 4, 0),
    overworld_frame(sFollowerPicHoppip, 4, 4, 1),
    overworld_frame(sFollowerPicHoppip, 4, 4, 2),
    overworld_frame(sFollowerPicHoppip, 4, 4, 3),
    overworld_frame(sFollowerPicHoppip, 4, 4, 4),
    overworld_frame(sFollowerPicHoppip, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoHoppip =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0BA,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableHoppip,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSkiploom[] = INCBIN_U32("mods/following_pokemon/graphics/followers/skiploom.4bpp");
static const u16 sFollowerPalSkiploom[] = INCBIN_U16("mods/following_pokemon/graphics/followers/skiploom.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSkiploom = { sFollowerPalSkiploom, 0xF0BB };
static const struct SpriteFrameImage sFollowerPicTableSkiploom[] =
{
    overworld_frame(sFollowerPicSkiploom, 4, 4, 0),
    overworld_frame(sFollowerPicSkiploom, 4, 4, 1),
    overworld_frame(sFollowerPicSkiploom, 4, 4, 2),
    overworld_frame(sFollowerPicSkiploom, 4, 4, 3),
    overworld_frame(sFollowerPicSkiploom, 4, 4, 4),
    overworld_frame(sFollowerPicSkiploom, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSkiploom =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0BB,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSkiploom,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicJumpluff[] = INCBIN_U32("mods/following_pokemon/graphics/followers/jumpluff.4bpp");
static const u16 sFollowerPalJumpluff[] = INCBIN_U16("mods/following_pokemon/graphics/followers/jumpluff.gbapal");
const struct SpritePalette gFollowingPokemonPaletteJumpluff = { sFollowerPalJumpluff, 0xF0BC };
static const struct SpriteFrameImage sFollowerPicTableJumpluff[] =
{
    overworld_frame(sFollowerPicJumpluff, 4, 4, 0),
    overworld_frame(sFollowerPicJumpluff, 4, 4, 1),
    overworld_frame(sFollowerPicJumpluff, 4, 4, 2),
    overworld_frame(sFollowerPicJumpluff, 4, 4, 3),
    overworld_frame(sFollowerPicJumpluff, 4, 4, 4),
    overworld_frame(sFollowerPicJumpluff, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoJumpluff =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0BC,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableJumpluff,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicAipom[] = INCBIN_U32("mods/following_pokemon/graphics/followers/aipom.4bpp");
static const u16 sFollowerPalAipom[] = INCBIN_U16("mods/following_pokemon/graphics/followers/aipom.gbapal");
const struct SpritePalette gFollowingPokemonPaletteAipom = { sFollowerPalAipom, 0xF0BD };
static const struct SpriteFrameImage sFollowerPicTableAipom[] =
{
    overworld_frame(sFollowerPicAipom, 4, 4, 0),
    overworld_frame(sFollowerPicAipom, 4, 4, 1),
    overworld_frame(sFollowerPicAipom, 4, 4, 2),
    overworld_frame(sFollowerPicAipom, 4, 4, 3),
    overworld_frame(sFollowerPicAipom, 4, 4, 4),
    overworld_frame(sFollowerPicAipom, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoAipom =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0BD,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableAipom,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSunkern[] = INCBIN_U32("mods/following_pokemon/graphics/followers/sunkern.4bpp");
static const u16 sFollowerPalSunkern[] = INCBIN_U16("mods/following_pokemon/graphics/followers/sunkern.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSunkern = { sFollowerPalSunkern, 0xF0BE };
static const struct SpriteFrameImage sFollowerPicTableSunkern[] =
{
    overworld_frame(sFollowerPicSunkern, 4, 4, 0),
    overworld_frame(sFollowerPicSunkern, 4, 4, 1),
    overworld_frame(sFollowerPicSunkern, 4, 4, 2),
    overworld_frame(sFollowerPicSunkern, 4, 4, 3),
    overworld_frame(sFollowerPicSunkern, 4, 4, 4),
    overworld_frame(sFollowerPicSunkern, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSunkern =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0BE,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSunkern,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSunflora[] = INCBIN_U32("mods/following_pokemon/graphics/followers/sunflora.4bpp");
static const u16 sFollowerPalSunflora[] = INCBIN_U16("mods/following_pokemon/graphics/followers/sunflora.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSunflora = { sFollowerPalSunflora, 0xF0BF };
static const struct SpriteFrameImage sFollowerPicTableSunflora[] =
{
    overworld_frame(sFollowerPicSunflora, 4, 4, 0),
    overworld_frame(sFollowerPicSunflora, 4, 4, 1),
    overworld_frame(sFollowerPicSunflora, 4, 4, 2),
    overworld_frame(sFollowerPicSunflora, 4, 4, 3),
    overworld_frame(sFollowerPicSunflora, 4, 4, 4),
    overworld_frame(sFollowerPicSunflora, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSunflora =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0BF,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSunflora,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicYanma[] = INCBIN_U32("mods/following_pokemon/graphics/followers/yanma.4bpp");
static const u16 sFollowerPalYanma[] = INCBIN_U16("mods/following_pokemon/graphics/followers/yanma.gbapal");
const struct SpritePalette gFollowingPokemonPaletteYanma = { sFollowerPalYanma, 0xF0C0 };
static const struct SpriteFrameImage sFollowerPicTableYanma[] =
{
    overworld_frame(sFollowerPicYanma, 4, 4, 0),
    overworld_frame(sFollowerPicYanma, 4, 4, 1),
    overworld_frame(sFollowerPicYanma, 4, 4, 2),
    overworld_frame(sFollowerPicYanma, 4, 4, 3),
    overworld_frame(sFollowerPicYanma, 4, 4, 4),
    overworld_frame(sFollowerPicYanma, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoYanma =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0C0,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableYanma,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicWooper[] = INCBIN_U32("mods/following_pokemon/graphics/followers/wooper.4bpp");
static const u16 sFollowerPalWooper[] = INCBIN_U16("mods/following_pokemon/graphics/followers/wooper.gbapal");
const struct SpritePalette gFollowingPokemonPaletteWooper = { sFollowerPalWooper, 0xF0C1 };
static const struct SpriteFrameImage sFollowerPicTableWooper[] =
{
    overworld_frame(sFollowerPicWooper, 4, 4, 0),
    overworld_frame(sFollowerPicWooper, 4, 4, 1),
    overworld_frame(sFollowerPicWooper, 4, 4, 2),
    overworld_frame(sFollowerPicWooper, 4, 4, 3),
    overworld_frame(sFollowerPicWooper, 4, 4, 4),
    overworld_frame(sFollowerPicWooper, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoWooper =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0C1,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableWooper,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicQuagsire[] = INCBIN_U32("mods/following_pokemon/graphics/followers/quagsire.4bpp");
static const u16 sFollowerPalQuagsire[] = INCBIN_U16("mods/following_pokemon/graphics/followers/quagsire.gbapal");
const struct SpritePalette gFollowingPokemonPaletteQuagsire = { sFollowerPalQuagsire, 0xF0C2 };
static const struct SpriteFrameImage sFollowerPicTableQuagsire[] =
{
    overworld_frame(sFollowerPicQuagsire, 4, 4, 0),
    overworld_frame(sFollowerPicQuagsire, 4, 4, 1),
    overworld_frame(sFollowerPicQuagsire, 4, 4, 2),
    overworld_frame(sFollowerPicQuagsire, 4, 4, 3),
    overworld_frame(sFollowerPicQuagsire, 4, 4, 4),
    overworld_frame(sFollowerPicQuagsire, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoQuagsire =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0C2,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableQuagsire,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicEspeon[] = INCBIN_U32("mods/following_pokemon/graphics/followers/espeon.4bpp");
static const u16 sFollowerPalEspeon[] = INCBIN_U16("mods/following_pokemon/graphics/followers/espeon.gbapal");
const struct SpritePalette gFollowingPokemonPaletteEspeon = { sFollowerPalEspeon, 0xF0C3 };
static const struct SpriteFrameImage sFollowerPicTableEspeon[] =
{
    overworld_frame(sFollowerPicEspeon, 4, 4, 0),
    overworld_frame(sFollowerPicEspeon, 4, 4, 1),
    overworld_frame(sFollowerPicEspeon, 4, 4, 2),
    overworld_frame(sFollowerPicEspeon, 4, 4, 3),
    overworld_frame(sFollowerPicEspeon, 4, 4, 4),
    overworld_frame(sFollowerPicEspeon, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoEspeon =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0C3,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableEspeon,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicUmbreon[] = INCBIN_U32("mods/following_pokemon/graphics/followers/umbreon.4bpp");
static const u16 sFollowerPalUmbreon[] = INCBIN_U16("mods/following_pokemon/graphics/followers/umbreon.gbapal");
const struct SpritePalette gFollowingPokemonPaletteUmbreon = { sFollowerPalUmbreon, 0xF0C4 };
static const struct SpriteFrameImage sFollowerPicTableUmbreon[] =
{
    overworld_frame(sFollowerPicUmbreon, 4, 4, 0),
    overworld_frame(sFollowerPicUmbreon, 4, 4, 1),
    overworld_frame(sFollowerPicUmbreon, 4, 4, 2),
    overworld_frame(sFollowerPicUmbreon, 4, 4, 3),
    overworld_frame(sFollowerPicUmbreon, 4, 4, 4),
    overworld_frame(sFollowerPicUmbreon, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoUmbreon =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0C4,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableUmbreon,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMurkrow[] = INCBIN_U32("mods/following_pokemon/graphics/followers/murkrow.4bpp");
static const u16 sFollowerPalMurkrow[] = INCBIN_U16("mods/following_pokemon/graphics/followers/murkrow.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMurkrow = { sFollowerPalMurkrow, 0xF0C5 };
static const struct SpriteFrameImage sFollowerPicTableMurkrow[] =
{
    overworld_frame(sFollowerPicMurkrow, 4, 4, 0),
    overworld_frame(sFollowerPicMurkrow, 4, 4, 1),
    overworld_frame(sFollowerPicMurkrow, 4, 4, 2),
    overworld_frame(sFollowerPicMurkrow, 4, 4, 3),
    overworld_frame(sFollowerPicMurkrow, 4, 4, 4),
    overworld_frame(sFollowerPicMurkrow, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMurkrow =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0C5,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMurkrow,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSlowking[] = INCBIN_U32("mods/following_pokemon/graphics/followers/slowking.4bpp");
static const u16 sFollowerPalSlowking[] = INCBIN_U16("mods/following_pokemon/graphics/followers/slowking.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSlowking = { sFollowerPalSlowking, 0xF0C6 };
static const struct SpriteFrameImage sFollowerPicTableSlowking[] =
{
    overworld_frame(sFollowerPicSlowking, 4, 4, 0),
    overworld_frame(sFollowerPicSlowking, 4, 4, 1),
    overworld_frame(sFollowerPicSlowking, 4, 4, 2),
    overworld_frame(sFollowerPicSlowking, 4, 4, 3),
    overworld_frame(sFollowerPicSlowking, 4, 4, 4),
    overworld_frame(sFollowerPicSlowking, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSlowking =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0C6,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSlowking,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMisdreavus[] = INCBIN_U32("mods/following_pokemon/graphics/followers/misdreavus.4bpp");
static const u16 sFollowerPalMisdreavus[] = INCBIN_U16("mods/following_pokemon/graphics/followers/misdreavus.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMisdreavus = { sFollowerPalMisdreavus, 0xF0C7 };
static const struct SpriteFrameImage sFollowerPicTableMisdreavus[] =
{
    overworld_frame(sFollowerPicMisdreavus, 4, 4, 0),
    overworld_frame(sFollowerPicMisdreavus, 4, 4, 1),
    overworld_frame(sFollowerPicMisdreavus, 4, 4, 2),
    overworld_frame(sFollowerPicMisdreavus, 4, 4, 3),
    overworld_frame(sFollowerPicMisdreavus, 4, 4, 4),
    overworld_frame(sFollowerPicMisdreavus, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMisdreavus =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0C7,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMisdreavus,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicUnown[] = INCBIN_U32("mods/following_pokemon/graphics/followers/unown.4bpp");
static const u16 sFollowerPalUnown[] = INCBIN_U16("mods/following_pokemon/graphics/followers/unown.gbapal");
const struct SpritePalette gFollowingPokemonPaletteUnown = { sFollowerPalUnown, 0xF0C8 };
static const struct SpriteFrameImage sFollowerPicTableUnown[] =
{
    overworld_frame(sFollowerPicUnown, 4, 4, 0),
    overworld_frame(sFollowerPicUnown, 4, 4, 1),
    overworld_frame(sFollowerPicUnown, 4, 4, 2),
    overworld_frame(sFollowerPicUnown, 4, 4, 3),
    overworld_frame(sFollowerPicUnown, 4, 4, 4),
    overworld_frame(sFollowerPicUnown, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoUnown =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0C8,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableUnown,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicWobbuffet[] = INCBIN_U32("mods/following_pokemon/graphics/followers/wobbuffet.4bpp");
static const u16 sFollowerPalWobbuffet[] = INCBIN_U16("mods/following_pokemon/graphics/followers/wobbuffet.gbapal");
const struct SpritePalette gFollowingPokemonPaletteWobbuffet = { sFollowerPalWobbuffet, 0xF0C9 };
static const struct SpriteFrameImage sFollowerPicTableWobbuffet[] =
{
    overworld_frame(sFollowerPicWobbuffet, 4, 4, 0),
    overworld_frame(sFollowerPicWobbuffet, 4, 4, 1),
    overworld_frame(sFollowerPicWobbuffet, 4, 4, 2),
    overworld_frame(sFollowerPicWobbuffet, 4, 4, 3),
    overworld_frame(sFollowerPicWobbuffet, 4, 4, 4),
    overworld_frame(sFollowerPicWobbuffet, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoWobbuffet =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0C9,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableWobbuffet,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicGirafarig[] = INCBIN_U32("mods/following_pokemon/graphics/followers/girafarig.4bpp");
static const u16 sFollowerPalGirafarig[] = INCBIN_U16("mods/following_pokemon/graphics/followers/girafarig.gbapal");
const struct SpritePalette gFollowingPokemonPaletteGirafarig = { sFollowerPalGirafarig, 0xF0CA };
static const struct SpriteFrameImage sFollowerPicTableGirafarig[] =
{
    overworld_frame(sFollowerPicGirafarig, 4, 4, 0),
    overworld_frame(sFollowerPicGirafarig, 4, 4, 1),
    overworld_frame(sFollowerPicGirafarig, 4, 4, 2),
    overworld_frame(sFollowerPicGirafarig, 4, 4, 3),
    overworld_frame(sFollowerPicGirafarig, 4, 4, 4),
    overworld_frame(sFollowerPicGirafarig, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoGirafarig =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0CA,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableGirafarig,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicPineco[] = INCBIN_U32("mods/following_pokemon/graphics/followers/pineco.4bpp");
static const u16 sFollowerPalPineco[] = INCBIN_U16("mods/following_pokemon/graphics/followers/pineco.gbapal");
const struct SpritePalette gFollowingPokemonPalettePineco = { sFollowerPalPineco, 0xF0CB };
static const struct SpriteFrameImage sFollowerPicTablePineco[] =
{
    overworld_frame(sFollowerPicPineco, 4, 4, 0),
    overworld_frame(sFollowerPicPineco, 4, 4, 1),
    overworld_frame(sFollowerPicPineco, 4, 4, 2),
    overworld_frame(sFollowerPicPineco, 4, 4, 3),
    overworld_frame(sFollowerPicPineco, 4, 4, 4),
    overworld_frame(sFollowerPicPineco, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoPineco =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0CB,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTablePineco,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicForretress[] = INCBIN_U32("mods/following_pokemon/graphics/followers/forretress.4bpp");
static const u16 sFollowerPalForretress[] = INCBIN_U16("mods/following_pokemon/graphics/followers/forretress.gbapal");
const struct SpritePalette gFollowingPokemonPaletteForretress = { sFollowerPalForretress, 0xF0CC };
static const struct SpriteFrameImage sFollowerPicTableForretress[] =
{
    overworld_frame(sFollowerPicForretress, 4, 4, 0),
    overworld_frame(sFollowerPicForretress, 4, 4, 1),
    overworld_frame(sFollowerPicForretress, 4, 4, 2),
    overworld_frame(sFollowerPicForretress, 4, 4, 3),
    overworld_frame(sFollowerPicForretress, 4, 4, 4),
    overworld_frame(sFollowerPicForretress, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoForretress =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0CC,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableForretress,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicDunsparce[] = INCBIN_U32("mods/following_pokemon/graphics/followers/dunsparce.4bpp");
static const u16 sFollowerPalDunsparce[] = INCBIN_U16("mods/following_pokemon/graphics/followers/dunsparce.gbapal");
const struct SpritePalette gFollowingPokemonPaletteDunsparce = { sFollowerPalDunsparce, 0xF0CD };
static const struct SpriteFrameImage sFollowerPicTableDunsparce[] =
{
    overworld_frame(sFollowerPicDunsparce, 4, 4, 0),
    overworld_frame(sFollowerPicDunsparce, 4, 4, 1),
    overworld_frame(sFollowerPicDunsparce, 4, 4, 2),
    overworld_frame(sFollowerPicDunsparce, 4, 4, 3),
    overworld_frame(sFollowerPicDunsparce, 4, 4, 4),
    overworld_frame(sFollowerPicDunsparce, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoDunsparce =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0CD,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableDunsparce,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicGligar[] = INCBIN_U32("mods/following_pokemon/graphics/followers/gligar.4bpp");
static const u16 sFollowerPalGligar[] = INCBIN_U16("mods/following_pokemon/graphics/followers/gligar.gbapal");
const struct SpritePalette gFollowingPokemonPaletteGligar = { sFollowerPalGligar, 0xF0CE };
static const struct SpriteFrameImage sFollowerPicTableGligar[] =
{
    overworld_frame(sFollowerPicGligar, 4, 4, 0),
    overworld_frame(sFollowerPicGligar, 4, 4, 1),
    overworld_frame(sFollowerPicGligar, 4, 4, 2),
    overworld_frame(sFollowerPicGligar, 4, 4, 3),
    overworld_frame(sFollowerPicGligar, 4, 4, 4),
    overworld_frame(sFollowerPicGligar, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoGligar =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0CE,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableGligar,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSteelix[] = INCBIN_U32("mods/following_pokemon/graphics/followers/steelix.4bpp");
static const u16 sFollowerPalSteelix[] = INCBIN_U16("mods/following_pokemon/graphics/followers/steelix.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSteelix = { sFollowerPalSteelix, 0xF0CF };
static const struct SpriteFrameImage sFollowerPicTableSteelix[] =
{
    overworld_frame(sFollowerPicSteelix, 8, 8, 0),
    overworld_frame(sFollowerPicSteelix, 8, 8, 1),
    overworld_frame(sFollowerPicSteelix, 8, 8, 2),
    overworld_frame(sFollowerPicSteelix, 8, 8, 3),
    overworld_frame(sFollowerPicSteelix, 8, 8, 4),
    overworld_frame(sFollowerPicSteelix, 8, 8, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSteelix =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0CF,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 2048,
    .width = 64,
    .height = 64,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_L,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_64x64,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSteelix,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSnubbull[] = INCBIN_U32("mods/following_pokemon/graphics/followers/snubbull.4bpp");
static const u16 sFollowerPalSnubbull[] = INCBIN_U16("mods/following_pokemon/graphics/followers/snubbull.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSnubbull = { sFollowerPalSnubbull, 0xF0D0 };
static const struct SpriteFrameImage sFollowerPicTableSnubbull[] =
{
    overworld_frame(sFollowerPicSnubbull, 4, 4, 0),
    overworld_frame(sFollowerPicSnubbull, 4, 4, 1),
    overworld_frame(sFollowerPicSnubbull, 4, 4, 2),
    overworld_frame(sFollowerPicSnubbull, 4, 4, 3),
    overworld_frame(sFollowerPicSnubbull, 4, 4, 4),
    overworld_frame(sFollowerPicSnubbull, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSnubbull =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0D0,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSnubbull,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicGranbull[] = INCBIN_U32("mods/following_pokemon/graphics/followers/granbull.4bpp");
static const u16 sFollowerPalGranbull[] = INCBIN_U16("mods/following_pokemon/graphics/followers/granbull.gbapal");
const struct SpritePalette gFollowingPokemonPaletteGranbull = { sFollowerPalGranbull, 0xF0D1 };
static const struct SpriteFrameImage sFollowerPicTableGranbull[] =
{
    overworld_frame(sFollowerPicGranbull, 4, 4, 0),
    overworld_frame(sFollowerPicGranbull, 4, 4, 1),
    overworld_frame(sFollowerPicGranbull, 4, 4, 2),
    overworld_frame(sFollowerPicGranbull, 4, 4, 3),
    overworld_frame(sFollowerPicGranbull, 4, 4, 4),
    overworld_frame(sFollowerPicGranbull, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoGranbull =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0D1,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableGranbull,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicQwilfish[] = INCBIN_U32("mods/following_pokemon/graphics/followers/qwilfish.4bpp");
static const u16 sFollowerPalQwilfish[] = INCBIN_U16("mods/following_pokemon/graphics/followers/qwilfish.gbapal");
const struct SpritePalette gFollowingPokemonPaletteQwilfish = { sFollowerPalQwilfish, 0xF0D2 };
static const struct SpriteFrameImage sFollowerPicTableQwilfish[] =
{
    overworld_frame(sFollowerPicQwilfish, 4, 4, 0),
    overworld_frame(sFollowerPicQwilfish, 4, 4, 1),
    overworld_frame(sFollowerPicQwilfish, 4, 4, 2),
    overworld_frame(sFollowerPicQwilfish, 4, 4, 3),
    overworld_frame(sFollowerPicQwilfish, 4, 4, 4),
    overworld_frame(sFollowerPicQwilfish, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoQwilfish =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0D2,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableQwilfish,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicScizor[] = INCBIN_U32("mods/following_pokemon/graphics/followers/scizor.4bpp");
static const u16 sFollowerPalScizor[] = INCBIN_U16("mods/following_pokemon/graphics/followers/scizor.gbapal");
const struct SpritePalette gFollowingPokemonPaletteScizor = { sFollowerPalScizor, 0xF0D3 };
static const struct SpriteFrameImage sFollowerPicTableScizor[] =
{
    overworld_frame(sFollowerPicScizor, 4, 4, 0),
    overworld_frame(sFollowerPicScizor, 4, 4, 1),
    overworld_frame(sFollowerPicScizor, 4, 4, 2),
    overworld_frame(sFollowerPicScizor, 4, 4, 3),
    overworld_frame(sFollowerPicScizor, 4, 4, 4),
    overworld_frame(sFollowerPicScizor, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoScizor =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0D3,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableScizor,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicShuckle[] = INCBIN_U32("mods/following_pokemon/graphics/followers/shuckle.4bpp");
static const u16 sFollowerPalShuckle[] = INCBIN_U16("mods/following_pokemon/graphics/followers/shuckle.gbapal");
const struct SpritePalette gFollowingPokemonPaletteShuckle = { sFollowerPalShuckle, 0xF0D4 };
static const struct SpriteFrameImage sFollowerPicTableShuckle[] =
{
    overworld_frame(sFollowerPicShuckle, 4, 4, 0),
    overworld_frame(sFollowerPicShuckle, 4, 4, 1),
    overworld_frame(sFollowerPicShuckle, 4, 4, 2),
    overworld_frame(sFollowerPicShuckle, 4, 4, 3),
    overworld_frame(sFollowerPicShuckle, 4, 4, 4),
    overworld_frame(sFollowerPicShuckle, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoShuckle =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0D4,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableShuckle,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicHeracross[] = INCBIN_U32("mods/following_pokemon/graphics/followers/heracross.4bpp");
static const u16 sFollowerPalHeracross[] = INCBIN_U16("mods/following_pokemon/graphics/followers/heracross.gbapal");
const struct SpritePalette gFollowingPokemonPaletteHeracross = { sFollowerPalHeracross, 0xF0D5 };
static const struct SpriteFrameImage sFollowerPicTableHeracross[] =
{
    overworld_frame(sFollowerPicHeracross, 4, 4, 0),
    overworld_frame(sFollowerPicHeracross, 4, 4, 1),
    overworld_frame(sFollowerPicHeracross, 4, 4, 2),
    overworld_frame(sFollowerPicHeracross, 4, 4, 3),
    overworld_frame(sFollowerPicHeracross, 4, 4, 4),
    overworld_frame(sFollowerPicHeracross, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoHeracross =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0D5,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableHeracross,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSneasel[] = INCBIN_U32("mods/following_pokemon/graphics/followers/sneasel.4bpp");
static const u16 sFollowerPalSneasel[] = INCBIN_U16("mods/following_pokemon/graphics/followers/sneasel.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSneasel = { sFollowerPalSneasel, 0xF0D6 };
static const struct SpriteFrameImage sFollowerPicTableSneasel[] =
{
    overworld_frame(sFollowerPicSneasel, 4, 4, 0),
    overworld_frame(sFollowerPicSneasel, 4, 4, 1),
    overworld_frame(sFollowerPicSneasel, 4, 4, 2),
    overworld_frame(sFollowerPicSneasel, 4, 4, 3),
    overworld_frame(sFollowerPicSneasel, 4, 4, 4),
    overworld_frame(sFollowerPicSneasel, 4, 4, 5),
    overworld_frame(sFollowerPicSneasel, 4, 4, 6),
    overworld_frame(sFollowerPicSneasel, 4, 4, 7),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSneasel =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0D6,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableUniqueEast,
    .images = sFollowerPicTableSneasel,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicTeddiursa[] = INCBIN_U32("mods/following_pokemon/graphics/followers/teddiursa.4bpp");
static const u16 sFollowerPalTeddiursa[] = INCBIN_U16("mods/following_pokemon/graphics/followers/teddiursa.gbapal");
const struct SpritePalette gFollowingPokemonPaletteTeddiursa = { sFollowerPalTeddiursa, 0xF0D7 };
static const struct SpriteFrameImage sFollowerPicTableTeddiursa[] =
{
    overworld_frame(sFollowerPicTeddiursa, 4, 4, 0),
    overworld_frame(sFollowerPicTeddiursa, 4, 4, 1),
    overworld_frame(sFollowerPicTeddiursa, 4, 4, 2),
    overworld_frame(sFollowerPicTeddiursa, 4, 4, 3),
    overworld_frame(sFollowerPicTeddiursa, 4, 4, 4),
    overworld_frame(sFollowerPicTeddiursa, 4, 4, 5),
    overworld_frame(sFollowerPicTeddiursa, 4, 4, 6),
    overworld_frame(sFollowerPicTeddiursa, 4, 4, 7),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoTeddiursa =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0D7,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableUniqueEast,
    .images = sFollowerPicTableTeddiursa,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicUrsaring[] = INCBIN_U32("mods/following_pokemon/graphics/followers/ursaring.4bpp");
static const u16 sFollowerPalUrsaring[] = INCBIN_U16("mods/following_pokemon/graphics/followers/ursaring.gbapal");
const struct SpritePalette gFollowingPokemonPaletteUrsaring = { sFollowerPalUrsaring, 0xF0D8 };
static const struct SpriteFrameImage sFollowerPicTableUrsaring[] =
{
    overworld_frame(sFollowerPicUrsaring, 4, 4, 0),
    overworld_frame(sFollowerPicUrsaring, 4, 4, 1),
    overworld_frame(sFollowerPicUrsaring, 4, 4, 2),
    overworld_frame(sFollowerPicUrsaring, 4, 4, 3),
    overworld_frame(sFollowerPicUrsaring, 4, 4, 4),
    overworld_frame(sFollowerPicUrsaring, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoUrsaring =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0D8,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableUrsaring,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSlugma[] = INCBIN_U32("mods/following_pokemon/graphics/followers/slugma.4bpp");
static const u16 sFollowerPalSlugma[] = INCBIN_U16("mods/following_pokemon/graphics/followers/slugma.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSlugma = { sFollowerPalSlugma, 0xF0D9 };
static const struct SpriteFrameImage sFollowerPicTableSlugma[] =
{
    overworld_frame(sFollowerPicSlugma, 4, 4, 0),
    overworld_frame(sFollowerPicSlugma, 4, 4, 1),
    overworld_frame(sFollowerPicSlugma, 4, 4, 2),
    overworld_frame(sFollowerPicSlugma, 4, 4, 3),
    overworld_frame(sFollowerPicSlugma, 4, 4, 4),
    overworld_frame(sFollowerPicSlugma, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSlugma =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0D9,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSlugma,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMagcargo[] = INCBIN_U32("mods/following_pokemon/graphics/followers/magcargo.4bpp");
static const u16 sFollowerPalMagcargo[] = INCBIN_U16("mods/following_pokemon/graphics/followers/magcargo.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMagcargo = { sFollowerPalMagcargo, 0xF0DA };
static const struct SpriteFrameImage sFollowerPicTableMagcargo[] =
{
    overworld_frame(sFollowerPicMagcargo, 4, 4, 0),
    overworld_frame(sFollowerPicMagcargo, 4, 4, 1),
    overworld_frame(sFollowerPicMagcargo, 4, 4, 2),
    overworld_frame(sFollowerPicMagcargo, 4, 4, 3),
    overworld_frame(sFollowerPicMagcargo, 4, 4, 4),
    overworld_frame(sFollowerPicMagcargo, 4, 4, 5),
    overworld_frame(sFollowerPicMagcargo, 4, 4, 6),
    overworld_frame(sFollowerPicMagcargo, 4, 4, 7),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMagcargo =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0DA,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableUniqueEast,
    .images = sFollowerPicTableMagcargo,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSwinub[] = INCBIN_U32("mods/following_pokemon/graphics/followers/swinub.4bpp");
static const u16 sFollowerPalSwinub[] = INCBIN_U16("mods/following_pokemon/graphics/followers/swinub.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSwinub = { sFollowerPalSwinub, 0xF0DB };
static const struct SpriteFrameImage sFollowerPicTableSwinub[] =
{
    overworld_frame(sFollowerPicSwinub, 4, 4, 0),
    overworld_frame(sFollowerPicSwinub, 4, 4, 1),
    overworld_frame(sFollowerPicSwinub, 4, 4, 2),
    overworld_frame(sFollowerPicSwinub, 4, 4, 3),
    overworld_frame(sFollowerPicSwinub, 4, 4, 4),
    overworld_frame(sFollowerPicSwinub, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSwinub =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0DB,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSwinub,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicPiloswine[] = INCBIN_U32("mods/following_pokemon/graphics/followers/piloswine.4bpp");
static const u16 sFollowerPalPiloswine[] = INCBIN_U16("mods/following_pokemon/graphics/followers/piloswine.gbapal");
const struct SpritePalette gFollowingPokemonPalettePiloswine = { sFollowerPalPiloswine, 0xF0DC };
static const struct SpriteFrameImage sFollowerPicTablePiloswine[] =
{
    overworld_frame(sFollowerPicPiloswine, 4, 4, 0),
    overworld_frame(sFollowerPicPiloswine, 4, 4, 1),
    overworld_frame(sFollowerPicPiloswine, 4, 4, 2),
    overworld_frame(sFollowerPicPiloswine, 4, 4, 3),
    overworld_frame(sFollowerPicPiloswine, 4, 4, 4),
    overworld_frame(sFollowerPicPiloswine, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoPiloswine =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0DC,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTablePiloswine,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicCorsola[] = INCBIN_U32("mods/following_pokemon/graphics/followers/corsola.4bpp");
static const u16 sFollowerPalCorsola[] = INCBIN_U16("mods/following_pokemon/graphics/followers/corsola.gbapal");
const struct SpritePalette gFollowingPokemonPaletteCorsola = { sFollowerPalCorsola, 0xF0DD };
static const struct SpriteFrameImage sFollowerPicTableCorsola[] =
{
    overworld_frame(sFollowerPicCorsola, 4, 4, 0),
    overworld_frame(sFollowerPicCorsola, 4, 4, 1),
    overworld_frame(sFollowerPicCorsola, 4, 4, 2),
    overworld_frame(sFollowerPicCorsola, 4, 4, 3),
    overworld_frame(sFollowerPicCorsola, 4, 4, 4),
    overworld_frame(sFollowerPicCorsola, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoCorsola =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0DD,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableCorsola,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicRemoraid[] = INCBIN_U32("mods/following_pokemon/graphics/followers/remoraid.4bpp");
static const u16 sFollowerPalRemoraid[] = INCBIN_U16("mods/following_pokemon/graphics/followers/remoraid.gbapal");
const struct SpritePalette gFollowingPokemonPaletteRemoraid = { sFollowerPalRemoraid, 0xF0DE };
static const struct SpriteFrameImage sFollowerPicTableRemoraid[] =
{
    overworld_frame(sFollowerPicRemoraid, 4, 4, 0),
    overworld_frame(sFollowerPicRemoraid, 4, 4, 1),
    overworld_frame(sFollowerPicRemoraid, 4, 4, 2),
    overworld_frame(sFollowerPicRemoraid, 4, 4, 3),
    overworld_frame(sFollowerPicRemoraid, 4, 4, 4),
    overworld_frame(sFollowerPicRemoraid, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoRemoraid =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0DE,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableRemoraid,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicOctillery[] = INCBIN_U32("mods/following_pokemon/graphics/followers/octillery.4bpp");
static const u16 sFollowerPalOctillery[] = INCBIN_U16("mods/following_pokemon/graphics/followers/octillery.gbapal");
const struct SpritePalette gFollowingPokemonPaletteOctillery = { sFollowerPalOctillery, 0xF0DF };
static const struct SpriteFrameImage sFollowerPicTableOctillery[] =
{
    overworld_frame(sFollowerPicOctillery, 4, 4, 0),
    overworld_frame(sFollowerPicOctillery, 4, 4, 1),
    overworld_frame(sFollowerPicOctillery, 4, 4, 2),
    overworld_frame(sFollowerPicOctillery, 4, 4, 3),
    overworld_frame(sFollowerPicOctillery, 4, 4, 4),
    overworld_frame(sFollowerPicOctillery, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoOctillery =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0DF,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableOctillery,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicDelibird[] = INCBIN_U32("mods/following_pokemon/graphics/followers/delibird.4bpp");
static const u16 sFollowerPalDelibird[] = INCBIN_U16("mods/following_pokemon/graphics/followers/delibird.gbapal");
const struct SpritePalette gFollowingPokemonPaletteDelibird = { sFollowerPalDelibird, 0xF0E0 };
static const struct SpriteFrameImage sFollowerPicTableDelibird[] =
{
    overworld_frame(sFollowerPicDelibird, 4, 4, 0),
    overworld_frame(sFollowerPicDelibird, 4, 4, 1),
    overworld_frame(sFollowerPicDelibird, 4, 4, 2),
    overworld_frame(sFollowerPicDelibird, 4, 4, 3),
    overworld_frame(sFollowerPicDelibird, 4, 4, 4),
    overworld_frame(sFollowerPicDelibird, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoDelibird =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0E0,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableDelibird,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMantine[] = INCBIN_U32("mods/following_pokemon/graphics/followers/mantine.4bpp");
static const u16 sFollowerPalMantine[] = INCBIN_U16("mods/following_pokemon/graphics/followers/mantine.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMantine = { sFollowerPalMantine, 0xF0E1 };
static const struct SpriteFrameImage sFollowerPicTableMantine[] =
{
    overworld_frame(sFollowerPicMantine, 4, 4, 0),
    overworld_frame(sFollowerPicMantine, 4, 4, 1),
    overworld_frame(sFollowerPicMantine, 4, 4, 2),
    overworld_frame(sFollowerPicMantine, 4, 4, 3),
    overworld_frame(sFollowerPicMantine, 4, 4, 4),
    overworld_frame(sFollowerPicMantine, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMantine =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0E1,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMantine,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSkarmory[] = INCBIN_U32("mods/following_pokemon/graphics/followers/skarmory.4bpp");
static const u16 sFollowerPalSkarmory[] = INCBIN_U16("mods/following_pokemon/graphics/followers/skarmory.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSkarmory = { sFollowerPalSkarmory, 0xF0E2 };
static const struct SpriteFrameImage sFollowerPicTableSkarmory[] =
{
    overworld_frame(sFollowerPicSkarmory, 4, 4, 0),
    overworld_frame(sFollowerPicSkarmory, 4, 4, 1),
    overworld_frame(sFollowerPicSkarmory, 4, 4, 2),
    overworld_frame(sFollowerPicSkarmory, 4, 4, 3),
    overworld_frame(sFollowerPicSkarmory, 4, 4, 4),
    overworld_frame(sFollowerPicSkarmory, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSkarmory =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0E2,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSkarmory,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicHoundour[] = INCBIN_U32("mods/following_pokemon/graphics/followers/houndour.4bpp");
static const u16 sFollowerPalHoundour[] = INCBIN_U16("mods/following_pokemon/graphics/followers/houndour.gbapal");
const struct SpritePalette gFollowingPokemonPaletteHoundour = { sFollowerPalHoundour, 0xF0E3 };
static const struct SpriteFrameImage sFollowerPicTableHoundour[] =
{
    overworld_frame(sFollowerPicHoundour, 4, 4, 0),
    overworld_frame(sFollowerPicHoundour, 4, 4, 1),
    overworld_frame(sFollowerPicHoundour, 4, 4, 2),
    overworld_frame(sFollowerPicHoundour, 4, 4, 3),
    overworld_frame(sFollowerPicHoundour, 4, 4, 4),
    overworld_frame(sFollowerPicHoundour, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoHoundour =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0E3,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableHoundour,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicHoundoom[] = INCBIN_U32("mods/following_pokemon/graphics/followers/houndoom.4bpp");
static const u16 sFollowerPalHoundoom[] = INCBIN_U16("mods/following_pokemon/graphics/followers/houndoom.gbapal");
const struct SpritePalette gFollowingPokemonPaletteHoundoom = { sFollowerPalHoundoom, 0xF0E4 };
static const struct SpriteFrameImage sFollowerPicTableHoundoom[] =
{
    overworld_frame(sFollowerPicHoundoom, 4, 4, 0),
    overworld_frame(sFollowerPicHoundoom, 4, 4, 1),
    overworld_frame(sFollowerPicHoundoom, 4, 4, 2),
    overworld_frame(sFollowerPicHoundoom, 4, 4, 3),
    overworld_frame(sFollowerPicHoundoom, 4, 4, 4),
    overworld_frame(sFollowerPicHoundoom, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoHoundoom =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0E4,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableHoundoom,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicKingdra[] = INCBIN_U32("mods/following_pokemon/graphics/followers/kingdra.4bpp");
static const u16 sFollowerPalKingdra[] = INCBIN_U16("mods/following_pokemon/graphics/followers/kingdra.gbapal");
const struct SpritePalette gFollowingPokemonPaletteKingdra = { sFollowerPalKingdra, 0xF0E5 };
static const struct SpriteFrameImage sFollowerPicTableKingdra[] =
{
    overworld_frame(sFollowerPicKingdra, 4, 4, 0),
    overworld_frame(sFollowerPicKingdra, 4, 4, 1),
    overworld_frame(sFollowerPicKingdra, 4, 4, 2),
    overworld_frame(sFollowerPicKingdra, 4, 4, 3),
    overworld_frame(sFollowerPicKingdra, 4, 4, 4),
    overworld_frame(sFollowerPicKingdra, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoKingdra =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0E5,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableKingdra,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicPhanpy[] = INCBIN_U32("mods/following_pokemon/graphics/followers/phanpy.4bpp");
static const u16 sFollowerPalPhanpy[] = INCBIN_U16("mods/following_pokemon/graphics/followers/phanpy.gbapal");
const struct SpritePalette gFollowingPokemonPalettePhanpy = { sFollowerPalPhanpy, 0xF0E6 };
static const struct SpriteFrameImage sFollowerPicTablePhanpy[] =
{
    overworld_frame(sFollowerPicPhanpy, 4, 4, 0),
    overworld_frame(sFollowerPicPhanpy, 4, 4, 1),
    overworld_frame(sFollowerPicPhanpy, 4, 4, 2),
    overworld_frame(sFollowerPicPhanpy, 4, 4, 3),
    overworld_frame(sFollowerPicPhanpy, 4, 4, 4),
    overworld_frame(sFollowerPicPhanpy, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoPhanpy =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0E6,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTablePhanpy,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicDonphan[] = INCBIN_U32("mods/following_pokemon/graphics/followers/donphan.4bpp");
static const u16 sFollowerPalDonphan[] = INCBIN_U16("mods/following_pokemon/graphics/followers/donphan.gbapal");
const struct SpritePalette gFollowingPokemonPaletteDonphan = { sFollowerPalDonphan, 0xF0E7 };
static const struct SpriteFrameImage sFollowerPicTableDonphan[] =
{
    overworld_frame(sFollowerPicDonphan, 4, 4, 0),
    overworld_frame(sFollowerPicDonphan, 4, 4, 1),
    overworld_frame(sFollowerPicDonphan, 4, 4, 2),
    overworld_frame(sFollowerPicDonphan, 4, 4, 3),
    overworld_frame(sFollowerPicDonphan, 4, 4, 4),
    overworld_frame(sFollowerPicDonphan, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoDonphan =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0E7,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableDonphan,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicPorygon2[] = INCBIN_U32("mods/following_pokemon/graphics/followers/porygon2.4bpp");
static const u16 sFollowerPalPorygon2[] = INCBIN_U16("mods/following_pokemon/graphics/followers/porygon2.gbapal");
const struct SpritePalette gFollowingPokemonPalettePorygon2 = { sFollowerPalPorygon2, 0xF0E8 };
static const struct SpriteFrameImage sFollowerPicTablePorygon2[] =
{
    overworld_frame(sFollowerPicPorygon2, 4, 4, 0),
    overworld_frame(sFollowerPicPorygon2, 4, 4, 1),
    overworld_frame(sFollowerPicPorygon2, 4, 4, 2),
    overworld_frame(sFollowerPicPorygon2, 4, 4, 3),
    overworld_frame(sFollowerPicPorygon2, 4, 4, 4),
    overworld_frame(sFollowerPicPorygon2, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoPorygon2 =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0E8,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTablePorygon2,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicStantler[] = INCBIN_U32("mods/following_pokemon/graphics/followers/stantler.4bpp");
static const u16 sFollowerPalStantler[] = INCBIN_U16("mods/following_pokemon/graphics/followers/stantler.gbapal");
const struct SpritePalette gFollowingPokemonPaletteStantler = { sFollowerPalStantler, 0xF0E9 };
static const struct SpriteFrameImage sFollowerPicTableStantler[] =
{
    overworld_frame(sFollowerPicStantler, 4, 4, 0),
    overworld_frame(sFollowerPicStantler, 4, 4, 1),
    overworld_frame(sFollowerPicStantler, 4, 4, 2),
    overworld_frame(sFollowerPicStantler, 4, 4, 3),
    overworld_frame(sFollowerPicStantler, 4, 4, 4),
    overworld_frame(sFollowerPicStantler, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoStantler =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0E9,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableStantler,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSmeargle[] = INCBIN_U32("mods/following_pokemon/graphics/followers/smeargle.4bpp");
static const u16 sFollowerPalSmeargle[] = INCBIN_U16("mods/following_pokemon/graphics/followers/smeargle.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSmeargle = { sFollowerPalSmeargle, 0xF0EA };
static const struct SpriteFrameImage sFollowerPicTableSmeargle[] =
{
    overworld_frame(sFollowerPicSmeargle, 4, 4, 0),
    overworld_frame(sFollowerPicSmeargle, 4, 4, 1),
    overworld_frame(sFollowerPicSmeargle, 4, 4, 2),
    overworld_frame(sFollowerPicSmeargle, 4, 4, 3),
    overworld_frame(sFollowerPicSmeargle, 4, 4, 4),
    overworld_frame(sFollowerPicSmeargle, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSmeargle =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0EA,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSmeargle,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicTyrogue[] = INCBIN_U32("mods/following_pokemon/graphics/followers/tyrogue.4bpp");
static const u16 sFollowerPalTyrogue[] = INCBIN_U16("mods/following_pokemon/graphics/followers/tyrogue.gbapal");
const struct SpritePalette gFollowingPokemonPaletteTyrogue = { sFollowerPalTyrogue, 0xF0EB };
static const struct SpriteFrameImage sFollowerPicTableTyrogue[] =
{
    overworld_frame(sFollowerPicTyrogue, 4, 4, 0),
    overworld_frame(sFollowerPicTyrogue, 4, 4, 1),
    overworld_frame(sFollowerPicTyrogue, 4, 4, 2),
    overworld_frame(sFollowerPicTyrogue, 4, 4, 3),
    overworld_frame(sFollowerPicTyrogue, 4, 4, 4),
    overworld_frame(sFollowerPicTyrogue, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoTyrogue =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0EB,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableTyrogue,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicHitmontop[] = INCBIN_U32("mods/following_pokemon/graphics/followers/hitmontop.4bpp");
static const u16 sFollowerPalHitmontop[] = INCBIN_U16("mods/following_pokemon/graphics/followers/hitmontop.gbapal");
const struct SpritePalette gFollowingPokemonPaletteHitmontop = { sFollowerPalHitmontop, 0xF0EC };
static const struct SpriteFrameImage sFollowerPicTableHitmontop[] =
{
    overworld_frame(sFollowerPicHitmontop, 4, 4, 0),
    overworld_frame(sFollowerPicHitmontop, 4, 4, 1),
    overworld_frame(sFollowerPicHitmontop, 4, 4, 2),
    overworld_frame(sFollowerPicHitmontop, 4, 4, 3),
    overworld_frame(sFollowerPicHitmontop, 4, 4, 4),
    overworld_frame(sFollowerPicHitmontop, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoHitmontop =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0EC,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableHitmontop,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSmoochum[] = INCBIN_U32("mods/following_pokemon/graphics/followers/smoochum.4bpp");
static const u16 sFollowerPalSmoochum[] = INCBIN_U16("mods/following_pokemon/graphics/followers/smoochum.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSmoochum = { sFollowerPalSmoochum, 0xF0ED };
static const struct SpriteFrameImage sFollowerPicTableSmoochum[] =
{
    overworld_frame(sFollowerPicSmoochum, 4, 4, 0),
    overworld_frame(sFollowerPicSmoochum, 4, 4, 1),
    overworld_frame(sFollowerPicSmoochum, 4, 4, 2),
    overworld_frame(sFollowerPicSmoochum, 4, 4, 3),
    overworld_frame(sFollowerPicSmoochum, 4, 4, 4),
    overworld_frame(sFollowerPicSmoochum, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSmoochum =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0ED,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSmoochum,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicElekid[] = INCBIN_U32("mods/following_pokemon/graphics/followers/elekid.4bpp");
static const u16 sFollowerPalElekid[] = INCBIN_U16("mods/following_pokemon/graphics/followers/elekid.gbapal");
const struct SpritePalette gFollowingPokemonPaletteElekid = { sFollowerPalElekid, 0xF0EE };
static const struct SpriteFrameImage sFollowerPicTableElekid[] =
{
    overworld_frame(sFollowerPicElekid, 4, 4, 0),
    overworld_frame(sFollowerPicElekid, 4, 4, 1),
    overworld_frame(sFollowerPicElekid, 4, 4, 2),
    overworld_frame(sFollowerPicElekid, 4, 4, 3),
    overworld_frame(sFollowerPicElekid, 4, 4, 4),
    overworld_frame(sFollowerPicElekid, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoElekid =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0EE,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableElekid,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMagby[] = INCBIN_U32("mods/following_pokemon/graphics/followers/magby.4bpp");
static const u16 sFollowerPalMagby[] = INCBIN_U16("mods/following_pokemon/graphics/followers/magby.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMagby = { sFollowerPalMagby, 0xF0EF };
static const struct SpriteFrameImage sFollowerPicTableMagby[] =
{
    overworld_frame(sFollowerPicMagby, 4, 4, 0),
    overworld_frame(sFollowerPicMagby, 4, 4, 1),
    overworld_frame(sFollowerPicMagby, 4, 4, 2),
    overworld_frame(sFollowerPicMagby, 4, 4, 3),
    overworld_frame(sFollowerPicMagby, 4, 4, 4),
    overworld_frame(sFollowerPicMagby, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMagby =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0EF,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMagby,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMiltank[] = INCBIN_U32("mods/following_pokemon/graphics/followers/miltank.4bpp");
static const u16 sFollowerPalMiltank[] = INCBIN_U16("mods/following_pokemon/graphics/followers/miltank.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMiltank = { sFollowerPalMiltank, 0xF0F0 };
static const struct SpriteFrameImage sFollowerPicTableMiltank[] =
{
    overworld_frame(sFollowerPicMiltank, 4, 4, 0),
    overworld_frame(sFollowerPicMiltank, 4, 4, 1),
    overworld_frame(sFollowerPicMiltank, 4, 4, 2),
    overworld_frame(sFollowerPicMiltank, 4, 4, 3),
    overworld_frame(sFollowerPicMiltank, 4, 4, 4),
    overworld_frame(sFollowerPicMiltank, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMiltank =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0F0,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMiltank,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicBlissey[] = INCBIN_U32("mods/following_pokemon/graphics/followers/blissey.4bpp");
static const u16 sFollowerPalBlissey[] = INCBIN_U16("mods/following_pokemon/graphics/followers/blissey.gbapal");
const struct SpritePalette gFollowingPokemonPaletteBlissey = { sFollowerPalBlissey, 0xF0F1 };
static const struct SpriteFrameImage sFollowerPicTableBlissey[] =
{
    overworld_frame(sFollowerPicBlissey, 4, 4, 0),
    overworld_frame(sFollowerPicBlissey, 4, 4, 1),
    overworld_frame(sFollowerPicBlissey, 4, 4, 2),
    overworld_frame(sFollowerPicBlissey, 4, 4, 3),
    overworld_frame(sFollowerPicBlissey, 4, 4, 4),
    overworld_frame(sFollowerPicBlissey, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoBlissey =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0F1,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableBlissey,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicRaikou[] = INCBIN_U32("mods/following_pokemon/graphics/followers/raikou.4bpp");
static const u16 sFollowerPalRaikou[] = INCBIN_U16("mods/following_pokemon/graphics/followers/raikou.gbapal");
const struct SpritePalette gFollowingPokemonPaletteRaikou = { sFollowerPalRaikou, 0xF0F2 };
static const struct SpriteFrameImage sFollowerPicTableRaikou[] =
{
    overworld_frame(sFollowerPicRaikou, 4, 4, 0),
    overworld_frame(sFollowerPicRaikou, 4, 4, 1),
    overworld_frame(sFollowerPicRaikou, 4, 4, 2),
    overworld_frame(sFollowerPicRaikou, 4, 4, 3),
    overworld_frame(sFollowerPicRaikou, 4, 4, 4),
    overworld_frame(sFollowerPicRaikou, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoRaikou =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0F2,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableRaikou,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicEntei[] = INCBIN_U32("mods/following_pokemon/graphics/followers/entei.4bpp");
static const u16 sFollowerPalEntei[] = INCBIN_U16("mods/following_pokemon/graphics/followers/entei.gbapal");
const struct SpritePalette gFollowingPokemonPaletteEntei = { sFollowerPalEntei, 0xF0F3 };
static const struct SpriteFrameImage sFollowerPicTableEntei[] =
{
    overworld_frame(sFollowerPicEntei, 4, 4, 0),
    overworld_frame(sFollowerPicEntei, 4, 4, 1),
    overworld_frame(sFollowerPicEntei, 4, 4, 2),
    overworld_frame(sFollowerPicEntei, 4, 4, 3),
    overworld_frame(sFollowerPicEntei, 4, 4, 4),
    overworld_frame(sFollowerPicEntei, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoEntei =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0F3,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableEntei,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSuicune[] = INCBIN_U32("mods/following_pokemon/graphics/followers/suicune.4bpp");
static const u16 sFollowerPalSuicune[] = INCBIN_U16("mods/following_pokemon/graphics/followers/suicune.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSuicune = { sFollowerPalSuicune, 0xF0F4 };
static const struct SpriteFrameImage sFollowerPicTableSuicune[] =
{
    overworld_frame(sFollowerPicSuicune, 4, 4, 0),
    overworld_frame(sFollowerPicSuicune, 4, 4, 1),
    overworld_frame(sFollowerPicSuicune, 4, 4, 2),
    overworld_frame(sFollowerPicSuicune, 4, 4, 3),
    overworld_frame(sFollowerPicSuicune, 4, 4, 4),
    overworld_frame(sFollowerPicSuicune, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSuicune =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0F4,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSuicune,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicLarvitar[] = INCBIN_U32("mods/following_pokemon/graphics/followers/larvitar.4bpp");
static const u16 sFollowerPalLarvitar[] = INCBIN_U16("mods/following_pokemon/graphics/followers/larvitar.gbapal");
const struct SpritePalette gFollowingPokemonPaletteLarvitar = { sFollowerPalLarvitar, 0xF0F5 };
static const struct SpriteFrameImage sFollowerPicTableLarvitar[] =
{
    overworld_frame(sFollowerPicLarvitar, 4, 4, 0),
    overworld_frame(sFollowerPicLarvitar, 4, 4, 1),
    overworld_frame(sFollowerPicLarvitar, 4, 4, 2),
    overworld_frame(sFollowerPicLarvitar, 4, 4, 3),
    overworld_frame(sFollowerPicLarvitar, 4, 4, 4),
    overworld_frame(sFollowerPicLarvitar, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoLarvitar =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0F5,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableLarvitar,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicPupitar[] = INCBIN_U32("mods/following_pokemon/graphics/followers/pupitar.4bpp");
static const u16 sFollowerPalPupitar[] = INCBIN_U16("mods/following_pokemon/graphics/followers/pupitar.gbapal");
const struct SpritePalette gFollowingPokemonPalettePupitar = { sFollowerPalPupitar, 0xF0F6 };
static const struct SpriteFrameImage sFollowerPicTablePupitar[] =
{
    overworld_frame(sFollowerPicPupitar, 4, 4, 0),
    overworld_frame(sFollowerPicPupitar, 4, 4, 1),
    overworld_frame(sFollowerPicPupitar, 4, 4, 2),
    overworld_frame(sFollowerPicPupitar, 4, 4, 3),
    overworld_frame(sFollowerPicPupitar, 4, 4, 4),
    overworld_frame(sFollowerPicPupitar, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoPupitar =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0F6,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTablePupitar,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicTyranitar[] = INCBIN_U32("mods/following_pokemon/graphics/followers/tyranitar.4bpp");
static const u16 sFollowerPalTyranitar[] = INCBIN_U16("mods/following_pokemon/graphics/followers/tyranitar.gbapal");
const struct SpritePalette gFollowingPokemonPaletteTyranitar = { sFollowerPalTyranitar, 0xF0F7 };
static const struct SpriteFrameImage sFollowerPicTableTyranitar[] =
{
    overworld_frame(sFollowerPicTyranitar, 4, 4, 0),
    overworld_frame(sFollowerPicTyranitar, 4, 4, 1),
    overworld_frame(sFollowerPicTyranitar, 4, 4, 2),
    overworld_frame(sFollowerPicTyranitar, 4, 4, 3),
    overworld_frame(sFollowerPicTyranitar, 4, 4, 4),
    overworld_frame(sFollowerPicTyranitar, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoTyranitar =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0F7,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableTyranitar,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicLugia[] = INCBIN_U32("mods/following_pokemon/graphics/followers/lugia.4bpp");
static const u16 sFollowerPalLugia[] = INCBIN_U16("mods/following_pokemon/graphics/followers/lugia.gbapal");
const struct SpritePalette gFollowingPokemonPaletteLugia = { sFollowerPalLugia, 0xF0F8 };
static const struct SpriteFrameImage sFollowerPicTableLugia[] =
{
    overworld_frame(sFollowerPicLugia, 8, 8, 0),
    overworld_frame(sFollowerPicLugia, 8, 8, 1),
    overworld_frame(sFollowerPicLugia, 8, 8, 2),
    overworld_frame(sFollowerPicLugia, 8, 8, 3),
    overworld_frame(sFollowerPicLugia, 8, 8, 4),
    overworld_frame(sFollowerPicLugia, 8, 8, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoLugia =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0F8,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 2048,
    .width = 64,
    .height = 64,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_L,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_64x64,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableLugia,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicHoOh[] = INCBIN_U32("mods/following_pokemon/graphics/followers/ho_oh.4bpp");
static const u16 sFollowerPalHoOh[] = INCBIN_U16("mods/following_pokemon/graphics/followers/ho_oh.gbapal");
const struct SpritePalette gFollowingPokemonPaletteHoOh = { sFollowerPalHoOh, 0xF0F9 };
static const struct SpriteFrameImage sFollowerPicTableHoOh[] =
{
    overworld_frame(sFollowerPicHoOh, 8, 8, 0),
    overworld_frame(sFollowerPicHoOh, 8, 8, 1),
    overworld_frame(sFollowerPicHoOh, 8, 8, 2),
    overworld_frame(sFollowerPicHoOh, 8, 8, 3),
    overworld_frame(sFollowerPicHoOh, 8, 8, 4),
    overworld_frame(sFollowerPicHoOh, 8, 8, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoHoOh =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0F9,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 2048,
    .width = 64,
    .height = 64,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_L,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_64x64,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableHoOh,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicCelebi[] = INCBIN_U32("mods/following_pokemon/graphics/followers/celebi.4bpp");
static const u16 sFollowerPalCelebi[] = INCBIN_U16("mods/following_pokemon/graphics/followers/celebi.gbapal");
const struct SpritePalette gFollowingPokemonPaletteCelebi = { sFollowerPalCelebi, 0xF0FA };
static const struct SpriteFrameImage sFollowerPicTableCelebi[] =
{
    overworld_frame(sFollowerPicCelebi, 4, 4, 0),
    overworld_frame(sFollowerPicCelebi, 4, 4, 1),
    overworld_frame(sFollowerPicCelebi, 4, 4, 2),
    overworld_frame(sFollowerPicCelebi, 4, 4, 3),
    overworld_frame(sFollowerPicCelebi, 4, 4, 4),
    overworld_frame(sFollowerPicCelebi, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoCelebi =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0FA,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableCelebi,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicTreecko[] = INCBIN_U32("mods/following_pokemon/graphics/followers/treecko.4bpp");
static const u16 sFollowerPalTreecko[] = INCBIN_U16("mods/following_pokemon/graphics/followers/treecko.gbapal");
const struct SpritePalette gFollowingPokemonPaletteTreecko = { sFollowerPalTreecko, 0xF0FB };
static const struct SpriteFrameImage sFollowerPicTableTreecko[] =
{
    overworld_frame(sFollowerPicTreecko, 4, 4, 0),
    overworld_frame(sFollowerPicTreecko, 4, 4, 1),
    overworld_frame(sFollowerPicTreecko, 4, 4, 2),
    overworld_frame(sFollowerPicTreecko, 4, 4, 3),
    overworld_frame(sFollowerPicTreecko, 4, 4, 4),
    overworld_frame(sFollowerPicTreecko, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoTreecko =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0FB,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableTreecko,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicGrovyle[] = INCBIN_U32("mods/following_pokemon/graphics/followers/grovyle.4bpp");
static const u16 sFollowerPalGrovyle[] = INCBIN_U16("mods/following_pokemon/graphics/followers/grovyle.gbapal");
const struct SpritePalette gFollowingPokemonPaletteGrovyle = { sFollowerPalGrovyle, 0xF0FC };
static const struct SpriteFrameImage sFollowerPicTableGrovyle[] =
{
    overworld_frame(sFollowerPicGrovyle, 4, 4, 0),
    overworld_frame(sFollowerPicGrovyle, 4, 4, 1),
    overworld_frame(sFollowerPicGrovyle, 4, 4, 2),
    overworld_frame(sFollowerPicGrovyle, 4, 4, 3),
    overworld_frame(sFollowerPicGrovyle, 4, 4, 4),
    overworld_frame(sFollowerPicGrovyle, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoGrovyle =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0FC,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableGrovyle,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSceptile[] = INCBIN_U32("mods/following_pokemon/graphics/followers/sceptile.4bpp");
static const u16 sFollowerPalSceptile[] = INCBIN_U16("mods/following_pokemon/graphics/followers/sceptile.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSceptile = { sFollowerPalSceptile, 0xF0FD };
static const struct SpriteFrameImage sFollowerPicTableSceptile[] =
{
    overworld_frame(sFollowerPicSceptile, 4, 4, 0),
    overworld_frame(sFollowerPicSceptile, 4, 4, 1),
    overworld_frame(sFollowerPicSceptile, 4, 4, 2),
    overworld_frame(sFollowerPicSceptile, 4, 4, 3),
    overworld_frame(sFollowerPicSceptile, 4, 4, 4),
    overworld_frame(sFollowerPicSceptile, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSceptile =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0FD,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSceptile,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicTorchic[] = INCBIN_U32("mods/following_pokemon/graphics/followers/torchic.4bpp");
static const u16 sFollowerPalTorchic[] = INCBIN_U16("mods/following_pokemon/graphics/followers/torchic.gbapal");
const struct SpritePalette gFollowingPokemonPaletteTorchic = { sFollowerPalTorchic, 0xF0FE };
static const struct SpriteFrameImage sFollowerPicTableTorchic[] =
{
    overworld_frame(sFollowerPicTorchic, 4, 4, 0),
    overworld_frame(sFollowerPicTorchic, 4, 4, 1),
    overworld_frame(sFollowerPicTorchic, 4, 4, 2),
    overworld_frame(sFollowerPicTorchic, 4, 4, 3),
    overworld_frame(sFollowerPicTorchic, 4, 4, 4),
    overworld_frame(sFollowerPicTorchic, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoTorchic =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0FE,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableTorchic,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicCombusken[] = INCBIN_U32("mods/following_pokemon/graphics/followers/combusken.4bpp");
static const u16 sFollowerPalCombusken[] = INCBIN_U16("mods/following_pokemon/graphics/followers/combusken.gbapal");
const struct SpritePalette gFollowingPokemonPaletteCombusken = { sFollowerPalCombusken, 0xF0FF };
static const struct SpriteFrameImage sFollowerPicTableCombusken[] =
{
    overworld_frame(sFollowerPicCombusken, 4, 4, 0),
    overworld_frame(sFollowerPicCombusken, 4, 4, 1),
    overworld_frame(sFollowerPicCombusken, 4, 4, 2),
    overworld_frame(sFollowerPicCombusken, 4, 4, 3),
    overworld_frame(sFollowerPicCombusken, 4, 4, 4),
    overworld_frame(sFollowerPicCombusken, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoCombusken =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF0FF,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableCombusken,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicBlaziken[] = INCBIN_U32("mods/following_pokemon/graphics/followers/blaziken.4bpp");
static const u16 sFollowerPalBlaziken[] = INCBIN_U16("mods/following_pokemon/graphics/followers/blaziken.gbapal");
const struct SpritePalette gFollowingPokemonPaletteBlaziken = { sFollowerPalBlaziken, 0xF100 };
static const struct SpriteFrameImage sFollowerPicTableBlaziken[] =
{
    overworld_frame(sFollowerPicBlaziken, 4, 4, 0),
    overworld_frame(sFollowerPicBlaziken, 4, 4, 1),
    overworld_frame(sFollowerPicBlaziken, 4, 4, 2),
    overworld_frame(sFollowerPicBlaziken, 4, 4, 3),
    overworld_frame(sFollowerPicBlaziken, 4, 4, 4),
    overworld_frame(sFollowerPicBlaziken, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoBlaziken =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF100,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableBlaziken,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMudkip[] = INCBIN_U32("mods/following_pokemon/graphics/followers/mudkip.4bpp");
static const u16 sFollowerPalMudkip[] = INCBIN_U16("mods/following_pokemon/graphics/followers/mudkip.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMudkip = { sFollowerPalMudkip, 0xF101 };
static const struct SpriteFrameImage sFollowerPicTableMudkip[] =
{
    overworld_frame(sFollowerPicMudkip, 4, 4, 0),
    overworld_frame(sFollowerPicMudkip, 4, 4, 1),
    overworld_frame(sFollowerPicMudkip, 4, 4, 2),
    overworld_frame(sFollowerPicMudkip, 4, 4, 3),
    overworld_frame(sFollowerPicMudkip, 4, 4, 4),
    overworld_frame(sFollowerPicMudkip, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMudkip =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF101,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMudkip,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMarshtomp[] = INCBIN_U32("mods/following_pokemon/graphics/followers/marshtomp.4bpp");
static const u16 sFollowerPalMarshtomp[] = INCBIN_U16("mods/following_pokemon/graphics/followers/marshtomp.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMarshtomp = { sFollowerPalMarshtomp, 0xF102 };
static const struct SpriteFrameImage sFollowerPicTableMarshtomp[] =
{
    overworld_frame(sFollowerPicMarshtomp, 4, 4, 0),
    overworld_frame(sFollowerPicMarshtomp, 4, 4, 1),
    overworld_frame(sFollowerPicMarshtomp, 4, 4, 2),
    overworld_frame(sFollowerPicMarshtomp, 4, 4, 3),
    overworld_frame(sFollowerPicMarshtomp, 4, 4, 4),
    overworld_frame(sFollowerPicMarshtomp, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMarshtomp =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF102,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMarshtomp,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSwampert[] = INCBIN_U32("mods/following_pokemon/graphics/followers/swampert.4bpp");
static const u16 sFollowerPalSwampert[] = INCBIN_U16("mods/following_pokemon/graphics/followers/swampert.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSwampert = { sFollowerPalSwampert, 0xF103 };
static const struct SpriteFrameImage sFollowerPicTableSwampert[] =
{
    overworld_frame(sFollowerPicSwampert, 4, 4, 0),
    overworld_frame(sFollowerPicSwampert, 4, 4, 1),
    overworld_frame(sFollowerPicSwampert, 4, 4, 2),
    overworld_frame(sFollowerPicSwampert, 4, 4, 3),
    overworld_frame(sFollowerPicSwampert, 4, 4, 4),
    overworld_frame(sFollowerPicSwampert, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSwampert =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF103,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSwampert,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicPoochyena[] = INCBIN_U32("mods/following_pokemon/graphics/followers/poochyena.4bpp");
static const u16 sFollowerPalPoochyena[] = INCBIN_U16("mods/following_pokemon/graphics/followers/poochyena.gbapal");
const struct SpritePalette gFollowingPokemonPalettePoochyena = { sFollowerPalPoochyena, 0xF104 };
static const struct SpriteFrameImage sFollowerPicTablePoochyena[] =
{
    overworld_frame(sFollowerPicPoochyena, 4, 4, 0),
    overworld_frame(sFollowerPicPoochyena, 4, 4, 1),
    overworld_frame(sFollowerPicPoochyena, 4, 4, 2),
    overworld_frame(sFollowerPicPoochyena, 4, 4, 3),
    overworld_frame(sFollowerPicPoochyena, 4, 4, 4),
    overworld_frame(sFollowerPicPoochyena, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoPoochyena =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF104,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTablePoochyena,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMightyena[] = INCBIN_U32("mods/following_pokemon/graphics/followers/mightyena.4bpp");
static const u16 sFollowerPalMightyena[] = INCBIN_U16("mods/following_pokemon/graphics/followers/mightyena.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMightyena = { sFollowerPalMightyena, 0xF105 };
static const struct SpriteFrameImage sFollowerPicTableMightyena[] =
{
    overworld_frame(sFollowerPicMightyena, 4, 4, 0),
    overworld_frame(sFollowerPicMightyena, 4, 4, 1),
    overworld_frame(sFollowerPicMightyena, 4, 4, 2),
    overworld_frame(sFollowerPicMightyena, 4, 4, 3),
    overworld_frame(sFollowerPicMightyena, 4, 4, 4),
    overworld_frame(sFollowerPicMightyena, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMightyena =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF105,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMightyena,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicZigzagoon[] = INCBIN_U32("mods/following_pokemon/graphics/followers/zigzagoon.4bpp");
static const u16 sFollowerPalZigzagoon[] = INCBIN_U16("mods/following_pokemon/graphics/followers/zigzagoon.gbapal");
const struct SpritePalette gFollowingPokemonPaletteZigzagoon = { sFollowerPalZigzagoon, 0xF106 };
static const struct SpriteFrameImage sFollowerPicTableZigzagoon[] =
{
    overworld_frame(sFollowerPicZigzagoon, 4, 4, 0),
    overworld_frame(sFollowerPicZigzagoon, 4, 4, 1),
    overworld_frame(sFollowerPicZigzagoon, 4, 4, 2),
    overworld_frame(sFollowerPicZigzagoon, 4, 4, 3),
    overworld_frame(sFollowerPicZigzagoon, 4, 4, 4),
    overworld_frame(sFollowerPicZigzagoon, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoZigzagoon =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF106,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableZigzagoon,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicLinoone[] = INCBIN_U32("mods/following_pokemon/graphics/followers/linoone.4bpp");
static const u16 sFollowerPalLinoone[] = INCBIN_U16("mods/following_pokemon/graphics/followers/linoone.gbapal");
const struct SpritePalette gFollowingPokemonPaletteLinoone = { sFollowerPalLinoone, 0xF107 };
static const struct SpriteFrameImage sFollowerPicTableLinoone[] =
{
    overworld_frame(sFollowerPicLinoone, 4, 4, 0),
    overworld_frame(sFollowerPicLinoone, 4, 4, 1),
    overworld_frame(sFollowerPicLinoone, 4, 4, 2),
    overworld_frame(sFollowerPicLinoone, 4, 4, 3),
    overworld_frame(sFollowerPicLinoone, 4, 4, 4),
    overworld_frame(sFollowerPicLinoone, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoLinoone =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF107,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableLinoone,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicWurmple[] = INCBIN_U32("mods/following_pokemon/graphics/followers/wurmple.4bpp");
static const u16 sFollowerPalWurmple[] = INCBIN_U16("mods/following_pokemon/graphics/followers/wurmple.gbapal");
const struct SpritePalette gFollowingPokemonPaletteWurmple = { sFollowerPalWurmple, 0xF108 };
static const struct SpriteFrameImage sFollowerPicTableWurmple[] =
{
    overworld_frame(sFollowerPicWurmple, 4, 4, 0),
    overworld_frame(sFollowerPicWurmple, 4, 4, 1),
    overworld_frame(sFollowerPicWurmple, 4, 4, 2),
    overworld_frame(sFollowerPicWurmple, 4, 4, 3),
    overworld_frame(sFollowerPicWurmple, 4, 4, 4),
    overworld_frame(sFollowerPicWurmple, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoWurmple =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF108,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableWurmple,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSilcoon[] = INCBIN_U32("mods/following_pokemon/graphics/followers/silcoon.4bpp");
static const u16 sFollowerPalSilcoon[] = INCBIN_U16("mods/following_pokemon/graphics/followers/silcoon.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSilcoon = { sFollowerPalSilcoon, 0xF109 };
static const struct SpriteFrameImage sFollowerPicTableSilcoon[] =
{
    overworld_frame(sFollowerPicSilcoon, 4, 4, 0),
    overworld_frame(sFollowerPicSilcoon, 4, 4, 1),
    overworld_frame(sFollowerPicSilcoon, 4, 4, 2),
    overworld_frame(sFollowerPicSilcoon, 4, 4, 3),
    overworld_frame(sFollowerPicSilcoon, 4, 4, 4),
    overworld_frame(sFollowerPicSilcoon, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSilcoon =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF109,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSilcoon,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicBeautifly[] = INCBIN_U32("mods/following_pokemon/graphics/followers/beautifly.4bpp");
static const u16 sFollowerPalBeautifly[] = INCBIN_U16("mods/following_pokemon/graphics/followers/beautifly.gbapal");
const struct SpritePalette gFollowingPokemonPaletteBeautifly = { sFollowerPalBeautifly, 0xF10A };
static const struct SpriteFrameImage sFollowerPicTableBeautifly[] =
{
    overworld_frame(sFollowerPicBeautifly, 4, 4, 0),
    overworld_frame(sFollowerPicBeautifly, 4, 4, 1),
    overworld_frame(sFollowerPicBeautifly, 4, 4, 2),
    overworld_frame(sFollowerPicBeautifly, 4, 4, 3),
    overworld_frame(sFollowerPicBeautifly, 4, 4, 4),
    overworld_frame(sFollowerPicBeautifly, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoBeautifly =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF10A,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableBeautifly,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicCascoon[] = INCBIN_U32("mods/following_pokemon/graphics/followers/cascoon.4bpp");
static const u16 sFollowerPalCascoon[] = INCBIN_U16("mods/following_pokemon/graphics/followers/cascoon.gbapal");
const struct SpritePalette gFollowingPokemonPaletteCascoon = { sFollowerPalCascoon, 0xF10B };
static const struct SpriteFrameImage sFollowerPicTableCascoon[] =
{
    overworld_frame(sFollowerPicCascoon, 4, 4, 0),
    overworld_frame(sFollowerPicCascoon, 4, 4, 1),
    overworld_frame(sFollowerPicCascoon, 4, 4, 2),
    overworld_frame(sFollowerPicCascoon, 4, 4, 3),
    overworld_frame(sFollowerPicCascoon, 4, 4, 4),
    overworld_frame(sFollowerPicCascoon, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoCascoon =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF10B,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableCascoon,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicDustox[] = INCBIN_U32("mods/following_pokemon/graphics/followers/dustox.4bpp");
static const u16 sFollowerPalDustox[] = INCBIN_U16("mods/following_pokemon/graphics/followers/dustox.gbapal");
const struct SpritePalette gFollowingPokemonPaletteDustox = { sFollowerPalDustox, 0xF10C };
static const struct SpriteFrameImage sFollowerPicTableDustox[] =
{
    overworld_frame(sFollowerPicDustox, 4, 4, 0),
    overworld_frame(sFollowerPicDustox, 4, 4, 1),
    overworld_frame(sFollowerPicDustox, 4, 4, 2),
    overworld_frame(sFollowerPicDustox, 4, 4, 3),
    overworld_frame(sFollowerPicDustox, 4, 4, 4),
    overworld_frame(sFollowerPicDustox, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoDustox =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF10C,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableDustox,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicLotad[] = INCBIN_U32("mods/following_pokemon/graphics/followers/lotad.4bpp");
static const u16 sFollowerPalLotad[] = INCBIN_U16("mods/following_pokemon/graphics/followers/lotad.gbapal");
const struct SpritePalette gFollowingPokemonPaletteLotad = { sFollowerPalLotad, 0xF10D };
static const struct SpriteFrameImage sFollowerPicTableLotad[] =
{
    overworld_frame(sFollowerPicLotad, 4, 4, 0),
    overworld_frame(sFollowerPicLotad, 4, 4, 1),
    overworld_frame(sFollowerPicLotad, 4, 4, 2),
    overworld_frame(sFollowerPicLotad, 4, 4, 3),
    overworld_frame(sFollowerPicLotad, 4, 4, 4),
    overworld_frame(sFollowerPicLotad, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoLotad =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF10D,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableLotad,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicLombre[] = INCBIN_U32("mods/following_pokemon/graphics/followers/lombre.4bpp");
static const u16 sFollowerPalLombre[] = INCBIN_U16("mods/following_pokemon/graphics/followers/lombre.gbapal");
const struct SpritePalette gFollowingPokemonPaletteLombre = { sFollowerPalLombre, 0xF10E };
static const struct SpriteFrameImage sFollowerPicTableLombre[] =
{
    overworld_frame(sFollowerPicLombre, 4, 4, 0),
    overworld_frame(sFollowerPicLombre, 4, 4, 1),
    overworld_frame(sFollowerPicLombre, 4, 4, 2),
    overworld_frame(sFollowerPicLombre, 4, 4, 3),
    overworld_frame(sFollowerPicLombre, 4, 4, 4),
    overworld_frame(sFollowerPicLombre, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoLombre =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF10E,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableLombre,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicLudicolo[] = INCBIN_U32("mods/following_pokemon/graphics/followers/ludicolo.4bpp");
static const u16 sFollowerPalLudicolo[] = INCBIN_U16("mods/following_pokemon/graphics/followers/ludicolo.gbapal");
const struct SpritePalette gFollowingPokemonPaletteLudicolo = { sFollowerPalLudicolo, 0xF10F };
static const struct SpriteFrameImage sFollowerPicTableLudicolo[] =
{
    overworld_frame(sFollowerPicLudicolo, 4, 4, 0),
    overworld_frame(sFollowerPicLudicolo, 4, 4, 1),
    overworld_frame(sFollowerPicLudicolo, 4, 4, 2),
    overworld_frame(sFollowerPicLudicolo, 4, 4, 3),
    overworld_frame(sFollowerPicLudicolo, 4, 4, 4),
    overworld_frame(sFollowerPicLudicolo, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoLudicolo =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF10F,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableLudicolo,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSeedot[] = INCBIN_U32("mods/following_pokemon/graphics/followers/seedot.4bpp");
static const u16 sFollowerPalSeedot[] = INCBIN_U16("mods/following_pokemon/graphics/followers/seedot.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSeedot = { sFollowerPalSeedot, 0xF110 };
static const struct SpriteFrameImage sFollowerPicTableSeedot[] =
{
    overworld_frame(sFollowerPicSeedot, 4, 4, 0),
    overworld_frame(sFollowerPicSeedot, 4, 4, 1),
    overworld_frame(sFollowerPicSeedot, 4, 4, 2),
    overworld_frame(sFollowerPicSeedot, 4, 4, 3),
    overworld_frame(sFollowerPicSeedot, 4, 4, 4),
    overworld_frame(sFollowerPicSeedot, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSeedot =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF110,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSeedot,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicNuzleaf[] = INCBIN_U32("mods/following_pokemon/graphics/followers/nuzleaf.4bpp");
static const u16 sFollowerPalNuzleaf[] = INCBIN_U16("mods/following_pokemon/graphics/followers/nuzleaf.gbapal");
const struct SpritePalette gFollowingPokemonPaletteNuzleaf = { sFollowerPalNuzleaf, 0xF111 };
static const struct SpriteFrameImage sFollowerPicTableNuzleaf[] =
{
    overworld_frame(sFollowerPicNuzleaf, 4, 4, 0),
    overworld_frame(sFollowerPicNuzleaf, 4, 4, 1),
    overworld_frame(sFollowerPicNuzleaf, 4, 4, 2),
    overworld_frame(sFollowerPicNuzleaf, 4, 4, 3),
    overworld_frame(sFollowerPicNuzleaf, 4, 4, 4),
    overworld_frame(sFollowerPicNuzleaf, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoNuzleaf =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF111,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableNuzleaf,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicShiftry[] = INCBIN_U32("mods/following_pokemon/graphics/followers/shiftry.4bpp");
static const u16 sFollowerPalShiftry[] = INCBIN_U16("mods/following_pokemon/graphics/followers/shiftry.gbapal");
const struct SpritePalette gFollowingPokemonPaletteShiftry = { sFollowerPalShiftry, 0xF112 };
static const struct SpriteFrameImage sFollowerPicTableShiftry[] =
{
    overworld_frame(sFollowerPicShiftry, 4, 4, 0),
    overworld_frame(sFollowerPicShiftry, 4, 4, 1),
    overworld_frame(sFollowerPicShiftry, 4, 4, 2),
    overworld_frame(sFollowerPicShiftry, 4, 4, 3),
    overworld_frame(sFollowerPicShiftry, 4, 4, 4),
    overworld_frame(sFollowerPicShiftry, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoShiftry =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF112,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableShiftry,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicNincada[] = INCBIN_U32("mods/following_pokemon/graphics/followers/nincada.4bpp");
static const u16 sFollowerPalNincada[] = INCBIN_U16("mods/following_pokemon/graphics/followers/nincada.gbapal");
const struct SpritePalette gFollowingPokemonPaletteNincada = { sFollowerPalNincada, 0xF113 };
static const struct SpriteFrameImage sFollowerPicTableNincada[] =
{
    overworld_frame(sFollowerPicNincada, 4, 4, 0),
    overworld_frame(sFollowerPicNincada, 4, 4, 1),
    overworld_frame(sFollowerPicNincada, 4, 4, 2),
    overworld_frame(sFollowerPicNincada, 4, 4, 3),
    overworld_frame(sFollowerPicNincada, 4, 4, 4),
    overworld_frame(sFollowerPicNincada, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoNincada =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF113,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableNincada,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicNinjask[] = INCBIN_U32("mods/following_pokemon/graphics/followers/ninjask.4bpp");
static const u16 sFollowerPalNinjask[] = INCBIN_U16("mods/following_pokemon/graphics/followers/ninjask.gbapal");
const struct SpritePalette gFollowingPokemonPaletteNinjask = { sFollowerPalNinjask, 0xF114 };
static const struct SpriteFrameImage sFollowerPicTableNinjask[] =
{
    overworld_frame(sFollowerPicNinjask, 4, 4, 0),
    overworld_frame(sFollowerPicNinjask, 4, 4, 1),
    overworld_frame(sFollowerPicNinjask, 4, 4, 2),
    overworld_frame(sFollowerPicNinjask, 4, 4, 3),
    overworld_frame(sFollowerPicNinjask, 4, 4, 4),
    overworld_frame(sFollowerPicNinjask, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoNinjask =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF114,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableNinjask,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicShedinja[] = INCBIN_U32("mods/following_pokemon/graphics/followers/shedinja.4bpp");
static const u16 sFollowerPalShedinja[] = INCBIN_U16("mods/following_pokemon/graphics/followers/shedinja.gbapal");
const struct SpritePalette gFollowingPokemonPaletteShedinja = { sFollowerPalShedinja, 0xF115 };
static const struct SpriteFrameImage sFollowerPicTableShedinja[] =
{
    overworld_frame(sFollowerPicShedinja, 4, 4, 0),
    overworld_frame(sFollowerPicShedinja, 4, 4, 1),
    overworld_frame(sFollowerPicShedinja, 4, 4, 2),
    overworld_frame(sFollowerPicShedinja, 4, 4, 3),
    overworld_frame(sFollowerPicShedinja, 4, 4, 4),
    overworld_frame(sFollowerPicShedinja, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoShedinja =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF115,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableShedinja,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicTaillow[] = INCBIN_U32("mods/following_pokemon/graphics/followers/taillow.4bpp");
static const u16 sFollowerPalTaillow[] = INCBIN_U16("mods/following_pokemon/graphics/followers/taillow.gbapal");
const struct SpritePalette gFollowingPokemonPaletteTaillow = { sFollowerPalTaillow, 0xF116 };
static const struct SpriteFrameImage sFollowerPicTableTaillow[] =
{
    overworld_frame(sFollowerPicTaillow, 4, 4, 0),
    overworld_frame(sFollowerPicTaillow, 4, 4, 1),
    overworld_frame(sFollowerPicTaillow, 4, 4, 2),
    overworld_frame(sFollowerPicTaillow, 4, 4, 3),
    overworld_frame(sFollowerPicTaillow, 4, 4, 4),
    overworld_frame(sFollowerPicTaillow, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoTaillow =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF116,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableTaillow,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSwellow[] = INCBIN_U32("mods/following_pokemon/graphics/followers/swellow.4bpp");
static const u16 sFollowerPalSwellow[] = INCBIN_U16("mods/following_pokemon/graphics/followers/swellow.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSwellow = { sFollowerPalSwellow, 0xF117 };
static const struct SpriteFrameImage sFollowerPicTableSwellow[] =
{
    overworld_frame(sFollowerPicSwellow, 4, 4, 0),
    overworld_frame(sFollowerPicSwellow, 4, 4, 1),
    overworld_frame(sFollowerPicSwellow, 4, 4, 2),
    overworld_frame(sFollowerPicSwellow, 4, 4, 3),
    overworld_frame(sFollowerPicSwellow, 4, 4, 4),
    overworld_frame(sFollowerPicSwellow, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSwellow =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF117,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSwellow,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicShroomish[] = INCBIN_U32("mods/following_pokemon/graphics/followers/shroomish.4bpp");
static const u16 sFollowerPalShroomish[] = INCBIN_U16("mods/following_pokemon/graphics/followers/shroomish.gbapal");
const struct SpritePalette gFollowingPokemonPaletteShroomish = { sFollowerPalShroomish, 0xF118 };
static const struct SpriteFrameImage sFollowerPicTableShroomish[] =
{
    overworld_frame(sFollowerPicShroomish, 4, 4, 0),
    overworld_frame(sFollowerPicShroomish, 4, 4, 1),
    overworld_frame(sFollowerPicShroomish, 4, 4, 2),
    overworld_frame(sFollowerPicShroomish, 4, 4, 3),
    overworld_frame(sFollowerPicShroomish, 4, 4, 4),
    overworld_frame(sFollowerPicShroomish, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoShroomish =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF118,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableShroomish,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicBreloom[] = INCBIN_U32("mods/following_pokemon/graphics/followers/breloom.4bpp");
static const u16 sFollowerPalBreloom[] = INCBIN_U16("mods/following_pokemon/graphics/followers/breloom.gbapal");
const struct SpritePalette gFollowingPokemonPaletteBreloom = { sFollowerPalBreloom, 0xF119 };
static const struct SpriteFrameImage sFollowerPicTableBreloom[] =
{
    overworld_frame(sFollowerPicBreloom, 4, 4, 0),
    overworld_frame(sFollowerPicBreloom, 4, 4, 1),
    overworld_frame(sFollowerPicBreloom, 4, 4, 2),
    overworld_frame(sFollowerPicBreloom, 4, 4, 3),
    overworld_frame(sFollowerPicBreloom, 4, 4, 4),
    overworld_frame(sFollowerPicBreloom, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoBreloom =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF119,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableBreloom,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSpinda[] = INCBIN_U32("mods/following_pokemon/graphics/followers/spinda.4bpp");
static const u16 sFollowerPalSpinda[] = INCBIN_U16("mods/following_pokemon/graphics/followers/spinda.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSpinda = { sFollowerPalSpinda, 0xF11A };
static const struct SpriteFrameImage sFollowerPicTableSpinda[] =
{
    overworld_frame(sFollowerPicSpinda, 4, 4, 0),
    overworld_frame(sFollowerPicSpinda, 4, 4, 1),
    overworld_frame(sFollowerPicSpinda, 4, 4, 2),
    overworld_frame(sFollowerPicSpinda, 4, 4, 3),
    overworld_frame(sFollowerPicSpinda, 4, 4, 4),
    overworld_frame(sFollowerPicSpinda, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSpinda =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF11A,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSpinda,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicWingull[] = INCBIN_U32("mods/following_pokemon/graphics/followers/wingull.4bpp");
static const u16 sFollowerPalWingull[] = INCBIN_U16("mods/following_pokemon/graphics/followers/wingull.gbapal");
const struct SpritePalette gFollowingPokemonPaletteWingull = { sFollowerPalWingull, 0xF11B };
static const struct SpriteFrameImage sFollowerPicTableWingull[] =
{
    overworld_frame(sFollowerPicWingull, 4, 4, 0),
    overworld_frame(sFollowerPicWingull, 4, 4, 1),
    overworld_frame(sFollowerPicWingull, 4, 4, 2),
    overworld_frame(sFollowerPicWingull, 4, 4, 3),
    overworld_frame(sFollowerPicWingull, 4, 4, 4),
    overworld_frame(sFollowerPicWingull, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoWingull =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF11B,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableWingull,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicPelipper[] = INCBIN_U32("mods/following_pokemon/graphics/followers/pelipper.4bpp");
static const u16 sFollowerPalPelipper[] = INCBIN_U16("mods/following_pokemon/graphics/followers/pelipper.gbapal");
const struct SpritePalette gFollowingPokemonPalettePelipper = { sFollowerPalPelipper, 0xF11C };
static const struct SpriteFrameImage sFollowerPicTablePelipper[] =
{
    overworld_frame(sFollowerPicPelipper, 4, 4, 0),
    overworld_frame(sFollowerPicPelipper, 4, 4, 1),
    overworld_frame(sFollowerPicPelipper, 4, 4, 2),
    overworld_frame(sFollowerPicPelipper, 4, 4, 3),
    overworld_frame(sFollowerPicPelipper, 4, 4, 4),
    overworld_frame(sFollowerPicPelipper, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoPelipper =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF11C,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTablePelipper,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSurskit[] = INCBIN_U32("mods/following_pokemon/graphics/followers/surskit.4bpp");
static const u16 sFollowerPalSurskit[] = INCBIN_U16("mods/following_pokemon/graphics/followers/surskit.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSurskit = { sFollowerPalSurskit, 0xF11D };
static const struct SpriteFrameImage sFollowerPicTableSurskit[] =
{
    overworld_frame(sFollowerPicSurskit, 4, 4, 0),
    overworld_frame(sFollowerPicSurskit, 4, 4, 1),
    overworld_frame(sFollowerPicSurskit, 4, 4, 2),
    overworld_frame(sFollowerPicSurskit, 4, 4, 3),
    overworld_frame(sFollowerPicSurskit, 4, 4, 4),
    overworld_frame(sFollowerPicSurskit, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSurskit =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF11D,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSurskit,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMasquerain[] = INCBIN_U32("mods/following_pokemon/graphics/followers/masquerain.4bpp");
static const u16 sFollowerPalMasquerain[] = INCBIN_U16("mods/following_pokemon/graphics/followers/masquerain.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMasquerain = { sFollowerPalMasquerain, 0xF11E };
static const struct SpriteFrameImage sFollowerPicTableMasquerain[] =
{
    overworld_frame(sFollowerPicMasquerain, 4, 4, 0),
    overworld_frame(sFollowerPicMasquerain, 4, 4, 1),
    overworld_frame(sFollowerPicMasquerain, 4, 4, 2),
    overworld_frame(sFollowerPicMasquerain, 4, 4, 3),
    overworld_frame(sFollowerPicMasquerain, 4, 4, 4),
    overworld_frame(sFollowerPicMasquerain, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMasquerain =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF11E,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMasquerain,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicWailmer[] = INCBIN_U32("mods/following_pokemon/graphics/followers/wailmer.4bpp");
static const u16 sFollowerPalWailmer[] = INCBIN_U16("mods/following_pokemon/graphics/followers/wailmer.gbapal");
const struct SpritePalette gFollowingPokemonPaletteWailmer = { sFollowerPalWailmer, 0xF11F };
static const struct SpriteFrameImage sFollowerPicTableWailmer[] =
{
    overworld_frame(sFollowerPicWailmer, 4, 4, 0),
    overworld_frame(sFollowerPicWailmer, 4, 4, 1),
    overworld_frame(sFollowerPicWailmer, 4, 4, 2),
    overworld_frame(sFollowerPicWailmer, 4, 4, 3),
    overworld_frame(sFollowerPicWailmer, 4, 4, 4),
    overworld_frame(sFollowerPicWailmer, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoWailmer =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF11F,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableWailmer,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicWailord[] = INCBIN_U32("mods/following_pokemon/graphics/followers/wailord.4bpp");
static const u16 sFollowerPalWailord[] = INCBIN_U16("mods/following_pokemon/graphics/followers/wailord.gbapal");
const struct SpritePalette gFollowingPokemonPaletteWailord = { sFollowerPalWailord, 0xF120 };
static const struct SpriteFrameImage sFollowerPicTableWailord[] =
{
    overworld_frame(sFollowerPicWailord, 8, 8, 0),
    overworld_frame(sFollowerPicWailord, 8, 8, 1),
    overworld_frame(sFollowerPicWailord, 8, 8, 2),
    overworld_frame(sFollowerPicWailord, 8, 8, 3),
    overworld_frame(sFollowerPicWailord, 8, 8, 4),
    overworld_frame(sFollowerPicWailord, 8, 8, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoWailord =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF120,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 2048,
    .width = 64,
    .height = 64,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_L,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_64x64,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableWailord,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSkitty[] = INCBIN_U32("mods/following_pokemon/graphics/followers/skitty.4bpp");
static const u16 sFollowerPalSkitty[] = INCBIN_U16("mods/following_pokemon/graphics/followers/skitty.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSkitty = { sFollowerPalSkitty, 0xF121 };
static const struct SpriteFrameImage sFollowerPicTableSkitty[] =
{
    overworld_frame(sFollowerPicSkitty, 4, 4, 0),
    overworld_frame(sFollowerPicSkitty, 4, 4, 1),
    overworld_frame(sFollowerPicSkitty, 4, 4, 2),
    overworld_frame(sFollowerPicSkitty, 4, 4, 3),
    overworld_frame(sFollowerPicSkitty, 4, 4, 4),
    overworld_frame(sFollowerPicSkitty, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSkitty =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF121,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSkitty,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicDelcatty[] = INCBIN_U32("mods/following_pokemon/graphics/followers/delcatty.4bpp");
static const u16 sFollowerPalDelcatty[] = INCBIN_U16("mods/following_pokemon/graphics/followers/delcatty.gbapal");
const struct SpritePalette gFollowingPokemonPaletteDelcatty = { sFollowerPalDelcatty, 0xF122 };
static const struct SpriteFrameImage sFollowerPicTableDelcatty[] =
{
    overworld_frame(sFollowerPicDelcatty, 4, 4, 0),
    overworld_frame(sFollowerPicDelcatty, 4, 4, 1),
    overworld_frame(sFollowerPicDelcatty, 4, 4, 2),
    overworld_frame(sFollowerPicDelcatty, 4, 4, 3),
    overworld_frame(sFollowerPicDelcatty, 4, 4, 4),
    overworld_frame(sFollowerPicDelcatty, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoDelcatty =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF122,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableDelcatty,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicKecleon[] = INCBIN_U32("mods/following_pokemon/graphics/followers/kecleon.4bpp");
static const u16 sFollowerPalKecleon[] = INCBIN_U16("mods/following_pokemon/graphics/followers/kecleon.gbapal");
const struct SpritePalette gFollowingPokemonPaletteKecleon = { sFollowerPalKecleon, 0xF123 };
static const struct SpriteFrameImage sFollowerPicTableKecleon[] =
{
    overworld_frame(sFollowerPicKecleon, 4, 4, 0),
    overworld_frame(sFollowerPicKecleon, 4, 4, 1),
    overworld_frame(sFollowerPicKecleon, 4, 4, 2),
    overworld_frame(sFollowerPicKecleon, 4, 4, 3),
    overworld_frame(sFollowerPicKecleon, 4, 4, 4),
    overworld_frame(sFollowerPicKecleon, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoKecleon =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF123,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableKecleon,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicBaltoy[] = INCBIN_U32("mods/following_pokemon/graphics/followers/baltoy.4bpp");
static const u16 sFollowerPalBaltoy[] = INCBIN_U16("mods/following_pokemon/graphics/followers/baltoy.gbapal");
const struct SpritePalette gFollowingPokemonPaletteBaltoy = { sFollowerPalBaltoy, 0xF124 };
static const struct SpriteFrameImage sFollowerPicTableBaltoy[] =
{
    overworld_frame(sFollowerPicBaltoy, 4, 4, 0),
    overworld_frame(sFollowerPicBaltoy, 4, 4, 1),
    overworld_frame(sFollowerPicBaltoy, 4, 4, 2),
    overworld_frame(sFollowerPicBaltoy, 4, 4, 3),
    overworld_frame(sFollowerPicBaltoy, 4, 4, 4),
    overworld_frame(sFollowerPicBaltoy, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoBaltoy =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF124,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableBaltoy,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicClaydol[] = INCBIN_U32("mods/following_pokemon/graphics/followers/claydol.4bpp");
static const u16 sFollowerPalClaydol[] = INCBIN_U16("mods/following_pokemon/graphics/followers/claydol.gbapal");
const struct SpritePalette gFollowingPokemonPaletteClaydol = { sFollowerPalClaydol, 0xF125 };
static const struct SpriteFrameImage sFollowerPicTableClaydol[] =
{
    overworld_frame(sFollowerPicClaydol, 4, 4, 0),
    overworld_frame(sFollowerPicClaydol, 4, 4, 1),
    overworld_frame(sFollowerPicClaydol, 4, 4, 2),
    overworld_frame(sFollowerPicClaydol, 4, 4, 3),
    overworld_frame(sFollowerPicClaydol, 4, 4, 4),
    overworld_frame(sFollowerPicClaydol, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoClaydol =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF125,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableClaydol,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicNosepass[] = INCBIN_U32("mods/following_pokemon/graphics/followers/nosepass.4bpp");
static const u16 sFollowerPalNosepass[] = INCBIN_U16("mods/following_pokemon/graphics/followers/nosepass.gbapal");
const struct SpritePalette gFollowingPokemonPaletteNosepass = { sFollowerPalNosepass, 0xF126 };
static const struct SpriteFrameImage sFollowerPicTableNosepass[] =
{
    overworld_frame(sFollowerPicNosepass, 4, 4, 0),
    overworld_frame(sFollowerPicNosepass, 4, 4, 1),
    overworld_frame(sFollowerPicNosepass, 4, 4, 2),
    overworld_frame(sFollowerPicNosepass, 4, 4, 3),
    overworld_frame(sFollowerPicNosepass, 4, 4, 4),
    overworld_frame(sFollowerPicNosepass, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoNosepass =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF126,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableNosepass,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicTorkoal[] = INCBIN_U32("mods/following_pokemon/graphics/followers/torkoal.4bpp");
static const u16 sFollowerPalTorkoal[] = INCBIN_U16("mods/following_pokemon/graphics/followers/torkoal.gbapal");
const struct SpritePalette gFollowingPokemonPaletteTorkoal = { sFollowerPalTorkoal, 0xF127 };
static const struct SpriteFrameImage sFollowerPicTableTorkoal[] =
{
    overworld_frame(sFollowerPicTorkoal, 4, 4, 0),
    overworld_frame(sFollowerPicTorkoal, 4, 4, 1),
    overworld_frame(sFollowerPicTorkoal, 4, 4, 2),
    overworld_frame(sFollowerPicTorkoal, 4, 4, 3),
    overworld_frame(sFollowerPicTorkoal, 4, 4, 4),
    overworld_frame(sFollowerPicTorkoal, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoTorkoal =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF127,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableTorkoal,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSableye[] = INCBIN_U32("mods/following_pokemon/graphics/followers/sableye.4bpp");
static const u16 sFollowerPalSableye[] = INCBIN_U16("mods/following_pokemon/graphics/followers/sableye.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSableye = { sFollowerPalSableye, 0xF128 };
static const struct SpriteFrameImage sFollowerPicTableSableye[] =
{
    overworld_frame(sFollowerPicSableye, 4, 4, 0),
    overworld_frame(sFollowerPicSableye, 4, 4, 1),
    overworld_frame(sFollowerPicSableye, 4, 4, 2),
    overworld_frame(sFollowerPicSableye, 4, 4, 3),
    overworld_frame(sFollowerPicSableye, 4, 4, 4),
    overworld_frame(sFollowerPicSableye, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSableye =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF128,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSableye,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicBarboach[] = INCBIN_U32("mods/following_pokemon/graphics/followers/barboach.4bpp");
static const u16 sFollowerPalBarboach[] = INCBIN_U16("mods/following_pokemon/graphics/followers/barboach.gbapal");
const struct SpritePalette gFollowingPokemonPaletteBarboach = { sFollowerPalBarboach, 0xF129 };
static const struct SpriteFrameImage sFollowerPicTableBarboach[] =
{
    overworld_frame(sFollowerPicBarboach, 4, 4, 0),
    overworld_frame(sFollowerPicBarboach, 4, 4, 1),
    overworld_frame(sFollowerPicBarboach, 4, 4, 2),
    overworld_frame(sFollowerPicBarboach, 4, 4, 3),
    overworld_frame(sFollowerPicBarboach, 4, 4, 4),
    overworld_frame(sFollowerPicBarboach, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoBarboach =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF129,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableBarboach,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicWhiscash[] = INCBIN_U32("mods/following_pokemon/graphics/followers/whiscash.4bpp");
static const u16 sFollowerPalWhiscash[] = INCBIN_U16("mods/following_pokemon/graphics/followers/whiscash.gbapal");
const struct SpritePalette gFollowingPokemonPaletteWhiscash = { sFollowerPalWhiscash, 0xF12A };
static const struct SpriteFrameImage sFollowerPicTableWhiscash[] =
{
    overworld_frame(sFollowerPicWhiscash, 4, 4, 0),
    overworld_frame(sFollowerPicWhiscash, 4, 4, 1),
    overworld_frame(sFollowerPicWhiscash, 4, 4, 2),
    overworld_frame(sFollowerPicWhiscash, 4, 4, 3),
    overworld_frame(sFollowerPicWhiscash, 4, 4, 4),
    overworld_frame(sFollowerPicWhiscash, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoWhiscash =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF12A,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableWhiscash,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicLuvdisc[] = INCBIN_U32("mods/following_pokemon/graphics/followers/luvdisc.4bpp");
static const u16 sFollowerPalLuvdisc[] = INCBIN_U16("mods/following_pokemon/graphics/followers/luvdisc.gbapal");
const struct SpritePalette gFollowingPokemonPaletteLuvdisc = { sFollowerPalLuvdisc, 0xF12B };
static const struct SpriteFrameImage sFollowerPicTableLuvdisc[] =
{
    overworld_frame(sFollowerPicLuvdisc, 4, 4, 0),
    overworld_frame(sFollowerPicLuvdisc, 4, 4, 1),
    overworld_frame(sFollowerPicLuvdisc, 4, 4, 2),
    overworld_frame(sFollowerPicLuvdisc, 4, 4, 3),
    overworld_frame(sFollowerPicLuvdisc, 4, 4, 4),
    overworld_frame(sFollowerPicLuvdisc, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoLuvdisc =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF12B,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableLuvdisc,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicCorphish[] = INCBIN_U32("mods/following_pokemon/graphics/followers/corphish.4bpp");
static const u16 sFollowerPalCorphish[] = INCBIN_U16("mods/following_pokemon/graphics/followers/corphish.gbapal");
const struct SpritePalette gFollowingPokemonPaletteCorphish = { sFollowerPalCorphish, 0xF12C };
static const struct SpriteFrameImage sFollowerPicTableCorphish[] =
{
    overworld_frame(sFollowerPicCorphish, 4, 4, 0),
    overworld_frame(sFollowerPicCorphish, 4, 4, 1),
    overworld_frame(sFollowerPicCorphish, 4, 4, 2),
    overworld_frame(sFollowerPicCorphish, 4, 4, 3),
    overworld_frame(sFollowerPicCorphish, 4, 4, 4),
    overworld_frame(sFollowerPicCorphish, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoCorphish =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF12C,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableCorphish,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicCrawdaunt[] = INCBIN_U32("mods/following_pokemon/graphics/followers/crawdaunt.4bpp");
static const u16 sFollowerPalCrawdaunt[] = INCBIN_U16("mods/following_pokemon/graphics/followers/crawdaunt.gbapal");
const struct SpritePalette gFollowingPokemonPaletteCrawdaunt = { sFollowerPalCrawdaunt, 0xF12D };
static const struct SpriteFrameImage sFollowerPicTableCrawdaunt[] =
{
    overworld_frame(sFollowerPicCrawdaunt, 4, 4, 0),
    overworld_frame(sFollowerPicCrawdaunt, 4, 4, 1),
    overworld_frame(sFollowerPicCrawdaunt, 4, 4, 2),
    overworld_frame(sFollowerPicCrawdaunt, 4, 4, 3),
    overworld_frame(sFollowerPicCrawdaunt, 4, 4, 4),
    overworld_frame(sFollowerPicCrawdaunt, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoCrawdaunt =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF12D,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableCrawdaunt,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicFeebas[] = INCBIN_U32("mods/following_pokemon/graphics/followers/feebas.4bpp");
static const u16 sFollowerPalFeebas[] = INCBIN_U16("mods/following_pokemon/graphics/followers/feebas.gbapal");
const struct SpritePalette gFollowingPokemonPaletteFeebas = { sFollowerPalFeebas, 0xF12E };
static const struct SpriteFrameImage sFollowerPicTableFeebas[] =
{
    overworld_frame(sFollowerPicFeebas, 4, 4, 0),
    overworld_frame(sFollowerPicFeebas, 4, 4, 1),
    overworld_frame(sFollowerPicFeebas, 4, 4, 2),
    overworld_frame(sFollowerPicFeebas, 4, 4, 3),
    overworld_frame(sFollowerPicFeebas, 4, 4, 4),
    overworld_frame(sFollowerPicFeebas, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoFeebas =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF12E,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableFeebas,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMilotic[] = INCBIN_U32("mods/following_pokemon/graphics/followers/milotic.4bpp");
static const u16 sFollowerPalMilotic[] = INCBIN_U16("mods/following_pokemon/graphics/followers/milotic.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMilotic = { sFollowerPalMilotic, 0xF12F };
static const struct SpriteFrameImage sFollowerPicTableMilotic[] =
{
    overworld_frame(sFollowerPicMilotic, 4, 4, 0),
    overworld_frame(sFollowerPicMilotic, 4, 4, 1),
    overworld_frame(sFollowerPicMilotic, 4, 4, 2),
    overworld_frame(sFollowerPicMilotic, 4, 4, 3),
    overworld_frame(sFollowerPicMilotic, 4, 4, 4),
    overworld_frame(sFollowerPicMilotic, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMilotic =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF12F,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMilotic,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicCarvanha[] = INCBIN_U32("mods/following_pokemon/graphics/followers/carvanha.4bpp");
static const u16 sFollowerPalCarvanha[] = INCBIN_U16("mods/following_pokemon/graphics/followers/carvanha.gbapal");
const struct SpritePalette gFollowingPokemonPaletteCarvanha = { sFollowerPalCarvanha, 0xF130 };
static const struct SpriteFrameImage sFollowerPicTableCarvanha[] =
{
    overworld_frame(sFollowerPicCarvanha, 4, 4, 0),
    overworld_frame(sFollowerPicCarvanha, 4, 4, 1),
    overworld_frame(sFollowerPicCarvanha, 4, 4, 2),
    overworld_frame(sFollowerPicCarvanha, 4, 4, 3),
    overworld_frame(sFollowerPicCarvanha, 4, 4, 4),
    overworld_frame(sFollowerPicCarvanha, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoCarvanha =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF130,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableCarvanha,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSharpedo[] = INCBIN_U32("mods/following_pokemon/graphics/followers/sharpedo.4bpp");
static const u16 sFollowerPalSharpedo[] = INCBIN_U16("mods/following_pokemon/graphics/followers/sharpedo.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSharpedo = { sFollowerPalSharpedo, 0xF131 };
static const struct SpriteFrameImage sFollowerPicTableSharpedo[] =
{
    overworld_frame(sFollowerPicSharpedo, 4, 4, 0),
    overworld_frame(sFollowerPicSharpedo, 4, 4, 1),
    overworld_frame(sFollowerPicSharpedo, 4, 4, 2),
    overworld_frame(sFollowerPicSharpedo, 4, 4, 3),
    overworld_frame(sFollowerPicSharpedo, 4, 4, 4),
    overworld_frame(sFollowerPicSharpedo, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSharpedo =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF131,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSharpedo,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicTrapinch[] = INCBIN_U32("mods/following_pokemon/graphics/followers/trapinch.4bpp");
static const u16 sFollowerPalTrapinch[] = INCBIN_U16("mods/following_pokemon/graphics/followers/trapinch.gbapal");
const struct SpritePalette gFollowingPokemonPaletteTrapinch = { sFollowerPalTrapinch, 0xF132 };
static const struct SpriteFrameImage sFollowerPicTableTrapinch[] =
{
    overworld_frame(sFollowerPicTrapinch, 4, 4, 0),
    overworld_frame(sFollowerPicTrapinch, 4, 4, 1),
    overworld_frame(sFollowerPicTrapinch, 4, 4, 2),
    overworld_frame(sFollowerPicTrapinch, 4, 4, 3),
    overworld_frame(sFollowerPicTrapinch, 4, 4, 4),
    overworld_frame(sFollowerPicTrapinch, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoTrapinch =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF132,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableTrapinch,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicVibrava[] = INCBIN_U32("mods/following_pokemon/graphics/followers/vibrava.4bpp");
static const u16 sFollowerPalVibrava[] = INCBIN_U16("mods/following_pokemon/graphics/followers/vibrava.gbapal");
const struct SpritePalette gFollowingPokemonPaletteVibrava = { sFollowerPalVibrava, 0xF133 };
static const struct SpriteFrameImage sFollowerPicTableVibrava[] =
{
    overworld_frame(sFollowerPicVibrava, 4, 4, 0),
    overworld_frame(sFollowerPicVibrava, 4, 4, 1),
    overworld_frame(sFollowerPicVibrava, 4, 4, 2),
    overworld_frame(sFollowerPicVibrava, 4, 4, 3),
    overworld_frame(sFollowerPicVibrava, 4, 4, 4),
    overworld_frame(sFollowerPicVibrava, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoVibrava =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF133,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableVibrava,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicFlygon[] = INCBIN_U32("mods/following_pokemon/graphics/followers/flygon.4bpp");
static const u16 sFollowerPalFlygon[] = INCBIN_U16("mods/following_pokemon/graphics/followers/flygon.gbapal");
const struct SpritePalette gFollowingPokemonPaletteFlygon = { sFollowerPalFlygon, 0xF134 };
static const struct SpriteFrameImage sFollowerPicTableFlygon[] =
{
    overworld_frame(sFollowerPicFlygon, 4, 4, 0),
    overworld_frame(sFollowerPicFlygon, 4, 4, 1),
    overworld_frame(sFollowerPicFlygon, 4, 4, 2),
    overworld_frame(sFollowerPicFlygon, 4, 4, 3),
    overworld_frame(sFollowerPicFlygon, 4, 4, 4),
    overworld_frame(sFollowerPicFlygon, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoFlygon =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF134,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableFlygon,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMakuhita[] = INCBIN_U32("mods/following_pokemon/graphics/followers/makuhita.4bpp");
static const u16 sFollowerPalMakuhita[] = INCBIN_U16("mods/following_pokemon/graphics/followers/makuhita.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMakuhita = { sFollowerPalMakuhita, 0xF135 };
static const struct SpriteFrameImage sFollowerPicTableMakuhita[] =
{
    overworld_frame(sFollowerPicMakuhita, 4, 4, 0),
    overworld_frame(sFollowerPicMakuhita, 4, 4, 1),
    overworld_frame(sFollowerPicMakuhita, 4, 4, 2),
    overworld_frame(sFollowerPicMakuhita, 4, 4, 3),
    overworld_frame(sFollowerPicMakuhita, 4, 4, 4),
    overworld_frame(sFollowerPicMakuhita, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMakuhita =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF135,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMakuhita,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicHariyama[] = INCBIN_U32("mods/following_pokemon/graphics/followers/hariyama.4bpp");
static const u16 sFollowerPalHariyama[] = INCBIN_U16("mods/following_pokemon/graphics/followers/hariyama.gbapal");
const struct SpritePalette gFollowingPokemonPaletteHariyama = { sFollowerPalHariyama, 0xF136 };
static const struct SpriteFrameImage sFollowerPicTableHariyama[] =
{
    overworld_frame(sFollowerPicHariyama, 4, 4, 0),
    overworld_frame(sFollowerPicHariyama, 4, 4, 1),
    overworld_frame(sFollowerPicHariyama, 4, 4, 2),
    overworld_frame(sFollowerPicHariyama, 4, 4, 3),
    overworld_frame(sFollowerPicHariyama, 4, 4, 4),
    overworld_frame(sFollowerPicHariyama, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoHariyama =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF136,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableHariyama,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicElectrike[] = INCBIN_U32("mods/following_pokemon/graphics/followers/electrike.4bpp");
static const u16 sFollowerPalElectrike[] = INCBIN_U16("mods/following_pokemon/graphics/followers/electrike.gbapal");
const struct SpritePalette gFollowingPokemonPaletteElectrike = { sFollowerPalElectrike, 0xF137 };
static const struct SpriteFrameImage sFollowerPicTableElectrike[] =
{
    overworld_frame(sFollowerPicElectrike, 4, 4, 0),
    overworld_frame(sFollowerPicElectrike, 4, 4, 1),
    overworld_frame(sFollowerPicElectrike, 4, 4, 2),
    overworld_frame(sFollowerPicElectrike, 4, 4, 3),
    overworld_frame(sFollowerPicElectrike, 4, 4, 4),
    overworld_frame(sFollowerPicElectrike, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoElectrike =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF137,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableElectrike,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicManectric[] = INCBIN_U32("mods/following_pokemon/graphics/followers/manectric.4bpp");
static const u16 sFollowerPalManectric[] = INCBIN_U16("mods/following_pokemon/graphics/followers/manectric.gbapal");
const struct SpritePalette gFollowingPokemonPaletteManectric = { sFollowerPalManectric, 0xF138 };
static const struct SpriteFrameImage sFollowerPicTableManectric[] =
{
    overworld_frame(sFollowerPicManectric, 4, 4, 0),
    overworld_frame(sFollowerPicManectric, 4, 4, 1),
    overworld_frame(sFollowerPicManectric, 4, 4, 2),
    overworld_frame(sFollowerPicManectric, 4, 4, 3),
    overworld_frame(sFollowerPicManectric, 4, 4, 4),
    overworld_frame(sFollowerPicManectric, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoManectric =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF138,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableManectric,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicNumel[] = INCBIN_U32("mods/following_pokemon/graphics/followers/numel.4bpp");
static const u16 sFollowerPalNumel[] = INCBIN_U16("mods/following_pokemon/graphics/followers/numel.gbapal");
const struct SpritePalette gFollowingPokemonPaletteNumel = { sFollowerPalNumel, 0xF139 };
static const struct SpriteFrameImage sFollowerPicTableNumel[] =
{
    overworld_frame(sFollowerPicNumel, 4, 4, 0),
    overworld_frame(sFollowerPicNumel, 4, 4, 1),
    overworld_frame(sFollowerPicNumel, 4, 4, 2),
    overworld_frame(sFollowerPicNumel, 4, 4, 3),
    overworld_frame(sFollowerPicNumel, 4, 4, 4),
    overworld_frame(sFollowerPicNumel, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoNumel =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF139,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableNumel,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicCamerupt[] = INCBIN_U32("mods/following_pokemon/graphics/followers/camerupt.4bpp");
static const u16 sFollowerPalCamerupt[] = INCBIN_U16("mods/following_pokemon/graphics/followers/camerupt.gbapal");
const struct SpritePalette gFollowingPokemonPaletteCamerupt = { sFollowerPalCamerupt, 0xF13A };
static const struct SpriteFrameImage sFollowerPicTableCamerupt[] =
{
    overworld_frame(sFollowerPicCamerupt, 4, 4, 0),
    overworld_frame(sFollowerPicCamerupt, 4, 4, 1),
    overworld_frame(sFollowerPicCamerupt, 4, 4, 2),
    overworld_frame(sFollowerPicCamerupt, 4, 4, 3),
    overworld_frame(sFollowerPicCamerupt, 4, 4, 4),
    overworld_frame(sFollowerPicCamerupt, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoCamerupt =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF13A,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableCamerupt,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSpheal[] = INCBIN_U32("mods/following_pokemon/graphics/followers/spheal.4bpp");
static const u16 sFollowerPalSpheal[] = INCBIN_U16("mods/following_pokemon/graphics/followers/spheal.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSpheal = { sFollowerPalSpheal, 0xF13B };
static const struct SpriteFrameImage sFollowerPicTableSpheal[] =
{
    overworld_frame(sFollowerPicSpheal, 4, 4, 0),
    overworld_frame(sFollowerPicSpheal, 4, 4, 1),
    overworld_frame(sFollowerPicSpheal, 4, 4, 2),
    overworld_frame(sFollowerPicSpheal, 4, 4, 3),
    overworld_frame(sFollowerPicSpheal, 4, 4, 4),
    overworld_frame(sFollowerPicSpheal, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSpheal =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF13B,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSpheal,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSealeo[] = INCBIN_U32("mods/following_pokemon/graphics/followers/sealeo.4bpp");
static const u16 sFollowerPalSealeo[] = INCBIN_U16("mods/following_pokemon/graphics/followers/sealeo.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSealeo = { sFollowerPalSealeo, 0xF13C };
static const struct SpriteFrameImage sFollowerPicTableSealeo[] =
{
    overworld_frame(sFollowerPicSealeo, 4, 4, 0),
    overworld_frame(sFollowerPicSealeo, 4, 4, 1),
    overworld_frame(sFollowerPicSealeo, 4, 4, 2),
    overworld_frame(sFollowerPicSealeo, 4, 4, 3),
    overworld_frame(sFollowerPicSealeo, 4, 4, 4),
    overworld_frame(sFollowerPicSealeo, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSealeo =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF13C,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSealeo,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicWalrein[] = INCBIN_U32("mods/following_pokemon/graphics/followers/walrein.4bpp");
static const u16 sFollowerPalWalrein[] = INCBIN_U16("mods/following_pokemon/graphics/followers/walrein.gbapal");
const struct SpritePalette gFollowingPokemonPaletteWalrein = { sFollowerPalWalrein, 0xF13D };
static const struct SpriteFrameImage sFollowerPicTableWalrein[] =
{
    overworld_frame(sFollowerPicWalrein, 4, 4, 0),
    overworld_frame(sFollowerPicWalrein, 4, 4, 1),
    overworld_frame(sFollowerPicWalrein, 4, 4, 2),
    overworld_frame(sFollowerPicWalrein, 4, 4, 3),
    overworld_frame(sFollowerPicWalrein, 4, 4, 4),
    overworld_frame(sFollowerPicWalrein, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoWalrein =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF13D,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableWalrein,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicCacnea[] = INCBIN_U32("mods/following_pokemon/graphics/followers/cacnea.4bpp");
static const u16 sFollowerPalCacnea[] = INCBIN_U16("mods/following_pokemon/graphics/followers/cacnea.gbapal");
const struct SpritePalette gFollowingPokemonPaletteCacnea = { sFollowerPalCacnea, 0xF13E };
static const struct SpriteFrameImage sFollowerPicTableCacnea[] =
{
    overworld_frame(sFollowerPicCacnea, 4, 4, 0),
    overworld_frame(sFollowerPicCacnea, 4, 4, 1),
    overworld_frame(sFollowerPicCacnea, 4, 4, 2),
    overworld_frame(sFollowerPicCacnea, 4, 4, 3),
    overworld_frame(sFollowerPicCacnea, 4, 4, 4),
    overworld_frame(sFollowerPicCacnea, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoCacnea =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF13E,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableCacnea,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicCacturne[] = INCBIN_U32("mods/following_pokemon/graphics/followers/cacturne.4bpp");
static const u16 sFollowerPalCacturne[] = INCBIN_U16("mods/following_pokemon/graphics/followers/cacturne.gbapal");
const struct SpritePalette gFollowingPokemonPaletteCacturne = { sFollowerPalCacturne, 0xF13F };
static const struct SpriteFrameImage sFollowerPicTableCacturne[] =
{
    overworld_frame(sFollowerPicCacturne, 4, 4, 0),
    overworld_frame(sFollowerPicCacturne, 4, 4, 1),
    overworld_frame(sFollowerPicCacturne, 4, 4, 2),
    overworld_frame(sFollowerPicCacturne, 4, 4, 3),
    overworld_frame(sFollowerPicCacturne, 4, 4, 4),
    overworld_frame(sFollowerPicCacturne, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoCacturne =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF13F,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableCacturne,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSnorunt[] = INCBIN_U32("mods/following_pokemon/graphics/followers/snorunt.4bpp");
static const u16 sFollowerPalSnorunt[] = INCBIN_U16("mods/following_pokemon/graphics/followers/snorunt.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSnorunt = { sFollowerPalSnorunt, 0xF140 };
static const struct SpriteFrameImage sFollowerPicTableSnorunt[] =
{
    overworld_frame(sFollowerPicSnorunt, 4, 4, 0),
    overworld_frame(sFollowerPicSnorunt, 4, 4, 1),
    overworld_frame(sFollowerPicSnorunt, 4, 4, 2),
    overworld_frame(sFollowerPicSnorunt, 4, 4, 3),
    overworld_frame(sFollowerPicSnorunt, 4, 4, 4),
    overworld_frame(sFollowerPicSnorunt, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSnorunt =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF140,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSnorunt,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicGlalie[] = INCBIN_U32("mods/following_pokemon/graphics/followers/glalie.4bpp");
static const u16 sFollowerPalGlalie[] = INCBIN_U16("mods/following_pokemon/graphics/followers/glalie.gbapal");
const struct SpritePalette gFollowingPokemonPaletteGlalie = { sFollowerPalGlalie, 0xF141 };
static const struct SpriteFrameImage sFollowerPicTableGlalie[] =
{
    overworld_frame(sFollowerPicGlalie, 4, 4, 0),
    overworld_frame(sFollowerPicGlalie, 4, 4, 1),
    overworld_frame(sFollowerPicGlalie, 4, 4, 2),
    overworld_frame(sFollowerPicGlalie, 4, 4, 3),
    overworld_frame(sFollowerPicGlalie, 4, 4, 4),
    overworld_frame(sFollowerPicGlalie, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoGlalie =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF141,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableGlalie,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicLunatone[] = INCBIN_U32("mods/following_pokemon/graphics/followers/lunatone.4bpp");
static const u16 sFollowerPalLunatone[] = INCBIN_U16("mods/following_pokemon/graphics/followers/lunatone.gbapal");
const struct SpritePalette gFollowingPokemonPaletteLunatone = { sFollowerPalLunatone, 0xF142 };
static const struct SpriteFrameImage sFollowerPicTableLunatone[] =
{
    overworld_frame(sFollowerPicLunatone, 4, 4, 0),
    overworld_frame(sFollowerPicLunatone, 4, 4, 1),
    overworld_frame(sFollowerPicLunatone, 4, 4, 2),
    overworld_frame(sFollowerPicLunatone, 4, 4, 3),
    overworld_frame(sFollowerPicLunatone, 4, 4, 4),
    overworld_frame(sFollowerPicLunatone, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoLunatone =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF142,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableLunatone,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSolrock[] = INCBIN_U32("mods/following_pokemon/graphics/followers/solrock.4bpp");
static const u16 sFollowerPalSolrock[] = INCBIN_U16("mods/following_pokemon/graphics/followers/solrock.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSolrock = { sFollowerPalSolrock, 0xF143 };
static const struct SpriteFrameImage sFollowerPicTableSolrock[] =
{
    overworld_frame(sFollowerPicSolrock, 4, 4, 0),
    overworld_frame(sFollowerPicSolrock, 4, 4, 1),
    overworld_frame(sFollowerPicSolrock, 4, 4, 2),
    overworld_frame(sFollowerPicSolrock, 4, 4, 3),
    overworld_frame(sFollowerPicSolrock, 4, 4, 4),
    overworld_frame(sFollowerPicSolrock, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSolrock =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF143,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSolrock,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicAzurill[] = INCBIN_U32("mods/following_pokemon/graphics/followers/azurill.4bpp");
static const u16 sFollowerPalAzurill[] = INCBIN_U16("mods/following_pokemon/graphics/followers/azurill.gbapal");
const struct SpritePalette gFollowingPokemonPaletteAzurill = { sFollowerPalAzurill, 0xF144 };
static const struct SpriteFrameImage sFollowerPicTableAzurill[] =
{
    overworld_frame(sFollowerPicAzurill, 4, 4, 0),
    overworld_frame(sFollowerPicAzurill, 4, 4, 1),
    overworld_frame(sFollowerPicAzurill, 4, 4, 2),
    overworld_frame(sFollowerPicAzurill, 4, 4, 3),
    overworld_frame(sFollowerPicAzurill, 4, 4, 4),
    overworld_frame(sFollowerPicAzurill, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoAzurill =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF144,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableAzurill,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSpoink[] = INCBIN_U32("mods/following_pokemon/graphics/followers/spoink.4bpp");
static const u16 sFollowerPalSpoink[] = INCBIN_U16("mods/following_pokemon/graphics/followers/spoink.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSpoink = { sFollowerPalSpoink, 0xF145 };
static const struct SpriteFrameImage sFollowerPicTableSpoink[] =
{
    overworld_frame(sFollowerPicSpoink, 4, 4, 0),
    overworld_frame(sFollowerPicSpoink, 4, 4, 1),
    overworld_frame(sFollowerPicSpoink, 4, 4, 2),
    overworld_frame(sFollowerPicSpoink, 4, 4, 3),
    overworld_frame(sFollowerPicSpoink, 4, 4, 4),
    overworld_frame(sFollowerPicSpoink, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSpoink =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF145,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSpoink,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicGrumpig[] = INCBIN_U32("mods/following_pokemon/graphics/followers/grumpig.4bpp");
static const u16 sFollowerPalGrumpig[] = INCBIN_U16("mods/following_pokemon/graphics/followers/grumpig.gbapal");
const struct SpritePalette gFollowingPokemonPaletteGrumpig = { sFollowerPalGrumpig, 0xF146 };
static const struct SpriteFrameImage sFollowerPicTableGrumpig[] =
{
    overworld_frame(sFollowerPicGrumpig, 4, 4, 0),
    overworld_frame(sFollowerPicGrumpig, 4, 4, 1),
    overworld_frame(sFollowerPicGrumpig, 4, 4, 2),
    overworld_frame(sFollowerPicGrumpig, 4, 4, 3),
    overworld_frame(sFollowerPicGrumpig, 4, 4, 4),
    overworld_frame(sFollowerPicGrumpig, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoGrumpig =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF146,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableGrumpig,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicPlusle[] = INCBIN_U32("mods/following_pokemon/graphics/followers/plusle.4bpp");
static const u16 sFollowerPalPlusle[] = INCBIN_U16("mods/following_pokemon/graphics/followers/plusle.gbapal");
const struct SpritePalette gFollowingPokemonPalettePlusle = { sFollowerPalPlusle, 0xF147 };
static const struct SpriteFrameImage sFollowerPicTablePlusle[] =
{
    overworld_frame(sFollowerPicPlusle, 4, 4, 0),
    overworld_frame(sFollowerPicPlusle, 4, 4, 1),
    overworld_frame(sFollowerPicPlusle, 4, 4, 2),
    overworld_frame(sFollowerPicPlusle, 4, 4, 3),
    overworld_frame(sFollowerPicPlusle, 4, 4, 4),
    overworld_frame(sFollowerPicPlusle, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoPlusle =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF147,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTablePlusle,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMinun[] = INCBIN_U32("mods/following_pokemon/graphics/followers/minun.4bpp");
static const u16 sFollowerPalMinun[] = INCBIN_U16("mods/following_pokemon/graphics/followers/minun.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMinun = { sFollowerPalMinun, 0xF148 };
static const struct SpriteFrameImage sFollowerPicTableMinun[] =
{
    overworld_frame(sFollowerPicMinun, 4, 4, 0),
    overworld_frame(sFollowerPicMinun, 4, 4, 1),
    overworld_frame(sFollowerPicMinun, 4, 4, 2),
    overworld_frame(sFollowerPicMinun, 4, 4, 3),
    overworld_frame(sFollowerPicMinun, 4, 4, 4),
    overworld_frame(sFollowerPicMinun, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMinun =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF148,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMinun,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMawile[] = INCBIN_U32("mods/following_pokemon/graphics/followers/mawile.4bpp");
static const u16 sFollowerPalMawile[] = INCBIN_U16("mods/following_pokemon/graphics/followers/mawile.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMawile = { sFollowerPalMawile, 0xF149 };
static const struct SpriteFrameImage sFollowerPicTableMawile[] =
{
    overworld_frame(sFollowerPicMawile, 4, 4, 0),
    overworld_frame(sFollowerPicMawile, 4, 4, 1),
    overworld_frame(sFollowerPicMawile, 4, 4, 2),
    overworld_frame(sFollowerPicMawile, 4, 4, 3),
    overworld_frame(sFollowerPicMawile, 4, 4, 4),
    overworld_frame(sFollowerPicMawile, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMawile =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF149,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMawile,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMeditite[] = INCBIN_U32("mods/following_pokemon/graphics/followers/meditite.4bpp");
static const u16 sFollowerPalMeditite[] = INCBIN_U16("mods/following_pokemon/graphics/followers/meditite.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMeditite = { sFollowerPalMeditite, 0xF14A };
static const struct SpriteFrameImage sFollowerPicTableMeditite[] =
{
    overworld_frame(sFollowerPicMeditite, 4, 4, 0),
    overworld_frame(sFollowerPicMeditite, 4, 4, 1),
    overworld_frame(sFollowerPicMeditite, 4, 4, 2),
    overworld_frame(sFollowerPicMeditite, 4, 4, 3),
    overworld_frame(sFollowerPicMeditite, 4, 4, 4),
    overworld_frame(sFollowerPicMeditite, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMeditite =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF14A,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMeditite,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMedicham[] = INCBIN_U32("mods/following_pokemon/graphics/followers/medicham.4bpp");
static const u16 sFollowerPalMedicham[] = INCBIN_U16("mods/following_pokemon/graphics/followers/medicham.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMedicham = { sFollowerPalMedicham, 0xF14B };
static const struct SpriteFrameImage sFollowerPicTableMedicham[] =
{
    overworld_frame(sFollowerPicMedicham, 4, 4, 0),
    overworld_frame(sFollowerPicMedicham, 4, 4, 1),
    overworld_frame(sFollowerPicMedicham, 4, 4, 2),
    overworld_frame(sFollowerPicMedicham, 4, 4, 3),
    overworld_frame(sFollowerPicMedicham, 4, 4, 4),
    overworld_frame(sFollowerPicMedicham, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMedicham =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF14B,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMedicham,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSwablu[] = INCBIN_U32("mods/following_pokemon/graphics/followers/swablu.4bpp");
static const u16 sFollowerPalSwablu[] = INCBIN_U16("mods/following_pokemon/graphics/followers/swablu.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSwablu = { sFollowerPalSwablu, 0xF14C };
static const struct SpriteFrameImage sFollowerPicTableSwablu[] =
{
    overworld_frame(sFollowerPicSwablu, 4, 4, 0),
    overworld_frame(sFollowerPicSwablu, 4, 4, 1),
    overworld_frame(sFollowerPicSwablu, 4, 4, 2),
    overworld_frame(sFollowerPicSwablu, 4, 4, 3),
    overworld_frame(sFollowerPicSwablu, 4, 4, 4),
    overworld_frame(sFollowerPicSwablu, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSwablu =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF14C,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSwablu,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicAltaria[] = INCBIN_U32("mods/following_pokemon/graphics/followers/altaria.4bpp");
static const u16 sFollowerPalAltaria[] = INCBIN_U16("mods/following_pokemon/graphics/followers/altaria.gbapal");
const struct SpritePalette gFollowingPokemonPaletteAltaria = { sFollowerPalAltaria, 0xF14D };
static const struct SpriteFrameImage sFollowerPicTableAltaria[] =
{
    overworld_frame(sFollowerPicAltaria, 4, 4, 0),
    overworld_frame(sFollowerPicAltaria, 4, 4, 1),
    overworld_frame(sFollowerPicAltaria, 4, 4, 2),
    overworld_frame(sFollowerPicAltaria, 4, 4, 3),
    overworld_frame(sFollowerPicAltaria, 4, 4, 4),
    overworld_frame(sFollowerPicAltaria, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoAltaria =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF14D,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableAltaria,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicWynaut[] = INCBIN_U32("mods/following_pokemon/graphics/followers/wynaut.4bpp");
static const u16 sFollowerPalWynaut[] = INCBIN_U16("mods/following_pokemon/graphics/followers/wynaut.gbapal");
const struct SpritePalette gFollowingPokemonPaletteWynaut = { sFollowerPalWynaut, 0xF14E };
static const struct SpriteFrameImage sFollowerPicTableWynaut[] =
{
    overworld_frame(sFollowerPicWynaut, 4, 4, 0),
    overworld_frame(sFollowerPicWynaut, 4, 4, 1),
    overworld_frame(sFollowerPicWynaut, 4, 4, 2),
    overworld_frame(sFollowerPicWynaut, 4, 4, 3),
    overworld_frame(sFollowerPicWynaut, 4, 4, 4),
    overworld_frame(sFollowerPicWynaut, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoWynaut =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF14E,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableWynaut,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicDuskull[] = INCBIN_U32("mods/following_pokemon/graphics/followers/duskull.4bpp");
static const u16 sFollowerPalDuskull[] = INCBIN_U16("mods/following_pokemon/graphics/followers/duskull.gbapal");
const struct SpritePalette gFollowingPokemonPaletteDuskull = { sFollowerPalDuskull, 0xF14F };
static const struct SpriteFrameImage sFollowerPicTableDuskull[] =
{
    overworld_frame(sFollowerPicDuskull, 4, 4, 0),
    overworld_frame(sFollowerPicDuskull, 4, 4, 1),
    overworld_frame(sFollowerPicDuskull, 4, 4, 2),
    overworld_frame(sFollowerPicDuskull, 4, 4, 3),
    overworld_frame(sFollowerPicDuskull, 4, 4, 4),
    overworld_frame(sFollowerPicDuskull, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoDuskull =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF14F,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableDuskull,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicDusclops[] = INCBIN_U32("mods/following_pokemon/graphics/followers/dusclops.4bpp");
static const u16 sFollowerPalDusclops[] = INCBIN_U16("mods/following_pokemon/graphics/followers/dusclops.gbapal");
const struct SpritePalette gFollowingPokemonPaletteDusclops = { sFollowerPalDusclops, 0xF150 };
static const struct SpriteFrameImage sFollowerPicTableDusclops[] =
{
    overworld_frame(sFollowerPicDusclops, 4, 4, 0),
    overworld_frame(sFollowerPicDusclops, 4, 4, 1),
    overworld_frame(sFollowerPicDusclops, 4, 4, 2),
    overworld_frame(sFollowerPicDusclops, 4, 4, 3),
    overworld_frame(sFollowerPicDusclops, 4, 4, 4),
    overworld_frame(sFollowerPicDusclops, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoDusclops =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF150,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableDusclops,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicRoselia[] = INCBIN_U32("mods/following_pokemon/graphics/followers/roselia.4bpp");
static const u16 sFollowerPalRoselia[] = INCBIN_U16("mods/following_pokemon/graphics/followers/roselia.gbapal");
const struct SpritePalette gFollowingPokemonPaletteRoselia = { sFollowerPalRoselia, 0xF151 };
static const struct SpriteFrameImage sFollowerPicTableRoselia[] =
{
    overworld_frame(sFollowerPicRoselia, 4, 4, 0),
    overworld_frame(sFollowerPicRoselia, 4, 4, 1),
    overworld_frame(sFollowerPicRoselia, 4, 4, 2),
    overworld_frame(sFollowerPicRoselia, 4, 4, 3),
    overworld_frame(sFollowerPicRoselia, 4, 4, 4),
    overworld_frame(sFollowerPicRoselia, 4, 4, 5),
    overworld_frame(sFollowerPicRoselia, 4, 4, 6),
    overworld_frame(sFollowerPicRoselia, 4, 4, 7),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoRoselia =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF151,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableUniqueEast,
    .images = sFollowerPicTableRoselia,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSlakoth[] = INCBIN_U32("mods/following_pokemon/graphics/followers/slakoth.4bpp");
static const u16 sFollowerPalSlakoth[] = INCBIN_U16("mods/following_pokemon/graphics/followers/slakoth.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSlakoth = { sFollowerPalSlakoth, 0xF152 };
static const struct SpriteFrameImage sFollowerPicTableSlakoth[] =
{
    overworld_frame(sFollowerPicSlakoth, 4, 4, 0),
    overworld_frame(sFollowerPicSlakoth, 4, 4, 1),
    overworld_frame(sFollowerPicSlakoth, 4, 4, 2),
    overworld_frame(sFollowerPicSlakoth, 4, 4, 3),
    overworld_frame(sFollowerPicSlakoth, 4, 4, 4),
    overworld_frame(sFollowerPicSlakoth, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSlakoth =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF152,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSlakoth,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicVigoroth[] = INCBIN_U32("mods/following_pokemon/graphics/followers/vigoroth.4bpp");
static const u16 sFollowerPalVigoroth[] = INCBIN_U16("mods/following_pokemon/graphics/followers/vigoroth.gbapal");
const struct SpritePalette gFollowingPokemonPaletteVigoroth = { sFollowerPalVigoroth, 0xF153 };
static const struct SpriteFrameImage sFollowerPicTableVigoroth[] =
{
    overworld_frame(sFollowerPicVigoroth, 4, 4, 0),
    overworld_frame(sFollowerPicVigoroth, 4, 4, 1),
    overworld_frame(sFollowerPicVigoroth, 4, 4, 2),
    overworld_frame(sFollowerPicVigoroth, 4, 4, 3),
    overworld_frame(sFollowerPicVigoroth, 4, 4, 4),
    overworld_frame(sFollowerPicVigoroth, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoVigoroth =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF153,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableVigoroth,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSlaking[] = INCBIN_U32("mods/following_pokemon/graphics/followers/slaking.4bpp");
static const u16 sFollowerPalSlaking[] = INCBIN_U16("mods/following_pokemon/graphics/followers/slaking.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSlaking = { sFollowerPalSlaking, 0xF154 };
static const struct SpriteFrameImage sFollowerPicTableSlaking[] =
{
    overworld_frame(sFollowerPicSlaking, 4, 4, 0),
    overworld_frame(sFollowerPicSlaking, 4, 4, 1),
    overworld_frame(sFollowerPicSlaking, 4, 4, 2),
    overworld_frame(sFollowerPicSlaking, 4, 4, 3),
    overworld_frame(sFollowerPicSlaking, 4, 4, 4),
    overworld_frame(sFollowerPicSlaking, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSlaking =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF154,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSlaking,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicGulpin[] = INCBIN_U32("mods/following_pokemon/graphics/followers/gulpin.4bpp");
static const u16 sFollowerPalGulpin[] = INCBIN_U16("mods/following_pokemon/graphics/followers/gulpin.gbapal");
const struct SpritePalette gFollowingPokemonPaletteGulpin = { sFollowerPalGulpin, 0xF155 };
static const struct SpriteFrameImage sFollowerPicTableGulpin[] =
{
    overworld_frame(sFollowerPicGulpin, 4, 4, 0),
    overworld_frame(sFollowerPicGulpin, 4, 4, 1),
    overworld_frame(sFollowerPicGulpin, 4, 4, 2),
    overworld_frame(sFollowerPicGulpin, 4, 4, 3),
    overworld_frame(sFollowerPicGulpin, 4, 4, 4),
    overworld_frame(sFollowerPicGulpin, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoGulpin =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF155,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableGulpin,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSwalot[] = INCBIN_U32("mods/following_pokemon/graphics/followers/swalot.4bpp");
static const u16 sFollowerPalSwalot[] = INCBIN_U16("mods/following_pokemon/graphics/followers/swalot.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSwalot = { sFollowerPalSwalot, 0xF156 };
static const struct SpriteFrameImage sFollowerPicTableSwalot[] =
{
    overworld_frame(sFollowerPicSwalot, 4, 4, 0),
    overworld_frame(sFollowerPicSwalot, 4, 4, 1),
    overworld_frame(sFollowerPicSwalot, 4, 4, 2),
    overworld_frame(sFollowerPicSwalot, 4, 4, 3),
    overworld_frame(sFollowerPicSwalot, 4, 4, 4),
    overworld_frame(sFollowerPicSwalot, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSwalot =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF156,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSwalot,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicTropius[] = INCBIN_U32("mods/following_pokemon/graphics/followers/tropius.4bpp");
static const u16 sFollowerPalTropius[] = INCBIN_U16("mods/following_pokemon/graphics/followers/tropius.gbapal");
const struct SpritePalette gFollowingPokemonPaletteTropius = { sFollowerPalTropius, 0xF157 };
static const struct SpriteFrameImage sFollowerPicTableTropius[] =
{
    overworld_frame(sFollowerPicTropius, 4, 4, 0),
    overworld_frame(sFollowerPicTropius, 4, 4, 1),
    overworld_frame(sFollowerPicTropius, 4, 4, 2),
    overworld_frame(sFollowerPicTropius, 4, 4, 3),
    overworld_frame(sFollowerPicTropius, 4, 4, 4),
    overworld_frame(sFollowerPicTropius, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoTropius =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF157,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableTropius,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicWhismur[] = INCBIN_U32("mods/following_pokemon/graphics/followers/whismur.4bpp");
static const u16 sFollowerPalWhismur[] = INCBIN_U16("mods/following_pokemon/graphics/followers/whismur.gbapal");
const struct SpritePalette gFollowingPokemonPaletteWhismur = { sFollowerPalWhismur, 0xF158 };
static const struct SpriteFrameImage sFollowerPicTableWhismur[] =
{
    overworld_frame(sFollowerPicWhismur, 4, 4, 0),
    overworld_frame(sFollowerPicWhismur, 4, 4, 1),
    overworld_frame(sFollowerPicWhismur, 4, 4, 2),
    overworld_frame(sFollowerPicWhismur, 4, 4, 3),
    overworld_frame(sFollowerPicWhismur, 4, 4, 4),
    overworld_frame(sFollowerPicWhismur, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoWhismur =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF158,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableWhismur,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicLoudred[] = INCBIN_U32("mods/following_pokemon/graphics/followers/loudred.4bpp");
static const u16 sFollowerPalLoudred[] = INCBIN_U16("mods/following_pokemon/graphics/followers/loudred.gbapal");
const struct SpritePalette gFollowingPokemonPaletteLoudred = { sFollowerPalLoudred, 0xF159 };
static const struct SpriteFrameImage sFollowerPicTableLoudred[] =
{
    overworld_frame(sFollowerPicLoudred, 4, 4, 0),
    overworld_frame(sFollowerPicLoudred, 4, 4, 1),
    overworld_frame(sFollowerPicLoudred, 4, 4, 2),
    overworld_frame(sFollowerPicLoudred, 4, 4, 3),
    overworld_frame(sFollowerPicLoudred, 4, 4, 4),
    overworld_frame(sFollowerPicLoudred, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoLoudred =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF159,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableLoudred,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicExploud[] = INCBIN_U32("mods/following_pokemon/graphics/followers/exploud.4bpp");
static const u16 sFollowerPalExploud[] = INCBIN_U16("mods/following_pokemon/graphics/followers/exploud.gbapal");
const struct SpritePalette gFollowingPokemonPaletteExploud = { sFollowerPalExploud, 0xF15A };
static const struct SpriteFrameImage sFollowerPicTableExploud[] =
{
    overworld_frame(sFollowerPicExploud, 4, 4, 0),
    overworld_frame(sFollowerPicExploud, 4, 4, 1),
    overworld_frame(sFollowerPicExploud, 4, 4, 2),
    overworld_frame(sFollowerPicExploud, 4, 4, 3),
    overworld_frame(sFollowerPicExploud, 4, 4, 4),
    overworld_frame(sFollowerPicExploud, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoExploud =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF15A,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableExploud,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicClamperl[] = INCBIN_U32("mods/following_pokemon/graphics/followers/clamperl.4bpp");
static const u16 sFollowerPalClamperl[] = INCBIN_U16("mods/following_pokemon/graphics/followers/clamperl.gbapal");
const struct SpritePalette gFollowingPokemonPaletteClamperl = { sFollowerPalClamperl, 0xF15B };
static const struct SpriteFrameImage sFollowerPicTableClamperl[] =
{
    overworld_frame(sFollowerPicClamperl, 4, 4, 0),
    overworld_frame(sFollowerPicClamperl, 4, 4, 1),
    overworld_frame(sFollowerPicClamperl, 4, 4, 2),
    overworld_frame(sFollowerPicClamperl, 4, 4, 3),
    overworld_frame(sFollowerPicClamperl, 4, 4, 4),
    overworld_frame(sFollowerPicClamperl, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoClamperl =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF15B,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableClamperl,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicHuntail[] = INCBIN_U32("mods/following_pokemon/graphics/followers/huntail.4bpp");
static const u16 sFollowerPalHuntail[] = INCBIN_U16("mods/following_pokemon/graphics/followers/huntail.gbapal");
const struct SpritePalette gFollowingPokemonPaletteHuntail = { sFollowerPalHuntail, 0xF15C };
static const struct SpriteFrameImage sFollowerPicTableHuntail[] =
{
    overworld_frame(sFollowerPicHuntail, 4, 4, 0),
    overworld_frame(sFollowerPicHuntail, 4, 4, 1),
    overworld_frame(sFollowerPicHuntail, 4, 4, 2),
    overworld_frame(sFollowerPicHuntail, 4, 4, 3),
    overworld_frame(sFollowerPicHuntail, 4, 4, 4),
    overworld_frame(sFollowerPicHuntail, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoHuntail =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF15C,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_BIKE_TIRE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableHuntail,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicGorebyss[] = INCBIN_U32("mods/following_pokemon/graphics/followers/gorebyss.4bpp");
static const u16 sFollowerPalGorebyss[] = INCBIN_U16("mods/following_pokemon/graphics/followers/gorebyss.gbapal");
const struct SpritePalette gFollowingPokemonPaletteGorebyss = { sFollowerPalGorebyss, 0xF15D };
static const struct SpriteFrameImage sFollowerPicTableGorebyss[] =
{
    overworld_frame(sFollowerPicGorebyss, 4, 4, 0),
    overworld_frame(sFollowerPicGorebyss, 4, 4, 1),
    overworld_frame(sFollowerPicGorebyss, 4, 4, 2),
    overworld_frame(sFollowerPicGorebyss, 4, 4, 3),
    overworld_frame(sFollowerPicGorebyss, 4, 4, 4),
    overworld_frame(sFollowerPicGorebyss, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoGorebyss =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF15D,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_BIKE_TIRE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableGorebyss,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicAbsol[] = INCBIN_U32("mods/following_pokemon/graphics/followers/absol.4bpp");
static const u16 sFollowerPalAbsol[] = INCBIN_U16("mods/following_pokemon/graphics/followers/absol.gbapal");
const struct SpritePalette gFollowingPokemonPaletteAbsol = { sFollowerPalAbsol, 0xF15E };
static const struct SpriteFrameImage sFollowerPicTableAbsol[] =
{
    overworld_frame(sFollowerPicAbsol, 4, 4, 0),
    overworld_frame(sFollowerPicAbsol, 4, 4, 1),
    overworld_frame(sFollowerPicAbsol, 4, 4, 2),
    overworld_frame(sFollowerPicAbsol, 4, 4, 3),
    overworld_frame(sFollowerPicAbsol, 4, 4, 4),
    overworld_frame(sFollowerPicAbsol, 4, 4, 5),
    overworld_frame(sFollowerPicAbsol, 4, 4, 6),
    overworld_frame(sFollowerPicAbsol, 4, 4, 7),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoAbsol =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF15E,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableUniqueEast,
    .images = sFollowerPicTableAbsol,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicShuppet[] = INCBIN_U32("mods/following_pokemon/graphics/followers/shuppet.4bpp");
static const u16 sFollowerPalShuppet[] = INCBIN_U16("mods/following_pokemon/graphics/followers/shuppet.gbapal");
const struct SpritePalette gFollowingPokemonPaletteShuppet = { sFollowerPalShuppet, 0xF15F };
static const struct SpriteFrameImage sFollowerPicTableShuppet[] =
{
    overworld_frame(sFollowerPicShuppet, 4, 4, 0),
    overworld_frame(sFollowerPicShuppet, 4, 4, 1),
    overworld_frame(sFollowerPicShuppet, 4, 4, 2),
    overworld_frame(sFollowerPicShuppet, 4, 4, 3),
    overworld_frame(sFollowerPicShuppet, 4, 4, 4),
    overworld_frame(sFollowerPicShuppet, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoShuppet =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF15F,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableShuppet,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicBanette[] = INCBIN_U32("mods/following_pokemon/graphics/followers/banette.4bpp");
static const u16 sFollowerPalBanette[] = INCBIN_U16("mods/following_pokemon/graphics/followers/banette.gbapal");
const struct SpritePalette gFollowingPokemonPaletteBanette = { sFollowerPalBanette, 0xF160 };
static const struct SpriteFrameImage sFollowerPicTableBanette[] =
{
    overworld_frame(sFollowerPicBanette, 4, 4, 0),
    overworld_frame(sFollowerPicBanette, 4, 4, 1),
    overworld_frame(sFollowerPicBanette, 4, 4, 2),
    overworld_frame(sFollowerPicBanette, 4, 4, 3),
    overworld_frame(sFollowerPicBanette, 4, 4, 4),
    overworld_frame(sFollowerPicBanette, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoBanette =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF160,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableBanette,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSeviper[] = INCBIN_U32("mods/following_pokemon/graphics/followers/seviper.4bpp");
static const u16 sFollowerPalSeviper[] = INCBIN_U16("mods/following_pokemon/graphics/followers/seviper.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSeviper = { sFollowerPalSeviper, 0xF161 };
static const struct SpriteFrameImage sFollowerPicTableSeviper[] =
{
    overworld_frame(sFollowerPicSeviper, 4, 4, 0),
    overworld_frame(sFollowerPicSeviper, 4, 4, 1),
    overworld_frame(sFollowerPicSeviper, 4, 4, 2),
    overworld_frame(sFollowerPicSeviper, 4, 4, 3),
    overworld_frame(sFollowerPicSeviper, 4, 4, 4),
    overworld_frame(sFollowerPicSeviper, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSeviper =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF161,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_BIKE_TIRE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSeviper,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicZangoose[] = INCBIN_U32("mods/following_pokemon/graphics/followers/zangoose.4bpp");
static const u16 sFollowerPalZangoose[] = INCBIN_U16("mods/following_pokemon/graphics/followers/zangoose.gbapal");
const struct SpritePalette gFollowingPokemonPaletteZangoose = { sFollowerPalZangoose, 0xF162 };
static const struct SpriteFrameImage sFollowerPicTableZangoose[] =
{
    overworld_frame(sFollowerPicZangoose, 4, 4, 0),
    overworld_frame(sFollowerPicZangoose, 4, 4, 1),
    overworld_frame(sFollowerPicZangoose, 4, 4, 2),
    overworld_frame(sFollowerPicZangoose, 4, 4, 3),
    overworld_frame(sFollowerPicZangoose, 4, 4, 4),
    overworld_frame(sFollowerPicZangoose, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoZangoose =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF162,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableZangoose,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicRelicanth[] = INCBIN_U32("mods/following_pokemon/graphics/followers/relicanth.4bpp");
static const u16 sFollowerPalRelicanth[] = INCBIN_U16("mods/following_pokemon/graphics/followers/relicanth.gbapal");
const struct SpritePalette gFollowingPokemonPaletteRelicanth = { sFollowerPalRelicanth, 0xF163 };
static const struct SpriteFrameImage sFollowerPicTableRelicanth[] =
{
    overworld_frame(sFollowerPicRelicanth, 4, 4, 0),
    overworld_frame(sFollowerPicRelicanth, 4, 4, 1),
    overworld_frame(sFollowerPicRelicanth, 4, 4, 2),
    overworld_frame(sFollowerPicRelicanth, 4, 4, 3),
    overworld_frame(sFollowerPicRelicanth, 4, 4, 4),
    overworld_frame(sFollowerPicRelicanth, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoRelicanth =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF163,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableRelicanth,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicAron[] = INCBIN_U32("mods/following_pokemon/graphics/followers/aron.4bpp");
static const u16 sFollowerPalAron[] = INCBIN_U16("mods/following_pokemon/graphics/followers/aron.gbapal");
const struct SpritePalette gFollowingPokemonPaletteAron = { sFollowerPalAron, 0xF164 };
static const struct SpriteFrameImage sFollowerPicTableAron[] =
{
    overworld_frame(sFollowerPicAron, 4, 4, 0),
    overworld_frame(sFollowerPicAron, 4, 4, 1),
    overworld_frame(sFollowerPicAron, 4, 4, 2),
    overworld_frame(sFollowerPicAron, 4, 4, 3),
    overworld_frame(sFollowerPicAron, 4, 4, 4),
    overworld_frame(sFollowerPicAron, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoAron =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF164,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableAron,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicLairon[] = INCBIN_U32("mods/following_pokemon/graphics/followers/lairon.4bpp");
static const u16 sFollowerPalLairon[] = INCBIN_U16("mods/following_pokemon/graphics/followers/lairon.gbapal");
const struct SpritePalette gFollowingPokemonPaletteLairon = { sFollowerPalLairon, 0xF165 };
static const struct SpriteFrameImage sFollowerPicTableLairon[] =
{
    overworld_frame(sFollowerPicLairon, 4, 4, 0),
    overworld_frame(sFollowerPicLairon, 4, 4, 1),
    overworld_frame(sFollowerPicLairon, 4, 4, 2),
    overworld_frame(sFollowerPicLairon, 4, 4, 3),
    overworld_frame(sFollowerPicLairon, 4, 4, 4),
    overworld_frame(sFollowerPicLairon, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoLairon =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF165,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableLairon,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicAggron[] = INCBIN_U32("mods/following_pokemon/graphics/followers/aggron.4bpp");
static const u16 sFollowerPalAggron[] = INCBIN_U16("mods/following_pokemon/graphics/followers/aggron.gbapal");
const struct SpritePalette gFollowingPokemonPaletteAggron = { sFollowerPalAggron, 0xF166 };
static const struct SpriteFrameImage sFollowerPicTableAggron[] =
{
    overworld_frame(sFollowerPicAggron, 4, 4, 0),
    overworld_frame(sFollowerPicAggron, 4, 4, 1),
    overworld_frame(sFollowerPicAggron, 4, 4, 2),
    overworld_frame(sFollowerPicAggron, 4, 4, 3),
    overworld_frame(sFollowerPicAggron, 4, 4, 4),
    overworld_frame(sFollowerPicAggron, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoAggron =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF166,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableAggron,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicCastform[] = INCBIN_U32("mods/following_pokemon/graphics/followers/castform.4bpp");
static const u16 sFollowerPalCastform[] = INCBIN_U16("mods/following_pokemon/graphics/followers/castform.gbapal");
const struct SpritePalette gFollowingPokemonPaletteCastform = { sFollowerPalCastform, 0xF167 };
static const struct SpriteFrameImage sFollowerPicTableCastform[] =
{
    overworld_frame(sFollowerPicCastform, 4, 4, 0),
    overworld_frame(sFollowerPicCastform, 4, 4, 1),
    overworld_frame(sFollowerPicCastform, 4, 4, 2),
    overworld_frame(sFollowerPicCastform, 4, 4, 3),
    overworld_frame(sFollowerPicCastform, 4, 4, 4),
    overworld_frame(sFollowerPicCastform, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoCastform =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF167,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableCastform,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicVolbeat[] = INCBIN_U32("mods/following_pokemon/graphics/followers/volbeat.4bpp");
static const u16 sFollowerPalVolbeat[] = INCBIN_U16("mods/following_pokemon/graphics/followers/volbeat.gbapal");
const struct SpritePalette gFollowingPokemonPaletteVolbeat = { sFollowerPalVolbeat, 0xF168 };
static const struct SpriteFrameImage sFollowerPicTableVolbeat[] =
{
    overworld_frame(sFollowerPicVolbeat, 4, 4, 0),
    overworld_frame(sFollowerPicVolbeat, 4, 4, 1),
    overworld_frame(sFollowerPicVolbeat, 4, 4, 2),
    overworld_frame(sFollowerPicVolbeat, 4, 4, 3),
    overworld_frame(sFollowerPicVolbeat, 4, 4, 4),
    overworld_frame(sFollowerPicVolbeat, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoVolbeat =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF168,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableVolbeat,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicIllumise[] = INCBIN_U32("mods/following_pokemon/graphics/followers/illumise.4bpp");
static const u16 sFollowerPalIllumise[] = INCBIN_U16("mods/following_pokemon/graphics/followers/illumise.gbapal");
const struct SpritePalette gFollowingPokemonPaletteIllumise = { sFollowerPalIllumise, 0xF169 };
static const struct SpriteFrameImage sFollowerPicTableIllumise[] =
{
    overworld_frame(sFollowerPicIllumise, 4, 4, 0),
    overworld_frame(sFollowerPicIllumise, 4, 4, 1),
    overworld_frame(sFollowerPicIllumise, 4, 4, 2),
    overworld_frame(sFollowerPicIllumise, 4, 4, 3),
    overworld_frame(sFollowerPicIllumise, 4, 4, 4),
    overworld_frame(sFollowerPicIllumise, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoIllumise =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF169,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableIllumise,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicLileep[] = INCBIN_U32("mods/following_pokemon/graphics/followers/lileep.4bpp");
static const u16 sFollowerPalLileep[] = INCBIN_U16("mods/following_pokemon/graphics/followers/lileep.gbapal");
const struct SpritePalette gFollowingPokemonPaletteLileep = { sFollowerPalLileep, 0xF16A };
static const struct SpriteFrameImage sFollowerPicTableLileep[] =
{
    overworld_frame(sFollowerPicLileep, 4, 4, 0),
    overworld_frame(sFollowerPicLileep, 4, 4, 1),
    overworld_frame(sFollowerPicLileep, 4, 4, 2),
    overworld_frame(sFollowerPicLileep, 4, 4, 3),
    overworld_frame(sFollowerPicLileep, 4, 4, 4),
    overworld_frame(sFollowerPicLileep, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoLileep =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF16A,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableLileep,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicCradily[] = INCBIN_U32("mods/following_pokemon/graphics/followers/cradily.4bpp");
static const u16 sFollowerPalCradily[] = INCBIN_U16("mods/following_pokemon/graphics/followers/cradily.gbapal");
const struct SpritePalette gFollowingPokemonPaletteCradily = { sFollowerPalCradily, 0xF16B };
static const struct SpriteFrameImage sFollowerPicTableCradily[] =
{
    overworld_frame(sFollowerPicCradily, 4, 4, 0),
    overworld_frame(sFollowerPicCradily, 4, 4, 1),
    overworld_frame(sFollowerPicCradily, 4, 4, 2),
    overworld_frame(sFollowerPicCradily, 4, 4, 3),
    overworld_frame(sFollowerPicCradily, 4, 4, 4),
    overworld_frame(sFollowerPicCradily, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoCradily =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF16B,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableCradily,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicAnorith[] = INCBIN_U32("mods/following_pokemon/graphics/followers/anorith.4bpp");
static const u16 sFollowerPalAnorith[] = INCBIN_U16("mods/following_pokemon/graphics/followers/anorith.gbapal");
const struct SpritePalette gFollowingPokemonPaletteAnorith = { sFollowerPalAnorith, 0xF16C };
static const struct SpriteFrameImage sFollowerPicTableAnorith[] =
{
    overworld_frame(sFollowerPicAnorith, 4, 4, 0),
    overworld_frame(sFollowerPicAnorith, 4, 4, 1),
    overworld_frame(sFollowerPicAnorith, 4, 4, 2),
    overworld_frame(sFollowerPicAnorith, 4, 4, 3),
    overworld_frame(sFollowerPicAnorith, 4, 4, 4),
    overworld_frame(sFollowerPicAnorith, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoAnorith =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF16C,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableAnorith,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicArmaldo[] = INCBIN_U32("mods/following_pokemon/graphics/followers/armaldo.4bpp");
static const u16 sFollowerPalArmaldo[] = INCBIN_U16("mods/following_pokemon/graphics/followers/armaldo.gbapal");
const struct SpritePalette gFollowingPokemonPaletteArmaldo = { sFollowerPalArmaldo, 0xF16D };
static const struct SpriteFrameImage sFollowerPicTableArmaldo[] =
{
    overworld_frame(sFollowerPicArmaldo, 4, 4, 0),
    overworld_frame(sFollowerPicArmaldo, 4, 4, 1),
    overworld_frame(sFollowerPicArmaldo, 4, 4, 2),
    overworld_frame(sFollowerPicArmaldo, 4, 4, 3),
    overworld_frame(sFollowerPicArmaldo, 4, 4, 4),
    overworld_frame(sFollowerPicArmaldo, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoArmaldo =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF16D,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableArmaldo,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicRalts[] = INCBIN_U32("mods/following_pokemon/graphics/followers/ralts.4bpp");
static const u16 sFollowerPalRalts[] = INCBIN_U16("mods/following_pokemon/graphics/followers/ralts.gbapal");
const struct SpritePalette gFollowingPokemonPaletteRalts = { sFollowerPalRalts, 0xF16E };
static const struct SpriteFrameImage sFollowerPicTableRalts[] =
{
    overworld_frame(sFollowerPicRalts, 4, 4, 0),
    overworld_frame(sFollowerPicRalts, 4, 4, 1),
    overworld_frame(sFollowerPicRalts, 4, 4, 2),
    overworld_frame(sFollowerPicRalts, 4, 4, 3),
    overworld_frame(sFollowerPicRalts, 4, 4, 4),
    overworld_frame(sFollowerPicRalts, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoRalts =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF16E,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableRalts,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicKirlia[] = INCBIN_U32("mods/following_pokemon/graphics/followers/kirlia.4bpp");
static const u16 sFollowerPalKirlia[] = INCBIN_U16("mods/following_pokemon/graphics/followers/kirlia.gbapal");
const struct SpritePalette gFollowingPokemonPaletteKirlia = { sFollowerPalKirlia, 0xF16F };
static const struct SpriteFrameImage sFollowerPicTableKirlia[] =
{
    overworld_frame(sFollowerPicKirlia, 4, 4, 0),
    overworld_frame(sFollowerPicKirlia, 4, 4, 1),
    overworld_frame(sFollowerPicKirlia, 4, 4, 2),
    overworld_frame(sFollowerPicKirlia, 4, 4, 3),
    overworld_frame(sFollowerPicKirlia, 4, 4, 4),
    overworld_frame(sFollowerPicKirlia, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoKirlia =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF16F,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableKirlia,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicGardevoir[] = INCBIN_U32("mods/following_pokemon/graphics/followers/gardevoir.4bpp");
static const u16 sFollowerPalGardevoir[] = INCBIN_U16("mods/following_pokemon/graphics/followers/gardevoir.gbapal");
const struct SpritePalette gFollowingPokemonPaletteGardevoir = { sFollowerPalGardevoir, 0xF170 };
static const struct SpriteFrameImage sFollowerPicTableGardevoir[] =
{
    overworld_frame(sFollowerPicGardevoir, 4, 4, 0),
    overworld_frame(sFollowerPicGardevoir, 4, 4, 1),
    overworld_frame(sFollowerPicGardevoir, 4, 4, 2),
    overworld_frame(sFollowerPicGardevoir, 4, 4, 3),
    overworld_frame(sFollowerPicGardevoir, 4, 4, 4),
    overworld_frame(sFollowerPicGardevoir, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoGardevoir =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF170,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableGardevoir,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicBagon[] = INCBIN_U32("mods/following_pokemon/graphics/followers/bagon.4bpp");
static const u16 sFollowerPalBagon[] = INCBIN_U16("mods/following_pokemon/graphics/followers/bagon.gbapal");
const struct SpritePalette gFollowingPokemonPaletteBagon = { sFollowerPalBagon, 0xF171 };
static const struct SpriteFrameImage sFollowerPicTableBagon[] =
{
    overworld_frame(sFollowerPicBagon, 4, 4, 0),
    overworld_frame(sFollowerPicBagon, 4, 4, 1),
    overworld_frame(sFollowerPicBagon, 4, 4, 2),
    overworld_frame(sFollowerPicBagon, 4, 4, 3),
    overworld_frame(sFollowerPicBagon, 4, 4, 4),
    overworld_frame(sFollowerPicBagon, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoBagon =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF171,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableBagon,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicShelgon[] = INCBIN_U32("mods/following_pokemon/graphics/followers/shelgon.4bpp");
static const u16 sFollowerPalShelgon[] = INCBIN_U16("mods/following_pokemon/graphics/followers/shelgon.gbapal");
const struct SpritePalette gFollowingPokemonPaletteShelgon = { sFollowerPalShelgon, 0xF172 };
static const struct SpriteFrameImage sFollowerPicTableShelgon[] =
{
    overworld_frame(sFollowerPicShelgon, 4, 4, 0),
    overworld_frame(sFollowerPicShelgon, 4, 4, 1),
    overworld_frame(sFollowerPicShelgon, 4, 4, 2),
    overworld_frame(sFollowerPicShelgon, 4, 4, 3),
    overworld_frame(sFollowerPicShelgon, 4, 4, 4),
    overworld_frame(sFollowerPicShelgon, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoShelgon =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF172,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableShelgon,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicSalamence[] = INCBIN_U32("mods/following_pokemon/graphics/followers/salamence.4bpp");
static const u16 sFollowerPalSalamence[] = INCBIN_U16("mods/following_pokemon/graphics/followers/salamence.gbapal");
const struct SpritePalette gFollowingPokemonPaletteSalamence = { sFollowerPalSalamence, 0xF173 };
static const struct SpriteFrameImage sFollowerPicTableSalamence[] =
{
    overworld_frame(sFollowerPicSalamence, 4, 4, 0),
    overworld_frame(sFollowerPicSalamence, 4, 4, 1),
    overworld_frame(sFollowerPicSalamence, 4, 4, 2),
    overworld_frame(sFollowerPicSalamence, 4, 4, 3),
    overworld_frame(sFollowerPicSalamence, 4, 4, 4),
    overworld_frame(sFollowerPicSalamence, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoSalamence =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF173,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableSalamence,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicBeldum[] = INCBIN_U32("mods/following_pokemon/graphics/followers/beldum.4bpp");
static const u16 sFollowerPalBeldum[] = INCBIN_U16("mods/following_pokemon/graphics/followers/beldum.gbapal");
const struct SpritePalette gFollowingPokemonPaletteBeldum = { sFollowerPalBeldum, 0xF174 };
static const struct SpriteFrameImage sFollowerPicTableBeldum[] =
{
    overworld_frame(sFollowerPicBeldum, 4, 4, 0),
    overworld_frame(sFollowerPicBeldum, 4, 4, 1),
    overworld_frame(sFollowerPicBeldum, 4, 4, 2),
    overworld_frame(sFollowerPicBeldum, 4, 4, 3),
    overworld_frame(sFollowerPicBeldum, 4, 4, 4),
    overworld_frame(sFollowerPicBeldum, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoBeldum =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF174,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableBeldum,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMetang[] = INCBIN_U32("mods/following_pokemon/graphics/followers/metang.4bpp");
static const u16 sFollowerPalMetang[] = INCBIN_U16("mods/following_pokemon/graphics/followers/metang.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMetang = { sFollowerPalMetang, 0xF175 };
static const struct SpriteFrameImage sFollowerPicTableMetang[] =
{
    overworld_frame(sFollowerPicMetang, 4, 4, 0),
    overworld_frame(sFollowerPicMetang, 4, 4, 1),
    overworld_frame(sFollowerPicMetang, 4, 4, 2),
    overworld_frame(sFollowerPicMetang, 4, 4, 3),
    overworld_frame(sFollowerPicMetang, 4, 4, 4),
    overworld_frame(sFollowerPicMetang, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMetang =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF175,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMetang,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicMetagross[] = INCBIN_U32("mods/following_pokemon/graphics/followers/metagross.4bpp");
static const u16 sFollowerPalMetagross[] = INCBIN_U16("mods/following_pokemon/graphics/followers/metagross.gbapal");
const struct SpritePalette gFollowingPokemonPaletteMetagross = { sFollowerPalMetagross, 0xF176 };
static const struct SpriteFrameImage sFollowerPicTableMetagross[] =
{
    overworld_frame(sFollowerPicMetagross, 4, 4, 0),
    overworld_frame(sFollowerPicMetagross, 4, 4, 1),
    overworld_frame(sFollowerPicMetagross, 4, 4, 2),
    overworld_frame(sFollowerPicMetagross, 4, 4, 3),
    overworld_frame(sFollowerPicMetagross, 4, 4, 4),
    overworld_frame(sFollowerPicMetagross, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoMetagross =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF176,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableMetagross,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicRegirock[] = INCBIN_U32("mods/following_pokemon/graphics/followers/regirock.4bpp");
static const u16 sFollowerPalRegirock[] = INCBIN_U16("mods/following_pokemon/graphics/followers/regirock.gbapal");
const struct SpritePalette gFollowingPokemonPaletteRegirock = { sFollowerPalRegirock, 0xF177 };
static const struct SpriteFrameImage sFollowerPicTableRegirock[] =
{
    overworld_frame(sFollowerPicRegirock, 4, 4, 0),
    overworld_frame(sFollowerPicRegirock, 4, 4, 1),
    overworld_frame(sFollowerPicRegirock, 4, 4, 2),
    overworld_frame(sFollowerPicRegirock, 4, 4, 3),
    overworld_frame(sFollowerPicRegirock, 4, 4, 4),
    overworld_frame(sFollowerPicRegirock, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoRegirock =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF177,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableRegirock,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicRegice[] = INCBIN_U32("mods/following_pokemon/graphics/followers/regice.4bpp");
static const u16 sFollowerPalRegice[] = INCBIN_U16("mods/following_pokemon/graphics/followers/regice.gbapal");
const struct SpritePalette gFollowingPokemonPaletteRegice = { sFollowerPalRegice, 0xF178 };
static const struct SpriteFrameImage sFollowerPicTableRegice[] =
{
    overworld_frame(sFollowerPicRegice, 4, 4, 0),
    overworld_frame(sFollowerPicRegice, 4, 4, 1),
    overworld_frame(sFollowerPicRegice, 4, 4, 2),
    overworld_frame(sFollowerPicRegice, 4, 4, 3),
    overworld_frame(sFollowerPicRegice, 4, 4, 4),
    overworld_frame(sFollowerPicRegice, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoRegice =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF178,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableRegice,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicRegisteel[] = INCBIN_U32("mods/following_pokemon/graphics/followers/registeel.4bpp");
static const u16 sFollowerPalRegisteel[] = INCBIN_U16("mods/following_pokemon/graphics/followers/registeel.gbapal");
const struct SpritePalette gFollowingPokemonPaletteRegisteel = { sFollowerPalRegisteel, 0xF179 };
static const struct SpriteFrameImage sFollowerPicTableRegisteel[] =
{
    overworld_frame(sFollowerPicRegisteel, 4, 4, 0),
    overworld_frame(sFollowerPicRegisteel, 4, 4, 1),
    overworld_frame(sFollowerPicRegisteel, 4, 4, 2),
    overworld_frame(sFollowerPicRegisteel, 4, 4, 3),
    overworld_frame(sFollowerPicRegisteel, 4, 4, 4),
    overworld_frame(sFollowerPicRegisteel, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoRegisteel =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF179,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableRegisteel,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicKyogre[] = INCBIN_U32("mods/following_pokemon/graphics/followers/kyogre.4bpp");
static const u16 sFollowerPalKyogre[] = INCBIN_U16("mods/following_pokemon/graphics/followers/kyogre.gbapal");
const struct SpritePalette gFollowingPokemonPaletteKyogre = { sFollowerPalKyogre, 0xF17A };
static const struct SpriteFrameImage sFollowerPicTableKyogre[] =
{
    overworld_frame(sFollowerPicKyogre, 4, 4, 0),
    overworld_frame(sFollowerPicKyogre, 4, 4, 1),
    overworld_frame(sFollowerPicKyogre, 4, 4, 2),
    overworld_frame(sFollowerPicKyogre, 4, 4, 3),
    overworld_frame(sFollowerPicKyogre, 4, 4, 4),
    overworld_frame(sFollowerPicKyogre, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoKyogre =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF17A,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_L,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableKyogre,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicGroudon[] = INCBIN_U32("mods/following_pokemon/graphics/followers/groudon.4bpp");
static const u16 sFollowerPalGroudon[] = INCBIN_U16("mods/following_pokemon/graphics/followers/groudon.gbapal");
const struct SpritePalette gFollowingPokemonPaletteGroudon = { sFollowerPalGroudon, 0xF17B };
static const struct SpriteFrameImage sFollowerPicTableGroudon[] =
{
    overworld_frame(sFollowerPicGroudon, 4, 4, 0),
    overworld_frame(sFollowerPicGroudon, 4, 4, 1),
    overworld_frame(sFollowerPicGroudon, 4, 4, 2),
    overworld_frame(sFollowerPicGroudon, 4, 4, 3),
    overworld_frame(sFollowerPicGroudon, 4, 4, 4),
    overworld_frame(sFollowerPicGroudon, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoGroudon =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF17B,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_L,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableGroudon,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicRayquaza[] = INCBIN_U32("mods/following_pokemon/graphics/followers/rayquaza.4bpp");
static const u16 sFollowerPalRayquaza[] = INCBIN_U16("mods/following_pokemon/graphics/followers/rayquaza.gbapal");
const struct SpritePalette gFollowingPokemonPaletteRayquaza = { sFollowerPalRayquaza, 0xF17C };
static const struct SpriteFrameImage sFollowerPicTableRayquaza[] =
{
    overworld_frame(sFollowerPicRayquaza, 4, 4, 0),
    overworld_frame(sFollowerPicRayquaza, 4, 4, 1),
    overworld_frame(sFollowerPicRayquaza, 4, 4, 2),
    overworld_frame(sFollowerPicRayquaza, 4, 4, 3),
    overworld_frame(sFollowerPicRayquaza, 4, 4, 4),
    overworld_frame(sFollowerPicRayquaza, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoRayquaza =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF17C,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_L,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableRayquaza,
    .images = sFollowerPicTableRayquaza,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicLatias[] = INCBIN_U32("mods/following_pokemon/graphics/followers/latias.4bpp");
static const u16 sFollowerPalLatias[] = INCBIN_U16("mods/following_pokemon/graphics/followers/latias.gbapal");
const struct SpritePalette gFollowingPokemonPaletteLatias = { sFollowerPalLatias, 0xF17D };
static const struct SpriteFrameImage sFollowerPicTableLatias[] =
{
    overworld_frame(sFollowerPicLatias, 4, 4, 0),
    overworld_frame(sFollowerPicLatias, 4, 4, 1),
    overworld_frame(sFollowerPicLatias, 4, 4, 2),
    overworld_frame(sFollowerPicLatias, 4, 4, 3),
    overworld_frame(sFollowerPicLatias, 4, 4, 4),
    overworld_frame(sFollowerPicLatias, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoLatias =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF17D,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableLatias,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicLatios[] = INCBIN_U32("mods/following_pokemon/graphics/followers/latios.4bpp");
static const u16 sFollowerPalLatios[] = INCBIN_U16("mods/following_pokemon/graphics/followers/latios.gbapal");
const struct SpritePalette gFollowingPokemonPaletteLatios = { sFollowerPalLatios, 0xF17E };
static const struct SpriteFrameImage sFollowerPicTableLatios[] =
{
    overworld_frame(sFollowerPicLatios, 4, 4, 0),
    overworld_frame(sFollowerPicLatios, 4, 4, 1),
    overworld_frame(sFollowerPicLatios, 4, 4, 2),
    overworld_frame(sFollowerPicLatios, 4, 4, 3),
    overworld_frame(sFollowerPicLatios, 4, 4, 4),
    overworld_frame(sFollowerPicLatios, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoLatios =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF17E,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableLatios,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicJirachi[] = INCBIN_U32("mods/following_pokemon/graphics/followers/jirachi.4bpp");
static const u16 sFollowerPalJirachi[] = INCBIN_U16("mods/following_pokemon/graphics/followers/jirachi.gbapal");
const struct SpritePalette gFollowingPokemonPaletteJirachi = { sFollowerPalJirachi, 0xF17F };
static const struct SpriteFrameImage sFollowerPicTableJirachi[] =
{
    overworld_frame(sFollowerPicJirachi, 4, 4, 0),
    overworld_frame(sFollowerPicJirachi, 4, 4, 1),
    overworld_frame(sFollowerPicJirachi, 4, 4, 2),
    overworld_frame(sFollowerPicJirachi, 4, 4, 3),
    overworld_frame(sFollowerPicJirachi, 4, 4, 4),
    overworld_frame(sFollowerPicJirachi, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoJirachi =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF17F,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableJirachi,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicDeoxys[] = INCBIN_U32("mods/following_pokemon/graphics/followers/deoxys.4bpp");
static const u16 sFollowerPalDeoxys[] = INCBIN_U16("mods/following_pokemon/graphics/followers/deoxys.gbapal");
const struct SpritePalette gFollowingPokemonPaletteDeoxys = { sFollowerPalDeoxys, 0xF180 };
static const struct SpriteFrameImage sFollowerPicTableDeoxys[] =
{
    overworld_frame(sFollowerPicDeoxys, 4, 4, 0),
    overworld_frame(sFollowerPicDeoxys, 4, 4, 1),
    overworld_frame(sFollowerPicDeoxys, 4, 4, 2),
    overworld_frame(sFollowerPicDeoxys, 4, 4, 3),
    overworld_frame(sFollowerPicDeoxys, 4, 4, 4),
    overworld_frame(sFollowerPicDeoxys, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoDeoxys =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF180,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableDeoxys,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicChimecho[] = INCBIN_U32("mods/following_pokemon/graphics/followers/chimecho.4bpp");
static const u16 sFollowerPalChimecho[] = INCBIN_U16("mods/following_pokemon/graphics/followers/chimecho.gbapal");
const struct SpritePalette gFollowingPokemonPaletteChimecho = { sFollowerPalChimecho, 0xF181 };
static const struct SpriteFrameImage sFollowerPicTableChimecho[] =
{
    overworld_frame(sFollowerPicChimecho, 4, 4, 0),
    overworld_frame(sFollowerPicChimecho, 4, 4, 1),
    overworld_frame(sFollowerPicChimecho, 4, 4, 2),
    overworld_frame(sFollowerPicChimecho, 4, 4, 3),
    overworld_frame(sFollowerPicChimecho, 4, 4, 4),
    overworld_frame(sFollowerPicChimecho, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoChimecho =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF181,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableChimecho,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicUnownB[] = INCBIN_U32("mods/following_pokemon/graphics/followers/unown_b.4bpp");
static const u16 sFollowerPalUnownB[] = INCBIN_U16("mods/following_pokemon/graphics/followers/unown_b.gbapal");
const struct SpritePalette gFollowingPokemonPaletteUnownB = { sFollowerPalUnownB, 0xF182 };
static const struct SpriteFrameImage sFollowerPicTableUnownB[] =
{
    overworld_frame(sFollowerPicUnownB, 4, 4, 0),
    overworld_frame(sFollowerPicUnownB, 4, 4, 1),
    overworld_frame(sFollowerPicUnownB, 4, 4, 2),
    overworld_frame(sFollowerPicUnownB, 4, 4, 3),
    overworld_frame(sFollowerPicUnownB, 4, 4, 4),
    overworld_frame(sFollowerPicUnownB, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoUnownB =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF182,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableUnownB,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicUnownC[] = INCBIN_U32("mods/following_pokemon/graphics/followers/unown_c.4bpp");
static const u16 sFollowerPalUnownC[] = INCBIN_U16("mods/following_pokemon/graphics/followers/unown_c.gbapal");
const struct SpritePalette gFollowingPokemonPaletteUnownC = { sFollowerPalUnownC, 0xF183 };
static const struct SpriteFrameImage sFollowerPicTableUnownC[] =
{
    overworld_frame(sFollowerPicUnownC, 4, 4, 0),
    overworld_frame(sFollowerPicUnownC, 4, 4, 1),
    overworld_frame(sFollowerPicUnownC, 4, 4, 2),
    overworld_frame(sFollowerPicUnownC, 4, 4, 3),
    overworld_frame(sFollowerPicUnownC, 4, 4, 4),
    overworld_frame(sFollowerPicUnownC, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoUnownC =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF183,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableUnownC,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicUnownD[] = INCBIN_U32("mods/following_pokemon/graphics/followers/unown_d.4bpp");
static const u16 sFollowerPalUnownD[] = INCBIN_U16("mods/following_pokemon/graphics/followers/unown_d.gbapal");
const struct SpritePalette gFollowingPokemonPaletteUnownD = { sFollowerPalUnownD, 0xF184 };
static const struct SpriteFrameImage sFollowerPicTableUnownD[] =
{
    overworld_frame(sFollowerPicUnownD, 4, 4, 0),
    overworld_frame(sFollowerPicUnownD, 4, 4, 1),
    overworld_frame(sFollowerPicUnownD, 4, 4, 2),
    overworld_frame(sFollowerPicUnownD, 4, 4, 3),
    overworld_frame(sFollowerPicUnownD, 4, 4, 4),
    overworld_frame(sFollowerPicUnownD, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoUnownD =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF184,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableUnownD,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicUnownE[] = INCBIN_U32("mods/following_pokemon/graphics/followers/unown_e.4bpp");
static const u16 sFollowerPalUnownE[] = INCBIN_U16("mods/following_pokemon/graphics/followers/unown_e.gbapal");
const struct SpritePalette gFollowingPokemonPaletteUnownE = { sFollowerPalUnownE, 0xF185 };
static const struct SpriteFrameImage sFollowerPicTableUnownE[] =
{
    overworld_frame(sFollowerPicUnownE, 4, 4, 0),
    overworld_frame(sFollowerPicUnownE, 4, 4, 1),
    overworld_frame(sFollowerPicUnownE, 4, 4, 2),
    overworld_frame(sFollowerPicUnownE, 4, 4, 3),
    overworld_frame(sFollowerPicUnownE, 4, 4, 4),
    overworld_frame(sFollowerPicUnownE, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoUnownE =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF185,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableUnownE,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicUnownF[] = INCBIN_U32("mods/following_pokemon/graphics/followers/unown_f.4bpp");
static const u16 sFollowerPalUnownF[] = INCBIN_U16("mods/following_pokemon/graphics/followers/unown_f.gbapal");
const struct SpritePalette gFollowingPokemonPaletteUnownF = { sFollowerPalUnownF, 0xF186 };
static const struct SpriteFrameImage sFollowerPicTableUnownF[] =
{
    overworld_frame(sFollowerPicUnownF, 4, 4, 0),
    overworld_frame(sFollowerPicUnownF, 4, 4, 1),
    overworld_frame(sFollowerPicUnownF, 4, 4, 2),
    overworld_frame(sFollowerPicUnownF, 4, 4, 3),
    overworld_frame(sFollowerPicUnownF, 4, 4, 4),
    overworld_frame(sFollowerPicUnownF, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoUnownF =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF186,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableUnownF,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicUnownG[] = INCBIN_U32("mods/following_pokemon/graphics/followers/unown_g.4bpp");
static const u16 sFollowerPalUnownG[] = INCBIN_U16("mods/following_pokemon/graphics/followers/unown_g.gbapal");
const struct SpritePalette gFollowingPokemonPaletteUnownG = { sFollowerPalUnownG, 0xF187 };
static const struct SpriteFrameImage sFollowerPicTableUnownG[] =
{
    overworld_frame(sFollowerPicUnownG, 4, 4, 0),
    overworld_frame(sFollowerPicUnownG, 4, 4, 1),
    overworld_frame(sFollowerPicUnownG, 4, 4, 2),
    overworld_frame(sFollowerPicUnownG, 4, 4, 3),
    overworld_frame(sFollowerPicUnownG, 4, 4, 4),
    overworld_frame(sFollowerPicUnownG, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoUnownG =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF187,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableUnownG,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicUnownH[] = INCBIN_U32("mods/following_pokemon/graphics/followers/unown_h.4bpp");
static const u16 sFollowerPalUnownH[] = INCBIN_U16("mods/following_pokemon/graphics/followers/unown_h.gbapal");
const struct SpritePalette gFollowingPokemonPaletteUnownH = { sFollowerPalUnownH, 0xF188 };
static const struct SpriteFrameImage sFollowerPicTableUnownH[] =
{
    overworld_frame(sFollowerPicUnownH, 4, 4, 0),
    overworld_frame(sFollowerPicUnownH, 4, 4, 1),
    overworld_frame(sFollowerPicUnownH, 4, 4, 2),
    overworld_frame(sFollowerPicUnownH, 4, 4, 3),
    overworld_frame(sFollowerPicUnownH, 4, 4, 4),
    overworld_frame(sFollowerPicUnownH, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoUnownH =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF188,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableUnownH,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicUnownI[] = INCBIN_U32("mods/following_pokemon/graphics/followers/unown_i.4bpp");
static const u16 sFollowerPalUnownI[] = INCBIN_U16("mods/following_pokemon/graphics/followers/unown_i.gbapal");
const struct SpritePalette gFollowingPokemonPaletteUnownI = { sFollowerPalUnownI, 0xF189 };
static const struct SpriteFrameImage sFollowerPicTableUnownI[] =
{
    overworld_frame(sFollowerPicUnownI, 4, 4, 0),
    overworld_frame(sFollowerPicUnownI, 4, 4, 1),
    overworld_frame(sFollowerPicUnownI, 4, 4, 2),
    overworld_frame(sFollowerPicUnownI, 4, 4, 3),
    overworld_frame(sFollowerPicUnownI, 4, 4, 4),
    overworld_frame(sFollowerPicUnownI, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoUnownI =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF189,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableUnownI,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicUnownJ[] = INCBIN_U32("mods/following_pokemon/graphics/followers/unown_j.4bpp");
static const u16 sFollowerPalUnownJ[] = INCBIN_U16("mods/following_pokemon/graphics/followers/unown_j.gbapal");
const struct SpritePalette gFollowingPokemonPaletteUnownJ = { sFollowerPalUnownJ, 0xF18A };
static const struct SpriteFrameImage sFollowerPicTableUnownJ[] =
{
    overworld_frame(sFollowerPicUnownJ, 4, 4, 0),
    overworld_frame(sFollowerPicUnownJ, 4, 4, 1),
    overworld_frame(sFollowerPicUnownJ, 4, 4, 2),
    overworld_frame(sFollowerPicUnownJ, 4, 4, 3),
    overworld_frame(sFollowerPicUnownJ, 4, 4, 4),
    overworld_frame(sFollowerPicUnownJ, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoUnownJ =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF18A,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableUnownJ,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicUnownK[] = INCBIN_U32("mods/following_pokemon/graphics/followers/unown_k.4bpp");
static const u16 sFollowerPalUnownK[] = INCBIN_U16("mods/following_pokemon/graphics/followers/unown_k.gbapal");
const struct SpritePalette gFollowingPokemonPaletteUnownK = { sFollowerPalUnownK, 0xF18B };
static const struct SpriteFrameImage sFollowerPicTableUnownK[] =
{
    overworld_frame(sFollowerPicUnownK, 4, 4, 0),
    overworld_frame(sFollowerPicUnownK, 4, 4, 1),
    overworld_frame(sFollowerPicUnownK, 4, 4, 2),
    overworld_frame(sFollowerPicUnownK, 4, 4, 3),
    overworld_frame(sFollowerPicUnownK, 4, 4, 4),
    overworld_frame(sFollowerPicUnownK, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoUnownK =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF18B,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableUnownK,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicUnownL[] = INCBIN_U32("mods/following_pokemon/graphics/followers/unown_l.4bpp");
static const u16 sFollowerPalUnownL[] = INCBIN_U16("mods/following_pokemon/graphics/followers/unown_l.gbapal");
const struct SpritePalette gFollowingPokemonPaletteUnownL = { sFollowerPalUnownL, 0xF18C };
static const struct SpriteFrameImage sFollowerPicTableUnownL[] =
{
    overworld_frame(sFollowerPicUnownL, 4, 4, 0),
    overworld_frame(sFollowerPicUnownL, 4, 4, 1),
    overworld_frame(sFollowerPicUnownL, 4, 4, 2),
    overworld_frame(sFollowerPicUnownL, 4, 4, 3),
    overworld_frame(sFollowerPicUnownL, 4, 4, 4),
    overworld_frame(sFollowerPicUnownL, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoUnownL =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF18C,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableUnownL,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicUnownM[] = INCBIN_U32("mods/following_pokemon/graphics/followers/unown_m.4bpp");
static const u16 sFollowerPalUnownM[] = INCBIN_U16("mods/following_pokemon/graphics/followers/unown_m.gbapal");
const struct SpritePalette gFollowingPokemonPaletteUnownM = { sFollowerPalUnownM, 0xF18D };
static const struct SpriteFrameImage sFollowerPicTableUnownM[] =
{
    overworld_frame(sFollowerPicUnownM, 4, 4, 0),
    overworld_frame(sFollowerPicUnownM, 4, 4, 1),
    overworld_frame(sFollowerPicUnownM, 4, 4, 2),
    overworld_frame(sFollowerPicUnownM, 4, 4, 3),
    overworld_frame(sFollowerPicUnownM, 4, 4, 4),
    overworld_frame(sFollowerPicUnownM, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoUnownM =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF18D,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableUnownM,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicUnownN[] = INCBIN_U32("mods/following_pokemon/graphics/followers/unown_n.4bpp");
static const u16 sFollowerPalUnownN[] = INCBIN_U16("mods/following_pokemon/graphics/followers/unown_n.gbapal");
const struct SpritePalette gFollowingPokemonPaletteUnownN = { sFollowerPalUnownN, 0xF18E };
static const struct SpriteFrameImage sFollowerPicTableUnownN[] =
{
    overworld_frame(sFollowerPicUnownN, 4, 4, 0),
    overworld_frame(sFollowerPicUnownN, 4, 4, 1),
    overworld_frame(sFollowerPicUnownN, 4, 4, 2),
    overworld_frame(sFollowerPicUnownN, 4, 4, 3),
    overworld_frame(sFollowerPicUnownN, 4, 4, 4),
    overworld_frame(sFollowerPicUnownN, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoUnownN =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF18E,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableUnownN,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicUnownO[] = INCBIN_U32("mods/following_pokemon/graphics/followers/unown_o.4bpp");
static const u16 sFollowerPalUnownO[] = INCBIN_U16("mods/following_pokemon/graphics/followers/unown_o.gbapal");
const struct SpritePalette gFollowingPokemonPaletteUnownO = { sFollowerPalUnownO, 0xF18F };
static const struct SpriteFrameImage sFollowerPicTableUnownO[] =
{
    overworld_frame(sFollowerPicUnownO, 4, 4, 0),
    overworld_frame(sFollowerPicUnownO, 4, 4, 1),
    overworld_frame(sFollowerPicUnownO, 4, 4, 2),
    overworld_frame(sFollowerPicUnownO, 4, 4, 3),
    overworld_frame(sFollowerPicUnownO, 4, 4, 4),
    overworld_frame(sFollowerPicUnownO, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoUnownO =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF18F,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableUnownO,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicUnownP[] = INCBIN_U32("mods/following_pokemon/graphics/followers/unown_p.4bpp");
static const u16 sFollowerPalUnownP[] = INCBIN_U16("mods/following_pokemon/graphics/followers/unown_p.gbapal");
const struct SpritePalette gFollowingPokemonPaletteUnownP = { sFollowerPalUnownP, 0xF190 };
static const struct SpriteFrameImage sFollowerPicTableUnownP[] =
{
    overworld_frame(sFollowerPicUnownP, 4, 4, 0),
    overworld_frame(sFollowerPicUnownP, 4, 4, 1),
    overworld_frame(sFollowerPicUnownP, 4, 4, 2),
    overworld_frame(sFollowerPicUnownP, 4, 4, 3),
    overworld_frame(sFollowerPicUnownP, 4, 4, 4),
    overworld_frame(sFollowerPicUnownP, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoUnownP =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF190,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableUnownP,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicUnownQ[] = INCBIN_U32("mods/following_pokemon/graphics/followers/unown_q.4bpp");
static const u16 sFollowerPalUnownQ[] = INCBIN_U16("mods/following_pokemon/graphics/followers/unown_q.gbapal");
const struct SpritePalette gFollowingPokemonPaletteUnownQ = { sFollowerPalUnownQ, 0xF191 };
static const struct SpriteFrameImage sFollowerPicTableUnownQ[] =
{
    overworld_frame(sFollowerPicUnownQ, 4, 4, 0),
    overworld_frame(sFollowerPicUnownQ, 4, 4, 1),
    overworld_frame(sFollowerPicUnownQ, 4, 4, 2),
    overworld_frame(sFollowerPicUnownQ, 4, 4, 3),
    overworld_frame(sFollowerPicUnownQ, 4, 4, 4),
    overworld_frame(sFollowerPicUnownQ, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoUnownQ =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF191,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableUnownQ,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicUnownR[] = INCBIN_U32("mods/following_pokemon/graphics/followers/unown_r.4bpp");
static const u16 sFollowerPalUnownR[] = INCBIN_U16("mods/following_pokemon/graphics/followers/unown_r.gbapal");
const struct SpritePalette gFollowingPokemonPaletteUnownR = { sFollowerPalUnownR, 0xF192 };
static const struct SpriteFrameImage sFollowerPicTableUnownR[] =
{
    overworld_frame(sFollowerPicUnownR, 4, 4, 0),
    overworld_frame(sFollowerPicUnownR, 4, 4, 1),
    overworld_frame(sFollowerPicUnownR, 4, 4, 2),
    overworld_frame(sFollowerPicUnownR, 4, 4, 3),
    overworld_frame(sFollowerPicUnownR, 4, 4, 4),
    overworld_frame(sFollowerPicUnownR, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoUnownR =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF192,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableUnownR,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicUnownS[] = INCBIN_U32("mods/following_pokemon/graphics/followers/unown_s.4bpp");
static const u16 sFollowerPalUnownS[] = INCBIN_U16("mods/following_pokemon/graphics/followers/unown_s.gbapal");
const struct SpritePalette gFollowingPokemonPaletteUnownS = { sFollowerPalUnownS, 0xF193 };
static const struct SpriteFrameImage sFollowerPicTableUnownS[] =
{
    overworld_frame(sFollowerPicUnownS, 4, 4, 0),
    overworld_frame(sFollowerPicUnownS, 4, 4, 1),
    overworld_frame(sFollowerPicUnownS, 4, 4, 2),
    overworld_frame(sFollowerPicUnownS, 4, 4, 3),
    overworld_frame(sFollowerPicUnownS, 4, 4, 4),
    overworld_frame(sFollowerPicUnownS, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoUnownS =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF193,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableUnownS,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicUnownT[] = INCBIN_U32("mods/following_pokemon/graphics/followers/unown_t.4bpp");
static const u16 sFollowerPalUnownT[] = INCBIN_U16("mods/following_pokemon/graphics/followers/unown_t.gbapal");
const struct SpritePalette gFollowingPokemonPaletteUnownT = { sFollowerPalUnownT, 0xF194 };
static const struct SpriteFrameImage sFollowerPicTableUnownT[] =
{
    overworld_frame(sFollowerPicUnownT, 4, 4, 0),
    overworld_frame(sFollowerPicUnownT, 4, 4, 1),
    overworld_frame(sFollowerPicUnownT, 4, 4, 2),
    overworld_frame(sFollowerPicUnownT, 4, 4, 3),
    overworld_frame(sFollowerPicUnownT, 4, 4, 4),
    overworld_frame(sFollowerPicUnownT, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoUnownT =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF194,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableUnownT,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicUnownU[] = INCBIN_U32("mods/following_pokemon/graphics/followers/unown_u.4bpp");
static const u16 sFollowerPalUnownU[] = INCBIN_U16("mods/following_pokemon/graphics/followers/unown_u.gbapal");
const struct SpritePalette gFollowingPokemonPaletteUnownU = { sFollowerPalUnownU, 0xF195 };
static const struct SpriteFrameImage sFollowerPicTableUnownU[] =
{
    overworld_frame(sFollowerPicUnownU, 4, 4, 0),
    overworld_frame(sFollowerPicUnownU, 4, 4, 1),
    overworld_frame(sFollowerPicUnownU, 4, 4, 2),
    overworld_frame(sFollowerPicUnownU, 4, 4, 3),
    overworld_frame(sFollowerPicUnownU, 4, 4, 4),
    overworld_frame(sFollowerPicUnownU, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoUnownU =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF195,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableUnownU,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicUnownV[] = INCBIN_U32("mods/following_pokemon/graphics/followers/unown_v.4bpp");
static const u16 sFollowerPalUnownV[] = INCBIN_U16("mods/following_pokemon/graphics/followers/unown_v.gbapal");
const struct SpritePalette gFollowingPokemonPaletteUnownV = { sFollowerPalUnownV, 0xF196 };
static const struct SpriteFrameImage sFollowerPicTableUnownV[] =
{
    overworld_frame(sFollowerPicUnownV, 4, 4, 0),
    overworld_frame(sFollowerPicUnownV, 4, 4, 1),
    overworld_frame(sFollowerPicUnownV, 4, 4, 2),
    overworld_frame(sFollowerPicUnownV, 4, 4, 3),
    overworld_frame(sFollowerPicUnownV, 4, 4, 4),
    overworld_frame(sFollowerPicUnownV, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoUnownV =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF196,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableUnownV,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicUnownW[] = INCBIN_U32("mods/following_pokemon/graphics/followers/unown_w.4bpp");
static const u16 sFollowerPalUnownW[] = INCBIN_U16("mods/following_pokemon/graphics/followers/unown_w.gbapal");
const struct SpritePalette gFollowingPokemonPaletteUnownW = { sFollowerPalUnownW, 0xF197 };
static const struct SpriteFrameImage sFollowerPicTableUnownW[] =
{
    overworld_frame(sFollowerPicUnownW, 4, 4, 0),
    overworld_frame(sFollowerPicUnownW, 4, 4, 1),
    overworld_frame(sFollowerPicUnownW, 4, 4, 2),
    overworld_frame(sFollowerPicUnownW, 4, 4, 3),
    overworld_frame(sFollowerPicUnownW, 4, 4, 4),
    overworld_frame(sFollowerPicUnownW, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoUnownW =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF197,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableUnownW,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicUnownX[] = INCBIN_U32("mods/following_pokemon/graphics/followers/unown_x.4bpp");
static const u16 sFollowerPalUnownX[] = INCBIN_U16("mods/following_pokemon/graphics/followers/unown_x.gbapal");
const struct SpritePalette gFollowingPokemonPaletteUnownX = { sFollowerPalUnownX, 0xF198 };
static const struct SpriteFrameImage sFollowerPicTableUnownX[] =
{
    overworld_frame(sFollowerPicUnownX, 4, 4, 0),
    overworld_frame(sFollowerPicUnownX, 4, 4, 1),
    overworld_frame(sFollowerPicUnownX, 4, 4, 2),
    overworld_frame(sFollowerPicUnownX, 4, 4, 3),
    overworld_frame(sFollowerPicUnownX, 4, 4, 4),
    overworld_frame(sFollowerPicUnownX, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoUnownX =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF198,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableUnownX,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicUnownY[] = INCBIN_U32("mods/following_pokemon/graphics/followers/unown_y.4bpp");
static const u16 sFollowerPalUnownY[] = INCBIN_U16("mods/following_pokemon/graphics/followers/unown_y.gbapal");
const struct SpritePalette gFollowingPokemonPaletteUnownY = { sFollowerPalUnownY, 0xF199 };
static const struct SpriteFrameImage sFollowerPicTableUnownY[] =
{
    overworld_frame(sFollowerPicUnownY, 4, 4, 0),
    overworld_frame(sFollowerPicUnownY, 4, 4, 1),
    overworld_frame(sFollowerPicUnownY, 4, 4, 2),
    overworld_frame(sFollowerPicUnownY, 4, 4, 3),
    overworld_frame(sFollowerPicUnownY, 4, 4, 4),
    overworld_frame(sFollowerPicUnownY, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoUnownY =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF199,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableUnownY,
    .affineAnims = gDummySpriteAffineAnimTable,
};

static const u32 sFollowerPicUnownZ[] = INCBIN_U32("mods/following_pokemon/graphics/followers/unown_z.4bpp");
static const u16 sFollowerPalUnownZ[] = INCBIN_U16("mods/following_pokemon/graphics/followers/unown_z.gbapal");
const struct SpritePalette gFollowingPokemonPaletteUnownZ = { sFollowerPalUnownZ, 0xF19A };
static const struct SpriteFrameImage sFollowerPicTableUnownZ[] =
{
    overworld_frame(sFollowerPicUnownZ, 4, 4, 0),
    overworld_frame(sFollowerPicUnownZ, 4, 4, 1),
    overworld_frame(sFollowerPicUnownZ, 4, 4, 2),
    overworld_frame(sFollowerPicUnownZ, 4, 4, 3),
    overworld_frame(sFollowerPicUnownZ, 4, 4, 4),
    overworld_frame(sFollowerPicUnownZ, 4, 4, 5),
};
const struct ObjectEventGraphicsInfo gFollowingPokemonGraphicsInfoUnownZ =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0xF19A,
    .reflectionPaletteTag = MOD_FOLLOWER_OBJ_EVENT_PAL_TAG_NONE,
    .size = 512,
    .width = 32,
    .height = 32,
    .paletteSlot = 0,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .disableReflectionPaletteLoad = FALSE,
    .tracks = TRACKS_NONE,
    .oam = &gObjectEventBaseOam_32x32,
    .subspriteTables = NULL,
    .anims = sFollowerAnimTableStandard,
    .images = sFollowerPicTableUnownZ,
    .affineAnims = gDummySpriteAffineAnimTable,
};
