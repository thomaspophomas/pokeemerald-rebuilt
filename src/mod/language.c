#include "global.h"
#include "generated/mod_registry.h"
#include "mod/language.h"
#include "mod/runtime_profile.h"
#include "string_util.h"
#include "text.h"

static EWRAM_DATA char sActiveLanguage[LANGUAGE_API_MAX_ID_LENGTH + 1];
static const u8 sMissingText[] = _("");

static const struct ModLanguageText *FindTextForLanguage(const char *key, const char *language)
{
    u16 i;

    if (key == NULL || language == NULL)
        return NULL;

    for (i = 0; i < gModLanguageTextCount; i++)
    {
        if (strcmp(gModLanguageTexts[i].key, key) == 0
         && strcmp(gModLanguageTexts[i].language, language) == 0)
            return &gModLanguageTexts[i];
    }

    return NULL;
}

void LanguageApi_Init(void)
{
    strcpy(sActiveLanguage, LANGUAGE_API_DEFAULT_LANGUAGE);
}

const char *LanguageApi_GetActiveLanguage(void)
{
    if (sActiveLanguage[0] == '\0')
        LanguageApi_Init();

    return sActiveLanguage;
}

bool8 LanguageApi_SetActiveLanguage(const char *languageId)
{
    u8 i;

    if (languageId == NULL || languageId[0] == '\0')
        return FALSE;

    for (i = 0; i < LANGUAGE_API_MAX_ID_LENGTH && languageId[i] != '\0'; i++)
        sActiveLanguage[i] = languageId[i];
    sActiveLanguage[i] = '\0';
    return TRUE;
}

const u8 *LanguageApi_GetText(const char *key)
{
    const struct ModLanguageText *text;
    u16 i;
    const u8 *runtimeText;

    runtimeText = ModRuntimeProfile_GetText(key, LanguageApi_GetActiveLanguage());
    if (runtimeText != NULL)
        return runtimeText;
    runtimeText = ModRuntimeProfile_GetText(key, LANGUAGE_API_DEFAULT_LANGUAGE);
    if (runtimeText != NULL)
        return runtimeText;

    text = FindTextForLanguage(key, LanguageApi_GetActiveLanguage());
    if (text != NULL)
        return text->text;

    text = FindTextForLanguage(key, LANGUAGE_API_DEFAULT_LANGUAGE);
    if (text != NULL)
        return text->text;

    for (i = 0; i < gModLanguageTextCount; i++)
    {
        if (key != NULL && strcmp(gModLanguageTexts[i].key, key) == 0)
            return gModLanguageTexts[i].text;
    }

    return sMissingText;
}

const u8 *LanguageApi_Format(const char *key, const struct LanguageFormatArgs *args)
{
    gStringVar1[0] = EOS;
    gStringVar2[0] = EOS;
    gStringVar3[0] = EOS;

    if (args != NULL)
    {
        if (args->arg0 != NULL)
            StringCopy(gStringVar1, args->arg0);
        if (args->arg1 != NULL)
            StringCopy(gStringVar2, args->arg1);
        if (args->arg2 != NULL)
            StringCopy(gStringVar3, args->arg2);
    }

    StringExpandPlaceholders(gStringVar4, LanguageApi_GetText(key));
    return gStringVar4;
}
