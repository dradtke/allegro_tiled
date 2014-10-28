/*
 * This addon adds Tiled map support to the Allegro game library.
 * Copyright (c) 2012-2014 Damien Radtke - www.damienradtke.com
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *    1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 *    2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 *    3. This notice may not be removed or altered from any source
 *    distribution.
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
