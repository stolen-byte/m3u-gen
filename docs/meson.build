# SPDX-License-Identifier: GPL-3.0-or-later
mansrc = 'm3u-gen.adoc'

# where are my fucking globs!
docdeps = files(
	'includes/format-string.adoc',
	'includes/links.adoc',
	'includes/usage.adoc',
)

# =============================================================================
adoc = find_program('asciidoctor', native: true, required: true)
adoc_exts = ['extensions.rb']
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
custom_target(
	command: adoc_cmd + ['-b', 'manpage', '-o', '@OUTPUT@', '@INPUT@'],
	input: mansrc,
	output: '@0@.1'.format(fs.stem(mansrc)),
	depend_files: docdeps,
	install: true,
	install_dir: get_option('mandir') / 'man1',
)
