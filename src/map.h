#include <stdio.h>
#include "list.h"

#ifndef MAP_H
#define MAP_H

// map_data
typedef struct {
	int width;				// width in tiles
	int height;				// height in tiles
	int tile_width;			// width of each tile
	int tile_height;		// height of each tile
	char *orientation;		// "orthogonal" or ... isometric?
	list tilesets;			// a list of tileset structs
	list layers;			// a list of layers
} map_data;

// layer
typedef struct {
	char *name;				// name of the layer
	int width;				// width in tiles
	int height;				// height in tiles
	char *data;				// decoded data
	int datalen;			// length of the decoded data
} map_layer;

// tileset
typedef struct {
	int firstgid;			// first global id
	char *name;				// name
	int tile_width;			// width of each tile
	int tile_height;		// height of each tile
	list images;			// list of tileset_image structs
	list tiles;				// list of tiles
} map_tileset;

// tileset image
typedef struct {
	char *source;			// path to this image's source
	int width;				// width of this image
	int height;				// height of this image
} map_tileset_img;

// map tile
typedef struct {
	int id;					// the tile id
	list properties;		// a list of properties
} map_tile;

// property struct (for tile properties)
typedef struct {
	char *name;				// the property's name
	char *value;			// the property's value
} property;

void free_map(map_data *map);

#endif
