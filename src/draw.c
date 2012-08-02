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
 * Helper methods for drawing the parsed map.
 */

#include "draw.h"

/*
 * Draw the map's backbuffer to the target bitmap.
 */
void al_draw_map(ALLEGRO_MAP *map, float x, float y, int screen_width, int screen_height)
{
	al_draw_bitmap_region(map->backbuffer, x, y, screen_width, screen_height, 0, 0, 0);
}

/*
 * Draw all defined objects to the target bitmap.
 */
void al_draw_objects(ALLEGRO_MAP *map)
{
	GSList *objects = map->objects;
	while (objects) {
		ALLEGRO_MAP_OBJECT *object = (ALLEGRO_MAP_OBJECT*)objects->data;
		objects = g_slist_next(objects);
		if (object->bitmap) {
			// TODO: flip flags?
			int flags = 0;
			al_draw_bitmap(object->bitmap, object->x, object->y, flags);
		}
	}
}

/*
 * Update the map's backbuffer. This should be done whenever a tile
 * needs to change in appearance.
 */
void al_update_backbuffer(ALLEGRO_MAP *map)
{
	ALLEGRO_BITMAP *orig_backbuffer = al_get_target_bitmap();
	map->backbuffer = al_create_bitmap(map->width * map->tile_width, map->height * map->tile_height);
	al_set_target_bitmap(map->backbuffer);

	if (!strcmp(map->orientation, "orthogonal")) {
		GSList *layers = map->layers;
		while (layers) {
			ALLEGRO_MAP_LAYER *layer = (ALLEGRO_MAP_LAYER*)layers->data;
			layers = g_slist_next(layers);

			int i, j;
			for (i = 0; i<layer->height; i++) {
				for (j = 0; j<layer->width; j++) {
					char id = al_get_single_tile(layer, j, i);
					ALLEGRO_MAP_TILE *tile = al_get_tile_for_id(map, id);
					if (!tile)
						continue;

					int tx = j*(tile->tileset->tilewidth);
					int ty = i*(tile->tileset->tileheight);

					int flags = 0;
					if (flipped_horizontally(layer, j, i)) flags |= ALLEGRO_FLIP_HORIZONTAL;
					if (flipped_vertically(layer, j, i)) flags |= ALLEGRO_FLIP_VERTICAL;

					al_draw_bitmap(tile->bitmap, tx, ty, flags);
				}
			}
		}
	} else if (!strcmp(map->orientation, "isometric")) {
		fprintf(stderr, "Error: sorry, can't draw isometric maps right now. =(\n");
	} else {
		fprintf(stderr, "Error: unknown map orientation: %s\n", map->orientation);
	}

	al_set_target_bitmap(orig_backbuffer);
}
