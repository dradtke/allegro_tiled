/*
 * This addon adds Tiled map support to the Allegro game library.
 * Copyright (c) 2012-2014 Damien Radtke - www.damienradtke.com
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *    1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 *    2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 *    3. This notice may not be removed or altered from any source
 *    distribution.
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
	char *str = g_strstrip((char *)data_node->children->content);
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
			al_fseek(datasrc, 0, ALLEGRO_SEEK_SET);
			//al_fclose(datasrc);
			//datasrc = al_fopen(al_path_cstr(srcpath, ALLEGRO_NATIVE_PATH_SEP), "rb");
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
	GHashTable *props = g_hash_table_new_full(&g_str_hash, &g_str_equal, &g_free, &g_free);

	xmlNode *properties_node = get_first_child_for_name(node, "properties");
	if (!properties_node) {
		return props;
	}

	GSList *properties_list = get_children_for_name(properties_node, "property");
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

static enum relative_to resources_rel_to = RELATIVE_TO_EXE;

/*
 * Set where `al_open_map()` finds its maps; the two options are
 *   1. RELATIVE_TO_EXE (default)
 *   2. RELATIVE_TO_CWD
 */
void al_find_resources_as(enum relative_to rel)
{
	resources_rel_to = rel;
}

/*
 * Parse a map file into an `ALLEGRO_MAP` struct. By default, relative values of `dir`
 * will be resolved relative to the path of the running executable. To change this,
 * call `al_find_resources_as(RELATIVE_TO_CWD);` first.
 */
