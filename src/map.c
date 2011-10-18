#include "map.h"

/*
 * Frees a tileset image struct from memory
 */
void free_tileset_image(map_tileset_img *img) {
	free(img->source);
	free(img);
}

/*
 * Frees a property struct from memory
 */
void free_property(property *prop) {
	free(prop->name);
	free(prop->value);
	free(prop);
}

/*
 * Frees a tile struct from memory
 */
void free_tile(map_tile *tile) {
	list properties = tile->properties;
	while (properties != NULL) {
		property *prop = (property *)properties->data;
		free_property(prop);
		properties = properties->next;
	}
	free_list(properties);

	free(tile);
}

/*
 * Frees a tileset struct from memory
 */
void free_tileset(map_tileset *tileset) {
	free(tileset->name);

	// free all images
	list images = tileset->images;
	while (images != NULL) {
		map_tileset_img *img = (map_tileset_img*)images->data;
		free_tileset_image(img);
		images = images->next;
	}
	free_list(tileset->images);

	// free all tiles
	list tiles = tileset->tiles;
	while (tiles != NULL) {
		map_tile *tile = (map_tile *)tiles->data;
		free_tile(tile);
		tiles = tiles->next;
	}
	free_list(tileset->tiles);

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
	// free all tilesets
	list tilesets = map->tilesets;
	while (tilesets != NULL) {
		map_tileset *set = (map_tileset *)tilesets->data;
		free_tileset(set);
		tilesets = tilesets->next;
	}
	free_list(map->tilesets);

	// free all layers
	list layers = map->layers;
	while (layers != NULL) {
		map_layer *layer = (map_layer *)layers->data;
		free_layer(layer);
		layers = layers->next;
	}
	free_list(map->layers);

	// finally, free the whole thing
	free(map);
}
