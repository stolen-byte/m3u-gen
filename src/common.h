/* SPDX-License-Identifier: GPL-3.0-or-later */
#pragma once
#include "config.h"

// IWYU pragma: begin_exports
#include <assert.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <sys/types.h>
// IWYU pragma: end_exports

// =============================================================================
#undef ARRAYSIZE
#define ARRAYSIZE(x) (sizeof(x) / sizeof(*(x)))

#undef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#undef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

// usually provided by <limits.h>, but not everywhere
#ifndef PATH_MAX
	#define PATH_MAX 4096
#endif

#ifndef NAME_MAX
	#define NAME_MAX 255
#endif

// =============================================================================
#define bitsizeof(x) (CHAR_BIT * sizeof(x))

#define max_signed_value(a)   (INTMAX_MAX >> (bitsizeof(intmax_t) - bitsizeof(a)))
#define max_unsigned_value(a) (UINTMAX_MAX >> (bitsizeof(uintmax_t) - bitsizeof(a)))

#define signed_add_overflows(a, b)    ((b) > max_signed_value(a) - (a))
#define unsigned_add_overflows(a, b)  ((b) > max_unsigned_value(a) - (a))
#define unsigned_mult_overflows(a, b) ((a) && (b) > max_unsigned_value(a) / (a))

// this *may* look weird, however any decent compiler will generate
// the same code as the standard swap macro, and sometimes better.
// this version lets us swap entire structs without member-by-member shenanigans
#undef swap
#define swap(a, b)                                                    \
	do {                                                                \
		static_assert(sizeof(a) == sizeof(b), "types must be same size"); \
		void* _aptr = &(a);                                               \
		void* _bptr = &(b);                                               \
		unsigned char _swapbuf[sizeof(a)];                                \
		memcpy(_swapbuf, _aptr, sizeof(a));                               \
		memcpy(_aptr, _bptr, sizeof(a));                                  \
		memcpy(_bptr, _swapbuf, sizeof(a));                               \
	} while (0)

#ifndef HAVE_REALPATH
char*
realpath(const char* restrict name, char resolved[restrict PATH_MAX]);
#endif
