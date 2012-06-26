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

#include <allegro5/tiled.h>
#include "internal.h"

/*
 * Get a property from a tile.
 */
char *tiled_get_tile_property(TILED_MAP_TILE *tile, char *name)
{
	_AL_LIST_ITEM *prop_item = _al_list_front(tile->properties);
	while (prop_item) {
		TILED_PROPERTY *prop = _al_list_item_data(prop_item);
		if (!strcmp(prop->name, name))
			return prop->value;

		prop_item = _al_list_next(tile->properties, prop_item);
	}

	return NULL;
}

/*
 * Get a property from an object.
 */
char *tiled_get_object_property(TILED_OBJECT *object, char *name)
{
	_AL_LIST_ITEM *prop_item = _al_list_front(object->properties);
	while (prop_item) {
		TILED_PROPERTY *prop = _al_list_item_data(prop_item);
		if (!strcmp(prop->name, name))
			return prop->value;

		prop_item = _al_list_next(object->properties, prop_item);
	}

	return NULL;
}

void tiled_update_backbuffer(TILED_MAP *map)
{
	ALLEGRO_BITMAP *orig_backbuffer = al_get_target_bitmap();
	map->backbuffer = al_create_bitmap(map->pixel_width, map->pixel_height);
	al_set_target_bitmap(map->backbuffer);

	if (!strcmp(map->orientation, "orthogonal")) {
		_AL_LIST_ITEM *layer_item = _al_list_front(map->layers);
		while (layer_item != NULL) {
			TILED_MAP_LAYER *layer_ob = _al_list_item_data(layer_item);
			int i, j;
			for (i = 0; i<layer_ob->height; i++) {
				for (j = 0; j<layer_ob->width; j++) {
					char id = tile_id(layer_ob, j, i);
					TILED_MAP_TILE *tile_ob = tiled_get_tile_for_id(map, id);
					if (!tile_ob)
						continue;

					int tx = j*(tile_ob->tileset->tilewidth);
					int ty = i*(tile_ob->tileset->tileheight);

					int flags = 0;
					if (flipped_horizontally(layer_ob, j, i)) flags |= ALLEGRO_FLIP_HORIZONTAL;
					if (flipped_vertically(layer_ob, j, i)) flags |= ALLEGRO_FLIP_VERTICAL;

					al_draw_bitmap(tile_ob->bitmap, tx, ty, flags);
				}
			}

			layer_item = _al_list_next(map->layers, layer_item);
		}
	}
	else if (!strcmp(map->orientation, "isometric")) {
		fprintf(stderr, "Error: sorry, can't draw isometric maps right now. =(\n");
	}
	else {
		fprintf(stderr, "Error: unknown map orientation: %s\n", map->orientation);
	}

	al_set_target_bitmap(orig_backbuffer);
}
