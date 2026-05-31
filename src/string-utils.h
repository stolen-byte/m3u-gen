/* SPDX-License-Identifier: GPL-3.0-or-later */
#pragma once
#include "common.h"

// =============================================================================
bool
starts_with(const char* restrict str, const char prefix[restrict static 1]);

bool
istarts_with(const char* restrict str, const char prefix[restrict static 1]);

bool
ends_with(const char* restrict str, const char suffix[restrict static 1]);

bool
iends_with(const char* restrict str, const char suffix[restrict static 1]);

/**
 * locale-independent toupper
 */
static inline char
to_upper(char ch)
{
	if (ch >= 'a' && ch <= 'z')
		ch = (char)((unsigned char)ch & ~0x20);
	return ch;
}

/**
 * locale-independent tolower
 */
static inline char
to_lower(char ch)
{
	if (ch >= 'A' && ch <= 'Z')
		ch = (char)((unsigned char)ch | 0x20);
	return ch;
}

int
strnatcmp(const char a[restrict static 1], const char b[restrict static 1]);
