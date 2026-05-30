/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "common.h"
#include "compat/stat.h"
#include "compat/string.h"
#include "dir-iterator.h"
#include "dir.h"
#include "error.h"
#include "m3u.h"

#include <stdio.h>
#include <unistd.h>

// =============================================================================
typedef struct {
	FILE* restrict out;
	const char* title;
	di_flags flags;
} options;

// =============================================================================
static noreturn void
usage(int status, const char* name)
{
	printf("usage: %s [OPTION...] <URL>...\n"
	       "generate m3u playlist from URL(s).\n"
	       "\n"
	       "options:\n"
	       "  -o PATH    output file. (default: stdout)\n"
	       "  -t TITLE   playlist title.\n"
	       "  -r         recurse into subdirectories.\n"
	       "  -p         pedantic mode (stop on first error/warning).\n"
	       "  -h         display this help and exit.\n"
	       "  -V         display version information and exit.\n",
	       name);
	exit(status);
}

static bool
add_file(m3u_list list[restrict static 1], const char path[restrict static 1])
{
	m3u_entry* entry = m3u_push(list, path);
	return entry != NULL;
}

static bool
add_dir(m3u_list list[restrict static 1],
        const char path[restrict static 1],
        const options opts[restrict static 1])
{
	dir_iterator* it = dir_iterator_begin(path, opts->flags);
	if (UNLIKELY(!it))
		return false;

	bool ret = true;
	register bool pedantic = opts->flags & DIF_PEDANTIC;
	while (dir_iterator_next(it)) {
		if (!add_file(list, it->path.data) && pedantic) {
			ret = false;
			break;
		}
	}

	dir_iterator_end(it);
	return ret;
}

static bool
generate_playlist(m3u_list list[restrict static 1],
                  size_t n,
                  char* const urls[restrict static n],
                  const options opts[restrict static 1])
{
	struct stat st;

	for (size_t i = 0; i < n; ++i) {
		const char* url = urls[i];
		bool ret = false;

		int err = lstat(url, &st);
		if (err) {
			if (errno == ENOENT) {
				// assume this is a remote url
				ret = m3u_push(list, url) != NULL;
			} else {
				eerror("failed to stat '%s'", url);
			}
		} else {
			if (S_ISDIR(st.st_mode))
				ret = add_dir(list, url, opts);
			else if (S_ISREG(st.st_mode))
				ret = add_file(list, url);
			else
				warn("skipping '%s': not a file/directory.", url);
		}

		if (!ret && (opts->flags & DIF_PEDANTIC))
			return false;
	}

	return true;
}

int
main(int argc, char* argv[])
{
	const char* outpath = NULL;
	options opts = {
	  .out = stdout,
	};

	int opt;
	while ((opt = getopt(argc, argv, ":hVo:t:rp")) != -1) {
		switch (opt) {
		case 'o': outpath = optarg; break;
		case 't': opts.title = optarg; break;
		case 'r': opts.flags |= DIF_RECURSIVE; break;
		case 'p': opts.flags |= DIF_PEDANTIC; break;
		case 'h': usage(EXIT_SUCCESS, basename(argv[0]));
		case 'V': printf("m3u-gen v%s\n\n", PROG_VERSION); return EXIT_SUCCESS;
		case ':': die("option '%c' requires an argument", optopt);
		case '?': die("unknown option '%c'", optopt);
		default:
#ifdef DEBUG
			die("BUG: unhandled option: '%c'", optopt);
#else
			UNREACHABLE();
#endif
		}
	}

	if (argc <= optind)
		die("no inputs specified...");

	int status = EXIT_FAILURE;
	if (outpath) {
		opts.out = fopen(outpath, "wb");
		if (!opts.out)
			edie("%s", outpath);
	}

	m3u_list* list = m3u_create(opts.title, 0);
	if (generate_playlist(list, (size_t)(argc - optind), argv + optind, &opts)) {
		m3u_write(list, opts.out);
		status = EXIT_SUCCESS;
	}

	m3u_free(list);
	if (outpath)
		fclose(opts.out);

	return status;
}
