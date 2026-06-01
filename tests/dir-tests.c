/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "dir.h"
#include "testing.h"

// =============================================================================
TEST(is_dir_sep)
{
	require(is_dir_sep('/'));
#if defined(PLATFORM_WINDOWS)
	require(is_dir_sep('\\'));
#else
	require(!is_dir_sep('\\'));
#endif
}

TEST(is_dot_or_dotdot)
{
	require(is_dot_or_dotdot("."));
	require(!is_dot_or_dotdot("./"));

	require(is_dot_or_dotdot(".."));
	require(!is_dot_or_dotdot("../"));

	require(!is_dot_or_dotdot("asdf"));
	require(!is_dot_or_dotdot("/asdf"));
}

TEST(mg_basename)
{
#define dotest(path, want)                    \
	do {                                        \
		char buf[] = path;                        \
		require_streq(mg_basename(buf, 0), want); \
	} while (0)

	dotest("", ".");
	dotest("/", "/");
	dotest("//", "/");
	dotest("///", "/");

	dotest("usr", "usr");
	dotest("/usr/", "usr");

	dotest("/usr/lib", "lib");
	dotest("/home//dwc//test", "test");
	dotest("/home/.././test", "test");
	dotest("/home/dwc/.", ".");
	dotest("/home/dwc/..", "..");

#ifdef PLATFORM_WINDOWS
	dotest("c:", "c:");
	dotest("c:\\", "c:");
	dotest("c:/", "c:");
	dotest("c:\\users", "users");
	dotest("c:/users", "users");
	dotest("c:\\users\\dwc\\test.exe", "test.exe");
	dotest("c:\\users\\dwc\\.", ".");
	dotest("c:\\users\\dwc\\..", "..");
#endif
#undef dotest
}

// =============================================================================
DECLARE_TESTS("dir-tests", is_dir_sep, is_dot_or_dotdot, mg_basename)
