/* SPDX-License-Identifier: GPL-3.0-or-later */
#pragma once
#include "common.h"

#include <sys/stat.h> // IWYU pragma: export

// =============================================================================
#if !defined(HAVE_LSTAT)
static inline int
lstat(const char* restrict path, struct stat* restrict buf)
{
	return stat(path, buf);
}
#endif
