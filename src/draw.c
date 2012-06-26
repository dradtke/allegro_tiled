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

void tiled_draw_map(TILED_MAP *map, int screen_width, int screen_height)
{
	al_draw_bitmap_region(map->backbuffer, map->x, map->y,
			screen_width, screen_height, 0, 0, 0);
}

void tiled_draw_objects(TILED_MAP *map)
{
	_AL_LIST *objects = map->objects;
	_AL_LIST_ITEM *object_item = _al_list_front(objects);
	while (object_item) {
		TILED_OBJECT *object = _al_list_item_data(object_item);
		if (object->bitmap) {
			// TODO: flip flags?
			al_draw_bitmap(object->bitmap, object->x, object->y, 0);
		}

		object_item = _al_list_next(objects, object_item);
	}
}
