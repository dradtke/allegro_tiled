allegro\_tiled
==============

allegro\_tiled is an [Allegro 5](http://alleg.sourceforge.net/) module that adds support for parsing [TMX](https://github.com/bjorn/tiled/wiki/TMX-Map-Format) map files, plus some helper methods for drawing them to the screen.

Currently, the following is supported:

1. Orthogonal maps.
2. Base64 encoding with gzip, zlib, or no compression.
3. XML and CSV encoding. (though honestly, why would you?)
4. Tile "flipped" flags, both vertically and horizontally.
5. Objects (untested).

The following is not yet supported:

1. Isometric maps.

Compiling the Library
=====================

On Linux:
---------

Make sure the following dependencies are installed:

 * allegro
 * allegro\_image
 * xml2
 * zlib

Then simply run `make` in the root folder to compile it.

On Other Platorms:
------------------

No other platforms are supported yet, but this should change in the future. If you want to have this module available on your platform ASAP, then let me know!

Running the Example
===================

To run one of the examples, compile the module, cd to the example's root folder and use `make run` to compile and run it. Here's the first example in action:

![screenshot](https://github.com/dradtke/allegro_tiled/raw/master/example/ex1/screenshot.png)
