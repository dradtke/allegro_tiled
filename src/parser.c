/*
 * This addon adds Tiled map support to the Allegro game library.
 * Copyright (c) 2012 Damien Radtke - www.damienradtke.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * For more information, visit http://www.gnu.org/copyleft
 *
 *                               ---
 *
 * Methods for reading and parsing map files.
 */

#include "parser.h"

/*
 * Small workaround for Allegro's list creation.
 * _al_list_create_static() doesn't work for lists of size 0.
static inline _AL_LIST *create_list(size_t capacity)
{
	if (capacity == 0) {
		return _al_list_create();
	} else {
		return _al_list_create_static(capacity);
	}
}
*/

/*
 * Decodes map data from a <data> node
 */
static void decode_layer_data(xmlNode *data_node, ALLEGRO_MAP_LAYER *layer)
{
	char *str = trim((char *)data_node->children->content);
	int datalen = layer->width * layer->height;
	layer->data = (char *)calloc(datalen, sizeof(char));

	char *encoding = get_xml_attribute(data_node, "encoding");
	if (!encoding) {
		int i = 0;
		GSList *tiles = get_children_for_name(data_node, "tile");
		GSList *tile_item = tiles;
		while (tile_item) {
			xmlNode *tile_node = (xmlNode*)tile_item->data;
			tile_item = g_slist_next(tile_item);
			char *gid = get_xml_attribute(tile_node, "gid");
			layer->data[i] = atoi(gid);
			i++;
		}

		g_slist_free(tiles);
	}
	else if (!strcmp(encoding, "base64")) {
		// decompress
		gsize rawlen;
		unsigned char *rawdata = g_base64_decode(str, &rawlen);

		// check the compression
		char *compression = get_xml_attribute(data_node, "compression");
		if (compression != NULL) {
			if (strcmp(compression, "zlib") && strcmp(compression, "gzip")) {
				fprintf(stderr, "Error: unknown compression format '%s'\n", compression);
				return;
			}

			// set up files used by zlib to decompress the data
			ALLEGRO_PATH *srcpath;
			ALLEGRO_FILE *datasrc = al_make_temp_file("XXXXXX", &srcpath);
			al_fwrite(datasrc, rawdata, rawlen);
			al_fclose(datasrc);
			datasrc = al_fopen(al_path_cstr(srcpath, ALLEGRO_NATIVE_PATH_SEP), "rb");
			ALLEGRO_FILE *datadest = al_make_temp_file("XXXXXX", NULL);

			// decompress and print an error if it failed
			int status = inf(datasrc, datadest);
			if (status)
				zerr(status);

			// flush data and get the file length
			al_fflush(datadest);
			int len = al_fsize(datadest);

			// read in the file
			al_fseek(datadest, 0, ALLEGRO_SEEK_SET);
			char *data = (char *)calloc(len, sizeof(char));
			if (al_fread(datadest, data, len) != len) {
				fprintf(stderr, "Error: failed to read in map data\n");
				return;
			}

			// every tile id takes 4 bytes
			int i;
			for (i = 0; i<len; i += 4) {
				int tileid = 0;
				tileid |= data[i];
				tileid |= data[i+1] << 8;
				tileid |= data[i+2] << 16;
				tileid |= data[i+3] << 24;

				layer->data[i/4] = tileid;
			}
			/*	printf("layer dimensions: %dx%d, data length = %d\n", 
						layer->width, layer->height, len); */

			al_destroy_path(srcpath);
			al_fclose(datasrc);
			al_fclose(datadest);
			al_free(data);
		}
		else {
			// TODO: verify that this still works
			int i;
			for (i = 0; i<rawlen; i += 4) {
				int tileid = 0;
				tileid |= rawdata[i];
				tileid |= rawdata[i+1] << 8;
				tileid |= rawdata[i+2] << 16;
				tileid |= rawdata[i+3] << 24;

				layer->data[i/4] = tileid;
			}
		}

		g_free(rawdata);
	}
	else if (!strcmp(encoding, "csv")) {
		int i;
		for (i = 0; i<datalen; i++) {
			char *id = strtok((i == 0 ? str : NULL), ",");
			layer->data[i] = atoi(id);
		}
	}
	else {
		fprintf(stderr, "Error: unknown encoding format '%s'\n", encoding);
	}
}

