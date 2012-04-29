#include <allegro5/tiled.h>

void draw_map(map_data *map)
{
	al_draw_bitmap_region(map->backbuffer, map->x, map->y, 640, 480, 0, 0, 0);
}
