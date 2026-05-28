# SPDX-License-Identifier: GPL-3.0-or-later
core_sources = files(
	'error.c',
	'mem.c',
	'strbuf.c',
	'string-utils.c',
)

core_includes = include_directories('.')

# =============================================================================
configure_file(
	input: files('config.h.in'),
	output: 'config.h',
	configuration: conf,
)

_core_lib = static_library(
	meson.project_name(),
	[core_sources, compat_sources],
	include_directories: core_includes,
	build_by_default: false,
	install: false,
)

core_objects = _core_lib.extract_all_objects(recursive: true)
