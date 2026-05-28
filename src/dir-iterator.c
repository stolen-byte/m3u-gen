/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "dir-iterator.h"
#include "compat/stat.h"
#include "compat/string.h"
#include "dir.h"
#include "error.h"
#include "mem.h"

#include <dirent.h>

// =============================================================================
typedef struct {
	DIR* dir;
	size_t parent_len;
} dir_level;

typedef struct {
	dir_iterator base;
	dir_level* levels;
	size_t nlevels;
	size_t cap;
	di_flags flags;
} di_impl;

// =============================================================================
#define IS_RECURSIVE(f) ((f) & DIF_RECURSIVE)
#define IS_PEDANTIC(f)  ((f) & DIF_PEDANTIC)

/**
 * returns: -1 on error, 0 on entry read, and 1 when exhausted
 */
static int
next_entry(DIR* restrict dir, const char* restrict path, struct dirent* restrict* out)
{
	register struct dirent* de;

	do {
		errno = 0;
		de = readdir(dir);
		if (!de) {
			if (UNLIKELY(errno)) {
				eerror("error reading directory '%s'", path);
				return -1;
			}
			return 1;
		}
	} while (is_dot_or_dotdot(de->d_name));

	*out = de;
	return 0;
}

static bool
push_level(di_impl* impl)
{
	assert(impl != NULL);

	grow_array(impl->levels, impl->nlevels + 1, impl->cap);

	strbuf* path = &impl->base.path;
	if (!is_dir_sep(path->data[path->len - 1]))
		strbuf_add(path, '/');

	register dir_level* level = &impl->levels[impl->nlevels++];
	level->parent_len = path->len;
	level->dir = opendir(path->data);

	if (UNLIKELY(!level->dir)) {
		if (errno != ENOENT)
			ewarn("error opening directory '%s'", path->data);

		impl->nlevels--;
		return false;
	}

	return true;
}

static size_t
pop_level(di_impl* impl)
{
	register dir_level* level = &impl->levels[impl->nlevels - 1];
	if (level->dir && UNLIKELY(closedir(level->dir)))
		ewarn("error closing directory '%s'", impl->base.path.data);

	level->dir = NULL;
	return --impl->nlevels;
}

dir_iterator*
dir_iterator_begin(const char path[static 1], di_flags flags)
{
	register di_impl* impl = xcalloc(1, sizeof(*impl));
	dir_iterator* iter = &impl->base;

	strbuf_init(&iter->path, PATH_MAX);
	strbuf_set(&iter->path, path);
	impl->flags = flags;

	// try and save some memory when not recursive (ie: we need only 1 entry)
	if (IS_RECURSIVE(impl->flags)) {
		grow_array(impl->levels, 10, impl->cap);
	} else {
		impl->levels = xcalloc(1, sizeof(*impl->levels));
		impl->cap = 1;
	}

	if (UNLIKELY(!push_level(impl))) {
		dir_iterator_end(iter);
		return NULL;
	}

	return iter;
}

void
dir_iterator_end(dir_iterator iter[static 1])
{
	register di_impl* impl = (di_impl*)iter;

	for (; impl->nlevels; --impl->nlevels) {
		register dir_level* level = &impl->levels[impl->nlevels - 1];
		if (level->dir && closedir(level->dir)) {
			strbuf_resize(&iter->path, level->parent_len);
			ewarn("error closing directory '%s'", iter->path.data);
		}
	}

	free_array(impl->levels, impl->cap);
	strbuf_free(&iter->path);
	xfree(impl);
}

bool
dir_iterator_next(dir_iterator iter[static 1])
{
	struct stat st;
	register di_impl* impl = (di_impl*)iter;
	register bool pedantic = IS_PEDANTIC(impl->flags);

	while (1) {
		struct dirent* de;
		register dir_level* level = &impl->levels[impl->nlevels - 1];

		strbuf_resize(&iter->path, level->parent_len);
		int ret = next_entry(level->dir, iter->path.data, &de);
		if (UNLIKELY(ret < 0)) {
			if (pedantic)
				return false;
			continue;
		}

		if (ret > 0) {
			if (pop_level(impl) == 0)
				return false;
			continue;
		}

		strbuf_append(&iter->path, de->d_name);
		int err = lstat(iter->path.data, &st);
		if (UNLIKELY(err)) {
			if (errno != ENOENT) {
				ewarn("failed to stat '%s'", iter->path.data);
				if (pedantic)
					return false;
			}
			continue;
		}

		if (S_ISDIR(st.st_mode)) {
			if (IS_RECURSIVE(impl->flags) && UNLIKELY(!push_level(impl))) {
				if (errno != ENOENT && pedantic)
					return false;
				if (impl->nlevels == 0)
					return false;
			}
			continue;
		}

		if (!S_ISREG(st.st_mode))
			continue;

		return true;
	}
}
