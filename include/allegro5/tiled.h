#ifndef ALLEGRO_TILED_H
#define ALLEGRO_TILED_H

#include <allegro5/allegro.h>
#include <allegro5/internal/aintern_list.h>
#include <allegro5/internal/aintern_vector.h>
#include <ctype.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>
#include <zlib.h>

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
	map_data *map;			// reference to the map
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

// draw.h
void draw(map_data *map, float dx, float dy);

// map.h
inline char get_tile_id(map_layer *layer_ob, int x, int y);
map_tile *get_tile_for_id(map_data *map, char id);
void free_map(map_data *map);

// parser.h
map_data *parse_map(const char *dir, const char *filename);
map_tile *get_tile_for_pos(map_layer *layer, int x, int y);

// util.h
char *trim(char *str);
char *copy(const char *str);

// xml.h
_AL_VECTOR *get_children_for_name(xmlNode *parent, char *name);
xmlNode *get_first_child_for_name(xmlNode *parent, char *name);
char *get_xml_attribute(xmlNode *node, char *name);

// zlib.h
// Hack to make it work on Windows
// ???: Is this actually needed?
#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif

#define CHUNK 16384

int decompress(char *src, FILE *dest);

#endif
