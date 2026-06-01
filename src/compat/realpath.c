/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "common.h"

#include <errno.h>
#include <unistd.h>

// =============================================================================
#if defined(PLATFORM_WINDOWS)
char*
realpath(const char* restrict name, char resolved[restrict PATH_MAX])
{
	if (!name) {
		errno = EINVAL;
	} else if (access(name, R_OK) == 0) {
		// not 100% POSIX compliant, as since we will never pass NULL as `resolved`,
		// we don't need to `malloc`.
		char* p = _fullpath(resolved, name, PATH_MAX);
		if (!p) {
			errno = ENAMETOOLONG;
		}
	}

	return resolved;
}
#else
	#error "realpath not defined for this platform"
#endif
