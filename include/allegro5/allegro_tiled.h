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

#ifndef ALLEGRO_ALLEGRO_H
#define ALLEGRO_ALLEGRO_H

#include <allegro5/allegro.h>

enum LayerType {
	TILE_LAYER,
	OBJECT_LAYER
};

typedef struct _ALLEGRO_MAP                ALLEGRO_MAP;
typedef struct _ALLEGRO_MAP_LAYER          ALLEGRO_MAP_LAYER;
typedef struct _ALLEGRO_MAP_TILESET        ALLEGRO_MAP_TILESET;
typedef struct _ALLEGRO_MAP_TILE           ALLEGRO_MAP_TILE;
typedef struct _ALLEGRO_MAP_OBJECT_GROUP   ALLEGRO_MAP_OBJECT_GROUP;
typedef struct _ALLEGRO_MAP_OBJECT         ALLEGRO_MAP_OBJECT;

/*
 * Opens a map-file, parsing it into a ALLEGRO_MAP struct.
 * This should be freed with al_free_map() when no longer needd.
 */
ALLEGRO_MAP *al_open_map(const char *dir, const char *filename);

void al_draw_tinted_map(ALLEGRO_MAP *map, ALLEGRO_COLOR tint, float dx, float dy, int flags);
void al_draw_map(ALLEGRO_MAP *map, float dx, float dy, int flags);
void al_draw_tinted_map_region(ALLEGRO_MAP *map, ALLEGRO_COLOR tint, float sx, float sy, float sw, float sh, float dx, float dy, int flags);
void al_draw_map_region(ALLEGRO_MAP *map, float sx, float sy, float sw, float sh, float dx, float dy, int flags);
void al_draw_layer_for_name(ALLEGRO_MAP *map, char *name, float dx, float dy, int flags);
void al_draw_tinted_layer_region_for_name(ALLEGRO_MAP *map, char *name, ALLEGRO_COLOR tint, float sx, float sy, float sw, float sh, float dx, float dy, int flags);
void al_draw_layer_region_for_name(ALLEGRO_MAP *map, char *name, float sx, float sy, float sw, float sh, float dx, float dy, int flags);

/*
 * Draw all defined objects onto the target bitmap.
 */
void al_draw_objects(ALLEGRO_MAP *map);

/*
 * Given a tile's id, returns the corresponding ALLEGRO_MAP_TILE struct
 * for a tile with that id.
 */
ALLEGRO_MAP_TILE *al_get_tile_for_id(ALLEGRO_MAP *map, char id);

/*
 * Get the id of the tile at (x,y) on the given layer.
 */
char al_get_single_tile(ALLEGRO_MAP_LAYER *layer, int x, int y);

/*
 * Get a list of tile id's at (x,y) on the given map, one per layer.
 */
char *al_get_tiles(ALLEGRO_MAP *map, int x, int y);

/*
 * Get a property from a tile, returning some default value if not found.
 */
char *al_get_tile_property(ALLEGRO_MAP_TILE *tile, char *name, char *def);

/*
 * Get a property from an object, returning some default value if not found.
 */
char *al_get_object_property(ALLEGRO_MAP_OBJECT *object, char *name, char *def);

/*
 * Accessors. Their names should be pretty self-explanatory.
 */

int al_get_map_width(ALLEGRO_MAP *map);
int al_get_map_height(ALLEGRO_MAP *map);
int al_get_map_height(ALLEGRO_MAP *map);
int al_get_tile_width(ALLEGRO_MAP *map);
int al_get_tile_height(ALLEGRO_MAP *map);
char *al_get_map_orientation(ALLEGRO_MAP *map);
ALLEGRO_MAP_LAYER *al_get_map_layer(ALLEGRO_MAP *map, char *name);

/*
 * Free the map struct (and all associated structs) from memory.
 */
void al_free_map(ALLEGRO_MAP *map);

#endif
