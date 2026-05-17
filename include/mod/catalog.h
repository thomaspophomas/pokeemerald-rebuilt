#ifndef GUARD_MOD_CATALOG_H
#define GUARD_MOD_CATALOG_H

#include "global.h"

#define MOD_CATALOG_SCHEMA_HASH 0x00000001

enum ModCatalogEntryType
{
    MOD_CATALOG_ENTRY_TEXT = 1,
    MOD_CATALOG_ENTRY_WEATHER,
    MOD_CATALOG_ENTRY_ENGINE,
    MOD_CATALOG_ENTRY_NPC,
    MOD_CATALOG_ENTRY_SPRITE_ASSET,
};

struct ModCatalogEntry
{
    u8 type;
    u8 flags;
    u16 reserved;
    u32 keyHash;
    u32 contentHash;
} __attribute__((packed));

#endif // GUARD_MOD_CATALOG_H
