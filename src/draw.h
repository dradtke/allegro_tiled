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
 */

#ifndef _DRAW_H
#define _DRAW_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_tiled.h>
#include <stdio.h>
#include "data.h"
#include "map.h"

void al_draw_tinted_map(ALLEGRO_MAP *map, ALLEGRO_COLOR tint, float dx, float dy, int flags);
void al_draw_map(ALLEGRO_MAP *map, float dx, float dy, int flags);
void al_draw_tinted_map_region(ALLEGRO_MAP *map, ALLEGRO_COLOR tint, float sx, float sy, float sw, float sh, float dx, float dy, int flags);
void al_draw_map_region(ALLEGRO_MAP *map, float sx, float sy, float sw, float sh, float dx, float dy, int flags);
void al_draw_tile_layer_for_name(ALLEGRO_MAP *map, char *name, float dx, float dy, int flags);
void al_draw_tinted_tile_layer_region_for_name(ALLEGRO_MAP *map, char *name, ALLEGRO_COLOR tint, float sx, float sy, float sw, float sh, float dx, float dy, int flags);
void al_draw_tile_layer_region_for_name(ALLEGRO_MAP *map, char *name, float sx, float sy, float sw, float sh, float dx, float dy, int flags);
void al_draw_objects(ALLEGRO_MAP *map);

#endif
