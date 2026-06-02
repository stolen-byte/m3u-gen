# m3u-gen

**m3u-gen** is a command-line tool for generating m3u/m3u8 playlists for a list of file/directory/urls.

---

## Usage

```
usage: m3u-gen [OPTION...] <URL>...

options:
  -o PATH    output file. (default: stdout)
  -s MODE    sort mode, one of 'url', 'title', 'duration' (default: url).
  -t TITLE   playlist title.
  -T FORMAT  playlist entry title format string.
  -n         skip reading metadata.
  -A         be more aggressive at scanning metadata.
  -r         recurse into subdirectories.
  -p         pedantic mode (stop on first error/warning).
  -h         display this help and exit.
  -V         display version information and exit.
```

`URL` can be any number of directory/urls, or any file readable by ffmpeg. Only local files will be queried
for metadata, and file paths are used as-is (eg: relative vs absolute), so keep this in mind when redirecting
the output to a file (or using the `-o` option).

Usually i would recommend either using absolute paths, or running the program from the same directory you
intend to store the resulting playlist in.

For more detailed usage, see [here](docs/includes/usage.adoc), and [here](docs/includes/format-string.adoc).

## Building

Because some distros refuse to package static ffmpeg libraries, and other distros have problems statically
linking ffmpeg, release binaries will not be available, and the program must be installed by your system's
package manager, or built from source.

To build, you will need:

- a working c11 compiler
- [Meson](https://mesonbuild.com/)
- [Ninja](https://ninja-build.org/) (usually installed alongside meson)

You will also need access to the following [FFmpeg](https://ffmpeg.org/) libraries:

- `libavformat`
- `libavcodec`
- `libavutil`

These should be installed by your system's package manager, when installing ffmpeg (or a `-dev` package
in the case of debian derivatives).

To configure:

```
$ meson setup --prefix=/usr build
```

To build & install

```
$ meson compile -C build
$ meson install -C build
```

or via ninja directly:

```
$ ninja -C build
$ ninja -C build install
```

To run the unit tests, use:

```
$ meson test -C build
```

Build/Configure options are detailed in [meson.options](meson.options), and can be specified via the
standard `-D<name>=<value>` syntax to `meson setup`, eg:

```
$ meson setup -Dbuild-tests=false build # to disable unit tests
```

Documentation & Manpages are built by default, and can be disabled completely by passing
`-Dbuild-docs=false` to `meson setup`, or customised by passing the wanted formats as a
comma-separated list via `-Ddocs=man,pdf,html`.

The main build/install targets are:

- `m3u-gen`: the program itself
- `docs`

Whilst meson itself doesn't provide a means of listing target names, one can be produced
manually using `jq`:

```
$ meson introspect --targets build | jq -r '.[].name'
```

## License

```
m3u-gen is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

m3u-gen is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
```

To see the full license text, see [LICENSE](LICENSE).
