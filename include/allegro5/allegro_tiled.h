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

#ifndef ALLEGRO_TILED_H
#define ALLEGRO_TILED_H

#include <allegro5/allegro.h>

typedef struct _TILED_MAP            TILED_MAP;
typedef struct _TILED_MAP_LAYER      TILED_MAP_LAYER;
typedef struct _TILED_MAP_TILESET    TILED_MAP_TILESET;
typedef struct _TILED_MAP_TILE       TILED_MAP_TILE;
typedef struct _TILED_PROPERTY       TILED_PROPERTY;
typedef struct _TILED_OBJECT_GROUP   TILED_OBJECT_GROUP;
typedef struct _TILED_OBJECT         TILED_OBJECT;

/*
 * Opens a map-file, parsing it into a TILED_MAP struct.
 * This should be freed with al_free_map() when no longer needd.
 */
TILED_MAP *al_open_map(const char *dir, const char *filename);

/*
 * Draws the map onto the target bitmap at the given position.
 */
void al_draw_map(TILED_MAP *map, float x, float y, int screen_width, int screen_height);

/*
 * Draw all defined objects onto the target bitmap.
 */
void al_draw_objects(TILED_MAP *map);

/*
 * Given a tile's id, returns the corresponding TILED_MAP_TILE struct
 * for a tile with that id.
 */
TILED_MAP_TILE *al_get_tile_for_id(TILED_MAP *map, char id);

/*
 * Get the id of the tile at (x,y) on the given layer.
 */
char al_get_single_tile(TILED_MAP_LAYER *layer, int x, int y);

/*
 * Get a list of tile id's at (x,y) on the given map, one per layer.
 */
char *al_get_tiles(TILED_MAP *map, int x, int y);

int al_map_get_pixel_width(TILED_MAP *map);
int al_map_get_pixel_height(TILED_MAP *map);

/*
 * Get a property from a tile, returning some default value if not found.
 */
char *al_get_tile_property(TILED_MAP_TILE *tile, char *name, char *def);

/*
 * Get a property from an object, returning some default value if not found.
 */
char *al_get_object_property(TILED_OBJECT *object, char *name, char *def);

/*
 * Update the map's backbuffer. This should be done whenever a tile needs
 * to change in appearance.
 */
void al_update_backbuffer(TILED_MAP *map);

/*
 * Free the map struct (and all associated structs) from memory.
 */
void al_free_map(TILED_MAP *map);

#endif