/*
 * After all the tiles have been parsed out of their tilesets,
 * create the map's global list of tiles.
 */
static void cache_tile_list(ALLEGRO_MAP *map)
{
	map->tiles = g_hash_table_new(NULL, NULL);
	GSList *tileset_item = map->tilesets;

	while (tileset_item != NULL) {
		ALLEGRO_MAP_TILESET *tileset = (ALLEGRO_MAP_TILESET*)tileset_item->data;
		tileset_item = g_slist_next(tileset_item);
		GSList *tile_item = tileset->tiles;
		while (tile_item != NULL) {
			ALLEGRO_MAP_TILE *tile = (ALLEGRO_MAP_TILE*)tile_item->data;
			tile_item = g_slist_next(tile_item);
			// associate the tile's id with its ALLEGRO_TILE struct
			g_hash_table_insert(map->tiles, GINT_TO_POINTER(tile->id), tile);
		}
	}
}

/*
 * Parse a <properties> node into a list of property objects.
 */
static GHashTable *parse_properties(xmlNode *node)
{
	xmlNode *properties_node = get_first_child_for_name(node, "properties");
	if (!properties_node) {
		return NULL;
	}

	GSList *properties_list = get_children_for_name(properties_node, "property");
	// TODO: check if we need to specify the destructor
	GHashTable *props = g_hash_table_new_full(NULL, &g_str_equal, g_free, g_free);

	GSList *property_item = properties_list;
	while (property_item) {
		xmlNode *property_node = (xmlNode*)property_item->data;
		property_item = g_slist_next(property_item);

		char *name = g_strdup(get_xml_attribute(property_node, "name"));
		char *value = get_xml_attribute(property_node, "value");
		if (!value) {
			value = (char*)xmlNodeGetContent(property_node);
		}

		value = g_strdup(value);
		g_hash_table_insert(props, name, value);
	}

	g_slist_free(properties_list);
	return props;
}

/*
 * Parses a map file
 * Given the path to a map file, returns a new map struct
 * The struct must be freed once it's done being used
 */
