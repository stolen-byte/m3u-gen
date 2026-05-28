/* SPDX-License-Identifier: GPL-3.0-or-later */
#pragma once
#include "common.h"
#include "strbuf.h"

// =============================================================================
typedef struct {
	strbuf path;
} dir_iterator;

typedef enum {
	DIF_NONE = 0,
	DIF_RECURSIVE = (1 << 0),
	DIF_PEDANTIC = (1 << 1),
	DIF_ALL = DIF_RECURSIVE | DIF_PEDANTIC,
} di_flags;

// =============================================================================
dir_iterator*
dir_iterator_begin(const char path[static 1], di_flags flags);

void
dir_iterator_end(dir_iterator iter[static 1]);

bool
dir_iterator_next(dir_iterator iter[static 1]);
