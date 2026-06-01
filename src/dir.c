/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "dir.h"
#include "compat/string.h"

// =============================================================================
char*
mg_basename(char* path, size_t len)
{
	if (UNLIKELY(!path || !*path)) {
		// `basename(3)` is (imo) mis-specified, because it returns a non-const pointer
		// even though the spec says it sometimes returns a pointer to internal memory.
		// thus, this cast.
		return (char*)".";
	}

	if (!len)
		len = strlen(path);

	register size_t i = len - 1;
	for (; i && is_dir_sep(path[i]); --i)
		path[i] = '\0';

	while (i && !is_dir_sep(path[i - 1]))
		--i;

	return path + i;
}
