/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "strbuf.h"
#include "compat/string.h"
#include "error.h"
#include "mem.h"

#include <assert.h>
#include <stdarg.h>

// =============================================================================
static char slop[1] = {0};

void
strbuf_init(strbuf sb[static 1], size_t hint)
{
	sb->cap = sb->len = 0;
	sb->data = slop;
	if (hint)
		strbuf_grow(sb, hint);
}

void
strbuf_free(strbuf sb[static 1])
{
	if (LIKELY(sb->cap)) {
		xfree(sb->data);
		strbuf_init(sb, 0);
	}
}

void
strbuf_grow(strbuf sb[static 1], size_t size)
{
	int isnew = !sb->cap;
	if (UNLIKELY(unsigned_add_overflows(sb->len, size + 1)))
		die("unsigned overflow in strbuf_extend");

	if (isnew)
		sb->data = NULL;

	grow_array(sb->data, sb->len + size + 1, sb->cap);

	if (isnew) {
		sb->data[0] = '\0';
	}
}

size_t
strbuf_avail(const strbuf sb[static 1])
{
	return (sb->data != slop) ? (sb->cap - sb->len - 1) : 0;
}

void
strbuf_resize(strbuf sb[static 1], size_t newlen)
{
	if (UNLIKELY(newlen > sb->cap))
		die("BUG: cannot grow a strbuf with strbuf_resize");

	sb->len = newlen;
	if (sb->data != slop) {
		sb->data[newlen] = '\0';
	} else {
		assert(!slop[0]);
	}
}

void
strbuf_reset(strbuf sb[static 1])
{
	strbuf_resize(sb, 0);
}

void
strbuf_set(strbuf sb[restrict static 1], const char str[restrict static 1])
{
	sb->len = 0;
	strbuf_append(sb, str);
}

void
strbuf_add(strbuf sb[static 1], char ch)
{
	if (!strbuf_avail(sb))
		strbuf_grow(sb, 1);
	sb->data[sb->len++] = ch;
	sb->data[sb->len] = '\0';
}

void
strbuf_addn(strbuf sb[static 1], char ch, size_t n)
{
	if (!n)
		return;
	strbuf_grow(sb, n);
	memset(sb->data + sb->len, ch, n);
	strbuf_resize(sb, sb->len + n);
}

void
strbuf_append(strbuf sb[restrict static 1], const char str[restrict static 1])
{
	strbuf_appendn(sb, strlen(str), str);
}

void
strbuf_appendn(strbuf sb[restrict static 1], size_t len, const char str[restrict len])
{
	if (!len)
		return;
	strbuf_grow(sb, len);
	memcpy(sb->data + sb->len, str, len);
	strbuf_resize(sb, sb->len + len);
}

void
strbuf_vappendf(strbuf sb[restrict static 1], const char fmt[restrict static 1], va_list args)
{
	if (!strbuf_avail(sb))
		strbuf_grow(sb, 64);

	va_list cp;
	va_copy(cp, args);
	int len = vsnprintf(sb->data + sb->len, sb->cap - sb->len, fmt, cp);
	va_end(cp);

	if (UNLIKELY(len < 0)) {
		die("unable to format message: %s", fmt);
	}

	if ((size_t)len > strbuf_avail(sb)) {
		strbuf_grow(sb, (size_t)len);
		len = vsnprintf(sb->data + sb->len, sb->cap - sb->len, fmt, args);
		if ((size_t)len > strbuf_avail(sb)) {
			die("BUG: program was compiled with a broken vsnprintf");
		}
	}

	strbuf_resize(sb, sb->len + (size_t)len);
}

void
strbuf_appendf(strbuf sb[restrict static 1], const char fmt[restrict static 1], ...)
{
	va_list ap;
	va_start(ap, fmt);
	strbuf_vappendf(sb, fmt, ap);
	va_end(ap);
}

int
strbuf_cmp(const strbuf a[restrict static 1], const strbuf b[restrict static 1])
{
	return strcmp(a->data, b->data);
}

void
strbuf_swap(strbuf a[restrict static 1], strbuf b[restrict static 1])
{
	swap(*a, *b);
}
