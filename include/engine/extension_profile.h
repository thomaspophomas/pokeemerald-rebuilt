#ifndef GUARD_ENGINE_EXTENSION_PROFILE_H
#define GUARD_ENGINE_EXTENSION_PROFILE_H

#include "global.h"
#include "mod/catalog.h"

#define ENGINE_EXTENSION_PROFILE_PROTOCOL_VERSION 1
#define ENGINE_EXTENSION_PROFILE_CAPABILITY_HASH 0x00000006

#define ENGINE_EXTENSION_PROFILE_RESULT_OK                     0
#define ENGINE_EXTENSION_PROFILE_RESULT_UNSUPPORTED_VERSION    1
#define ENGINE_EXTENSION_PROFILE_RESULT_UNSUPPORTED_CAPABILITY 2
#define ENGINE_EXTENSION_PROFILE_RESULT_BAD_HASH               3
#define ENGINE_EXTENSION_PROFILE_RESULT_BAD_SIZE               4
#define ENGINE_EXTENSION_PROFILE_RESULT_OUT_OF_MEMORY          5
#define ENGINE_EXTENSION_PROFILE_RESULT_BAD_RECORD             6

void EngineExtensionProfile_Clear(void);
u32 EngineExtensionProfile_GetActiveHash(void);
u16 EngineExtensionProfile_GetProtocolVersion(void);
u32 EngineExtensionProfile_GetCapabilityFlags(void);
u32 EngineExtensionProfile_GetCapabilityHash(void);
bool8 EngineExtensionProfile_CanReceive(u32 capability_flags, u32 capability_hash);
bool8 EngineExtensionProfile_BeginReceive(u32 profile_hash, u16 profile_size, u16 chunk_count);
bool8 EngineExtensionProfile_ReceiveChunk(u32 profile_hash, u16 chunk_index, u16 chunk_offset, const void *runtime_profile_chunk_data, u8 chunk_size);
u8 EngineExtensionProfile_CommitReceive(u32 profile_hash);
u32 EngineExtensionProfile_GetCatalogHash(void);
u16 EngineExtensionProfile_GetCatalogEntryCount(void);
u32 EngineExtensionProfile_GetCatalogSchemaHash(void);
const struct ModCatalogEntry *EngineExtensionProfile_GetCatalogEntries(void);

#endif // GUARD_ENGINE_EXTENSION_PROFILE_H
