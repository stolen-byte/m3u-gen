/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "m3u.h"
#include "strbuf.h"
#include "testing.h"

// =============================================================================
TEST(m3u_list)
{
	m3u_list* list = m3u_create("testing", 3);

	require_streq(list->title, "testing");

	m3u_entry* e = m3u_push(list, "testfile.mkv");
	require(e != NULL);
	require_feq(e->duration, -1);

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
	require_streq(list->entries[2]->url, "b2.mkv");
	require_streq(list->entries[3]->url, "b03.mkv");
	require_streq(list->entries[4]->url, "b10.mkv");
	require_streq(list->entries[5]->url, "b21.mkv");
	require_streq(list->entries[6]->url, "c.mkv");

	m3u_free(list);
}

TEST(m3u_format_title)
{
	strbuf scratch;
	strbuf_init(&scratch, PATH_MAX);

#define do_check(fmt, want)                  \
	do {                                       \
		m3u_format_title(&entry, fmt, &scratch); \
		require_streq(entry.title.data, want);   \
	} while (0)

	m3u_entry entry = {0};
	metadata_init(&entry.metadata);
	strbuf_init(&entry.title, 256);
	entry.url = "/some/path/testfile.mkv";

	do_check("random", "random");

	metadata_set(&entry.metadata, MID_ALBUMARTIST, "albumartist");
	metadata_set(&entry.metadata, MID_ARTIST, "artist");
	metadata_set(&entry.metadata, MID_ALBUM, "album");
	metadata_set(&entry.metadata, MID_TITLE, "title");
	metadata_set(&entry.metadata, MID_TOTALPARTS, "totalparts");
	metadata_set(&entry.metadata, MID_TRACK, "track");
	metadata_set(&entry.metadata, MID_EPISODE, "episode");
	metadata_set(&entry.metadata, MID_SEASON, "season");
	metadata_set(&entry.metadata, MID_VOLUME, "volume");
	metadata_set(&entry.metadata, MID_CHAPTER, "chapter");
	metadata_set(&entry.metadata, MID_GENRE, "genre");
	metadata_set(&entry.metadata, MID_YEAR, "year");

	do_check("%%", "%");
	do_check("%%%%%%", "%%%");

	do_check("%A", "albumartist");
	do_check("%a", "artist");
	do_check("%T", "album");
	do_check("%t", "title");
	do_check("%N", "totalparts");
	do_check("%n", "track");
	do_check("%e", "episode");
	do_check("%s", "season");
	do_check("%v", "volume");
	do_check("%c", "chapter");
	do_check("%g", "genre");
	do_check("%Y", "year");

	do_check("%s.%e: %t", "season.episode: title");

	do_check("%F", "testfile.mkv");
	do_check("%f", "testfile");

	metadata_insert(&entry.metadata, "something", "whatever");
	do_check("--%(something)--", "--whatever--");

	strbuf_free(&entry.title);
	metadata_free(&entry.metadata);

#undef do_check
}

// =============================================================================
DECLARE_TESTS("m3u-tests", m3u_list, m3u_sort, m3u_format_title)
