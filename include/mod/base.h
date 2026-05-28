#ifndef GUARD_MOD_BASE_H
#define GUARD_MOD_BASE_H

/*
 * Minimal public base for mod-facing headers.
 *
 * Keep this header small: stable scalar types, boolean constants, NULL, and
 * SDK version defines. Engine headers should stay behind explicit mod ports.
 */

#include <stddef.h>
#include "gba/defines.h"
#include "gba/types.h"

#define MOD_SDK_VERSION 1
#define MOD_SDK_ABI_VERSION 1

#endif // GUARD_MOD_BASE_H
