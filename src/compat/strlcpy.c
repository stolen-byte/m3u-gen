/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "compat/string.h"

// =============================================================================
size_t
strlcpy(char* restrict dst, const char* restrict src, size_t size)
{
	size_t copied = 0;

	while (++copied < size && *src)
		*dst++ = *src++;

	if (copied <= size)
		*dst = '\0';

	return copied + strlen(src) - 1;
}
