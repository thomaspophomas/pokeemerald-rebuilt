#ifndef GUARD_MOD_LANGUAGE_H
#define GUARD_MOD_LANGUAGE_H

#include "global.h"

#define LANGUAGE_API_DEFAULT_LANGUAGE "en"
#define LANGUAGE_API_MAX_ID_LENGTH 7

struct ModLanguageText
{
    const char *language;
    const char *key;
    const u8 *text;
};

struct LanguageFormatArgs
{
    const u8 *arg0;
    const u8 *arg1;
    const u8 *arg2;
};

void LanguageApi_Init(void);
const char *LanguageApi_GetActiveLanguage(void);
bool8 LanguageApi_SetActiveLanguage(const char *language_id);
const u8 *LanguageApi_GetText(const char *key);
const u8 *LanguageApi_Format(const char *key, const struct LanguageFormatArgs *args);

#endif // GUARD_MOD_LANGUAGE_H
