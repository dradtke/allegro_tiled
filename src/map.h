/*
 * This addon adds Tiled map support to the Allegro game library.
 * Copyright (c) 2012-2014 Damien Radtke - www.damienradtke.com
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *    1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 *    2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 *    3. This notice may not be removed or altered from any source
 *    distribution.
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

char al_get_single_tile_id(ALLEGRO_MAP_LAYER *layer, int x, int y);
ALLEGRO_MAP_TILE *al_get_single_tile(ALLEGRO_MAP *map, ALLEGRO_MAP_LAYER *layer, int x, int y);
ALLEGRO_MAP_TILE **al_get_tiles(ALLEGRO_MAP *map, int x, int y, int *length);
ALLEGRO_MAP_OBJECT **al_get_objects(ALLEGRO_MAP_LAYER *layer, int *length);
ALLEGRO_MAP_OBJECT **al_get_objects_for_name(ALLEGRO_MAP_LAYER *layer, char *name, int *length);
bool flipped_horizontally(ALLEGRO_MAP_LAYER *layer, int x, int y);
bool flipped_vertically(ALLEGRO_MAP_LAYER *layer, int x, int y);
char *al_get_tile_property(ALLEGRO_MAP_TILE *tile, char *name, char *def);
char *al_get_object_property(ALLEGRO_MAP_OBJECT *object, char *name, char *def);
ALLEGRO_MAP_LAYER *al_get_layer_for_name(ALLEGRO_MAP *map, char *name);

int al_map_get_pixel_width(ALLEGRO_MAP *map);
int al_map_get_pixel_height(ALLEGRO_MAP *map);

#endif
