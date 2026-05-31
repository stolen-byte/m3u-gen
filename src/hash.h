/* SPDX-License-Identifier: GPL-3.0-or-later */
#pragma once
#include "common.h"

// =============================================================================
/**
 * produces the 64-bit FNV1 hash of a given string.
 *
 * The hash is calculated from `str` after all ASCII characters
 * are converted to uppercase.
 *
 * notes: not encoding-aware.
 */
uint64_t
strhash(const char str[static 1]);
