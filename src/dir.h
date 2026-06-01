/* SPDX-License-Identifier: GPL-3.0-or-later */
#pragma once
#include "common.h"

// =============================================================================
static inline bool
is_dir_sep(char ch)
{
#ifdef PLATFORM_WINDOWS
	return (ch == '/') || (ch == '\\');
#else
	return (ch == '/');
#endif
}

static inline bool
is_dot_or_dotdot(const char* name)
{
	return (name && name[0] == '.' && (name[1] == '\0' || (name[1] == '.' && name[2] == '\0')));
}

char*
mg_basename(char* path, size_t len);

#undef basename
#define basename(x) mg_basename(x, 0)
