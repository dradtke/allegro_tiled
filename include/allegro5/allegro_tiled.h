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
#include <glib.h>

//{{{ map structs
/*
typedef struct
{
	int left;
	int right;
	int top;
	int bottom;
}
TILED_MAP_BOUNDS;
*/

typedef struct
{
	int x, y;			// map position
	int width, height;		// dimensions in tiles
	int tile_width;			// width of each tile in pixels
	int tile_height;		// height of each tile in pixels
	int pixel_width;		// width of the map in pixels
	int pixel_height;		// height of the map in pixels
	char *orientation;		// "orthogonal" or ... isometric?
	GSList *tilesets;		// list of tilesets
	GSList *layers;			// list of layers
	GSList *object_groups;	// list of object groups
	GSList *objects;		// list of objects
	GHashTable *tiles;		// full list of tiles
	ALLEGRO_BITMAP *backbuffer;	// back buffer
}
TILED_MAP;

typedef struct
{
	int width;			// width in tiles
	int height;			// height in tiles
	int datalen;			// length of the decoded data
	float opacity;			// the layer's opacity
	int visible;			// 0 for hidden, 1 for visible
	char *name;			// name of the layer
	char *data;			// decoded data
	TILED_MAP *map;			// reference to the map
}
TILED_MAP_LAYER;

typedef struct
{
	int firstgid;			// first global id
	int tilewidth;			// width of each tile
	int tileheight;			// height of each tile
	int width, height;		// total dimensions (in pixels)
	char *name;			// name
	char *source;			// path to this tileset's image source
	ALLEGRO_BITMAP *bitmap;		// image for this tileset
	GSList *tiles;		// list of tiles
}
TILED_MAP_TILESET;

typedef struct
{
	int id;				// the tile id
	TILED_MAP_TILESET *tileset;	// pointer to its tileset
	GSList *properties;		// tile properties
	ALLEGRO_BITMAP *bitmap; 	// this tile's image
}
TILED_MAP_TILE;

typedef struct
{
	char *name;			// the property's name
	char *value;			// the property's value
}
TILED_PROPERTY;

typedef struct
{
	char *name;
	// color?
	float opacity;
	bool visible;
}
TILED_OBJECT_GROUP;

typedef struct
{
	TILED_OBJECT_GROUP *group;
	char *name;
	char *type;
	int x, y;
	int width, height;
	bool visible;
	ALLEGRO_BITMAP *bitmap;
	GSList *properties;
}
TILED_OBJECT;
//}}}

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
