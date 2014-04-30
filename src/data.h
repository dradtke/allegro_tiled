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

#ifndef _DATA_H
#define _DATA_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_tiled.h>
#include <glib.h>

struct _ALLEGRO_MAP
{
	int width, height;          // dimensions in tiles
	int tile_width;             // width of each tile in pixels
	int tile_height;            // height of each tile in pixels
	char *orientation;          // "orthogonal" or ... isometric?
	GSList *layers;             // list of all layers
	GSList *tile_layers;        // list of tile layers
	GSList *object_layers;      // list of object layers
	GSList *tilesets;           // list of tilesets
	int tile_layer_count;       // number of tile layers
	int object_layer_count;     // number of object layers
	GHashTable *tiles;          // full list of tiles
};

struct _ALLEGRO_MAP_LAYER
{
	enum LayerType type;        // either 'TILE_LAYER' or 'OBJECT_LAYER'
	int width;                  // width in tiles
	int height;                 // height in tiles
	float opacity;              // the layer's opacity
	bool visible;               // 0 for hidden, 1 for visible
	char *name;                 // name of the layer
	char *data;                 // decoded data (tile layer only)
	GSList *objects;            // objects (object layer only)
	int object_count;           // number of objects (object layer only)
	GHashTable *properties;     // properties
};

struct _ALLEGRO_MAP_TILESET
{
	int firstgid;               // first global id
	int tilewidth;              // width of each tile
	int tileheight;             // height of each tile
	int width, height;          // total dimensions (in pixels)
	char *name;                 // name
	char *source;               // path to this tileset's image source
	ALLEGRO_BITMAP *bitmap;     // image for this tileset
	GSList *tiles;              // list of tiles
};

struct _ALLEGRO_MAP_TILE
{
	int id;                       // the tile id
	ALLEGRO_MAP_TILESET *tileset; // pointer to its tileset
	GHashTable *properties;       // tile properties
	ALLEGRO_BITMAP *bitmap;       // this tile's image
};

struct _ALLEGRO_MAP_OBJECT
{
	ALLEGRO_MAP_LAYER *layer;
	char *name;
	char *type;
	int gid;
	int x, y;
	int width, height;
	bool visible;
	ALLEGRO_BITMAP *bitmap;
	GHashTable *properties;
};

int al_get_map_width(ALLEGRO_MAP *map);
int al_get_map_height(ALLEGRO_MAP *map);
int al_get_map_height(ALLEGRO_MAP *map);
int al_get_tile_height(ALLEGRO_MAP *map);
char *al_get_map_orientation(ALLEGRO_MAP *map);
ALLEGRO_MAP_LAYER *al_get_map_layer(ALLEGRO_MAP *map, char *name);
int al_get_layer_width(ALLEGRO_MAP_LAYER *layer);
int al_get_layer_height(ALLEGRO_MAP_LAYER *layer);
float al_get_layer_opacity(ALLEGRO_MAP_LAYER *layer);
bool al_get_layer_visible(ALLEGRO_MAP_LAYER *layer);
char *al_get_layer_name(ALLEGRO_MAP_LAYER *layer);
int al_get_tile_id(ALLEGRO_MAP_TILE *tile);
char *al_get_object_name(ALLEGRO_MAP_OBJECT *object);
char *al_get_object_type(ALLEGRO_MAP_OBJECT *object);
int al_get_object_gid(ALLEGRO_MAP_OBJECT *object);
int al_get_object_x(ALLEGRO_MAP_OBJECT *object);
int al_get_object_y(ALLEGRO_MAP_OBJECT *object);
void al_get_object_pos(ALLEGRO_MAP_OBJECT *object, int *x, int *y);
int al_get_object_width(ALLEGRO_MAP_OBJECT *object);
int al_get_object_height(ALLEGRO_MAP_OBJECT *object);
void al_get_object_dims(ALLEGRO_MAP_OBJECT *object, int *width, int *height);
bool al_get_object_visible(ALLEGRO_MAP_OBJECT *object);

void _al_free_tile(gpointer data);
void _al_free_tileset(gpointer data);
void _al_free_object(gpointer data);
void _al_free_layer(gpointer data);
void al_free_map(ALLEGRO_MAP *map);

#endif