ALLEGRO_MAP *al_open_map(const char *dir, const char *filename)
{
	xmlDoc *doc;
	xmlNode *root;
	ALLEGRO_MAP *map;

	unsigned i, j;

	ALLEGRO_PATH *cwd = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
	ALLEGRO_PATH *resources = al_clone_path(cwd);
	ALLEGRO_PATH *maps = al_create_path(dir);

	al_join_paths(resources, maps);
	if (!al_change_directory(al_path_cstr(resources, ALLEGRO_NATIVE_PATH_SEP))) {
		fprintf(stderr, "Error: failed to change directory in al_parse_map().");
	}

	al_destroy_path(resources);
	al_destroy_path(maps);

	// Read in the data file
	doc = xmlReadFile(filename, NULL, 0);
	if (!doc) {
		fprintf(stderr, "Error: failed to parse map data: %s\n", filename);
		return NULL;
	}

	// Get the root element, <map>
	root = xmlDocGetRootElement(doc);

	// Get some basic info
	map = MALLOC(ALLEGRO_MAP);
	map->width = atoi(get_xml_attribute(root, "width"));
	map->height = atoi(get_xml_attribute(root, "height"));
	map->tile_width = atoi(get_xml_attribute(root, "tilewidth"));
	map->tile_height = atoi(get_xml_attribute(root, "tileheight"));
	map->orientation = g_strdup(get_xml_attribute(root, "orientation"));

	//map->bounds = MALLOC(ALLEGRO_MAP_BOUNDS);
	//map->bounds->left = 0;
	//map->bounds->top = 0;
	//map->bounds->right = map->width * map->tile_width;
	//map->bounds->bottom = map->height * map->tile_height;

	// Get the tilesets
	GSList *tilesets = get_children_for_name(root, "tileset");
	map->tilesets = NULL;

	GSList *tileset_item = tilesets;
	while (tileset_item) {
		xmlNode *tileset_node = (xmlNode*)tileset_item->data;
		tileset_item = g_slist_next(tileset_item);

		ALLEGRO_MAP_TILESET *tileset = MALLOC(ALLEGRO_MAP_TILESET);
		tileset->firstgid = atoi(get_xml_attribute(tileset_node, "firstgid"));
		tileset->tilewidth = atoi(get_xml_attribute(tileset_node, "tilewidth"));
		tileset->tileheight = atoi(get_xml_attribute(tileset_node, "tileheight"));
		tileset->name = g_strdup(get_xml_attribute(tileset_node, "name"));

		// Get this tileset's image
		xmlNode *image_node = get_first_child_for_name(tileset_node, "image");
		tileset->width = atoi(get_xml_attribute(image_node, "width"));
		tileset->height = atoi(get_xml_attribute(image_node, "height"));
		tileset->source = g_strdup(get_xml_attribute(image_node, "source"));
		tileset->bitmap = al_load_bitmap(tileset->source);

		// Get this tileset's tiles
		GSList *tiles = get_children_for_name(tileset_node, "tile");
		tileset->tiles = NULL;

		GSList *tile_item = tiles;
		while (tile_item) {
			xmlNode *tile_node = (xmlNode*)tile_item->data;
			tile_item = g_slist_next(tile_item);

			ALLEGRO_MAP_TILE *tile = MALLOC(ALLEGRO_MAP_TILE);
			tile->id = tileset->firstgid + atoi(get_xml_attribute(tile_node, "id"));
			tile->tileset = tileset;
			tile->bitmap = NULL;

			// Get this tile's properties
			tile->properties = parse_properties(tile_node);

			// TODO: add a destructor
			tileset->tiles = g_slist_prepend(tileset->tiles, tile);
		}

		g_slist_free(tiles);
		//tileset->tiles = g_slist_reverse(tileset->tiles);

		// TODO: add a destructor
		map->tilesets = g_slist_prepend(map->tilesets, tileset);
	}

	g_slist_free(tilesets);
	//map->tilesets = g_slist_reverse(map->tilesets);

	// Create the map's master list of tiles
	cache_tile_list(map);

	// Get the layers
	GSList *layers = get_children_for_name(root, "layer");
	map->layers = NULL;

	GSList *layer_item = layers;
	while (layer_item) {
		xmlNode *layer_node = (xmlNode*)layer_item->data;
		layer_item = g_slist_next(layer_item);

		ALLEGRO_MAP_LAYER *layer = MALLOC(ALLEGRO_MAP_LAYER);
		layer->name = g_strdup(get_xml_attribute(layer_node, "name"));
		layer->width = atoi(get_xml_attribute(layer_node, "width"));
		layer->height = atoi(get_xml_attribute(layer_node, "height"));
		layer->map = map;

		char *layer_visible = get_xml_attribute(layer_node, "visible");
		layer->visible = (layer_visible != NULL ? atoi(layer_visible) : 1);

		char *layer_opacity = get_xml_attribute(layer_node, "opacity");
		layer->opacity = (layer_opacity != NULL ? atof(layer_opacity) : 1.0);

		decode_layer_data(get_first_child_for_name(layer_node, "data"), layer);

		// Create any missing tile objects
		for (i = 0; i<layer->height; i++) {
			for (j = 0; j<layer->width; j++) {
				char id = al_get_single_tile(layer, j, i);

				if (id == 0)
					continue;

				ALLEGRO_MAP_TILE *tile = al_get_tile_for_id(map, id);
				if (!tile) {
					// wasn't defined in the map file, presumably because it had no properties
					tile = MALLOC(ALLEGRO_MAP_TILE);
					tile->id = id;
					tile->properties = g_hash_table_new(NULL, NULL);
					tile->tileset = NULL;
					tile->bitmap = NULL;

					// locate its tilemap
					GSList *tilesets = map->tilesets;
					ALLEGRO_MAP_TILESET *tileset_ref;
					while (tilesets) {
						ALLEGRO_MAP_TILESET *tileset = (ALLEGRO_MAP_TILESET*)tilesets->data;
						tilesets = g_slist_next(tilesets);
						if (tileset->firstgid <= id) {
							if (!tile->tileset || tileset->firstgid > tile->tileset->firstgid) {
								tileset_ref = tileset;
							}
						}

					}

					tile->tileset = tileset_ref;
					tileset_ref->tiles = g_slist_prepend(tileset_ref->tiles, tile);
					g_hash_table_insert(map->tiles, GINT_TO_POINTER(tile->id), tile);
				}

				// create this tile's bitmap if it hasn't been yet
				if (!tile->bitmap) {
					ALLEGRO_MAP_TILESET *tileset = tile->tileset;
					int id = tile->id - tileset->firstgid;
					int width = tileset->width / tileset->tilewidth;
					int x = (id % width) * tileset->tilewidth;
					int y = (id / width) * tileset->tileheight;
					tile->bitmap = al_create_sub_bitmap(
							tileset->bitmap,
							x, y,
							tileset->tilewidth,
							tileset->tileheight);
				}
			}
		}

		map->layers = g_slist_prepend(map->layers, layer);
	}

	g_slist_free(layers);
	//map->layers = g_slist_reverse(map->layers);

	// Get the objects
	map->object_groups = NULL;
	map->objects = NULL;
	GSList *object_groups = get_children_for_name(root, "objectgroup");

	GSList *group_item = object_groups;
	while (group_item) {
		xmlNode *group_node = (xmlNode*)group_item->data;
		group_item = g_slist_next(group_item);

		ALLEGRO_MAP_OBJECT_GROUP *group = MALLOC(ALLEGRO_MAP_OBJECT_GROUP);
		group->name = g_strdup(get_xml_attribute(group_node, "name"));

		char *group_opacity = get_xml_attribute(group_node, "opacity");
		group->opacity = (group_opacity ? atof(group_opacity) : 1);

		char *group_visible = get_xml_attribute(group_node, "visible");
		group->visible = (group_visible ? atoi(group_visible) : 1);

		GSList *objects = get_children_for_name(group_node, "object");

		GSList *object_item = objects;
		while (object_item) {
			xmlNode *object_node = (xmlNode*)object_item->data;
			object_item = g_slist_next(object_item);

			ALLEGRO_MAP_OBJECT *object = MALLOC(ALLEGRO_MAP_OBJECT);
			object->group = group;
			object->name = g_strdup(get_xml_attribute(object_node, "name"));
			object->type = g_strdup(get_xml_attribute(object_node, "type"));
			object->x = atoi(get_xml_attribute(object_node, "x"));
			object->y = atoi(get_xml_attribute(object_node, "y"));

			char *object_width = get_xml_attribute(object_node, "width");
			object->width = (object_width ? atoi(object_width) : 0);

			char *object_height = get_xml_attribute(object_node, "height");
			object->height = (object_height ? atoi(object_height) : 0);

			char *gid = get_xml_attribute(object_node, "gid");
			if (gid) {
				ALLEGRO_MAP_TILE *object_tile = al_get_tile_for_id(map, atoi(gid));
				object->bitmap = object_tile->bitmap;
			}

			char *object_visible = get_xml_attribute(object_node, "visible");
			object->visible = (object_visible ? atoi(object_visible) : 1);

			// Get the object's properties
			object->properties = parse_properties(object_node);

			// TODO: add a destructor
			map->objects = g_slist_prepend(map->objects, object);
		}

		// TODO: add a destructor
		map->object_groups = g_slist_prepend(map->object_groups, group);
		g_slist_free(objects);
	}

	g_slist_free(object_groups);

	xmlFreeDoc(doc);
	al_update_backbuffer(map);

	al_change_directory(al_path_cstr(cwd, ALLEGRO_NATIVE_PATH_SEP));

	return map;
}
