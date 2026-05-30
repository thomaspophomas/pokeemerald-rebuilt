#include "mod_sdk.h"
#include "constants/region_map_sections.h"
#include "constants/rgb.h"
#include "day_night_cycle.h"
#include "global.fieldmap.h"
#include "main.h"
#include "overworld.h"
#include "palette.h"
#include "task.h"

#define DAY_NIGHT_TINT_SCALE 256
#define MINUTES_PER_HOUR 60
#define MINUTES_PER_DAY (24 * MINUTES_PER_HOUR)
#define FIRST_MENU_BG_PALETTE 14
#define FIRST_OBJ_PALETTE 16
#define OBJ_PALETTE_COUNT 16
#define DAY_NIGHT_TASK_PRIORITY 0xFF
#define DAY_NIGHT_DEBUG_MINUTE_DISABLED 0xFFFF
#define PALETTE_CHECKSUM_OFFSET 2166136261u
#define PALETTE_CHECKSUM_PRIME 16777619u

struct DayNightTint
{
    u16 r;
    u16 g;
    u16 b;
};

struct DayNightMapTintOverride
{
    u16 mapSection;
    struct DayNightTint tint;
};

static EWRAM_DATA bool8 sTintApplied = FALSE;
static EWRAM_DATA bool8 sTintSuspended = FALSE;
static EWRAM_DATA u16 sDebugMinuteOfDay = DAY_NIGHT_DEBUG_MINUTE_DISABLED;
static EWRAM_DATA u16 sDebugMinuteStep = 0;
static EWRAM_DATA u16 sBaseUnfadedPalette[PLTT_BUFFER_SIZE] = {0};
static EWRAM_DATA u16 sBaseFadedPalette[PLTT_BUFFER_SIZE] = {0};
static EWRAM_DATA struct DayNightTint sAppliedTint = {
    DAY_NIGHT_TINT_SCALE,
    DAY_NIGHT_TINT_SCALE,
    DAY_NIGHT_TINT_SCALE
};
static EWRAM_DATA u32 sTintedUnfadedChecksum = 0;
static EWRAM_DATA u32 sTintedFadedChecksum = 0;

#include "../time/generated_tints.inc"

static void Task_DayNightCycleTint(u8 taskId);

extern bool8 BattleWeatherStack_GetOverworldTint(u16 *r, u16 *g, u16 *b) __attribute__((weak));

static void ResetTintState(void)
{
    sTintApplied = FALSE;
    sTintSuspended = FALSE;
    sTintedUnfadedChecksum = 0;
    sTintedFadedChecksum = 0;
    sAppliedTint.r = DAY_NIGHT_TINT_SCALE;
    sAppliedTint.g = DAY_NIGHT_TINT_SCALE;
    sAppliedTint.b = DAY_NIGHT_TINT_SCALE;
}

static void SuspendTintState(void)
{
    ResetTintState();
    sTintSuspended = TRUE;
}

bool8 DayNightCycle_DebugIsTintApplied(void)
{
    return sTintApplied;
}

bool8 DayNightCycle_DebugIsTintSuspended(void)
{
    return sTintSuspended;
}

bool8 DayNightCycle_DebugGetAppliedTint(u16 *r, u16 *g, u16 *b)
{
    if (!sTintApplied)
        return FALSE;

    if (r != NULL)
        *r = sAppliedTint.r;
    if (g != NULL)
        *g = sAppliedTint.g;
    if (b != NULL)
        *b = sAppliedTint.b;
    return TRUE;
}

void DayNightCycle_DebugResetTint(void)
{
    ResetTintState();
}

void DayNightCycle_DebugSuspendTint(void)
{
    SuspendTintState();
}

void DayNightCycle_DebugSetMinuteOfDay(u16 minuteOfDay)
{
    sDebugMinuteOfDay = minuteOfDay % MINUTES_PER_DAY;
    ResetTintState();
}

void DayNightCycle_DebugSetMinuteStep(u16 minutesPerTick)
{
    sDebugMinuteStep = minutesPerTick % MINUTES_PER_DAY;
    if (sDebugMinuteStep != 0 && sDebugMinuteOfDay == DAY_NIGHT_DEBUG_MINUTE_DISABLED)
        sDebugMinuteOfDay = 0;
    ResetTintState();
}

