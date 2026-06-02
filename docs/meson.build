# SPDX-License-Identifier: GPL-3.0-or-later
mansrc = 'm3u-gen.adoc'
docsrc = 'index.adoc'

# where are my fucking globs!
docdeps = files(
	'includes/format-string.adoc',
	'includes/license.adoc',
	'includes/links.adoc',
	'includes/usage.adoc',
)

# =============================================================================
adoc = find_program('asciidoctor', native: true, required: true)
adoc_exts = ['extensions.rb', 'asciidoctor-pdf']
adoc_ddt = run_command('date', '-u', '+%Y-%m-%d %H:%M:%S', check: true).stdout().strip()
adoc_cmd = [
	adoc,
	'-a', 'webfonts!',
	'-a', 'revnumber=@0@'.format(meson.project_version()),
	'-a', 'docdatetime=@0@'.format(adoc_ddt),
]

foreach ext : adoc_exts
	ext_path = meson.current_source_dir() / ext
	if not fs.exists(ext_path)
		ext_path = ext
	endif
	adoc_cmd += '-r' + ext_path
endforeach

# =============================================================================
doc_targets = []
todo = get_option('docs')

docdir = get_option('docdir')
if docdir == ''
	docdir = get_option('datadir') / 'doc' / 'm3u-gen'
endif

if todo.contains('man')
	doc_targets += custom_target(
		command: adoc_cmd + ['-b', 'manpage', '-o', '@OUTPUT@', '@INPUT@'],
		input: mansrc,
		output: '@0@.1'.format(fs.stem(mansrc)),
		depend_files: docdeps,
		install: true,
		install_dir: get_option('mandir') / 'man1',
	)
endif

if todo.contains('pdf')
	doc_targets += custom_target(
		command: adoc_cmd + ['-b', 'pdf', '-a', 'compress', '-o', '@OUTPUT@', '@INPUT@'],
		input: docsrc,
		output: 'm3u-gen.pdf',
		depend_files: docdeps,
		install: true,
		install_dir: docdir,
	)
endif

if todo.contains('html')
	doc_targets += custom_target(
		command: adoc_cmd + ['-b', 'html5', '-o', '@OUTPUT@', '@INPUT@'],
		input: docsrc,
		output: 'm3u-gen.html',
		depend_files: docdeps,
		install: true,
		install_dir: docdir,
	)
endif

if doc_targets.length() > 0
	alias_target('docs', doc_targets)
endif
