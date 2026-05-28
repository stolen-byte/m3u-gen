/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "error.h"

#include <stdarg.h>
#include <stdio.h>

// =============================================================================
ATTRIBUTE(format(printf, 3, 0))
static void
vfreportf(FILE* restrict out,
          const char pfx[restrict static 1],
          const char fmt[restrict static 1],
          va_list params,
          bool err)
{
	char msg[4096];
	char *p = msg, *pend = msg + sizeof(msg);

#ifdef DEBUG
	size_t pfx_len = strlen(pfx);
	if (sizeof(msg) <= pfx_len) {
		fprintf(stderr, "BUG: prefix too long ('%s')", pfx);
		abort();
	}
#endif

	p += strlcpy(msg, pfx, sizeof(msg));
	*(p++) = ':';
	*(p++) = ' ';

	int r = vsnprintf(p, (size_t)(pend - p), fmt, params);
	if (r >= 0) {
		p += r;

		if (err) {
			r = snprintf(p, (size_t)(pend - p), ": %s", serror());
			if (r > 0)
				p += r;
		}

		*(p++) = '\n';
	} else {
		fprintf(stderr, "error: unable to format message: %s\n", fmt);
		*p = '\0';
	}

	fflush(out);
	fputs(msg, out);
}

ATTRIBUTE(format(printf, 2, 0))
static void
vreportf(const char pfx[restrict static 1], const char fmt[restrict static 1], va_list params, bool err)
{
	vfreportf(stderr, pfx, fmt, params, err);
}

void
xwarn(bool with_errno, const char fmt[restrict static 1], ...)
{
	va_list ap;
	va_start(ap, fmt);
	vreportf("warn", fmt, ap, with_errno);
	va_end(ap);
}

void
xerror(bool with_errno, const char fmt[restrict static 1], ...)
{
	va_list ap;
	va_start(ap, fmt);
	vreportf("error", fmt, ap, with_errno);
	va_end(ap);
}

noreturn void
xdie(bool with_errno, const char fmt[restrict static 1], ...)
{
	va_list ap;
	va_start(ap, fmt);
	vreportf("fatal", fmt, ap, with_errno);
	va_end(ap);
	exit(EXIT_FAILURE);
}
