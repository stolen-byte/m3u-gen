# SPDX-License-Identifier: GPL-3.0-or-later
project(
	'm3u-gen',
	'c',
	version: run_command('scripts/gen-version', check: true).stdout().strip(),
	meson_version: '>= 1.4.0',
	default_options: [
		'buildtype=release',
		'c_std=c11',
		'warning_level=3',
		'b_lto=true',
	],
)

# =============================================================================
cc = meson.get_compiler('c')
bt = get_option('buildtype')
fs = import('fs')
sys = target_machine.system()

# =============================================================================
cflags = []
defs = [
	'-D_XOPEN_SOURCE=700',
	'-D_FILE_OFFSET_BITS=64',
	'-D_TIME_BITS=64',
]

if bt in ['release', 'debugoptimized']
	defs += '-DNDEBUG'
	cflags += cc.get_supported_arguments('-fstack-protector-strong', '-fstack-clash-protection')
	if target_machine.cpu_family() == 'x86_64'
		cflags += cc.get_supported_arguments('-fcf-protection')
	endif
	if bt == 'debugoptimized'
		cflags += '-fno-omit-frame-pointer'
	endif
elif bt == 'debug'
	defs += '-DDEBUG'
	if cc.get_id() in ['gcc', 'clang']
		cflags += [
			'-Wconversion',
			'-Wshadow',
			'-Wnull-dereference',
			'-Wdouble-promotion',
			'-Wunused',
			'-Wno-unused-parameter',
			'-Wcast-align',
			'-Winfinite-recursion',
			'-Wtype-limits',
			'-Wsign-compare',
			'-Wsign-conversion',
			'-Wformat=2',
			'-Wstrict-overflow=4',
			'-Wno-format-nonliteral',
		]
	endif
endif

if cc.get_id() == 'clang'
	cflags += '-Wno-unused-command-line-argument'
endif

if sys in ['cygwin', 'windows']
	defs += '-DPLATFORM_WINDOWS'
	cflags += [
		# disable this on windows, because MinGW's isnan/isfinite macros are buggy and
		# cause gcc to spew false warnings.
		# ref: https://sourceforge.net/p/mingw-w64/bugs/481/
		'-Wno-float-conversion',
	]

	if sys == 'windows'
		# avoid windows' broken non-standard printf implementations
		defs += '-D__USE_MINGW_ANSI_STDIO=1'
	endif
endif

deps = [
	dependency('libavformat', version: '>=60.16.100'),
	dependency('libavcodec', version: '>=60.31.102'),
	dependency('libavutil', version: '>=58.29.100'),
]

libm = cc.find_library('m', required: false)
if libm.found()
	deps += libm
endif

add_project_arguments(defs + cflags, language: 'c')

# =============================================================================
conf = configuration_data(
	{
		'VERSION': meson.project_version(),
	},
)

symbols = [
	['strlcpy', '#include <string.h>'],
	['lstat', '#include <sys/stat.h>'],
]

compat_sources = []
foreach sym : symbols
	result = cc.has_function(sym[0], args: defs, prefix: sym[1])
	if not result
		src = 'src/compat/@0@.c'.format(sym[0])
		if fs.exists(src)
			compat_sources += files(src)
		endif
	endif
	conf.set('HAVE_@0@'.format(sym[0].to_upper()), result)
endforeach

# =============================================================================
subdir('src')

executable(
	meson.project_name(),
	files('src/m3u-gen.c'),
	dependencies: deps,
	objects: core_objects,
	include_directories: core_includes,
	install: true,
)

if get_option('build-tests')
	subdir('tests')
endif

# =============================================================================
summary(
	{
		'build': bt,
		'version': meson.project_version(),
		'compiler': cc.get_id(),
	},
)
