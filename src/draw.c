#include "draw.h"

void draw(map_data *map, float dx, float dy)
{
	int x, y;
	_AL_LIST_ITEM *layer_item = _al_list_front(map->layers);
	while (layer_item != NULL) {
		map_layer *layer_ob = _al_list_item_data(layer_item);
		for (y = 0; y<layer_ob->height; y++) {
			for (x = 0; x<layer_ob->width; x++) {
				char id = get_tile_id(layer_ob, x, y);
				map_tile *tile_ob = get_tile_for_id(map, id);
				if (!tile_ob)
					continue;

				float tx = dx + x*(tile_ob->tileset->tilewidth);
				float ty = dy + y*(tile_ob->tileset->tileheight);

				al_draw_bitmap(tile_ob->bitmap, tx, ty, 0);
			}
		}

		layer_item = _al_list_next(map->layers, layer_item);
	}

	al_flip_display();
}
