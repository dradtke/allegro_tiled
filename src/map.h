#include <stdio.h>
#include "list.h"

#ifndef MAP_H
#define MAP_H

// property struct (for tile properties)
typedef struct {
	char *name;				// the property's name
	char *value;			// the property's value
} map_property;

typedef struct {
	int length;
	map_property *list;
} map_prop_list;

// map tile
typedef struct {
	int id;					// the tile id
	map_prop_list *properties; // a list of properties
} map_tile;

// tileset image
typedef struct {
	char *source;			// path to this image's source
	int width;				// width of this image
	int height;				// height of this image
} map_image;

// a list of images, for map_tileset
typedef struct {
	int length;
	map_image *list;
} map_image_list;

// a list of tiles, for map_tileset
typedef struct {
	int length;
	map_tile *list;
} map_tile_list;

// tileset
typedef struct {
	int firstgid;			// first global id
	char *name;				// name
	int tile_width;			// width of each tile
	int tile_height;		// height of each tile
	map_image_list *images; // list of tileset images
	map_tile_list *tiles;   // list of tiles
} map_tileset;

// layer
typedef struct {
	char *name;				// name of the layer
	int width;				// width in tiles
	int height;				// height in tiles
	char *data;				// decoded data
	int datalen;			// length of the decoded data
} map_layer;

// a list of tilesets, for map_data
typedef struct {
	int length;
	map_tileset *list;
} map_tileset_list;

// a list of layers, for map_data
typedef struct {
	int length;
	map_layer *list;
} map_layer_list;

// map_data
typedef struct {
	int width;				// width in tiles
	int height;				// height in tiles
	int tile_width;			// width of each tile
	int tile_height;		// height of each tile
	char *orientation;		// "orthogonal" or ... isometric?
	map_tileset_list *tilesets; // a list of tilesets
	map_layer_list *layers; // a list of layers
} map_data;

void free_map(map_data *map);
void free_layer(map_layer *layer);
void free_tileset(map_tileset *tileset);
void free_tile(map_tile *tile);
void free_property(map_property *prop);
void free_tileset_image(map_image *img);

#endif
