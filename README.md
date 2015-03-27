allegro\_tiled
==============

allegro\_tiled is an [Allegro 5](http://alleg.sourceforge.net/) module that adds support for parsing and using [TMX](https://github.com/bjorn/tiled/wiki/TMX-Map-Format) map files. This means that [Tiled](http://www.mapeditor.org), as well as any other program supporting the same map format, can be used to create and edit game worlds while the complexity of parsing and drawing them is completely taken care of:

```c
ALLEGRO_MAP *map = al_open_map("data/maps", "level1.tmx");
al_draw_map(map, 0, 0, 0); // (map, dx, dy, flags)
```

Additional methods are provided to draw one map layer at a time and/or draw only a particular section. Layers, tilesets, tiles, and objects are all given types, with methods that make them easy to access and use.

![screenshot](https://github.com/dradtke/allegro_tiled/raw/master/example/screenshot.png)

Currently, the following is supported:

1. Orthogonal maps.
2. Base64 encoding with gzip, zlib, or no compression.
3. XML and CSV encoding. (though honestly, why would you?)
4. Tile "flipped" flags, both vertically and horizontally.
5. Objects.

The following is not yet supported:

1. Isometric maps.

Dependencies
============

Linux:
------

Make sure the following dependencies are installed:

 * allegro
 * allegro\_image
 * xml2
 * zlib
 * glib
 * cmake

OSX:
----

Make sure the following dependencies are installed using [Homebrew](http://brew.sh/):

 * allegro5
 * libxml2
 * zlib
 * glib
 * cmake

Windows:
--------

Make sure the following dependencies are installed. Since there's no package manager, each one will need to be downloaded separately, and a `*_HOME` environment variable will need to be set to its location on your system; alternatively you *can* use `pkg-config` on Windows since it will check for that first, but unless you already have it installed, it's easier to use the following approach. Each dependency links to the download page and specifies which environment variable it expects to be set, excluding CMake since it's the executable used to process the build.

 * [Allegro 5](https://www.allegro.cc/files/) (`ALLEGRO_HOME`; also need to set `ALLEGRO_VERSION` to the installed version)
 * [libxml2](ftp://xmlsoft.org/libxml2/win32/) (`LIBXML_HOME`)
 * [zlib](http://www.zlib.net/) (`ZLIB_HOME`)
 * [GLib](http://www.gtk.org/download/win32.php) (`GLIB_HOME`) (not GTK+, just the GLib individual dev package)
 * [CMake](http://www.cmake.org/download/)

In addition to this, you'll need to have a C/C++ compiler installed. I had issues getting it to build with Visual Studio (the Visual Studio compiler doesn't play nicely with GLib in particular), but it seems to work fine with MinGW or MSYS.

Compiling
=========

If you wish to disable building the example, simply run cmake with `-DWANT_EXAMPLE=Off` before building the library.

CMake will configure the build for a static library by default. If you would like to build a shared library instead, add `-DBUILD_SHARED_LIBS` to the cmake command below.

Linux / OSX
-----------

Create a new directory called `build`, cd into it, then simply run `cmake ..` followed by `make` to compile it, and optionally `sudo make install` to handle installation. To run the example, cd to the examples folder and type `LD_LIBRARY_PATH=.. ./example`. Use the arrow keys to scroll and Space to reload the map file.

Windows:
--------

Same steps as Linux / OSX, except you'll need to specify that the generator be either MinGW or MSYS, whichever you have installed, so the command becomes one of `cmake -G "MinGW Makefiles" ..` or `cmake -G "MSYS Makefiles" ..`.

On Other Platorms:
------------------

Theoretically it should build fine on any platform for which all of the dependencies are available (either installable from source or a pre-compiled binary) and has support from CMake. Bug reports and pull requests for other platforms are welcome.
