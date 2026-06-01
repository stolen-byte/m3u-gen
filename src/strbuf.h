/* SPDX-License-Identifier: GPL-3.0-or-later */
#pragma once
#include "common.h"

#include <stdio.h>

// =============================================================================
typedef struct strbuf {
	size_t cap;
	size_t len;
	char* data;
} strbuf;

// =============================================================================
void
strbuf_init(strbuf sb[static 1], size_t hint);

void
strbuf_free(strbuf sb[static 1]);

void
strbuf_grow(strbuf sb[static 1], size_t size);

size_t
strbuf_avail(const strbuf sb[static 1]);

void
strbuf_resize(strbuf sb[static 1], size_t newlen);

void
strbuf_reset(strbuf sb[static 1]);

void
strbuf_set(strbuf sb[restrict static 1], const char str[restrict static 1]);

void
strbuf_add(strbuf sb[static 1], char ch);

void
strbuf_addn(strbuf sb[static 1], char ch, size_t n);

void
strbuf_append(strbuf sb[restrict static 1], const char str[restrict static 1]);

void
strbuf_appendn(strbuf sb[restrict static 1], size_t len, const char str[restrict len]);

ATTRIBUTE(format(printf, 2, 0))
void
strbuf_vappendf(strbuf sb[restrict static 1], const char fmt[restrict static 1], va_list args);

ATTRIBUTE(format(printf, 2, 3))
void
strbuf_appendf(strbuf sb[restrict static 1], const char fmt[restrict static 1], ...);

int
strbuf_cmp(const strbuf a[restrict static 1], const strbuf b[restrict static 1]);

void
strbuf_swap(strbuf a[restrict static 1], strbuf b[restrict static 1]);

const char*
strbuf_basename(strbuf sb[static 1]);

void
strbuf_strip_ext(strbuf sb[static 1]);