ALLEGRO_MAP *al_open_map(const char *dir, const char *filename)
{
	char *cwd = al_get_current_directory();
	if (!cwd) {
		fprintf(stderr, "failed to get cwd; errno = %d", al_get_errno());
		return NULL;
	}

	// `resources` will point to either cwd or the location of the running executable,
	// depending on what was passed in for `rel`.
	ALLEGRO_PATH *resources;
	switch (resources_rel_to) {
		case RELATIVE_TO_EXE:
			resources = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
			break;

		case RELATIVE_TO_CWD:
			resources = al_create_path(cwd);
			break;

		default:
			fprintf(stderr, "unexpected value for `resources_rel_to` in al_open_map(): %d\n",
					resources_rel_to);
			al_free(cwd);
			return NULL;
	}

	ALLEGRO_PATH *map_dir = al_create_path(dir);

	// Change directory to <cwd>/dir if dir is relative, otherwise dir.
	ALLEGRO_PATH *new_path = (al_join_paths(resources, map_dir) ? resources : map_dir);
	const char *new_path_cstr = al_path_cstr(new_path, ALLEGRO_NATIVE_PATH_SEP);
	if (!al_change_directory(new_path_cstr)) {
		fprintf(stderr, "Error: failed to cd into `%s` in al_open_map().\n",
				new_path_cstr);

		al_destroy_path(resources);
		al_destroy_path(map_dir);
		al_free(cwd);
		return NULL;
	}

	al_destroy_path(resources);
	al_destroy_path(map_dir);

	// Read in the data file
	xmlDoc *doc = xmlReadFile(filename, NULL, 0);
	if (!doc) {
		fprintf(stderr, "Error: failed to parse map data: %s\n", filename);
		al_free(cwd);
		return NULL;
	}

	// Get the root element, <map>
	xmlNode *root = xmlDocGetRootElement(doc);

	// Get some basic info
	ALLEGRO_MAP *map = MALLOC(ALLEGRO_MAP);
	map->width = atoi(get_xml_attribute(root, "width"));
	map->height = atoi(get_xml_attribute(root, "height"));
	map->tile_width = atoi(get_xml_attribute(root, "tilewidth"));
	map->tile_height = atoi(get_xml_attribute(root, "tileheight"));
	map->orientation = g_strdup(get_xml_attribute(root, "orientation"));
	map->tile_layer_count = 0;
	map->object_layer_count = 0;

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
	GSList *layers = get_children_for_either_name(root, "layer", "objectgroup");
	map->layers = NULL;

	GSList *layer_item = layers;
	while (layer_item) {
		xmlNode *layer_node = (xmlNode*)layer_item->data;
		layer_item = g_slist_next(layer_item);

		ALLEGRO_MAP_LAYER *layer = MALLOC(ALLEGRO_MAP_LAYER);
		layer->name = g_strdup(get_xml_attribute(layer_node, "name"));
		layer->properties = parse_properties(layer_node);

		char *layer_visible = get_xml_attribute(layer_node, "visible");
		layer->visible = (layer_visible != NULL ? atoi(layer_visible) : 1);

		char *layer_opacity = get_xml_attribute(layer_node, "opacity");
		layer->opacity = (layer_opacity != NULL ? atof(layer_opacity) : 1.0);

		if (!strcmp((const char*)layer_node->name, "layer")) {
			layer->type = TILE_LAYER;
			layer->width = atoi(get_xml_attribute(layer_node, "width"));
			layer->height = atoi(get_xml_attribute(layer_node, "height"));
			decode_layer_data(get_first_child_for_name(layer_node, "data"), layer);

			// Create any missing tile objects
			unsigned i, j;
			for (i = 0; i < layer->height; i++) {
				for (j = 0; j < layer->width; j++) {
					char id = al_get_single_tile_id(layer, j, i);

					if (id == 0) {
						continue;
					}

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
			map->tile_layer_count++;
			map->tile_layers = g_slist_prepend(map->tile_layers, layer);
		} else if (!strcmp((const char*)layer_node->name, "objectgroup")) {
			layer->type = OBJECT_LAYER;
			layer->objects = NULL;
			layer->object_count = 0;
			// TODO: color?
			GSList *objects = get_children_for_name(layer_node, "object");
			GSList *object_item = objects;
			while (object_item) {
				xmlNode *object_node = (xmlNode*)object_item->data;
				object_item = g_slist_next(object_item);

				ALLEGRO_MAP_OBJECT *object = MALLOC(ALLEGRO_MAP_OBJECT);
				object->layer = layer;
				object->name = g_strdup(get_xml_attribute(object_node, "name"));
				object->type = g_strdup(get_xml_attribute(object_node, "type"));
				object->x = atoi(get_xml_attribute(object_node, "x"));
				object->y = atoi(get_xml_attribute(object_node, "y"));
				object->bitmap = NULL;

				char *object_width = get_xml_attribute(object_node, "width");
				object->width = (object_width ? atoi(object_width) : 0);

				char *object_height = get_xml_attribute(object_node, "height");
				object->height = (object_height ? atoi(object_height) : 0);

				char *gid = get_xml_attribute(object_node, "gid");
				object->gid = (gid ? atoi(gid) : 0);

				char *object_visible = get_xml_attribute(object_node, "visible");
				object->visible = (object_visible ? atoi(object_visible) : 1);

				// Get the object's properties
				object->properties = parse_properties(object_node);
				layer->objects = g_slist_prepend(layer->objects, object);
				layer->object_count++;
			}
			map->object_layer_count++;
			map->object_layers = g_slist_prepend(map->object_layers, layer);
		} else {
			fprintf(stderr, "Error: found invalid layer node \"%s\"\n", layer_node->name);
			continue;
		}

		map->layers = g_slist_prepend(map->layers, layer);
	}

	g_slist_free(layers);

	// If any objects have a tile gid, cache their image
	layer_item = map->layers;
	while (layer_item) {
		ALLEGRO_MAP_LAYER *layer = (ALLEGRO_MAP_LAYER*)layer_item->data;
		layer_item = g_slist_next(layer_item);
		if (layer->type != OBJECT_LAYER) {
			continue;
		}

		GSList *objects = layer->objects;
		while (objects) {
			ALLEGRO_MAP_OBJECT *object = (ALLEGRO_MAP_OBJECT*)objects->data;
			objects = g_slist_next(objects);
			if (!object->gid) {
				continue;
			}

			object->bitmap = al_get_tile_for_id(map, object->gid)->bitmap;
			object->width = map->tile_width;
			object->height = map->tile_height;
		}
	}

	xmlFreeDoc(doc);
	al_change_directory(cwd);
	al_free(cwd);

	return map;
}
