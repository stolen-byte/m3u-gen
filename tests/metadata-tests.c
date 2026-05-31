/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "metadata.h"
#include "testing.h"

// =============================================================================
TEST(metadata)
{
	metadata_list meta;

	metadata_init(&meta);
	require(meta.len == 0);

	uint64_t tid = metadata_insert(&meta, "title", "test title");
	metadata_insert(&meta, "episode", "2");
	metadata_insert(&meta, "season", "1");
	require(meta.len == 3);

	require_streq(metadata_get(&meta, tid), "test title");

	require_streq(metadata_find(&meta, "season"), "1");
	require_streq(metadata_find(&meta, "episode"), "2");

	// nonexistant entries
	require(metadata_find(&meta, "nothing") == NULL);
	require(metadata_get(&meta, 0) == NULL);

	metadata_free(&meta);
}

TEST(metadata_sort_order)
{
	metadata_list meta;
	metadata_init(&meta);

	metadata_set(&meta, MID_ALBUMARTIST, "albumartist");
	metadata_set(&meta, MID_ARTIST, "artist");
	metadata_set(&meta, MID_ALBUM, "album");
	metadata_set(&meta, MID_TITLE, "title");
	metadata_set(&meta, MID_TOTALPARTS, "totalparts");
	metadata_set(&meta, MID_TRACK, "track");
	metadata_set(&meta, MID_PARTNUMBER, "partnumber");
	metadata_set(&meta, MID_EPISODE, "episode");
	metadata_set(&meta, MID_SEASON, "season");
	metadata_set(&meta, MID_VOLUME, "volume");
	metadata_set(&meta, MID_CHAPTER, "chapter");
	metadata_set(&meta, MID_GENRE, "genre");

	const char* check[] = {
	  "track",
	  "episode",
	  "partnumber",
	  "genre",
	  "chapter",
	  "volume",
	  "title",
	  "artist",
	  "totalparts",
	  "albumartist",
	  "album",
	  "season",
	};

	size_t i = 0;
	void* it = NULL;
	const char* value = NULL;
	while ((value = metadata_enumerate(&meta, &it)) != NULL) {
		if (strcmp(value, check[i]) != 0) {
			fail_fmt("list ordering incorrect at index %zu: %s != %s", i, value, check[i]);
		}
		++i;
	}

	metadata_free(&meta);
}

// =============================================================================
DECLARE_TESTS("metadata-tests", metadata, metadata_sort_order)
