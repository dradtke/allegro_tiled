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
 * Accessor methods for getting information from a struct.
 */

#include "data.h"

/*
 * Get the map's width in tiles.
 */
int al_get_map_width(ALLEGRO_MAP *map)
{
	return map->width;
}

/*
 * Get the map's height in tiles.
 */
int al_get_map_height(ALLEGRO_MAP *map)
{
	return map->height;
}

/*
 * Get the individual tile width.
 */
int al_get_tile_width(ALLEGRO_MAP *map)
{
	return map->tile_width;
}

/*
 * Get the individual tile height.
 */
int al_get_tile_height(ALLEGRO_MAP *map)
{
	return map->tile_height;
}

/*
 * Get the map's orientation.
 * Should be either "orthogonal" or "isometric".
 */
char *al_get_map_orientation(ALLEGRO_MAP *map)
{
	return map->orientation;
}

/*
 * Get the map's layer corresponding to the given name.
 */
ALLEGRO_MAP_LAYER *al_get_map_layer(ALLEGRO_MAP *map, char *name)
{
	GSList *layers = map->layers;
	while (layers) {
		ALLEGRO_MAP_LAYER *layer = (ALLEGRO_MAP_LAYER*)layers->data;
		if (!strcmp(layer->name, name)) {
			return layer;
		}
		layers = g_slist_next(layers);
	}

	return NULL;
}

/**
 * Get the layer's width in tiles.
 */
int al_get_layer_width(ALLEGRO_MAP_LAYER *layer)
{
	return layer->width;
}

/**
 * Get the layer's height in tiles.
 */
int al_get_layer_height(ALLEGRO_MAP_LAYER *layer)
{
	return layer->height;
}

/*
 * Get the layer's opacity, should be between 0 and 1.
 */
float al_get_layer_opacity(ALLEGRO_MAP_LAYER *layer)
{
	return layer->opacity;
}

/*
 * Returns true if this layer is visible.
 */
bool al_get_layer_visible(ALLEGRO_MAP_LAYER *layer)
{
	return layer->visible;
}

/*
 * Get the layer's name.
 */
char *al_get_layer_name(ALLEGRO_MAP_LAYER *layer)
{
	return layer->name;
}

int al_get_tile_id(ALLEGRO_MAP_TILE *tile)
{
	return tile->id;
}

char *al_get_object_name(ALLEGRO_MAP_OBJECT *object)
{
	return object->name;
}

char *al_get_object_type(ALLEGRO_MAP_OBJECT *object)
{
	return object->type;
}

int al_get_object_gid(ALLEGRO_MAP_OBJECT *object)
{
	return object->gid;
}

int al_get_object_x(ALLEGRO_MAP_OBJECT *object)
{
	return object->x;
}

int al_get_object_y(ALLEGRO_MAP_OBJECT *object)
{
	return object->y;
}

void al_get_object_pos(ALLEGRO_MAP_OBJECT *object, int *x, int *y)
{
	(*x) = object->x;
	(*y) = object->y;
}

int al_get_object_width(ALLEGRO_MAP_OBJECT *object)
{
	return object->width;
}

int al_get_object_height(ALLEGRO_MAP_OBJECT *object)
{
	return object->height;
}

void al_get_object_dims(ALLEGRO_MAP_OBJECT *object, int *width, int *height)
{
	(*width) = object->width;
	(*height) = object->height;
}

bool al_get_object_visible(ALLEGRO_MAP_OBJECT *object)
{
	return object->visible;
}

void _al_free_tile(gpointer data)
{
	ALLEGRO_MAP_TILE *tile = (ALLEGRO_MAP_TILE*)data;
	g_hash_table_unref(tile->properties);
	al_destroy_bitmap(tile->bitmap);
	al_free(tile);
}

void _al_free_tileset(gpointer data)
{
	ALLEGRO_MAP_TILESET *tileset = (ALLEGRO_MAP_TILESET*)data;
	al_free(tileset->name);
	al_free(tileset->source);
	g_slist_free_full(tileset->tiles, &_al_free_tile);
	al_destroy_bitmap(tileset->bitmap);
	al_free(tileset);
}

void _al_free_object(gpointer data)
{
	ALLEGRO_MAP_OBJECT *object = (ALLEGRO_MAP_OBJECT*)data;
	if (!object) {
		return;
	}
	al_free(object->name);
	al_free(object->type);
	g_hash_table_unref(object->properties);
	al_free(object);
}

void _al_free_layer(gpointer data)
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
