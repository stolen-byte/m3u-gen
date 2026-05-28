/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "testing.h"

#include <stdio.h>
#include <stdlib.h>

// =============================================================================
#define foreach_test(var) for (const test_case*(var) = g_tests; (var)->run; ++(var))

static const char* g_ok = "\033[32mok\033[0m";
static const char* g_fail = "\033[31mfail\033[0m";

// =============================================================================
// asan implicitly includes & enables the leak sanitizer, which is stupid, we didn't ask for it.
extern const char*
__asan_default_options(void) // NOLINT
{
	return "detect_leaks=0";
}

int
main(void)
{
	test_state state = {0};
	int failed = 0;

	foreach_test (ptest) {
		printf("%s::%s ... ", g_suite, ptest->name);

		state.status = 0;
		ptest->run(&state);
		if (state.status) {
			printf("%s\n%s\n\n", g_fail, state.last_error);
			++failed;
		} else {
			printf("%s\n", g_ok);
		}
	}

	return failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
