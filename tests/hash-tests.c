/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "hash.h"
#include "testing.h"

// =============================================================================
TEST(strhash)
{
#define test_hash(str, want)        \
	do {                              \
		uint64_t h = strhash(str);      \
		require_eq(h, want, "%#016lx"); \
	} while (0)

	// remember: `want` should be the hash of UPPERCASE `str`
	test_hash("", 0xcbf29ce484222325);
	test_hash("a", 0xaf63bd4c8601b79e);
	test_hash("z", 0xaf63bd4c8601b785);
	test_hash("testing", 0xc310c32772bc5e5f);
	test_hash("teSTIng", 0xc310c32772bc5e5f);

#undef test_hash
}

// =============================================================================
DECLARE_TESTS("hash-tests", strhash)
