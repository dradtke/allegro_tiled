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
 * Same as lookup_tile(), but zeroes out special bits first.
 */
char al_get_single_tile(ALLEGRO_MAP_LAYER *layer, int x, int y)
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
 * Get a list of tiles at the given location, one per layer.
 * The list is terminated with -1, because NULL maps to 0, which
 * means the absence of a tile rather than "no more tiles."
 */
char *al_get_tiles(ALLEGRO_MAP *map, int x, int y)
{
	int layer_count = g_slist_length(map->layers);
	char* tiles = (char *)malloc(sizeof(char) * (layer_count));
	tiles[layer_count] = (char)-1;

	int i = 0;
	GSList *layers = map->layers;
	while (layers) {
		ALLEGRO_MAP_LAYER *layer = (ALLEGRO_MAP_LAYER*)layers->data;
		layers = g_slist_next(layers);
		tiles[i] = al_get_single_tile(layer, x, y);
		i++;
	}

	return tiles;
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
 * Destructors.
 */

static void _al_free_tile(gpointer data)
{
	ALLEGRO_MAP_TILE *tile = (ALLEGRO_MAP_TILE*)data;
	g_hash_table_unref(tile->properties);
	al_destroy_bitmap(tile->bitmap);
	al_free(tile);
}

static void _al_free_tileset(gpointer data)
{
	ALLEGRO_MAP_TILESET *tileset = (ALLEGRO_MAP_TILESET*)data;
	al_free(tileset->name);
	al_free(tileset->source);
	g_slist_free_full(tileset->tiles, &_al_free_tile);
	al_destroy_bitmap(tileset->bitmap);
	al_free(tileset);
}

static void _al_free_object(gpointer data)
{
	ALLEGRO_MAP_OBJECT *object = (ALLEGRO_MAP_OBJECT*)data;
	if (!object)
		return;
	al_free(object->name);
	al_free(object->type);
	g_hash_table_unref(object->properties);
	al_free(object);
}

static void _al_free_layer(gpointer data)
{
	ALLEGRO_MAP_LAYER *layer = (ALLEGRO_MAP_LAYER*)data;
	al_free(layer->name);
	if (layer->type == TILE_LAYER) {
		al_free(layer->data);
	} else if (layer->type == OBJECT_LAYER) {
		g_slist_free_full(layer->objects, &_al_free_object);
	}
	g_hash_table_unref(layer->properties);
	al_free(layer);
}

/*
 * Frees a map struct from memory
 */
void al_free_map(ALLEGRO_MAP *map)
{
	al_free(map->orientation);
	g_slist_free_full(map->tilesets, &_al_free_tileset);
	g_slist_free_full(map->layers, &_al_free_layer);
	g_hash_table_unref(map->tiles);
	al_free(map);
}
