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

static int
qsort_wrapper(const void* a, const void* b)
{
	const char* restrict l = *(const char**)a;
	const char* restrict r = *(const char**)b;
	return strnatcmp(l, r);
}

TEST(strnatcmp)
{
	require(strnatcmp("a", "c") < 0);
	require(strnatcmp("abc", "abc") == 0);
	require(strnatcmp("abc", "ABC") == 0);
	require(strnatcmp("abc", "abcdef") < 0);
	require(strnatcmp("abc", "acc") < 0);
	require(strnatcmp("abz", "abcdef") > 0);

	require(strnatcmp("0", "1") < 0);
	require(strnatcmp("1", "2") < 0);
	require(strnatcmp("1", "10") < 0);
	require(strnatcmp("10", "12") < 0);

	require(strnatcmp("01", "2") < 0);
	require(strnatcmp("2", "03") < 0);
	require(strnatcmp("123", "0123") < 0);
	require(strnatcmp("0123", "00123") < 0);
	require(strnatcmp("001", "010") < 0);
	require(strnatcmp("001", "00010") < 0);

	require(strnatcmp("abc123abc", "abc123abc") == 0);
	require(strnatcmp("abc123abc", "abc123Abc") == 0);
	require(strnatcmp("abc123abc", "abc123abd") < 0);
	require(strnatcmp("a1", "a001") < 0);
	require(strnatcmp("a01", "a001") < 0);
	require(strnatcmp("b01", "b2") < 0);

	require(strnatcmp("0", "1.1") < 0);
	require(strnatcmp("0.5", "0.8") < 0);
	require(strnatcmp("1.234", "1.234") == 0);
	require(strnatcmp("1.234", "1.235") < 0);
	require(strnatcmp("1.234", "2.234") < 0);
	require(strnatcmp("1.23", "5") < 0);
	require(strnatcmp("12", "19.2") < 0);
	require(strnatcmp("1.001", "1.01") < 0);

	require(strnatcmp("abc10.234abc", "abc10.234Abc") == 0);
	require(strnatcmp("abc10.234abc", "abc10.234abd") < 0);
	require(strnatcmp("abc10.234abc", "abc10.235abc") < 0);
	require(strnatcmp("abc10z", "abc10.234z") < 0);
	require(strnatcmp("abc1.234z", "abc10z") < 0);

	const char* arr[] = {
	  "1.235",
	  "1.234",
	  "1.1",
	  "1.01",
	  "1.001",
	  "2.235",
	  "a001",
	  "a1",
	  "a01",
	  "10.001",
	  "03",
	  "4",
	  "2",
	};
	const char* check[] = {
	  "1.001",
	  "1.01",
	  "1.1",
	  "1.234",
	  "1.235",
	  "2",
	  "2.235",
	  "03",
	  "4",
	  "10.001",
	  "a1",
	  "a01",
	  "a001",
	};

	size_t n = ARRAYSIZE(arr);
	qsort(arr, n, sizeof(arr[0]), qsort_wrapper);
	for (size_t i = 0; i < n; ++i) {
		if (strcmp(arr[i], check[i]) != 0) {
			fail_fmt("error at index %zu: '%s' != '%s'", i, arr[i], check[i]);
		}
	}
}

// =============================================================================
DECLARE_TESTS("utils-tests", to_upper_lower, starts_with, ends_with, strnatcmp)
