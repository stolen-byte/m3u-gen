/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "m3u.h"
#include "testing.h"

// =============================================================================
TEST(m3u_list)
{
	m3u_list* list = m3u_create("testing", 3);

	require_streq(list->title, "testing");

	require(m3u_push(list, "testfile.mkv") != NULL);
	require(m3u_push(list, "testfile2.mkv") != NULL);
	require(m3u_push(list, "testfile3.mkv") != NULL);
	require(list->len == 3);

	m3u_pop(list);
	require(list->len == 2);

	require_streq(list->entries[0]->url, "testfile.mkv");
	require_streq(list->entries[1]->url, "testfile2.mkv");

	m3u_free(list);
}

TEST(m3u_sort)
{
	m3u_list* list = m3u_create(NULL, 7);

	m3u_push(list, "c.mkv");
	m3u_push(list, "b10.mkv");
	m3u_push(list, "b01.mkv");
	m3u_push(list, "a.mkv");
	m3u_push(list, "b03.mkv");
	m3u_push(list, "b21.mkv");
	m3u_push(list, "b2.mkv");

	m3u_sort(list);

	require(list->len == 7);
	require_streq(list->entries[0]->url, "a.mkv");
	require_streq(list->entries[1]->url, "b01.mkv");
	require_streq(list->entries[2]->url, "b03.mkv");
	require_streq(list->entries[3]->url, "b10.mkv");
	require_streq(list->entries[4]->url, "b2.mkv");
	require_streq(list->entries[5]->url, "b21.mkv");
	require_streq(list->entries[6]->url, "c.mkv");

	m3u_free(list);
}

// =============================================================================
DECLARE_TESTS("m3u-tests", m3u_list, m3u_sort)
