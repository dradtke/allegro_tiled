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

#include <allegro5/tiled.h>

// Bits on the far end of the 32-bit global tile ID are used for tile flags
#define FLIPPED_HORIZONTALLY_FLAG	0x80000000
#define FLIPPED_VERTICALLY_FLAG		0x40000000
#define FLIPPED_DIAGONALLY_FLAG		0x20000000

char tile_id(TILED_MAP_LAYER *layer_ob, int x, int y);
bool flipped_horizontally(TILED_MAP_LAYER *layer_ob, int x, int y);
bool flipped_vertically(TILED_MAP_LAYER *layer_ob, int x, int y);
void tiled_free_object_group(TILED_OBJECT_GROUP *group);

void dtor_map_tile(void *value, void *user_data);
void dtor_map_tileset(void *value, void *user_data);
void dtor_map_layer(void *value, void *user_data);
void dtor_map_object(void *value, void *user_data);
void dtor_prop(void *value, void *user_data);

#endif
