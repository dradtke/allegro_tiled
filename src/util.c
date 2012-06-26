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

/*
 * Trims whitespace from before and after a string
 */
char *trim(char *str)
{
	char *end;

	while (isspace(*str)) str++;
	if (*str == 0) return str;

	end = str + strlen(str) - 1;
	while (end > str && isspace(*end)) end--;

	*(end+1) = '\0';
	return str;
}

/*
 * Returns a copy of a string
 * Used to keep XML data that would otherwise be freed
 */
char *copy(const char *src)
{
	int len = strlen(src);
	char *result = (char *)malloc(sizeof(char) * (len+1));
	strcpy(result, src);
	return result;
}

