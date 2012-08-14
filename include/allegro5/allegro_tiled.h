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

ALLEGRO_MAP *al_open_map(const char *dir, const char *filename);

// drawing methods
void al_draw_tinted_map(ALLEGRO_MAP *map, ALLEGRO_COLOR tint, float dx, float dy, int flags);
void al_draw_map(ALLEGRO_MAP *map, float dx, float dy, int flags);
void al_draw_tinted_map_region(ALLEGRO_MAP *map, ALLEGRO_COLOR tint, float sx, float sy, float sw, float sh, float dx, float dy, int flags);
void al_draw_map_region(ALLEGRO_MAP *map, float sx, float sy, float sw, float sh, float dx, float dy, int flags);
void al_draw_layer_for_name(ALLEGRO_MAP *map, char *name, float dx, float dy, int flags);
void al_draw_tinted_layer_region_for_name(ALLEGRO_MAP *map, char *name, ALLEGRO_COLOR tint, float sx, float sy, float sw, float sh, float dx, float dy, int flags);
void al_draw_layer_region_for_name(ALLEGRO_MAP *map, char *name, float sx, float sy, float sw, float sh, float dx, float dy, int flags);

// tile and object methods
ALLEGRO_MAP_TILE *al_get_tile_for_id(ALLEGRO_MAP *map, char id);
char al_get_single_tile_id(ALLEGRO_MAP_LAYER *layer, int x, int y);
ALLEGRO_MAP_TILE *al_get_single_tile(ALLEGRO_MAP *map, ALLEGRO_MAP_LAYER *layer, int x, int y);
ALLEGRO_MAP_TILE **al_get_tiles(ALLEGRO_MAP *map, int x, int y, int *length);
ALLEGRO_MAP_OBJECT **al_get_objects(ALLEGRO_MAP_LAYER *layer, int *length);
ALLEGRO_MAP_OBJECT **al_get_objects_for_name(ALLEGRO_MAP_LAYER *layer, char *name, int *length);
char *al_get_tile_property(ALLEGRO_MAP_TILE *tile, char *name, char *def);
char *al_get_object_property(ALLEGRO_MAP_OBJECT *object, char *name, char *def);

// accessors
int al_get_map_width(ALLEGRO_MAP *map);
int al_get_map_height(ALLEGRO_MAP *map);
int al_get_map_height(ALLEGRO_MAP *map);
int al_get_tile_width(ALLEGRO_MAP *map);
int al_get_tile_height(ALLEGRO_MAP *map);
char *al_get_map_orientation(ALLEGRO_MAP *map);
ALLEGRO_MAP_LAYER *al_get_map_layer(ALLEGRO_MAP *map, char *name);

// destructors
void al_free_map(ALLEGRO_MAP *map);

#endif
