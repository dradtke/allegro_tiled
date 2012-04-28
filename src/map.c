#include <allegro5/tiled.h>

inline char get_tile_id(map_layer *layer_ob, int x, int y)
{
	// TODO: check bits 32-30 for "flipped" properties and clear them
	return layer_ob->data[x+(y*layer_ob->width)];
}

/*
 * Looks up tiles in a map by id.
 * TODO: is there a way to index this by id,
 * rather than looping through every time?
 */
map_tile *get_tile_for_id(map_data *map, char id)
{
	if (id == 0)
		return NULL;

	_AL_LIST_ITEM *tile_item = _al_list_front(map->tiles);
	while (tile_item != NULL) {
		map_tile *tile = _al_list_item_data(tile_item);
		if (tile->id == id)
			return tile;

		tile_item = _al_list_next(map->tiles, tile_item);
	}

	return NULL;
}

/*
 * Frees a map struct from memory
 * TODO: finish this
 */
void free_map(map_data *map)
{
	// free(map->tilesets);
	free(map);
}
