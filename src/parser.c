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
 */

#include "parser.h"

/*
 * Small workaround for Allegro's list creation.
 * _al_list_create_static() doesn't work for lists of size 0.
 */
static inline _AL_LIST *create_list(size_t capacity)
{
	if (capacity == 0) {
		return _al_list_create();
	} else {
		return _al_list_create_static(capacity);
	}
}

/*
 * Decodes map data from a <data> node
 */
static void decode_layer_data(xmlNode *data_node, TILED_MAP_LAYER *layer)
{
	char *str = trim((char *)data_node->children->content);
	int datalen = layer->width * layer->height;
	layer->data = (char *)calloc(datalen, sizeof(char));

	char *encoding = get_xml_attribute(data_node, "encoding");
	if (!encoding) {
		int i = 0;
		_AL_LIST *tiles = get_children_for_name(data_node, "tile");
		_AL_LIST_ITEM *tile_item = _al_list_front(tiles);
		while (tile_item) {
			xmlNode *tile_node = (xmlNode*)_al_list_item_data(tile_item);
			tile_item = _al_list_next(tiles, tile_item);
			char *gid = get_xml_attribute(tile_node, "gid");
			layer->data[i] = atoi(gid);
			i++;
		}
	}
	else if (!strcmp(encoding, "base64")) {
		char *compression = get_xml_attribute(data_node, "compression");
		if (compression != NULL) {
			if (strcmp(compression, "zlib") && strcmp(compression, "gzip")) {
				fprintf(stderr, "Error: unknown compression format '%s'\n", compression);
				return;
			}

			int flen = 0;
			char *data = NULL;
			FILE *tmp = tmpfile();

			decompress(str, tmp);
			fflush(tmp);

			// get file length
			flen = ftell(tmp);

			// read in the file
			rewind(tmp);
			data = (char *)calloc(flen, sizeof(char));
			if (fread(data, sizeof(char), flen, tmp) < flen) {
				fprintf(stderr, "Error: failed to read in map data\n");
				return;
			}

			// every tile id takes 4 bytes
			int i, j;
			j = 0;
			for (i = 0; i<datalen; i++) {
				int tileid = 0;
				tileid |= data[j];
				tileid |= data[j+1] << 8;
				tileid |= data[j+2] << 16;
				tileid |= data[j+3] << 24;

				layer->data[j/4] = tileid;
				j += 4;
			}
			//	printf("layer dimensions: %dx%d, data length = %d\n", 
			//			layer->width, layer->height, flen);

			fclose(tmp);
			al_free(data);
		}
		else {
			// uncompressed base64
			unsigned char in[CHUNK];
			unsigned char debased[CHUNK];
			int done = 0;
			int i = 0;
			while (done < datalen) {
				int len = strlen(str + done);
				if (len > CHUNK)
					len = CHUNK;

				strncpy((char *)in, str + done, len);
				int avail = UnBase64(debased, in, len) / 4;
				done += avail;

				int j;
				for (j = 0; j<avail; j++) {
					int tileid = 0;
					tileid |= debased[i];
					tileid |= debased[i+1] << 8;
					tileid |= debased[i+2] << 16;
					tileid |= debased[i+3] << 24;

					layer->data[i/4] = tileid;
					i += 4;
				}
			}
		}
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
static void cache_tile_list(TILED_MAP *map)
{
	map->tiles = create_list(0);
	_AL_LIST_ITEM *tileset_item = _al_list_front(map->tilesets);

	while (tileset_item != NULL) {
		TILED_MAP_TILESET *tileset = _al_list_item_data(tileset_item);
		tileset_item = _al_list_next(map->tilesets, tileset_item);
		_AL_LIST_ITEM *tile_item = _al_list_front(tileset->tiles);
		while (tile_item != NULL) {
			TILED_MAP_TILE *tile = _al_list_item_data(tile_item);
			tile_item = _al_list_next(tileset->tiles, tile_item);
			// this is a cache, so don't specify a destructor
			// it will get cleaned up with the associated tileset
			_al_list_push_back(map->tiles, tile);
		}
	}
}

/*
 * Parse a <properties> node into a list of property objects.
 */
static _AL_LIST *parse_properties(xmlNode *node)
{
	xmlNode *properties_node = get_first_child_for_name(node, "properties");
	if (!properties_node)
		return _al_list_create();

	_AL_LIST *properties_list = get_children_for_name(properties_node, "property");
	_AL_LIST *props = create_list(_al_list_size(properties_list));

	_AL_LIST_ITEM *property_item = _al_list_front(properties_list);
	while (property_item) {
		xmlNode *property_node = _al_list_item_data(property_item);
		property_item = _al_list_next(properties_list, property_item);

		TILED_PROPERTY *prop = MALLOC(TILED_PROPERTY);
		prop->name = copy(get_xml_attribute(property_node, "name"));
		prop->value = copy(get_xml_attribute(property_node, "value"));
		_al_list_push_back_ex(props, prop, dtor_prop);
	}

	_al_list_destroy(properties_list);

	return props;
}

/*
 * Parses a map file
 * Given the path to a map file, returns a new map struct
 * The struct must be freed once it's done being used
 */
TILED_MAP *al_open_map(const char *dir, const char *filename)
{
	xmlDoc *doc;
	xmlNode *root;
	TILED_MAP *map;

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
	map = MALLOC(TILED_MAP);
	map->x = 0;
	map->y = 0;
	map->width = atoi(get_xml_attribute(root, "width"));
	map->height = atoi(get_xml_attribute(root, "height"));
	map->tile_width = atoi(get_xml_attribute(root, "tilewidth"));
	map->tile_height = atoi(get_xml_attribute(root, "tileheight"));
	map->orientation = copy(get_xml_attribute(root, "orientation"));
	map->pixel_width = map->width * map->tile_width;
	map->pixel_height = map->height * map->tile_height;

	//map->bounds = MALLOC(TILED_MAP_BOUNDS);
	//map->bounds->left = 0;
	//map->bounds->top = 0;
	//map->bounds->right = map->width * map->tile_width;
	//map->bounds->bottom = map->height * map->tile_height;

	// Get the tilesets
	_AL_LIST *tilesets = get_children_for_name(root, "tileset");
	map->tilesets = create_list(_al_list_size(tilesets));

	_AL_LIST_ITEM *tileset_item = _al_list_front(tilesets);
	while (tileset_item) {
		xmlNode *tileset_node = (xmlNode*)_al_list_item_data(tileset_item);
		tileset_item = _al_list_next(tilesets, tileset_item);

		TILED_MAP_TILESET *tileset = MALLOC(TILED_MAP_TILESET);
		tileset->firstgid = atoi(get_xml_attribute(tileset_node, "firstgid"));
		tileset->tilewidth = atoi(get_xml_attribute(tileset_node, "tilewidth"));
		tileset->tileheight = atoi(get_xml_attribute(tileset_node, "tileheight"));
		tileset->name = copy(get_xml_attribute(tileset_node, "name"));

		// Get this tileset's image
		xmlNode *image_node = get_first_child_for_name(tileset_node, "image");
		tileset->width = atoi(get_xml_attribute(image_node, "width"));
		tileset->height = atoi(get_xml_attribute(image_node, "height"));
		tileset->source = copy(get_xml_attribute(image_node, "source"));
		tileset->bitmap = al_load_bitmap(tileset->source);

		// Get this tileset's tiles
		_AL_LIST *tiles = get_children_for_name(tileset_node, "tile");
		tileset->tiles = create_list(_al_list_size(tiles));

		_AL_LIST_ITEM *tile_item = _al_list_front(tiles);
		while (tile_item) {
			xmlNode *tile_node = (xmlNode*)_al_list_item_data(tile_item);
			tile_item = _al_list_next(tiles, tile_item);

			TILED_MAP_TILE *tile = MALLOC(TILED_MAP_TILE);
			tile->id = tileset->firstgid + atoi(get_xml_attribute(tile_node, "id"));
			tile->tileset = tileset;
			tile->bitmap = NULL;

			// Get this tile's properties
			tile->properties = parse_properties(tile_node);

			_al_list_push_back_ex(tileset->tiles, tile, dtor_map_tile);
		}

		_al_list_destroy(tiles);
		_al_list_push_back_ex(map->tilesets, tileset, dtor_map_tileset);
	}

	_al_list_destroy(tilesets);

	// Create the map's master list of tiles
	cache_tile_list(map);

	// Get the layers
	_AL_LIST *layers = get_children_for_name(root, "layer");
	map->layers = create_list(_al_list_size(layers));

	_AL_LIST_ITEM *layer_item = _al_list_front(layers);
	while (layer_item) {
		xmlNode *layer_node = _al_list_item_data(layer_item);
		layer_item = _al_list_next(layers, layer_item);

		TILED_MAP_LAYER *layer = MALLOC(TILED_MAP_LAYER);
		layer->name = copy(get_xml_attribute(layer_node, "name"));
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

				TILED_MAP_TILE *tile = al_get_tile_for_id(map, id);
				if (!tile) {
					// wasn't defined in the map file, presumably because it had no properties
					tile = MALLOC(TILED_MAP_TILE);
					tile->id = id;
					tile->properties = _al_list_create();
					tile->tileset = NULL;
					tile->bitmap = NULL;

					// locate its tilemap
					_AL_LIST_ITEM *tileset_item = _al_list_front(map->tilesets);
					while (tileset_item) {
						TILED_MAP_TILESET *tileset = _al_list_item_data(tileset_item);
						tileset_item = _al_list_next(map->tilesets, tileset_item);
						if (tileset->firstgid <= id) {
							if (!tile->tileset || tileset->firstgid > tile->tileset->firstgid) {
								tile->tileset = tileset;
							}
						}
					}

					_al_list_push_back_ex(map->tiles, tile, dtor_map_tile);
				}

				// create this tile's bitmap if it hasn't been yet
				if (!tile->bitmap) {
					TILED_MAP_TILESET *tileset = tile->tileset;
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

		_al_list_push_back_ex(map->layers, layer, dtor_map_layer);
	}

	_al_list_destroy(layers);

	// Get the objects
	map->object_groups = _al_list_create();
	map->objects = _al_list_create();
	_AL_LIST *object_groups = get_children_for_name(root, "objectgroup");
	_AL_LIST_ITEM *group_item = _al_list_front(object_groups);
	while (group_item) {
		xmlNode *group_node = _al_list_item_data(group_item);
		group_item = _al_list_next(object_groups, group_item);

		TILED_OBJECT_GROUP *group = MALLOC(TILED_OBJECT_GROUP);
		group->name = copy(get_xml_attribute(group_node, "name"));

		char *group_opacity = get_xml_attribute(group_node, "opacity");
		group->opacity = (group_opacity ? atof(group_opacity) : 1);

		char *group_visible = get_xml_attribute(group_node, "visible");
		group->visible = (group_visible ? atoi(group_visible) : 1);

		_AL_LIST *objects = get_children_for_name(group_node, "object");

		_AL_LIST_ITEM *object_item = _al_list_front(objects);
		while (object_item) {
			xmlNode *object_node = _al_list_item_data(object_item);
			object_item = _al_list_next(objects, object_item);

			TILED_OBJECT *object = MALLOC(TILED_OBJECT);
			object->group = group;
			object->name = copy(get_xml_attribute(object_node, "name"));
			object->type = copy(get_xml_attribute(object_node, "type"));
			object->x = atoi(get_xml_attribute(object_node, "x"));
			object->y = atoi(get_xml_attribute(object_node, "y"));

			char *object_width = get_xml_attribute(object_node, "width");
			object->width = (object_width ? atoi(object_width) : 0);

			char *object_height = get_xml_attribute(object_node, "height");
			object->height = (object_height ? atoi(object_height) : 0);

			char *gid = get_xml_attribute(object_node, "gid");
			if (gid) {
				TILED_MAP_TILE *object_tile = al_get_tile_for_id(map, atoi(gid));
				object->bitmap = object_tile->bitmap;
			}

			char *object_visible = get_xml_attribute(object_node, "visible");
			object->visible = (object_visible ? atoi(object_visible) : 1);

			// Get the object's properties
			object->properties = parse_properties(object_node);

			_al_list_push_back_ex(map->objects, object, dtor_map_object);
		}

		_al_list_push_back_ex(map->object_groups, group, dtor_map_object_group);
		_al_list_destroy(objects);
	}

	_al_list_destroy(object_groups);

	xmlFreeDoc(doc);
	al_update_backbuffer(map);

	al_change_directory(al_path_cstr(cwd, ALLEGRO_NATIVE_PATH_SEP));

	return map;
}
