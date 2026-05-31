/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "m3u.h"
#include "compat/string.h"
#include "mem.h"

// =============================================================================
typedef struct {
	m3u_list base;
	m3u_entry* storage;
	size_t cap;
} list_impl;

// =============================================================================
static int
cmp_url(m3u_entry* a, m3u_entry* b)
{
	return strcmp(a->url, b->url);
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
	m3u_entry* entry = &impl->storage[list->len++];

	entry->url = xstrdup(url);
	list->entries[list->len - 1] = entry;

	return entry;
}

void
m3u_pop(m3u_list list[restrict static 1])
{
	register m3u_entry* e = list->entries[--list->len];
	xfree((void*)e->url);
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
		fprintf(out, "%s\n", entry->url);
	}

	fflush(out);
}

void
m3u_sort(m3u_list list[static 1])
{
	if (list->len < 1)
		return;

	// simple insertion sort, large playlists are unlikely, and:
	// - quicksort has partitioning overhead, and loses out to this
	// - mergesort is stable, but has extra memory overhead & complexity
	register m3u_entry** arr = list->entries;
	for (register size_t i = 1; i < list->len; ++i) {
		register m3u_entry* k = arr[i];
		register size_t j = i;
		while (j > 0 && cmp_url(arr[j - 1], k) > 0) {
			swap(arr[j], arr[j - 1]);
			--j;
		}
		arr[j] = k;
	}
}
