#include "map.h"

void free_tileset_image(struct tileset_image *img) {
	free(img->source);
	free(img);
}

void free_property(property *prop) {
	free(prop->name);
	free(prop->value);
	free(prop);
}

void free_tile(map_tile *tile) {
	struct node *properties = tile->properties;
	while (properties != NULL) {
		property *prop = (property *)properties->data;
		free_property(prop);
		properties = properties->next;
	}
	free_list(properties);

	free(tile);
}

void free_tileset(map_tileset *tileset) {
	free(tileset->name);

	// free all images
	struct node *images = tileset->images;
	while (images != NULL) {
		struct tileset_image *img = (struct tileset_image*)images->data;
		free_tileset_image(img);
		images = images->next;
	}
	free_list(tileset->images);

	// free all tiles
	struct node *tiles = tileset->tiles;
	while (tiles != NULL) {
		map_tile *tile = (map_tile *)tiles->data;
		free_tile(tile);
		tiles = tiles->next;
	}
	free_list(tileset->tiles);

	free(tileset);
}

void free_layer(map_layer *layer) {
	free(layer->name);
	free(layer->data);
	free(layer);
}

void free_map(map_data *map) {
	// free all tilesets
	struct node *tilesets = map->tilesets;
	while (tilesets != NULL) {
		map_tileset *set = (map_tileset *)tilesets->data;
		free_tileset(set);
		tilesets = tilesets->next;
	}
	free_list(map->tilesets);

	// free all layers
	struct node *layers = map->layers;
	while (layers != NULL) {
		map_layer *layer = (map_layer *)layers->data;
		free_layer(layer);
		layers = layers->next;
	}
	free_list(map->layers);

	// finally, free the whole thing
	free(map);
}
