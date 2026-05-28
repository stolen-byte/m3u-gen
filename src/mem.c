/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "mem.h"
#include "compat/string.h"
#include "error.h"

// =============================================================================
void*
xmalloc(size_t size)
{
	assert(size > 0 && "tried to allocate zero bytes");

	void* ret = malloc(size);
	if (UNLIKELY(!ret))
		die("out of memory (trying to allocated %zu bytes)", size);

	return ret;
}

void*
xcalloc(size_t nmemb, size_t size)
{
	assert((nmemb > 0) && (size > 0) && "tried to allocate zero bytes");

	if (UNLIKELY(unsigned_mult_overflows(nmemb, size)))
		die("xcalloc: size too large");

	void* ret = calloc(nmemb, size);
	if (UNLIKELY(!ret))
		die("out of memory (trying to allocate %zu bytes)", nmemb * size);

	return ret;
}

void*
xrealloc(void* ptr, size_t size)
{
	if (!ptr)
		return xmalloc(size);

	assert(size > 0 && "tried to allocate zero bytes");
	void* ret = realloc(ptr, size);
	if (UNLIKELY(!ret))
		die("out of memory (trying to allocate %zu bytes)", size);

	return ret;
}

char*
xstrdup(const char* ptr)
{
	char* ret = strdup(ptr);
	if (UNLIKELY(!ret))
		die("xstrdup: out of memory");

	return ret;
}

void
xfree(void* ptr)
{
	free(ptr);
}
