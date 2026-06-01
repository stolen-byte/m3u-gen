/* SPDX-License-Identifier: GPL-3.0-or-later */
#pragma once
#include "common.h"
#include "metadata.h"
#include "strbuf.h"

#include <stdio.h>

// =============================================================================
/**
 * an unset duration is always -1
 */
typedef struct {
	const char* url;
	double duration;
	strbuf title;
	metadata_list metadata;
} m3u_entry;

typedef struct {
	size_t len;
	m3u_entry** entries;
	const char* title; ///< note: not owned
} m3u_list;

// =============================================================================
m3u_list*
m3u_create(const char* title, size_t size);

void
m3u_free(m3u_list list[static 1]);

m3u_entry*
m3u_push(m3u_list list[restrict static 1], const char url[restrict static 1]);

void
m3u_pop(m3u_list list[restrict static 1]);

void
m3u_write(const m3u_list list[restrict static 1], FILE* restrict out);

void
m3u_sort(m3u_list list[static 1], int mode);

int
m3u_get_sortmode(const char name[static 1]);

const char*
m3u_entry_title(const m3u_entry entry[static 1]);

/**
 * set an entry's `title` from its metadata and a format string.
 */
void
m3u_format_title(m3u_entry entry[restrict static 1],
                 const char fmt[restrict static 1],
                 strbuf* restrict scratch);
