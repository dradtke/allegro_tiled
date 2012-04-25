#include "draw.h"

void draw(map_data *map)
{
	unsigned i, j;
	_AL_LIST_ITEM *tileset_item = _al_list_front(map->tilesets);
	while (tileset_item != NULL) {
		map_tileset *tileset = _al_list_item_data(tileset_item);

		_AL_LIST_ITEM *image_item = _al_list_front(tileset->images);
		while (image_item != NULL) {
			map_image *image = _al_list_item_data(image_item);
			//printf("Image source: %s\n", image->source);
			image_item = _al_list_next(tileset->images, image_item);
		}

		tileset_item = _al_list_next(map->tilesets, tileset_item);
	}
}
