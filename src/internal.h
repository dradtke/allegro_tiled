/*
 * This addon adds Tiled map support to the Allegro game library.
 * Copyright (c) 2012 Damien Radtke - www.damienradtke.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * For more information, visit http://www.gnu.org/copyleft
 */

#ifndef INTERNAL_H
#define INTERNAL_H

#include <ctype.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <zlib.h>

// map.h
char tile_id(TILED_MAP_LAYER *layer_ob, int x, int y);

// util.h
char *trim(char *str);
char *copy(const char *str);

// xml.h
_AL_LIST *get_children_for_name(xmlNode *parent, char *name);
xmlNode *get_first_child_for_name(xmlNode *parent, char *name);
char *get_xml_attribute(xmlNode *node, char *name);

// zlib.h
// Hack to make it work on Windows
// ???: Is this actually needed?
#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif

#define CHUNK 16384

int decompress(char *src, FILE *dest);

// custom list item destructors
void dtor_map_tile(void *value, void *user_data);
void dtor_map_tileset(void *value, void *user_data);
void dtor_map_layer(void *value, void *user_data);
void dtor_tile_prop(void *value, void *user_data);

#endif