u16 DayNightCycle_DebugGetMinuteStep(void)
{
    return sDebugMinuteStep;
}

void DayNightCycle_DebugClearMinuteOverride(void)
{
    sDebugMinuteOfDay = DAY_NIGHT_DEBUG_MINUTE_DISABLED;
    sDebugMinuteStep = 0;
    ResetTintState();
}

bool8 DayNightCycle_DebugGetMinuteOverride(u16 *minuteOfDay)
{
    if (sDebugMinuteOfDay == DAY_NIGHT_DEBUG_MINUTE_DISABLED)
        return FALSE;
    if (minuteOfDay != NULL)
        *minuteOfDay = sDebugMinuteOfDay;
    return TRUE;
}

static void AdvanceDebugMinuteOverride(void)
{
    if (sDebugMinuteOfDay == DAY_NIGHT_DEBUG_MINUTE_DISABLED || sDebugMinuteStep == 0)
        return;
    sDebugMinuteOfDay = (sDebugMinuteOfDay + sDebugMinuteStep) % MINUTES_PER_DAY;
}

static bool8 IsOverworldTintContext(void)
{
    return gMain.callback2 == CB2_Overworld || gMain.callback2 == CB2_OverworldBasic;
}

static bool8 IsOutdoorLightingMap(void)
{
    return IsMapTypeOutdoors(gMapHeader.mapType);
}

static u32 AddPaletteRangeChecksum(
    u32 checksum,
    const u16 *palette,
    u16 startPalette,
    u16 paletteCount)
{
    u16 i;
    u16 offset = PLTT_ID(startPalette);
    u16 colorCount = paletteCount * 16;

    for (i = 0; i < colorCount; i++)
    {
        checksum ^= palette[offset + i] & 0xFF;
        checksum *= PALETTE_CHECKSUM_PRIME;
        checksum ^= palette[offset + i] >> 8;
        checksum *= PALETTE_CHECKSUM_PRIME;
    }

    return checksum;
}

static u32 CalcOwnedPaletteChecksum(const u16 *palette)
{
    u32 checksum = PALETTE_CHECKSUM_OFFSET;

    checksum = AddPaletteRangeChecksum(checksum, palette, 0, FIRST_MENU_BG_PALETTE);
    checksum = AddPaletteRangeChecksum(checksum, palette, FIRST_OBJ_PALETTE, OBJ_PALETTE_COUNT);
    return checksum;
}

static bool8 OwnsAppliedTint(void)
{
    if (!sTintApplied)
        return FALSE;

    return CalcOwnedPaletteChecksum(gPlttBufferUnfaded) == sTintedUnfadedChecksum
        && CalcOwnedPaletteChecksum(gPlttBufferFaded) == sTintedFadedChecksum;
}

static void RestorePaletteRange(u16 *target, const u16 *source, u16 startPalette, u16 paletteCount)
{
    u16 offset = PLTT_ID(startPalette);

    CpuCopy16(source + offset, target + offset, paletteCount * PLTT_SIZE_4BPP);
}

static void RestoreOwnedTint(void)
{
    if (!sTintApplied)
    {
        ResetTintState();
        return;
    }

    if (!OwnsAppliedTint())
    {
        SuspendTintState();
        return;
    }

    RestorePaletteRange(gPlttBufferUnfaded, sBaseUnfadedPalette, 0, FIRST_MENU_BG_PALETTE);
    RestorePaletteRange(gPlttBufferFaded, sBaseFadedPalette, 0, FIRST_MENU_BG_PALETTE);
    RestorePaletteRange(
        gPlttBufferUnfaded,
        sBaseUnfadedPalette,
        FIRST_OBJ_PALETTE,
        OBJ_PALETTE_COUNT);
    RestorePaletteRange(
        gPlttBufferFaded,
        sBaseFadedPalette,
        FIRST_OBJ_PALETTE,
        OBJ_PALETTE_COUNT);

    ResetTintState();
}

