#include <allegro5/tiled.h>
#include "internal.h"

char tile_id(TILED_MAP_LAYER *layer_ob, int x, int y)
{
	// TODO: check bits 32-30 for "flipped" properties and clear them
	return layer_ob->data[x+(y*layer_ob->width)];
}

/*
 * Looks up tiles in a map by id.
 * TODO: is there a way to index this by id,
 * rather than looping through every time?
 */
TILED_MAP_TILE *tiled_get_tile_for_id(TILED_MAP *map, char id)
{
	if (id == 0)
		return NULL;

	_AL_LIST_ITEM *tile_item = _al_list_front(map->tiles);
	while (tile_item != NULL) {
		TILED_MAP_TILE *tile = _al_list_item_data(tile_item);
		if (tile->id == id)
			return tile;

		tile_item = _al_list_next(map->tiles, tile_item);
	}

	return NULL;
}

/*
 * Frees a map struct from memory
 */
void tiled_free_map(TILED_MAP *map)
{
	free(map->bounds);
	free(map->orientation);
	_al_list_destroy(map->tilesets);
	_al_list_destroy(map->layers);
	_al_list_destroy(map->tiles);
	al_destroy_bitmap(map->backbuffer);
	free(map);
}

void dtor_map_tileset(void *value, void *user_data)
{
	TILED_MAP_TILESET *tileset_ob = (TILED_MAP_TILESET*)value;
	free(tileset_ob->name);
	free(tileset_ob->source);
	_al_list_destroy(tileset_ob->tiles);
	al_destroy_bitmap(tileset_ob->bitmap);
	free(tileset_ob);
}

void dtor_map_tile(void *value, void *user_data)
{
	TILED_MAP_TILE *tile_ob = (TILED_MAP_TILE*)value;
	_al_list_destroy(tile_ob->properties);
	al_destroy_bitmap(tile_ob->bitmap);
	free(tile_ob);
}

void dtor_map_layer(void *value, void *user_data)
{
	TILED_MAP_LAYER *layer_ob = (TILED_MAP_LAYER*)value;
	free(layer_ob->name);
	free(layer_ob->data);
	free(layer_ob);
}

void dtor_tile_prop(void *value, void *user_data)
{
	TILED_MAP_TILE_PROPERTY *prop_ob = (TILED_MAP_TILE_PROPERTY*)value;
	free(prop_ob->name);
	free(prop_ob->value);
	free(prop_ob);
}
