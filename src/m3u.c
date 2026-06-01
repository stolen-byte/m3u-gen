/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "compat/string.h"
#include "error.h"
#include "hash.h"
#include "m3u.h"
#include "mem.h"
#include "string-utils.h"

// =============================================================================
typedef int (*sortcmp)(m3u_entry*, m3u_entry*);

typedef struct {
	m3u_list base;
	m3u_entry* storage;
	size_t cap;
} list_impl;

typedef struct {
	uint64_t id;
	sortcmp cmp;
} sortmode;

// =============================================================================
// clang-format off
static int cmp_duration(m3u_entry* restrict, m3u_entry* restrict);
static int cmp_title(m3u_entry* restrict, m3u_entry* restrict);
static int cmp_url(m3u_entry* restrict, m3u_entry* restrict);
// clang-format on

#define SM_URL      0
#define SM_TITLE    1
#define SM_DURATION 2

static sortmode g_sortmodes[] = {
  [SM_URL] = {0xd82be2186b193788, cmp_url     },
  [SM_TITLE] = {MID_TITLE,          cmp_title   },
  [SM_DURATION] = {0xff9e2e8232a995f9, cmp_duration},
};

// =============================================================================
static int
cmp_duration(m3u_entry* restrict a, m3u_entry* restrict b)
{
	double da = a->duration;
	double db = b->duration;
	if (da < db)
		return -1;
	if (da > db)
		return 1;
	return 0;
}

static int
cmp_url(m3u_entry* a, m3u_entry* b)
{
	return strnatcmp(a->url, b->url);
}

static int
cmp_title(m3u_entry* a, m3u_entry* b)
{
	const char* ta = m3u_entry_title(a);
	const char* tb = m3u_entry_title(b);
	if (ta && !tb)
		return -1;
	if (!ta && tb)
		return 1;
	if (!ta && !tb)
		return 0;
	return strnatcmp(ta, tb);
}

static void
grow_list(register list_impl* impl, size_t n)
{
	size_t oldcap = impl->cap;
	m3u_entry* oldstorage = impl->storage;

	grow_array(impl->storage, n, impl->cap);
	if (impl->cap > oldcap)
		realloc_array(impl->base.entries, impl->cap);

	n = impl->base.len;
	if (n > 0) {
		// fixup entries
		ptrdiff_t diff = impl->storage - oldstorage;
		if (diff != 0) {
			register m3u_entry** arr = impl->base.entries;
			for (size_t i = 0; i < n; ++i)
				arr[i] += diff;
		}
	}
}

static m3u_entry*
init_entry(m3u_entry* restrict e, const char* restrict url)
{
	e->url = xstrdup(url);
	e->duration = -1;
	metadata_init(&e->metadata);
	strbuf_init(&e->title, 0);
	return e;
}

static void
free_entry(m3u_entry* e)
{
	strbuf_free(&e->title);
	metadata_free(&e->metadata);
	e->duration = -1;
	xfree((void*)e->url);
}

static const char*
parse_tag(const char* restrict* p,
          metadata_list* restrict meta,
          strbuf* restrict buf,
          const char* restrict fmt)
{
	const char* start = ++(*p);
	const char* end = strchr(start, ')');

	if (!end)
		die("no matching ')' in format string '%s'", fmt);
	if (end == start)
		die("expected tag name in format string '%s'", fmt);

	strbuf_resize(buf, 0);
	strbuf_appendn(buf, (size_t)(end - start), start);
	*p = end;

	return metadata_find(meta, buf->data);
}

m3u_list*
m3u_create(const char* title, size_t size)
{
	list_impl* list = xcalloc(1, sizeof(*list));
	m3u_list* base = &list->base;

	if (size)
		grow_list(list, size);

	base->title = title;
	return base;
}

void
m3u_free(m3u_list list[static 1])
{
	register list_impl* impl = (list_impl*)list;

	while (list->len)
		m3u_pop(list);

	xfree(list->entries);
	free_array(impl->storage, impl->cap);
	xfree(impl);
}

m3u_entry*
m3u_push(m3u_list list[restrict static 1], const char url[restrict static 1])
{
	register list_impl* impl = (list_impl*)list;

	grow_list(impl, list->len + 1);

	m3u_entry* entry = init_entry(&impl->storage[list->len++], url);
	list->entries[list->len - 1] = entry;

	return entry;
}

void
m3u_pop(m3u_list list[restrict static 1])
{
	free_entry(list->entries[--list->len]);
}

