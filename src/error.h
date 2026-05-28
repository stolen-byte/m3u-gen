/* SPDX-License-Identifier: GPL-3.0-or-later */
#pragma once
#include "common.h"
#include "compat/string.h"

#include <errno.h> // IWYU pragma: export

// =============================================================================
ATTRIBUTE(format(printf, 2, 3))
void
xwarn(bool with_errno, const char fmt[restrict static 1], ...);

ATTRIBUTE(format(printf, 2, 3))
void
xerror(bool with_errno, const char fmt[restrict static 1], ...);

ATTRIBUTE(format(printf, 2, 3))
noreturn void
xdie(bool with_errno, const char fmt[restrict static 1], ...);

#define warn(...)  xwarn(false, __VA_ARGS__)
#define ewarn(...) xwarn(true, __VA_ARGS__)

#define error(...)  xerror(false, __VA_ARGS__)
#define eerror(...) xerror(true, __VA_ARGS__)

#define die(...)  xdie(false, __VA_ARGS__)
#define edie(...) xdie(true, __VA_ARGS__)

/**
 * shortcut for `strerror(errno)`
 */
static inline char*
serror(void)
{
	return strerror(errno);
}
