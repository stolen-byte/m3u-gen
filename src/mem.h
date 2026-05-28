/* SPDX-License-Identifier: GPL-3.0-or-later */
#pragma once
#include "common.h"

// =============================================================================
#define ATTR_MALLOC      ATTRIBUTE(__malloc__)
#define ATTR_XALLOC(...) ATTR_MALLOC ATTRIBUTE(__alloc_size__(__VA_ARGS__))

#define alloc_n(x) (((x) + 16) * 3 / 2)

#define realloc_array(x, cap) (x) = xrealloc((x), sizeof(*(x)) * (cap))
#define memzero_array(x, cap) memset((x), 0, sizeof(*(x)) * (cap))

#define grow_array(x, n, cap)  \
	do {                         \
		size_t _n = (n);           \
		size_t _an = alloc_n(cap); \
		if (_n > (cap)) {          \
			(cap) = MAX(_an, _n);    \
			realloc_array(x, cap);   \
		}                          \
	} while (0)

#define free_array(x, cap) \
	do {                     \
		xfreez(x);             \
		(cap) = 0;             \
	} while (0)

void*
xmalloc(size_t size) ATTR_XALLOC(1);

void*
xcalloc(size_t nmemb, size_t size) ATTR_XALLOC(1, 2);

void*
xrealloc(void* ptr, size_t size) ATTR_XALLOC(2);

char*
xstrdup(const char* ptr) ATTR_MALLOC;

void
xfree(void* ptr);

#define xfreez(x)      \
	do {                 \
		xfree((void*)(x)); \
		(x) = NULL;        \
	} while (0)
