#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "list.h"
#include "global.h"

#ifndef MAP_PARSER_H
#define MAP_PARSER_H

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

struct node *get_nodes_for_name(xmlNode *parent, char *name);
xmlNode *get_first_node_for_name(xmlNode *parent, char *name);
char *get_xml_attribute(xmlNode *node, char *name);
map_data *parse_map(const char *filename);

#endif
