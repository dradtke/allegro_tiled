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

#include "map.h"

/*
 * Look up the raw data of a tile in the given layer.
 */
static inline char lookup_tile(TILED_MAP_LAYER *layer, int x, int y)
{
	return layer->data[x+(y*layer->width)];
}

/*
 * Same as lookup_tile(), but zeroes out special bits first.
 */
char al_get_single_tile(TILED_MAP_LAYER *layer, int x, int y)
{
	char id = lookup_tile(layer, x, y);
	id &= ~(FLIPPED_HORIZONTALLY_FLAG
			|FLIPPED_VERTICALLY_FLAG
			|FLIPPED_DIAGONALLY_FLAG);

	return id;
}

/*
 * Get a list of tiles at the given location, one per layer.
 * The list is terminated with -1, because NULL maps to 0, which
 * means the absence of a tile rather than "no more tiles."
 */
char *al_get_tiles(TILED_MAP *map, int x, int y)
{
	int layer_count = _al_list_size(map->layers);
	char* tiles = (char *)malloc(sizeof(char) * (layer_count));
	tiles[layer_count] = (char)-1;

	int i = 0;
	_AL_LIST_ITEM *layer_item = _al_list_front(map->layers);
	while (layer_item) {
		TILED_MAP_LAYER *layer = (TILED_MAP_LAYER*)_al_list_item_data(layer_item);
		layer_item = _al_list_next(map->layers, layer_item);
		tiles[i] = al_get_single_tile(layer, x, y);
		i++;
	}

	return tiles;
}

/*
 * Returns true if the tile at the given location is flipped horizontally.
 */
bool flipped_horizontally(TILED_MAP_LAYER *layer, int x, int y)
{
	return lookup_tile(layer, x, y) & FLIPPED_HORIZONTALLY_FLAG;
}

/*
 * Returns true if the tile at the given location is flipped vertically.
 */
bool flipped_vertically(TILED_MAP_LAYER *layer, int x, int y)
{
	return lookup_tile(layer, x, y) & FLIPPED_VERTICALLY_FLAG;
}

/*
 * Looks up tiles in a map by id.
 * TODO: is there a way to index this by id,
 * rather than looping through every time?
 */
TILED_MAP_TILE *al_get_tile_for_id(TILED_MAP *map, char id)
{
	if (id == 0)
		return NULL;

	_AL_LIST_ITEM *tile_item = _al_list_front(map->tiles);
	while (tile_item != NULL) {
		TILED_MAP_TILE *tile = _al_list_item_data(tile_item);
		if (tile->id == id) {
			return tile;
		} else {
			tile_item = _al_list_next(map->tiles, tile_item);
		}
	}

	return NULL;
}

/*
 * Get a property from a tile.
 */
char *al_get_tile_property(TILED_MAP_TILE *tile, char *name, char *def)
{
	if (tile) {
		_AL_LIST_ITEM *prop_item = _al_list_front(tile->properties);
		while (prop_item) {
			TILED_PROPERTY *prop = _al_list_item_data(prop_item);
			if (!strcmp(prop->name, name)) {
				return prop->value;
			} else {
				prop_item = _al_list_next(tile->properties, prop_item);
			}
		}
	}

	return def;
}

/*
 * Get a property from an object.
 */
char *al_get_object_property(TILED_OBJECT *object, char *name, char *def)
{
	_AL_LIST_ITEM *prop_item = _al_list_front(object->properties);
	while (prop_item) {
		TILED_PROPERTY *prop = _al_list_item_data(prop_item);
		if (!strcmp(prop->name, name)) {
			return prop->value;
		} else {
			prop_item = _al_list_next(object->properties, prop_item);
		}
	}

	return def;
}

/*
 * Frees a map struct from memory
 */
void al_free_map(TILED_MAP *map)
{
	al_free(map->orientation);
	_al_list_destroy(map->tilesets);
	_al_list_destroy(map->layers);
	_al_list_destroy(map->tiles);
	_al_list_destroy(map->objects);
	_al_list_destroy(map->object_groups);
	al_destroy_bitmap(map->backbuffer);
	al_free(map);
}

/*
 * Object group destructor.
 */
void dtor_map_object_group(void *value, void *user_data)
{
	TILED_OBJECT_GROUP *group = (TILED_OBJECT_GROUP*)value;
	al_free(group->name);
	al_free(group);
}

/*
 * Object destructor.
 */
void dtor_map_object(void *value, void *user_data)
{
	TILED_OBJECT *object = (TILED_OBJECT*)value;
	_al_list_destroy(object->properties);
	al_free(object->name);
	al_free(object->type);
	al_free(object);
}

/*
 * Tileset destructor.
 */
void dtor_map_tileset(void *value, void *user_data)
{
	TILED_MAP_TILESET *tileset = (TILED_MAP_TILESET*)value;
	al_free(tileset->name);
	al_free(tileset->source);
	_al_list_destroy(tileset->tiles);
	al_destroy_bitmap(tileset->bitmap);
	al_free(tileset);
}

/*
 * Tile destructor.
 */
void dtor_map_tile(void *value, void *user_data)
{
	TILED_MAP_TILE *tile = (TILED_MAP_TILE*)value;
	_al_list_destroy(tile->properties);
	al_destroy_bitmap(tile->bitmap);
	al_free(tile);
}

/*
 * Layer destructor.
 */
void dtor_map_layer(void *value, void *user_data)
{
	TILED_MAP_LAYER *layer = (TILED_MAP_LAYER*)value;
	al_free(layer->name);
	al_free(layer->data);
	al_free(layer);
}

/*
 * Property destructor.
 */
void dtor_prop(void *value, void *user_data)
{
	TILED_PROPERTY *prop = (TILED_PROPERTY*)value;
	al_free(prop->name);
	al_free(prop->value);
	al_free(prop);
}
