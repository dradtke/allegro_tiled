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
