#include <allegro5/allegro.h>
#include <allegro5/internal/aintern_list.h>
#include <stdio.h>

#ifndef MAP_H
#define MAP_H

//{{{ map structs
// property struct (for tile properties)
typedef struct
{
	char *name;				// the property's name
	char *value;			// the property's value
}
map_property;


// map tile
typedef struct
{
	int id;					// the tile id
	_AL_LIST *properties;	// tile properties
}
map_tile;


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
	char *name;				// name
	int tilewidth;			// width of each tile
	int tileheight;		// height of each tile
	_AL_LIST *images;		// images for this tileset
	_AL_LIST *tiles;		// list of tiles
}
map_tileset;


// layer
typedef struct
{
	char *name;				// name of the layer
	int width;				// width in tiles
	int height;				// height in tiles
	char *data;				// decoded data
	int datalen;			// length of the decoded data
}
map_layer;


// map_data
typedef struct
{
	int width;				// width in tiles
	int height;				// height in tiles
	int tile_width;			// width of each tile
	int tile_height;		// height of each tile
	char *orientation;		// "orthogonal" or ... isometric?
	_AL_LIST *tilesets;	// list of tilesets
}
map_data;
//}}}

void free_map(map_data *map);
void free_layer(map_layer *layer);
void free_tileset(map_tileset *tileset);
void free_tile(map_tile *tile);
void free_property(map_property *prop);
void free_tileset_image(map_image *img);

#endif

