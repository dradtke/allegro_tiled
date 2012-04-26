#include <allegro5/allegro.h>
#include <allegro5/internal/aintern_list.h>
#include <stdio.h>

#ifndef MAP_H
#define MAP_H

//{{{ map structs
// map_data
typedef struct
{
	int width;				// width in tiles
	int height;				// height in tiles
	int tile_width;			// width of each tile
	int tile_height;		// height of each tile
	char *orientation;		// "orthogonal" or ... isometric?
	_AL_LIST *tilesets;		// list of tilesets
	_AL_LIST *layers;		// list of layers
	_AL_LIST *tiles;		// full list of tiles
}
map_data;


// layer
typedef struct
{
	int width;				// width in tiles
	int height;				// height in tiles
	int datalen;			// length of the decoded data
	char *name;				// name of the layer
	char *data;				// decoded data
}
map_layer;


// tileset image
typedef struct
{
	char *source;			// path to this image's source
	ALLEGRO_BITMAP *bitmap; // allegro bitmap object
	int width;				// width of this image
	int height;				// height of this image
}
map_image;


// tileset
typedef struct
{
	int firstgid;			// first global id
	int tilewidth;			// width of each tile
	int tileheight;			// height of each tile
	char *name;				// name
	map_image *image;		// image for this tileset
	_AL_LIST *tiles;		// list of tiles
}
map_tileset;


// map tile
typedef struct
{
	int id;					// the tile id
	map_tileset *tileset;	// pointer to its tileset
	_AL_LIST *properties;	// tile properties
	ALLEGRO_BITMAP *bitmap; // this tile's image
}
map_tile;


// property struct (for tile properties)
typedef struct
{
	char *name;				// the property's name
	char *value;			// the property's value
}
map_property;
//}}}

map_tile *get_tile_for_id(map_data *map, char id);
void free_map(map_data *map);
void free_layer(map_layer *layer);
void free_tileset(map_tileset *tileset);
void free_tile(map_tile *tile);
void free_property(map_property *prop);
void free_tileset_image(map_image *img);

#endif

