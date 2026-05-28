/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "strbuf.h"
#include "testing.h"

// =============================================================================
TEST(sb_alloc)
{
	strbuf sb;

	strbuf_init(&sb, 0);
	require(sb.data != NULL);
	require(strbuf_avail(&sb) == 0);

	strbuf_grow(&sb, 100);
	require(strbuf_avail(&sb) >= 100);

	sb.len = 50; // fake writing some stuff
	strbuf_grow(&sb, 100);
	sb.len = 0;
	require(strbuf_avail(&sb) >= 150);

	strbuf_free(&sb);
	require(sb.data != NULL);
}

TEST(sb_reset)
{
	strbuf sb;

	strbuf_init(&sb, 64);

	size_t old = sb.cap;
	strbuf_reset(&sb);

	// checking that reset() does not dealloc
	require(sb.cap == old);
}

TEST(sb_append)
{
	strbuf sb;

	strbuf_init(&sb, 64);

	strbuf_add(&sb, 'X');
	require_streq(sb.data, "X");
	require(sb.len == 1);

	strbuf_addn(&sb, 'X', 2);
	require_streq(sb.data, "XXX");
	require(sb.len == 3);

	strbuf_append(&sb, "yyyyy");
	require_streq(sb.data, "XXXyyyyy");

	strbuf_reset(&sb);
	require(sb.len == 0);
	require(sb.data[0] == '\0');

	strbuf_free(&sb);
}

TEST(sb_format)
{
	strbuf sb;

	strbuf_init(&sb, 0);
	strbuf_appendf(&sb, "%d testing %d", 1, 2);

	require_streq(sb.data, "1 testing 2");

	strbuf_free(&sb);
}

// =============================================================================
DECLARE_TESTS("strbuf-tests", sb_alloc, sb_reset, sb_append, sb_format)
