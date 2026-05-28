/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "string-utils.h"
#include "compat/string.h"

#include <strings.h>

// =============================================================================
bool
starts_with(const char* restrict str, const char prefix[restrict static 1])
{
	if (!str)
		return false;

	for (;; ++str, ++prefix) {
		if (!*prefix)
			return true;
		if (*str != *prefix)
			return false;
	}
	UNREACHABLE();
}

bool
istarts_with(const char* restrict str, const char prefix[restrict static 1])
{
	if (!str)
		return false;

	for (;; ++str, ++prefix) {
		if (!*prefix)
			return true;
		if (to_lower(*str) != to_lower(*prefix))
			return false;
	}
	UNREACHABLE();
}

bool
ends_with(const char* restrict str, const char suffix[restrict static 1])
{
	if (!str)
		return false;

	size_t len = strlen(str);
	size_t slen = strlen(suffix);

	if (len < slen || memcmp(str + (len - slen), suffix, slen))
		return false;

	return true;
}

bool
iends_with(const char* restrict str, const char suffix[restrict static 1])
{
	if (!str)
		return false;

	size_t len = strlen(str);
	size_t slen = strlen(suffix);
	if (len < slen)
		return false;
	if (slen == 0)
		return true;

	return strncasecmp(str + (len - slen), suffix, slen) == 0;
}
