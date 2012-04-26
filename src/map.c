#include "map.h"

/*
 * Looks up tiles in a map by id.
 * TODO: is there a way to index this by id,
 * rather than looping through every time?
 */
map_tile *get_tile_for_id(map_data *map, char id)
{
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
 * Frees a tileset image struct from memory
 */
void free_tileset_image(map_image *img)
{
	free(img->source);
	free(img->bitmap);
	free(img);
}

/*
 * Frees a property struct from memory
 */
void free_property(map_property *prop)
{
	free(prop->name);
	free(prop->value);
	free(prop);
}

/*
 * Frees a tile struct from memory
 */
void free_tile(map_tile *tile)
{
	//free(tile->properties);
	free(tile);
}

/*
 * Frees a tileset struct from memory
 */
void free_tileset(map_tileset *tileset)
{
	free(tileset->name);
	//free(tileset->images);
	//free(tileset->tiles);

	free(tileset);
}

/*
 * Frees a layer struct from memory
 */
void free_layer(map_layer *layer)
{
	free(layer->name);
	free(layer->data);
	free(layer);
}

/*
 * Frees a map struct from memory
 */
void free_map(map_data *map)
{
	// free(map->tilesets);
	free(map);
}
