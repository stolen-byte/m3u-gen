/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "hash.h"
#include "string-utils.h"

// =============================================================================
#define FNV64_BASE  ((uint64_t)0xcbf29ce484222325)
#define FNV64_PRIME ((uint64_t)0x00000100000001b3)

// =============================================================================
uint64_t
strhash(const char str[static 1])
{
	uint64_t h = FNV64_BASE;
	char ch;
	while ((ch = *str++))
		h = (h * FNV64_PRIME) ^ ((uint64_t)to_upper(ch));

	return h;
}
