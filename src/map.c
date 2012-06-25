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

// Bits on the far end of the 32-bit global tile ID are used for tile flags
const unsigned FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
const unsigned FLIPPED_VERTICALLY_FLAG   = 0x40000000;
const unsigned FLIPPED_DIAGONALLY_FLAG   = 0x20000000;

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
 * Frees a map struct from memory
 */
void tiled_free_map(TILED_MAP *map)
{
	al_free(map->bounds);
	al_free(map->orientation);
	_al_list_destroy(map->tilesets);
	_al_list_destroy(map->layers);
	_al_list_destroy(map->tiles);
	// _al_list_destroy(map->objects);
	al_destroy_bitmap(map->backbuffer);
	al_free(map);
}

void tiled_free_object_group(TILED_OBJECT_GROUP *group)
{
	al_free(group->name);
	al_free(group);
}

void dtor_map_object(void *value, void *user_data)
{
	TILED_OBJECT *object_ob = (TILED_OBJECT*)value;
	
	object_ob->group->ref--;
	if (object_ob->group->ref <= 0)
		tiled_free_object_group(object_ob->group);

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

void dtor_tile_prop(void *value, void *user_data)
{
	TILED_MAP_TILE_PROPERTY *prop_ob = (TILED_MAP_TILE_PROPERTY*)value;
	al_free(prop_ob->name);
	al_free(prop_ob->value);
	al_free(prop_ob);
}
