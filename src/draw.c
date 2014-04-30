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
 * Helper methods for drawing the parsed map.
 */

#include "draw.h"

static void _al_draw_orthogonal_tile_layer(ALLEGRO_MAP_LAYER *layer, ALLEGRO_MAP *map, ALLEGRO_COLOR tint, float sx, float sy, float sw, float sh, float dx, float dy, int flags)
{
	if (!layer->visible) {
		return;
	}

	float r, g, b, a;
	al_unmap_rgba_f(tint, &r, &g, &b, &a);
	ALLEGRO_COLOR color = al_map_rgba_f(r, g, b, a * layer->opacity);

	int mx, my;
	int ystart = sy / map->tile_height, yend = (sy + sh) / map->tile_height;
	int xstart = sx / map->tile_width, xend = (sx + sw) / map->tile_width;

	// defer rendering until everything is drawn
	al_hold_bitmap_drawing(true);

	for (my = ystart; my <= yend; my++) {
		for (mx = xstart; mx <= xend; mx++) {
			ALLEGRO_MAP_TILE *tile = al_get_single_tile(map, layer, mx, my);

			if (!tile) {
				continue;
			}

			float x = mx*(map->tile_width) - sx + dx;
			float y = my*(map->tile_height) - sy + dy;

			if (flipped_horizontally(layer, mx, my)) flags |= ALLEGRO_FLIP_HORIZONTAL;
			if (flipped_vertically(layer, mx, my)) flags |= ALLEGRO_FLIP_VERTICAL;

			al_draw_tinted_bitmap(tile->bitmap, color, x, y, flags);
		}
	}

	al_hold_bitmap_drawing(false);
}

static void _al_draw_orthogonal_object_layer(ALLEGRO_MAP_LAYER *layer, ALLEGRO_MAP *map, ALLEGRO_COLOR tint, float sx, float sy, float sw, float sh, float dx, float dy, int flags)
{
	if (!layer->visible) {
		return;
	}

	float r, g, b, a;
	al_unmap_rgba_f(tint, &r, &g, &b, &a);
	ALLEGRO_COLOR color = al_map_rgba_f(r, g, b, a * layer->opacity);

	// defer rendering until everything is drawn
	al_hold_bitmap_drawing(true);

	GSList *objects = layer->objects;
	while (objects) {
		ALLEGRO_MAP_OBJECT *object = (ALLEGRO_MAP_OBJECT*)objects->data;
		objects = g_slist_next(objects);

		// no need to draw invisible objects
		if (!object->bitmap) {
			continue;
		}

		int x = object->x - sx;
		int y = object->y - sy;

		// make sure it's on-screen; if it's not, don't draw it
		if ((x + object->width) < 0 || x > sw || y < 0 || (y - object->height) > sh) {
			continue;
		}

		al_draw_tinted_bitmap(object->bitmap, color, x, y-object->height, flags);
	}

	al_hold_bitmap_drawing(false);
}

static void _al_draw_orthogonal_map(ALLEGRO_MAP *map, ALLEGRO_COLOR tint, float sx, float sy, float sw, float sh, float dx, float dy, int flags)
{
	GSList *layers = map->layers;
	while (layers) {
		ALLEGRO_MAP_LAYER *layer = (ALLEGRO_MAP_LAYER*)layers->data;
		layers = g_slist_next(layers);
		if (layer->type == TILE_LAYER) {
			_al_draw_orthogonal_tile_layer(layer, map, tint, sx, sy, sw, sh, dx, dy, flags);
		} else if (layer->type == OBJECT_LAYER) {
			_al_draw_orthogonal_object_layer(layer, map, tint, sx, sy, sw, sh, dx, dy, flags);
		}
	}
}

/*
 * Draw the whole map to the target backbuffer at the given location using the given tint.
 * NOTE: the tint will not override the layer opacity property; the two alpha values are combined.
 */
void al_draw_tinted_map(ALLEGRO_MAP *map, ALLEGRO_COLOR tint, float dx, float dy, int flags)
{
	if (!strcmp(map->orientation, "orthogonal")) {
		_al_draw_orthogonal_map(map, tint, 0, 0, map->width * map->tile_width, map->height * map->tile_width, dx, dy, flags);
	} else {
		fprintf(stderr, "Error: can't draw map with orientation \"%s\"\n", map->orientation);
	}
}

/*
 * Draw a region of the map to the target backbuffer using the given tint.
 * NOTE: the tint will not override the layer opacity property; the two alpha values are combined.
 */
void al_draw_tinted_map_region(ALLEGRO_MAP *map, ALLEGRO_COLOR tint, float sx, float sy, float sw, float sh, float dx, float dy, int flags)
{
	if (!strcmp(map->orientation, "orthogonal")) {
		_al_draw_orthogonal_map(map, tint, sx, sy, sw, sh, dx, dy, flags);
	} else {
		fprintf(stderr, "Error: can't draw map with orientation \"%s\"\n", map->orientation);
	}
}

/*
 * Draw a region of the map to the target backbuffer.
 */
void al_draw_map_region(ALLEGRO_MAP *map, float sx, float sy, float sw, float sh, float dx, float dy, int flags)
{
	al_draw_tinted_map_region(map, al_map_rgba_f(1, 1, 1, 1), sx, sy, sw, sh, dx, dy, flags);
}

/*
 * Draw the whole map to the target backbuffer at the given location.
 */
void al_draw_map(ALLEGRO_MAP *map, float dx, float dy, int flags)
{
	al_draw_tinted_map(map, al_map_rgba_f(1, 1, 1, 1), dx, dy, flags);
}

/*
 * Draw a layer to the target backbuffer using the given tint.
 */
void al_draw_tinted_tile_layer_for_name(ALLEGRO_MAP *map, char *name, ALLEGRO_COLOR tint, float dx, float dy, int flags)
{
	_al_draw_orthogonal_tile_layer(al_get_layer_for_name(map, name), map, tint, 0, 0, map->width * map->tile_width, map->height * map->tile_height, dx, dy, flags);
}

/*
 * Draw the whole layer to the target backbuffer at the given location.
 */
void al_draw_tile_layer_for_name(ALLEGRO_MAP *map, char *name, float dx, float dy, int flags)
{
	al_draw_tinted_tile_layer_for_name(map, name, al_map_rgba_f(1, 1, 1, 1), dx, dy, flags);
}

/*
 * Draw, tinted, a region of the layer with the given name.
 */
void al_draw_tinted_tile_layer_region_for_name(ALLEGRO_MAP *map, char *name, ALLEGRO_COLOR tint, float sx, float sy, float sw, float sh, float dx, float dy, int flags)
{
	if (!strcmp(map->orientation, "orthogonal")) {
		_al_draw_orthogonal_tile_layer(al_get_layer_for_name(map, name), map, tint, sx, sy, sw, sh, dx, dy, flags);
	} else {
		fprintf(stderr, "Error: can't draw layer with orientation \"%s\"\n", map->orientation);
	}
}

/*
 * Draw a region of the layer with the given name.
 */
void al_draw_tile_layer_region_for_name(ALLEGRO_MAP *map, char *name, float sx, float sy, float sw, float sh, float dx, float dy, int flags)
{
	al_draw_tinted_tile_layer_region_for_name(map, name, al_map_rgba_f(1, 1, 1, 1), sx, sy, sw, sh, dx, dy, flags);
}
