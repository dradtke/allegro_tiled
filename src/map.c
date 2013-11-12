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
 *
 *                               ---
 *
 * Various map-related utility methods.
 */

#include "map.h"

/*
 * Look up the raw data of a tile in the given layer.
 */
static inline char lookup_tile(ALLEGRO_MAP_LAYER *layer, int x, int y)
{
	return layer->data[x+(y*layer->width)];
}

/*
 * Same as lookup_tile, but zeros out the special bits first.
 */
char al_get_single_tile_id(ALLEGRO_MAP_LAYER *layer, int x, int y)
{
	if (layer->type != TILE_LAYER) {
		return 0;
	}

	char id = lookup_tile(layer, x, y);
	id &= ~(FLIPPED_HORIZONTALLY_FLAG
			|FLIPPED_VERTICALLY_FLAG
			|FLIPPED_DIAGONALLY_FLAG);

	return id;
}

/*
 * Gets the tile at the given location on the given layer.
 */
ALLEGRO_MAP_TILE *al_get_single_tile(ALLEGRO_MAP *map, ALLEGRO_MAP_LAYER *layer, int x, int y)
{
	return al_get_tile_for_id(map, al_get_single_tile_id(layer, x, y));
}

/*
 * Get a list of tiles at the given location, one per layer.
 * Note: this list is NOT null-terminated. NULL in this case signals
 * the lack of a tile, not the end of the list. To iterate over
 * the list, use the value stored in length instead.
 *
 * This list is a shallow copy and must be freed when it's no longer
 * needed.
 */
ALLEGRO_MAP_TILE **al_get_tiles(ALLEGRO_MAP *map, int x, int y, int *length)
{
	ALLEGRO_MAP_TILE **results = (ALLEGRO_MAP_TILE**)al_malloc(sizeof(ALLEGRO_MAP_TILE*) * (map->tile_layer_count));

	int i;
	GSList *layers = map->tile_layers;
	for (i = 0; layers; i++) {
		ALLEGRO_MAP_LAYER *layer = (ALLEGRO_MAP_LAYER*)layers->data;
		layers = g_slist_next(layers);
		results[i] = al_get_single_tile(map, layer, x, y);
	}

	(*length) = i + 1;
	return results;
}

/*
 * Gets a list of all objects on the given layer with the given name.
 * Note: this list is NOT null-terminated. To iterate over it,
 * use the value stored in length instead.
 *
 * This list is a shallow copy and must be freed when it's no longer
 * needed.
 */
ALLEGRO_MAP_OBJECT **al_get_objects_for_name(ALLEGRO_MAP_LAYER *layer, char *name, int *length)
{
	(*length) = 0;
	if (layer->type != OBJECT_LAYER) {
		return NULL;
	}

	GSList *matches = NULL;
	GSList *objects = layer->objects;
	while (objects) {
		ALLEGRO_MAP_OBJECT *object = (ALLEGRO_MAP_OBJECT*)objects->data;
		objects = g_slist_next(objects);
		if (!strcmp(object->name, name)) {
			matches = g_slist_prepend(matches, object);
			(*length)++;
		}
	}

	ALLEGRO_MAP_OBJECT **results = (ALLEGRO_MAP_OBJECT**)al_malloc(sizeof(ALLEGRO_MAP_OBJECT*) * (*length));
	int i;
	for (i = 0; i<(*length); i++) {
		results[i] = matches->data;
		matches = g_slist_next(matches);
	}

	g_slist_free(matches);
	return results;
}

/*
 * Gets a list of all objects on the given layer.
 * Note: this list is NOT null-terminated. To iterate over it,
 * use the value stored in length instead.
 *
 * This list is a shallow copy and must be freed when it's no longer
 * needed.
 */
ALLEGRO_MAP_OBJECT **al_get_objects(ALLEGRO_MAP_LAYER *layer, int *length)
{
	(*length) = 0;
	if (layer->type != OBJECT_LAYER) {
		return NULL;
	}

	(*length) = layer->object_count;
	ALLEGRO_MAP_OBJECT **results = (ALLEGRO_MAP_OBJECT**)al_malloc(sizeof(ALLEGRO_MAP_OBJECT*) * (*length));

	GSList *objects = layer->objects;
	int i;
	for (i = 0; i<(*length); i++) {
		results[i] = objects->data;
		objects = g_slist_next(objects);
	}

	return results;
}

/*
 * Returns true if the tile at the given location is flipped horizontally.
 */
bool flipped_horizontally(ALLEGRO_MAP_LAYER *layer, int x, int y)
{
	return lookup_tile(layer, x, y) & FLIPPED_HORIZONTALLY_FLAG;
}

/*
 * Returns true if the tile at the given location is flipped vertically.
 */
bool flipped_vertically(ALLEGRO_MAP_LAYER *layer, int x, int y)
{
	return lookup_tile(layer, x, y) & FLIPPED_VERTICALLY_FLAG;
}

/*
 * Looks up tiles in a map by id.
 */
ALLEGRO_MAP_TILE *al_get_tile_for_id(ALLEGRO_MAP *map, char id)
{
	if (id == 0) {
		return NULL;
	}

	return (ALLEGRO_MAP_TILE*)g_hash_table_lookup(map->tiles, GINT_TO_POINTER(id));
}

/*
 * Get a property from a tile.
 */
char *al_get_tile_property(ALLEGRO_MAP_TILE *tile, char *name, char *def)
{
	if (tile) {
		char *value = g_hash_table_lookup(tile->properties, name);
		return value ? value : def;
	}

	return def;
}

/*
 * Get a property from an object.
 */
char *al_get_object_property(ALLEGRO_MAP_OBJECT *object, char *name, char *def)
{
	if (object) {
		char *value = g_hash_table_lookup(object->properties, name);
		return value ? value : def;
	}

	return def;
}

/*
 * Get the layer with the corresponding name.
 */
ALLEGRO_MAP_LAYER *al_get_layer_for_name(ALLEGRO_MAP *map, char *name)
{
	GSList *layers = map->layers;
	while (layers) {
		ALLEGRO_MAP_LAYER *layer = (ALLEGRO_MAP_LAYER*)layers->data;
		if (!strcmp(layer->name, name)) {
			return layer;
		} else {
			layers = g_slist_next(layers);
		}
	}

	return NULL;
}

/*
 * Return the first object found in the map with the corresponding name.
 */
ALLEGRO_MAP_OBJECT *al_get_object_for_name(ALLEGRO_MAP *map, char *name)
{
	GSList *layers = map->layers;
	while (layers) {
		ALLEGRO_MAP_LAYER *layer = (ALLEGRO_MAP_LAYER*)layers->data;
		layers = g_slist_next(layers);

		GSList *objects = layer->objects;
		while (objects) {
			ALLEGRO_MAP_OBJECT *object = (ALLEGRO_MAP_OBJECT*)objects->data;
			objects = g_slist_next(objects);
			if (!strcmp(object->name, name)) {
				return object;
			}
		}
	}
	return NULL;
}
