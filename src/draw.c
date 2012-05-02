#include <allegro5/tiled.h>
#include "internal.h"

void draw_map(TILED_MAP *map, int screen_width, int screen_height)
{
	al_draw_bitmap_region(map->backbuffer, map->x, map->y,
			screen_width, screen_height, 0, 0, 0);
}
