/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "string-utils.h"
#include "testing.h"

// =============================================================================
TEST(to_upper_lower)
{
	const char* hi = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	const char* lo = "abcdefghijklmnopqrstuvwxyz";
	const size_t len = strlen(hi);

	for (size_t i = 0; i < len; ++i) {
		char ch = to_lower(hi[i]);
		require_eq(ch, lo[i], "'%c'");

		ch = to_upper(lo[i]);
		require_eq(ch, hi[i], "'%c'");
	}
}

TEST(starts_with)
{
	require(!starts_with(NULL, ""));
	require(starts_with("testing", ""));
	require(starts_with("testing", "t"));
	require(starts_with("testing", "test"));
	require(!starts_with("testing", "not"));

	require(!istarts_with(NULL, ""));
	require(istarts_with("testing", ""));
	require(istarts_with("testing", "T"));
	require(istarts_with("testing", "TesT"));
	require(!istarts_with("testing", "nOt"));
}

TEST(ends_with)
{
	require(!ends_with(NULL, ""));
	require(ends_with("testing", ""));
	require(ends_with("testing", "g"));
	require(ends_with("testing", "ing"));
	require(!ends_with("testing", "not"));

	require(!iends_with(NULL, ""));
	require(iends_with("testing", ""));
	require(iends_with("testing", "G"));
	require(iends_with("testing", "InG"));
	require(!iends_with("testing", "nOt"));
}

// =============================================================================
DECLARE_TESTS("utils-tests", to_upper_lower, starts_with, ends_with)
