/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "string-utils.h"
#include "compat/string.h"

#include <ctype.h>
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

/**
 * compares the integer part of 2 strings containing numbers
 *
 * leading zeros are not insignificant, and will treat them the same as
 * a regular string-length difference, even if the actual numerical value
 * is equal (eg., "00123" < "0000123")
 */
static int
icmp(const unsigned char* restrict* a, const unsigned char* restrict* b)
{
	register const unsigned char* l = *a;
	register const unsigned char* r = *b;

	// skip over, but keep track of leading zeroes
	int llead = 0, rlead = 0;
	for (; *l == '0'; ++l)
		++llead;
	for (; *r == '0'; ++r)
		++rlead;

	// count significant digits
	int ldigits = 0, rdigits = 0;
	for (; isdigit(*l); ++l)
		++ldigits;
	for (; isdigit(*r); ++r)
		++rdigits;

	// fewer digits => smaller number
	if (ldigits != rdigits)
		return (ldigits - rdigits);

	// equal number of digits, compare 1 by 1
	l -= ldigits;
	r -= rdigits;
	for (; ldigits; ++l, ++r, --ldigits) {
		int res = (int)(*l - *r);
		if (res)
			return res;
	}

	// all digits equal. shorter run of leading zeros wins
	if (llead != rlead)
		return (llead - rlead);

	*a = l;
	*b = r;
	return 0;
}

/**
 * compare the fractional part of 2 strings containing decimal numbers
 *
 * treats the shorter part as smaller, (eg., "1.2" < "1.20") for consistency
 */
static int
fcmp(const unsigned char* restrict* a, const unsigned char* restrict* b)
{
	register const unsigned char* l = *a;
	register const unsigned char* r = *b;

	for (;; ++l, ++r) {
		bool dl = isdigit(*l);
		bool dr = isdigit(*r);

		if (!dl && !dr)
			break;
		if (!dl)
			return -1;
		if (!dr)
			return 1;

		int res = (int)(*l - *r);
		if (res)
			return res;
	}

	*a = l;
	*b = r;
	return 0;
}

int
strnatcmp(const char a[restrict static 1], const char b[restrict static 1])
{
	const unsigned char* restrict l = (const unsigned char*)a;
	const unsigned char* restrict r = (const unsigned char*)b;

	while (1) {
		if (isdigit(*l) && isdigit(*r)) {
			int res;
			if ((res = icmp(&l, &r)) != 0)
				return res;

			if ((*l == '.' && isdigit(l[1])) || (*r == '.' && isdigit(r[1]))) {
				if (*l == '.')
					++l;
				if (*r == '.')
					++r;
				if ((res = fcmp(&l, &r)) != 0)
					return res;
			}
		}

		if (!*l && !*r)
			return 0;

		unsigned char chl = (unsigned char)to_upper((char)*l);
		unsigned char chr = (unsigned char)to_upper((char)*r);
		if (chl != chr)
			return (int)(chl - chr);

		++l;
		++r;
	}
}
