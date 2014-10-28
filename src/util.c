/*
 * This addon adds Tiled map support to the Allegro game library.
 * Copyright (c) 2014 Damien Radtke - www.damienradtke.org
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
 * Random utility methods.
 */

#include "util.h"

/*
 * Locates a parent path by name. It starts at base (or the current
 * directory if base is NULL) and traverses upwards until it finds
 * a folder of the given name.
 *
 * If found, it returns a pointer to an ALLEGRO_PATH representing that
 * directory; if not, it returns NULL.
 */
ALLEGRO_PATH *al_find_parent(char *base, char *name)
{
	int i, n;
	ALLEGRO_PATH *path;

	if (base == NULL) {
		base = al_get_current_directory();
	}

	path = al_create_path_for_directory(base);
	n = al_get_path_num_components(path);

	for (i = 0; i < n; i++) {
		char *tail = al_get_path_tail(path);
		if (!strcmp(tail, name)) {
			return path;
		}
		al_drop_path_tail(path);
	}

	al_destroy_path(path);
	return NULL;
}
