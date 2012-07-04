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

static inline char lookup_tile(TILED_MAP_LAYER *layer_ob, int x, int y)
{
	return layer_ob->data[x+(y*layer_ob->width)];
}

char tile_id(TILED_MAP_LAYER *layer_ob, int x, int y)
{
	char id = lookup_tile(layer_ob, x, y);
	id &= ~(FLIPPED_HORIZONTALLY_FLAG
		   |FLIPPED_VERTICALLY_FLAG
		   |FLIPPED_DIAGONALLY_FLAG);

	return id;
}

bool flipped_horizontally(TILED_MAP_LAYER *layer_ob, int x, int y)
{
	return lookup_tile(layer_ob, x, y) & FLIPPED_HORIZONTALLY_FLAG;
}

bool flipped_vertically(TILED_MAP_LAYER *layer_ob, int x, int y)
{
	return lookup_tile(layer_ob, x, y) & FLIPPED_VERTICALLY_FLAG;
}

/*
 * Looks up tiles in a map by id.
 * TODO: is there a way to index this by id,
 * rather than looping through every time?
 */
TILED_MAP_TILE *tiled_get_tile_for_id(TILED_MAP *map, char id)
{
	if (id == 0)
		return NULL;

	_AL_LIST_ITEM *tile_item = _al_list_front(map->tiles);
	while (tile_item != NULL) {
		TILED_MAP_TILE *tile = _al_list_item_data(tile_item);
		if (tile->id == id)
			return tile;
		else
			tile_item = _al_list_next(map->tiles, tile_item);
	}

	return NULL;
}

/*
 * Update the map's backbuffer. This should be done whenever a tile
 * needs to change in appearance.
 */
void tiled_update_backbuffer(TILED_MAP *map)
{
	ALLEGRO_BITMAP *orig_backbuffer = al_get_target_bitmap();
	map->backbuffer = al_create_bitmap(map->pixel_width, map->pixel_height);
	al_set_target_bitmap(map->backbuffer);

	if (!strcmp(map->orientation, "orthogonal")) {
		_AL_LIST_ITEM *layer_item = _al_list_front(map->layers);
		while (layer_item != NULL) {
			TILED_MAP_LAYER *layer_ob = _al_list_item_data(layer_item);
			layer_item = _al_list_next(map->layers, layer_item);

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
		}
	} else if (!strcmp(map->orientation, "isometric")) {
		fprintf(stderr, "Error: sorry, can't draw isometric maps right now. =(\n");
	} else {
		fprintf(stderr, "Error: unknown map orientation: %s\n", map->orientation);
	}

	al_set_target_bitmap(orig_backbuffer);
}

/*
 * Frees a map struct from memory
 */
void tiled_free_map(TILED_MAP *map)
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

void dtor_map_object_group(void *value, void *user_data)
{
	TILED_OBJECT_GROUP *group_ob = (TILED_OBJECT_GROUP*)value;
	al_free(group_ob->name);
	al_free(group_ob);
}

void dtor_map_object(void *value, void *user_data)
{
	TILED_OBJECT *object_ob = (TILED_OBJECT*)value;
	_al_list_destroy(object_ob->properties);
	al_free(object_ob->name);
	al_free(object_ob->type);
	al_free(object_ob);
}

void dtor_map_tileset(void *value, void *user_data)
{
	TILED_MAP_TILESET *tileset_ob = (TILED_MAP_TILESET*)value;
	al_free(tileset_ob->name);
	al_free(tileset_ob->source);
	_al_list_destroy(tileset_ob->tiles);
	al_destroy_bitmap(tileset_ob->bitmap);
	al_free(tileset_ob);
}

void dtor_map_tile(void *value, void *user_data)
{
	TILED_MAP_TILE *tile_ob = (TILED_MAP_TILE*)value;
	_al_list_destroy(tile_ob->properties);
	al_destroy_bitmap(tile_ob->bitmap);
	al_free(tile_ob);
}

void dtor_map_layer(void *value, void *user_data)
{
	TILED_MAP_LAYER *layer_ob = (TILED_MAP_LAYER*)value;
	al_free(layer_ob->name);
	al_free(layer_ob->data);
	al_free(layer_ob);
}

void dtor_prop(void *value, void *user_data)
{
	TILED_PROPERTY *prop_ob = (TILED_PROPERTY*)value;
	al_free(prop_ob->name);
	al_free(prop_ob->value);
	al_free(prop_ob);
}
