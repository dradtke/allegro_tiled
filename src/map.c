#include "map.h"

/*
 * Frees a tileset image struct from memory
 */
void free_tileset_image(map_image *img) {
	free(img->source);
	free(img);
}

/*
 * Frees a property struct from memory
 */
void free_property(map_property *prop) {
	free(prop->name);
	free(prop->value);
	free(prop);
}

/*
 * Frees a tile struct from memory
 */
void free_tile(map_tile *tile) {
	free(tile->properties->list);
	free(tile->properties);
	free(tile);
}

/*
 * Frees a tileset struct from memory
 */
void free_tileset(map_tileset *tileset) {
	free(tileset->name);
	free(tileset->images->list);
	free(tileset->images);
	free(tileset->tiles->list);
	free(tileset->tiles);

	free(tileset);
}

/*
 * Frees a layer struct from memory
 */
void free_layer(map_layer *layer) {
	free(layer->name);
	free(layer->data);
	free(layer);
}

/*
 * Frees a map struct from memory
 */
void free_map(map_data *map) {
	free(map->tilesets->list);
	free(map->tilesets);
	free(map);
}
