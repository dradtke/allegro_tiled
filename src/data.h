#ifndef _DATA_H
#define _DATA_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_tiled.h>
#include <glib.h>

struct _ALLEGRO_MAP
{
	int width, height;          // dimensions in tiles
	int tile_width;             // width of each tile in pixels
	int tile_height;            // height of each tile in pixels
    char *orientation;          // "orthogonal" or ... isometric?
    GSList *layers;             // list of layers
    GSList *tilesets;           // list of tilesets
    GSList *object_groups;      // list of object groups
    GSList *objects;            // list of objects
    GHashTable *tiles;          // full list of tiles
};

struct _ALLEGRO_MAP_LAYER
{
	int width;                  // width in tiles
	int height;                 // height in tiles
	float opacity;              // the layer's opacity
	bool visible;               // 0 for hidden, 1 for visible
	char *name;                 // name of the layer
	char *data;                 // decoded data
};

struct _ALLEGRO_MAP_TILESET
{
	int firstgid;               // first global id
	int tilewidth;              // width of each tile
	int tileheight;             // height of each tile
	int width, height;          // total dimensions (in pixels)
	char *name;                 // name
	char *source;               // path to this tileset's image source
	ALLEGRO_BITMAP *bitmap;     // image for this tileset
	GSList *tiles;              // list of tiles
};

struct _ALLEGRO_MAP_TILE
{
	int id;                       // the tile id
	ALLEGRO_MAP_TILESET *tileset; // pointer to its tileset
	GHashTable *properties;       // tile properties
	ALLEGRO_BITMAP *bitmap;       // this tile's image
};

struct _ALLEGRO_MAP_OBJECT_GROUP
{
	char *name;
	// color?
	float opacity;
	bool visible;
};

struct _ALLEGRO_MAP_OBJECT
{
	ALLEGRO_MAP_OBJECT_GROUP *group;
	char *name;
	char *type;
	int x, y;
	int width, height;
	bool visible;
	ALLEGRO_BITMAP *bitmap;
	GHashTable *properties;
};

int al_get_map_width(ALLEGRO_MAP *map);
int al_get_map_height(ALLEGRO_MAP *map);
int al_get_map_height(ALLEGRO_MAP *map);
int al_get_tile_height(ALLEGRO_MAP *map);
char *al_get_map_orientation(ALLEGRO_MAP *map);
ALLEGRO_MAP_LAYER *al_get_map_layer(ALLEGRO_MAP *map, char *name);

#endif
