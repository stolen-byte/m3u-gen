/* SPDX-License-Identifier: GPL-3.0-or-later */
#pragma once
#include "common.h"

#include <stdio.h>

// =============================================================================
typedef struct {
	const char* url;
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
m3u_sort(m3u_list list[static 1]);