void
m3u_write(const m3u_list list[restrict static 1], FILE* restrict out)
{
	// header
	fprintf(out, "#EXTM3U\n#EXTENC:UTF-8\n");
	if (list->title)
		fprintf(out, "#PLAYLIST:%s\n", list->title);

	// entries
	for (size_t i = 0; i < list->len; ++i) {
		register m3u_entry* entry = list->entries[i];

		const char* title = m3u_entry_title(entry);
		if (title)
			fprintf(out, "#EXTINF:%.3f,%s\n", entry->duration, title);

		fprintf(out, "%s\n", entry->url);
	}

	fflush(out);
}

void
m3u_sort(m3u_list list[static 1], int mode)
{
	assert(mode < (int)ARRAYSIZE(g_sortmodes));

	if (list->len < 1)
		return;
	if (mode == -1)
		mode = SM_URL;

	sortcmp cmp = g_sortmodes[mode].cmp;

	// simple insertion sort, large playlists are unlikely, and:
	// - quicksort has partitioning overhead, and loses out to this
	// - mergesort is stable, but has extra memory overhead & complexity
	register m3u_entry** arr = list->entries;
	for (register size_t i = 1; i < list->len; ++i) {
		register m3u_entry* k = arr[i];
		register size_t j = i;
		while (j > 0 && cmp(arr[j - 1], k) > 0) {
			swap(arr[j], arr[j - 1]);
			--j;
		}
		arr[j] = k;
	}
}

int
m3u_get_sortmode(const char name[static 1])
{
	uint64_t id = strhash(name);
	for (size_t i = 0; i < ARRAYSIZE(g_sortmodes); ++i) {
		if (g_sortmodes[i].id == id)
			return (int)i;
	}
	return -1;
}

const char*
m3u_entry_title(const m3u_entry entry[static 1])
{
	if (entry->title.len)
		return entry->title.data;
	return metadata_get(&entry->metadata, MID_TITLE);
}

void
m3u_format_title(m3u_entry entry[restrict static 1],
                 const char fmt[restrict static 1],
                 strbuf* restrict scratch)
{
	strbuf buf;
	strbuf* sb = &entry->title;
	metadata_list* meta = &entry->metadata;

	strbuf_init(&buf, 0);
	if (!scratch) {
		strbuf_grow(&buf, PATH_MAX);
		scratch = &buf;
	} else {
		strbuf_resize(scratch, 0);
	}

	strbuf_resize(sb, 0);

	if (!*fmt)
		return;

	const char* prev = fmt;
	while (*prev) {
		const char* p = strchr(prev, '%');
		if (!p)
			break;

		strbuf_appendn(sb, (size_t)(p - prev), prev);

		const char* value = NULL;
		switch (*++p) {
		case '%': strbuf_add(sb, *p); break;
		case 'A': value = metadata_get(meta, MID_ALBUMARTIST); break;
		case 'a': value = metadata_get(meta, MID_ARTIST); break;
		case 'T': value = metadata_get(meta, MID_ALBUM); break;
		case 't': value = metadata_get(meta, MID_TITLE); break;
		case 'N': value = metadata_get(meta, MID_TOTALPARTS); break;
		case 'e': value = metadata_get(meta, MID_EPISODE); break;
		case 's': value = metadata_get(meta, MID_SEASON); break;
		case 'v': value = metadata_get(meta, MID_VOLUME); break;
		case 'c': value = metadata_get(meta, MID_CHAPTER); break;
		case 'g': value = metadata_get(meta, MID_GENRE); break;
		case 'Y': value = metadata_get(meta, MID_YEAR); break;
		case 'n':
			value = metadata_get(meta, MID_TRACK);
			if (!value)
				value = metadata_get(meta, MID_PARTNUMBER);
			break;

		// full filename
		case 'F':
			strbuf_set(scratch, entry->url);
			value = strbuf_basename(scratch);
			break;

		// filename without extension
		case 'f':
			strbuf_set(scratch, entry->url);
			strbuf_strip_ext(scratch);
			value = strbuf_basename(scratch);
			break;

		// custom tag
		case '(':
			value = parse_tag(&p, meta, scratch, fmt);
			if (!value)
				warn("no tag found with name '%s'", scratch->data);
			break;

		// errors
		case '\0': die("expected format specifier in '%s' at index %zu", fmt, (size_t)(p - fmt));
		default:   die("unknown format specifier '%c' in string '%s'.", *p, fmt);
		}

		if (value)
			strbuf_append(sb, value);

		prev = ++p;
	}

	if (*prev)
		strbuf_append(sb, prev);

	strbuf_free(&buf);
}
