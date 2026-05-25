#include "global.h"
#include "engine/extension_profile.h"

#if FEATURE_MODS
#include "generated/mod_registry.h"
#include "mod/runtime_profile.h"
#endif

void EngineExtensionProfile_Clear(void)
{
#if FEATURE_MODS
    ModRuntimeProfile_Clear();
#endif
}

u32 EngineExtensionProfile_GetActiveHash(void)
{
#if FEATURE_MODS
    return ModRuntimeProfile_GetActiveHash();
#else
    return 0;
#endif
}

u16 EngineExtensionProfile_GetProtocolVersion(void)
{
#if FEATURE_MODS
    return MOD_RUNTIME_PROFILE_PROTOCOL_VERSION;
#else
    return 0;
#endif
}

u32 EngineExtensionProfile_GetCapabilityFlags(void)
{
#if FEATURE_MODS
    return MOD_RUNTIME_PROFILE_CAPABILITIES;
#else
    return 0;
#endif
}

u32 EngineExtensionProfile_GetCapabilityHash(void)
{
#if FEATURE_MODS
    return MOD_RUNTIME_PROFILE_CAPABILITY_HASH;
#else
    return 0;
#endif
}

bool8 EngineExtensionProfile_CanReceive(u32 capability_flags, u32 capability_hash)
{
#if FEATURE_MODS
    return (capability_flags & ~MOD_RUNTIME_PROFILE_CAPABILITIES) == 0
        && capability_hash == MOD_RUNTIME_PROFILE_CAPABILITY_HASH;
#else
    return capability_flags == 0 && capability_hash == 0;
#endif
}

bool8 EngineExtensionProfile_BeginReceive(u32 profile_hash, u16 profile_size, u16 chunk_count)
{
#if FEATURE_MODS
    return ModRuntimeProfile_BeginReceive(profile_hash, profile_size, chunk_count);
#else
    return profile_hash == 0 && profile_size == 0 && chunk_count == 0;
#endif
}

bool8 EngineExtensionProfile_ReceiveChunk(u32 profile_hash, u16 chunk_index, u16 chunk_offset, const void *runtime_profile_chunk_data, u8 chunk_size)
{
#if FEATURE_MODS
    return ModRuntimeProfile_ReceiveChunk(profile_hash, chunk_index, chunk_offset, runtime_profile_chunk_data, chunk_size);
#else
    return profile_hash == 0 && chunk_index == 0 && chunk_offset == 0 && runtime_profile_chunk_data == NULL && chunk_size == 0;
#endif
}

u8 EngineExtensionProfile_CommitReceive(u32 profile_hash)
{
#if FEATURE_MODS
    return ModRuntimeProfile_CommitReceive(profile_hash);
#else
    return profile_hash == 0 ? ENGINE_EXTENSION_PROFILE_RESULT_OK : ENGINE_EXTENSION_PROFILE_RESULT_UNSUPPORTED_CAPABILITY;
#endif
}

u32 EngineExtensionProfile_GetCatalogHash(void)
{
#if FEATURE_MODS
    return gModCatalogHash;
#else
    return 0;
#endif
}

u16 EngineExtensionProfile_GetCatalogEntryCount(void)
{
#if FEATURE_MODS
    return gModCatalogEntryCount;
#else
    return 0;
#endif
}

u32 EngineExtensionProfile_GetCatalogSchemaHash(void)
{
#if FEATURE_MODS
    return MOD_CATALOG_SCHEMA_HASH;
#else
    return 0;
#endif
}

const struct ModCatalogEntry *EngineExtensionProfile_GetCatalogEntries(void)
{
#if FEATURE_MODS
    return gModCatalogEntries;
#else
    return NULL;
#endif
}
