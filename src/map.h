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

#ifndef _MAP_H
#define _MAP_H

#include <allegro5/allegro_tiled.h>
#include <stdio.h>
#include "data.h"

// Bits on the far end of the 32-bit global tile ID are used for tile flags
#define FLIPPED_HORIZONTALLY_FLAG	0x80000000
#define FLIPPED_VERTICALLY_FLAG		0x40000000
#define FLIPPED_DIAGONALLY_FLAG		0x20000000

char al_get_single_tile(TILED_MAP_LAYER *layer, int x, int y);
char* al_get_tiles(TILED_MAP *map, int x, int y);
bool flipped_horizontally(TILED_MAP_LAYER *layer, int x, int y);
bool flipped_vertically(TILED_MAP_LAYER *layer, int x, int y);
char *al_get_tile_property(TILED_MAP_TILE *tile, char *name, char *def);
char *al_get_object_property(TILED_OBJECT *object, char *name, char *def);

int al_map_get_pixel_width(TILED_MAP *map);
int al_map_get_pixel_height(TILED_MAP *map);

void dtor_map_tile(void *value, void *user_data);
void dtor_map_tileset(void *value, void *user_data);
void dtor_map_layer(void *value, void *user_data);
void dtor_map_object(void *value, void *user_data);
void dtor_map_object_group(void *value, void *user_data);
void dtor_prop(void *value, void *user_data);

#endif
