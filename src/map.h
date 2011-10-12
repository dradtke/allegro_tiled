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
	struct node *tilesets;	// a list of tileset structs
	struct node *layers;	// a list of layers
} map_data;

// layer
typedef struct {
	char *name;				// name of the layer
	int width;				// width in tiles
	int height;				// height in tiles
	char *data;				// encoded data (should be decoded here?)
} map_layer;

// tileset
typedef struct {
	int firstgid;			// first global id
	char *name;				// name
	int tile_width;			// width of each tile
	int tile_height;		// height of each tile
	struct node *images;	// list of tileset_image structs
	struct node *tiles;		// list of tiles
} map_tileset;

// tileset image
struct tileset_image {
	char *source;			// path to this image's source
	int width;				// width of this image
	int height;				// height of this image
};

// map tile
typedef struct {
	int id;					// the tile id
	struct node *properties;// a list of properties
} map_tile;

// property struct (for tile properties)
typedef struct {
	char *name;				// the property's name
	char *value;			// the property's value
} property;

void free_map(map_data *map);

#endif
