#ifndef ALLEGRO_TILED_H
#define ALLEGRO_TILED_H

#include <allegro5/allegro.h>
#include <allegro5/internal/aintern_list.h>
#include <allegro5/internal/aintern_vector.h>
#include <ctype.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <zlib.h>

//{{{ map structs
typedef struct
{
	int left;
	int right;
	int top;
	int bottom;
}
TILED_MAP_BOUNDS;

typedef struct
{
	int x, y;				// map position
	int width, height;		// dimensions in tiles
	int tile_width;			// width of each tile
	int tile_height;		// height of each tile
	char *orientation;		// "orthogonal" or ... isometric?
	TILED_MAP_BOUNDS *bounds; // map boundaries
	_AL_LIST *tilesets;		// list of tilesets
	_AL_LIST *layers;		// list of layers
	_AL_LIST *tiles;		// full list of tiles
	ALLEGRO_BITMAP *backbuffer; // back buffer
}
TILED_MAP;

typedef struct
{
	int width;				// width in tiles
	int height;				// height in tiles
	int datalen;			// length of the decoded data
	char *name;				// name of the layer
	char *data;				// decoded data
	TILED_MAP *map;			// reference to the map
}
TILED_MAP_LAYER;

typedef struct
{
	int firstgid;			// first global id
	int tilewidth;			// width of each tile
	int tileheight;			// height of each tile
	int width, height;		// total dimensions (in pixels)
	char *name;				// name
	char *source;			// path to this tileset's image source
	ALLEGRO_BITMAP *bitmap;	// image for this tileset
	_AL_LIST *tiles;		// list of tiles
}
TILED_MAP_TILESET;

typedef struct
{
	int id;					// the tile id
	TILED_MAP_TILESET *tileset; // pointer to its tileset
	_AL_LIST *properties;	// tile properties
	ALLEGRO_BITMAP *bitmap; // this tile's image
}
TILED_MAP_TILE;

typedef struct
{
	char *name;				// the property's name
	char *value;			// the property's value
}
TILED_MAP_TILE_PROPERTY;
//}}}

// draw.h
void draw_map(TILED_MAP *map, int screen_width, int screen_height);

// map.h
TILED_MAP_TILE *tiled_get_tile_for_id(TILED_MAP *map, char id);
void tiled_free_map(TILED_MAP *map);

// parser.h
TILED_MAP *tiled_parse_map(const char *dir, const char *filename);
TILED_MAP_TILE *tiled_get_tile_for_pos(TILED_MAP_LAYER *layer, int x, int y);

#endif
