/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "common.h"

#include <stdio.h>
#include <stdlib.h>

// =============================================================================
int
main(int argc, char* argv[])
{
	(void)argc;
	printf("%s v%s\n", argv[0], PROG_VERSION);
	return EXIT_SUCCESS;
}
