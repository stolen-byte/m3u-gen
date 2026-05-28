# SPDX-License-Identifier: GPL-3.0-or-later
unit_tests = [
	'dir-tests.c',
	'strbuf-tests.c',
	'string-utils-tests.c',
]

# =============================================================================
test_includes = include_directories('.')

# STILL no object libraries...ffs
_test_lib = static_library(
	'tmain',
	files('tmain.c'),
	build_by_default: false,
	install: false,
)

foreach test : unit_tests
	name = test.replace('.c', '')
	test(
		name,
		executable(
			name,
			files(test),
			objects: [_test_lib.extract_all_objects(recursive: true), core_objects],
			include_directories: [test_includes, core_includes],
			dependencies: deps,
		),
	)
endforeach
