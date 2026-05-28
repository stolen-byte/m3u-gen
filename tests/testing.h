/* SPDX-License-Identifier: GPL-3.0-or-later */
#pragma once
#include <ctype.h>
#include <inttypes.h>
#include <math.h> // IWYU pragma: keep
#include <stdbool.h>
#include <stdio.h> // IWYU pragma: keep
#include <string.h>

// =============================================================================
#define MAX_ERROR_LEN 1024
#define FLOAT_EPSILON 01E-12

typedef struct test_state {
	int status;
	char last_error[MAX_ERROR_LEN];
} test_state;

typedef struct test_case {
	const char* name;
	void (*run)(test_state* s);
} test_case;

// =============================================================================
extern const test_case g_tests[];
extern const char* g_suite;

// =============================================================================
#define PP_CONCAT_(a, b) a##b
#define PP_CONCAT(a, b)  PP_CONCAT_(a, b)

#define PP_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N

#define PP_NARG_(...) PP_ARG_N(__VA_ARGS__)
#define PP_NARG(...)  PP_NARG_(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define TFUNC_(name)   test_##name
#define TENTRY_(name)  {#name, TFUNC_(name)}
#define MAP_1(x)       TENTRY_(x)
#define MAP_2(x, ...)  TENTRY_(x), MAP_1(__VA_ARGS__)
#define MAP_3(x, ...)  TENTRY_(x), MAP_2(__VA_ARGS__)
#define MAP_4(x, ...)  TENTRY_(x), MAP_3(__VA_ARGS__)
#define MAP_5(x, ...)  TENTRY_(x), MAP_4(__VA_ARGS__)
#define MAP_6(x, ...)  TENTRY_(x), MAP_5(__VA_ARGS__)
#define MAP_7(x, ...)  TENTRY_(x), MAP_6(__VA_ARGS__)
#define MAP_8(x, ...)  TENTRY_(x), MAP_7(__VA_ARGS__)
#define MAP_9(x, ...)  TENTRY_(x), MAP_8(__VA_ARGS__)
#define MAP_10(x, ...) TENTRY_(x), MAP_9(__VA_ARGS__)

// NOLINTNEXTLINE(misc-unused-parameters)
#define TEST(name) static void TFUNC_(name)(test_state * s)

#define DECLARE_TESTS(suite, ...)                       \
	const char* g_suite = (suite);                        \
	const test_case g_tests[] = {                         \
	  PP_CONCAT(MAP_, PP_NARG(__VA_ARGS__))(__VA_ARGS__), \
	  {NULL, NULL},                                       \
	};

#define SAFECHAR(c) isprint(c) ? (c) : '?'

// =============================================================================
#define fail(msg)                                                                    \
	do {                                                                               \
		s->status = 1;                                                                   \
		snprintf(s->last_error, MAX_ERROR_LEN, "%s:%d:\n  %s", __FILE__, __LINE__, msg); \
		return;                                                                          \
	} while (0)

#define fail_fmt(fmt, ...)                                                                     \
	do {                                                                                         \
		s->status = 1;                                                                             \
		snprintf(s->last_error, MAX_ERROR_LEN, "%s:%d:\n  " fmt, __FILE__, __LINE__, __VA_ARGS__); \
		return;                                                                                    \
	} while (0)

#define require(test)                          \
	do {                                         \
		if (!(test)) {                             \
			fail_fmt("assertion failed: %s", #test); \
		}                                          \
	} while (0)

#define require_eq(got, want, fmt)         \
	do {                                     \
		if ((got) != (want)) {                 \
			fail_fmt(fmt " != " fmt, got, want); \
		}                                      \
	} while (0)

#define require_streq(got, want)          \
	do {                                    \
		const char* a = (got);                \
		const char* b = (want);               \
		if (!a)                               \
			a = "<null>";                       \
		if (!b)                               \
			b = "<null>";                       \
		if (strcmp(a, b) != 0) {              \
			fail_fmt("\"%s\" != \"%s\"", a, b); \
		}                                     \
	} while (0)

#define require_feq(got, want)                                  \
	do {                                                          \
		double _a = (got);                                          \
		double _b = (want);                                         \
		if (fabs(_a - _b) > FLOAT_EPSILON) {                        \
			fail_fmt("%.13f != %.13f (+-%g)", _a, _b, FLOAT_EPSILON); \
		}                                                           \
	} while (0)
