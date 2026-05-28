/* SPDX-License-Identifier: GPL-3.0-or-later */
#pragma once
#include "common.h"

#include <string.h> // IWYU pragma: export

// =============================================================================
#if !defined(HAVE_STRLCPY)
size_t
strlcpy(char* restrict dst, const char* restrict src, size_t size);
#endif
