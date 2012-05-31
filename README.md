This library adds support for [Tiled](http://www.mapeditor.org/) maps to the [Allegro](http://alleg.sourceforge.net/) game library. It is more or less functional, but is not quite yet ready for prime time.

Compiling the Library
=====================

Make sure the following dependencies are installed:

 * allegro
 * allegro\_image
 * xml2
 * zlib

Then simply run `make` in the root folder to compile it.

Running the Example
===================

There is one fully functional example provided under example/mario. To run it, first build the library, then cd to the example's folder and use `make run` to compile and run it:

![screenshot](https://github.com/dradtke/Allegro-Tiled/raw/master/example/ex1/screenshot.png)

The arrow keys will scroll the camera around the map, and pressing Space will reload data/maps/level1.tmx. Simply open up the map file in Tiled, make some changes, and press Space to see them appear instantly!