static bool8 TintsMatch(const struct DayNightTint *left, const struct DayNightTint *right)
{
    return left->r == right->r
        && left->g == right->g
        && left->b == right->b;
}

static u8 ClampRgbComponent(s32 value)
{
    if (value > 31)
        return 31;
    if (value < 0)
        return 0;

    return value;
}

static u16 ApplyTintToColor(u16 color, const struct DayNightTint *tint)
{
    u8 r = ClampRgbComponent((GET_R(color) * tint->r) >> 8);
    u8 g = ClampRgbComponent((GET_G(color) * tint->g) >> 8);
    u8 b = ClampRgbComponent((GET_B(color) * tint->b) >> 8);

    return RGB2(r, g, b) | (color & RGB_ALPHA);
}

static u16 LerpTintComponent(u16 from, u16 to, u16 coeff)
{
    return from + ((s32)(to - from) * coeff) / MINUTES_PER_HOUR;
}

static void GetTintForMinute(u16 minuteOfDay, struct DayNightTint *tint)
{
    u8 hour = (minuteOfDay / MINUTES_PER_HOUR) % 24;
    u8 nextHour = (hour + 1) % 24;
    u16 coeff = minuteOfDay % MINUTES_PER_HOUR;

    tint->r = LerpTintComponent(sHourlyTints[hour].r, sHourlyTints[nextHour].r, coeff);
    tint->g = LerpTintComponent(sHourlyTints[hour].g, sHourlyTints[nextHour].g, coeff);
    tint->b = LerpTintComponent(sHourlyTints[hour].b, sHourlyTints[nextHour].b, coeff);
}

static void ApplyMapTintOverride(struct DayNightTint *tint)
{
    u16 i;

    for (i = 0; i < ARRAY_COUNT(sMapTintOverrides); i++)
    {
        if (sMapTintOverrides[i].mapSection != gMapHeader.regionMapSectionId)
            continue;
        tint->r = ((u32)tint->r * sMapTintOverrides[i].tint.r + (DAY_NIGHT_TINT_SCALE / 2)) / DAY_NIGHT_TINT_SCALE;
        tint->g = ((u32)tint->g * sMapTintOverrides[i].tint.g + (DAY_NIGHT_TINT_SCALE / 2)) / DAY_NIGHT_TINT_SCALE;
        tint->b = ((u32)tint->b * sMapTintOverrides[i].tint.b + (DAY_NIGHT_TINT_SCALE / 2)) / DAY_NIGHT_TINT_SCALE;
        return;
    }
}

static bool8 IsIdentityTint(const struct DayNightTint *tint)
{
    return tint->r == DAY_NIGHT_TINT_SCALE
        && tint->g == DAY_NIGHT_TINT_SCALE
        && tint->b == DAY_NIGHT_TINT_SCALE;
}

static void ApplyWeatherOverlayTint(struct DayNightTint *tint)
{
    u16 r;
    u16 g;
    u16 b;

    if (BattleWeatherStack_GetOverworldTint == NULL)
        return;
    if (!BattleWeatherStack_GetOverworldTint(&r, &g, &b))
        return;

    tint->r = ((u32)tint->r * r + (DAY_NIGHT_TINT_SCALE / 2)) / DAY_NIGHT_TINT_SCALE;
    tint->g = ((u32)tint->g * g + (DAY_NIGHT_TINT_SCALE / 2)) / DAY_NIGHT_TINT_SCALE;
    tint->b = ((u32)tint->b * b + (DAY_NIGHT_TINT_SCALE / 2)) / DAY_NIGHT_TINT_SCALE;
}

static void ApplyTintToPaletteRange(
    u16 *palette,
    u16 startPalette,
    u16 paletteCount,
    const struct DayNightTint *tint)
{
    u16 i;
    u16 offset = PLTT_ID(startPalette);
    u16 colorCount = paletteCount * 16;

    for (i = 0; i < colorCount; i++)
        palette[offset + i] = ApplyTintToColor(palette[offset + i], tint);
}

