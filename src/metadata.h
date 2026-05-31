/* SPDX-License-Identifier: GPL-3.0-or-later */
#pragma once
#include "common.h"

// =============================================================================
typedef struct metadata_entry metadata_entry;

typedef struct {
	size_t cap;
	size_t len;
	metadata_entry* entries;
} metadata_list;

// =============================================================================
#define MID_TRACK       0x186bc01dd18f46da // TRACK
#define MID_EPISODE     0x5513e47e7baeec90 // EPISODE
#define MID_PARTNUMBER  0x60cc53cfd65910d2 // PART_NUMBER
#define MID_GENRE       0x8807ba881b8bca2e // GENRE
#define MID_CHAPTER     0xa2f3e9a7ff461708 // CHAPTER
#define MID_VOLUME      0xa56f0362b41db683 // VOLUME
#define MID_TITLE       0xcce7361e37623a6d // TITLE
#define MID_ARTIST      0xce26f38345beebd8 // ARTIST
#define MID_TOTALPARTS  0xd65bb85a04953d2e // TOTAL_PARTS
#define MID_ALBUMARTIST 0xd8193b6d0b7c4ef3 // ALBUMARTIST
#define MID_ALBUM       0xf831feb9f32962a2 // ALBUM
#define MID_SEASON      0xfd3800062cfadf5e // SEASON

// =============================================================================
void
metadata_init(metadata_list m[static 1]);

void
metadata_free(metadata_list m[static 1]);

void
metadata_set(metadata_list m[restrict static 1], uint64_t id, const char value[restrict static 1]);

uint64_t
metadata_insert(metadata_list m[restrict static 1],
                const char name[restrict static 1],
                const char value[restrict static 1]);

const char*
metadata_get(metadata_list m[static 1], uint64_t id);

const char*
metadata_find(metadata_list m[restrict static 1], const char name[restrict static 1]);

const char*
metadata_enumerate(metadata_list m[restrict static 1], void* restrict* it);

bool
metadata_read(const char path[restrict static 1],
              metadata_list out[restrict static 1],
              double duration[restrict static 1],
              bool aggressive);