static void SaveBasePalettes(void)
{
    CpuCopy16(gPlttBufferUnfaded, sBaseUnfadedPalette, PLTT_SIZE);
    CpuCopy16(gPlttBufferFaded, sBaseFadedPalette, PLTT_SIZE);
}

static void ApplyTintToOwnedRanges(struct DayNightTint *tint)
{
    ApplyTintToPaletteRange(gPlttBufferUnfaded, 0, FIRST_MENU_BG_PALETTE, tint);
    ApplyTintToPaletteRange(gPlttBufferFaded, 0, FIRST_MENU_BG_PALETTE, tint);
    ApplyTintToPaletteRange(gPlttBufferUnfaded, FIRST_OBJ_PALETTE, OBJ_PALETTE_COUNT, tint);
    ApplyTintToPaletteRange(gPlttBufferFaded, FIRST_OBJ_PALETTE, OBJ_PALETTE_COUNT, tint);
}

static void ApplyFieldPaletteTint(void)
{
    struct ModTimeSnapshot snapshot;
    struct DayNightTint tint;
    bool8 ownsTint;

    if (sTintSuspended)
        return;

    if (!IsOverworldTintContext() || !IsOutdoorLightingMap())
    {
        RestoreOwnedTint();
        return;
    }

    ModTime_Now(&snapshot);
    if (sDebugMinuteOfDay != DAY_NIGHT_DEBUG_MINUTE_DISABLED)
        snapshot.minute_of_day = sDebugMinuteOfDay;
    snapshot.minute_of_day %= MINUTES_PER_DAY;
    GetTintForMinute(snapshot.minute_of_day, &tint);
    ApplyMapTintOverride(&tint);
    ApplyWeatherOverlayTint(&tint);

    if (IsIdentityTint(&tint))
    {
        RestoreOwnedTint();
        return;
    }

    ownsTint = OwnsAppliedTint();
    if (sTintApplied && !ownsTint)
    {
        SuspendTintState();
        return;
    }

    if (ownsTint && TintsMatch(&sAppliedTint, &tint))
        return;

    if (ownsTint)
        RestoreOwnedTint();

    SaveBasePalettes();
    ApplyTintToOwnedRanges(&tint);
    sAppliedTint = tint;
    sTintedUnfadedChecksum = CalcOwnedPaletteChecksum(gPlttBufferUnfaded);
    sTintedFadedChecksum = CalcOwnedPaletteChecksum(gPlttBufferFaded);
    sTintApplied = TRUE;
}

static void EnsureTintTask(void)
{
    if (!FuncIsActiveTask(Task_DayNightCycleTint))
        CreateTask(Task_DayNightCycleTint, DAY_NIGHT_TASK_PRIORITY);
}

static void Task_DayNightCycleTint(u8 taskId)
{
    (void)taskId;
    ApplyFieldPaletteTint();
    AdvanceDebugMinuteOverride();
}

s8 DayNightCycle_OnGameInit(const struct ModEvent *event)
{
    (void)event;
    sDebugMinuteOfDay = DAY_NIGHT_DEBUG_MINUTE_DISABLED;
    sDebugMinuteStep = 0;
    ResetTintState();
    return MOD_EVENT_RESULT_CONTINUE;
}

s8 DayNightCycle_OnMapLoad(const struct ModEvent *event)
{
    (void)event;
    ResetTintState();
    EnsureTintTask();
    return MOD_EVENT_RESULT_CONTINUE;
}

s8 DayNightCycle_OnPlayerStep(const struct ModEvent *event)
{
    (void)event;
    EnsureTintTask();
    return MOD_EVENT_RESULT_CONTINUE;
}

s8 DayNightCycle_OnTimeSegmentChanged(const struct ModEvent *event)
{
    (void)event;
    EnsureTintTask();
    return MOD_EVENT_RESULT_CONTINUE;
}

s8 DayNightCycle_OnBattleStarted(const struct ModEvent *event)
{
    (void)event;
    RestoreOwnedTint();
    return MOD_EVENT_RESULT_CONTINUE;
}

s8 DayNightCycle_OnBattleEnded(const struct ModEvent *event)
{
    (void)event;
    ResetTintState();
    return MOD_EVENT_RESULT_CONTINUE;
}
