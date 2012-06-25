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

#include <allegro5/tiled.h>
#include "internal.h"


/*
 * Small workaround for Allegro's list creation.
 * _al_list_create_static() doesn't work for lists of size 0.
 */
static inline _AL_LIST *create_list(size_t capacity)
{
	if (capacity == 0)
		return _al_list_create();
	else
		return _al_list_create_static(capacity);
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
			char *gid = get_xml_attribute(tile_node, "gid");
			layer->data[i] = atoi(gid);
			i++;
			tile_item = _al_list_next(tiles, tile_item);
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
		TILED_MAP_TILESET *tileset_ob = _al_list_item_data(tileset_item);
		_AL_LIST_ITEM *tile_item = _al_list_front(tileset_ob->tiles);

		while (tile_item != NULL) {
			TILED_MAP_TILE *tile_ob = _al_list_item_data(tile_item);
			// this is a cache, so don't specify a destructor
			// it will get cleaned up with the associated tileset
			_al_list_push_back(map->tiles, tile_ob);
			tile_item = _al_list_next(tileset_ob->tiles, tile_item);
		}

		tileset_item = _al_list_next(map->tilesets, tileset_item);
	}
}

/*
 * Parses a map file
 * Given the path to a map file, returns a new map struct
 * The struct must be freed once it's done being used
 */
TILED_MAP *tiled_parse_map(const char *dir, const char *filename)
{
	xmlDoc *doc;
	xmlNode *root;
	TILED_MAP *map;

	unsigned i, j, k;

	ALLEGRO_PATH *cwd = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
	ALLEGRO_PATH *path = al_create_path(dir);

	al_join_paths(cwd, path);
	if (!al_change_directory(al_path_cstr(cwd, ALLEGRO_NATIVE_PATH_SEP))) {
		fprintf(stderr, "Error: failed to change directory in tiled_parse_map().");
	}

	al_destroy_path(cwd);
	al_destroy_path(path);

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

	map->bounds = MALLOC(TILED_MAP_BOUNDS);
	map->bounds->left = 0;
	map->bounds->top = 0;
	map->bounds->right = map->width * map->tile_width;
	map->bounds->bottom = map->height * map->tile_height;

	// Get the tilesets
	_AL_LIST *tilesets = get_children_for_name(root, "tileset");
	map->tilesets = create_list(_al_list_size(tilesets));

	_AL_LIST_ITEM *tileset_item = _al_list_front(tilesets);
	while (tileset_item) {
		xmlNode *tileset_node = (xmlNode*)_al_list_item_data(tileset_item);
		TILED_MAP_TILESET *tileset_ob = MALLOC(TILED_MAP_TILESET);
		tileset_ob->firstgid = atoi(get_xml_attribute(tileset_node, "firstgid"));
		tileset_ob->tilewidth = atoi(get_xml_attribute(tileset_node, "tilewidth"));
		tileset_ob->tileheight = atoi(get_xml_attribute(tileset_node, "tileheight"));
		tileset_ob->name = copy(get_xml_attribute(tileset_node, "name"));

		// Get this tileset's image
		xmlNode *image_node = get_first_child_for_name(tileset_node, "image");
		tileset_ob->width = atoi(get_xml_attribute(image_node, "width"));
		tileset_ob->height = atoi(get_xml_attribute(image_node, "height"));
		tileset_ob->source = copy(get_xml_attribute(image_node, "source"));
		tileset_ob->bitmap = al_load_bitmap(tileset_ob->source);

		// Get this tileset's tiles
		_AL_LIST *tiles = get_children_for_name(tileset_node, "tile");
		tileset_ob->tiles = create_list(_al_list_size(tiles));

		_AL_LIST_ITEM *tile_item = _al_list_front(tiles);
		while (tile_item) {
			xmlNode *tile_node = (xmlNode*)_al_list_item_data(tile_item);
			TILED_MAP_TILE *tile_ob = MALLOC(TILED_MAP_TILE);
			tile_ob->id = tileset_ob->firstgid + atoi(get_xml_attribute(tile_node, "id"));
			tile_ob->tileset = tileset_ob;
			tile_ob->bitmap = NULL;

			// Get this tile's properties
			_AL_LIST *properties = get_children_for_name(
					get_first_child_for_name(tile_node, "properties"),
					"property");
			tile_ob->properties = create_list(_al_list_size(properties));

			_AL_LIST_ITEM *property_item = _al_list_front(properties);
			while (property_item) {
				xmlNode *prop_node = (xmlNode*)_al_list_item_data(property_item);
				TILED_MAP_TILE_PROPERTY *prop_ob = MALLOC(TILED_MAP_TILE_PROPERTY);
				prop_ob->name = copy(get_xml_attribute(prop_node, "name"));
				prop_ob->value = copy(get_xml_attribute(prop_node, "value"));
				_al_list_push_back_ex(tile_ob->properties, prop_ob, dtor_tile_prop);
				property_item = _al_list_next(properties, property_item);
			}

			_al_list_push_back_ex(tileset_ob->tiles, tile_ob, dtor_map_tile);
			tile_item = _al_list_next(tiles, tile_item);
		}

		_al_list_destroy(tiles);
		_al_list_push_back_ex(map->tilesets, tileset_ob, dtor_map_tileset);
		tileset_item = _al_list_next(tilesets, tileset_item);
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
		TILED_MAP_LAYER *layer_ob = MALLOC(TILED_MAP_LAYER);
		layer_ob->name = copy(get_xml_attribute(layer_node, "name"));
		layer_ob->width = atoi(get_xml_attribute(layer_node, "width"));
		layer_ob->height = atoi(get_xml_attribute(layer_node, "height"));
		layer_ob->map = map;

		char *layer_visible = get_xml_attribute(layer_node, "visible");
		layer_ob->visible = (layer_visible != NULL ? atoi(layer_visible) : 1);

		char *layer_opacity = get_xml_attribute(layer_node, "opacity");
		layer_ob->opacity = (layer_opacity != NULL ? atof(layer_opacity) : 1.0);

		decode_layer_data(get_first_child_for_name(layer_node, "data"), layer_ob);

		// Create any missing tile objects
		for (j = 0; j<layer_ob->height; j++) {
			for (k = 0; k<layer_ob->width; k++) {
				char id = tile_id(layer_ob, k, j);

				if (id == 0)
					continue;

				TILED_MAP_TILE *tile_ob = tiled_get_tile_for_id(map, id);
				if (!tile_ob) {
					// wasn't defined in the map file, presumably because it had no properties
					tile_ob = MALLOC(TILED_MAP_TILE);
					tile_ob->id = id;
					tile_ob->properties = _al_list_create();
					tile_ob->tileset = NULL;
					tile_ob->bitmap = NULL;

					// locate its tilemap
					_AL_LIST_ITEM *tileset_item = _al_list_front(map->tilesets);
					while (tileset_item != NULL) {
						TILED_MAP_TILESET *tileset_ob = _al_list_item_data(tileset_item);
						if (tileset_ob->firstgid <= id) {
							if (!tile_ob->tileset || tileset_ob->firstgid > tile_ob->tileset->firstgid) {
								tile_ob->tileset = tileset_ob;
							}
						}
						tileset_item = _al_list_next(map->tilesets, tileset_item);
					}

					_al_list_push_back_ex(map->tiles, tile_ob, dtor_map_tile);
				}

				// create this tile's bitmap if it hasn't been yet
				if (!tile_ob->bitmap) {
					TILED_MAP_TILESET *tileset = tile_ob->tileset;
					int id = tile_ob->id - tileset->firstgid;
					int width = tileset->width / tileset->tilewidth;
					int x = (id % width) * tileset->tilewidth;
					int y = (id / width) * tileset->tileheight;
					tile_ob->bitmap = al_create_sub_bitmap(
							tileset->bitmap,
							x, y,
							tileset->tilewidth,
							tileset->tileheight);
				}
			}
		}

		_al_list_push_back_ex(map->layers, layer_ob, dtor_map_layer);
		layer_item = _al_list_next(layers, layer_item);
	}

	_al_list_destroy(layers);

	// Get the objects
	map->objects = _al_list_create();
	_AL_LIST *object_groups = get_children_for_name(root, "objectgroup");
	_AL_LIST_ITEM *group_item = _al_list_front(object_groups);
	while (group_item) {
		xmlNode *group_node = _al_list_item_data(group_item);
		TILED_OBJECT_GROUP *group_ob = MALLOC(TILED_OBJECT_GROUP);
		group_ob->name = copy(get_xml_attribute(group_node, "name"));
		group_ob->ref = 0;

		char *group_opacity = get_xml_attribute(group_node, "opacity");
		group_ob->opacity = (group_opacity ? atof(group_opacity) : 1);

		char *group_visible = get_xml_attribute(group_node, "visible");
		group_ob->visible = (group_visible ? atoi(group_visible) : 1);

		_AL_LIST *objects = get_children_for_name(group_node, "object");

		_AL_LIST_ITEM *object_item = _al_list_front(objects);
		while (object_item) {
			xmlNode *object_node = _al_list_item_data(object_item);
			TILED_OBJECT *object_ob = MALLOC(TILED_OBJECT);
			object_ob->group = group_ob;
			object_ob->name = copy(get_xml_attribute(object_node, "name"));
			object_ob->type = copy(get_xml_attribute(object_node, "type"));
			object_ob->x = atoi(get_xml_attribute(object_node, "x"));
			object_ob->y = atoi(get_xml_attribute(object_node, "y"));

			char *object_width = get_xml_attribute(object_node, "width");
			object_ob->width = (object_width ? atoi(object_width) : 0);

			char *object_height = get_xml_attribute(object_node, "height");
			object_ob->height = (object_height ? atoi(object_height) : 0);

			char *gid = get_xml_attribute(object_node, "gid");
			object_ob->gid = (gid ? atoi(gid) : 0);

			char *object_visible = get_xml_attribute(object_node, "visible");
			object_ob->visible = (object_visible ? atoi(object_visible) : 1);

			_al_list_push_back_ex(map->objects, object_ob, dtor_map_object);
			
			object_item = _al_list_next(objects, object_item);
			group_ob->ref++;
		}

		/*
		if (group_ob->ref == 0)
			tiled_free_object_group(group_ob);
		*/

		_al_list_destroy(objects);
		group_item = _al_list_next(object_groups, group_item);
	}

	_al_list_destroy(object_groups);

	// Done parsing XML, so let's free the doc
	xmlFreeDoc(doc);

	// Create the map's backbuffer
	ALLEGRO_BITMAP *orig_backbuffer = al_get_target_bitmap();
	map->backbuffer = al_create_bitmap(map->bounds->right, map->bounds->bottom);
	map->bounds->right -= al_get_bitmap_width(orig_backbuffer);
	map->bounds->bottom -= al_get_bitmap_height(orig_backbuffer);
	al_set_target_bitmap(map->backbuffer);

	if (!strcmp(map->orientation, "orthogonal")) {
		_AL_LIST_ITEM *layer_item = _al_list_front(map->layers);
		while (layer_item != NULL) {
			TILED_MAP_LAYER *layer_ob = _al_list_item_data(layer_item);
			for (i = 0; i<layer_ob->height; i++) {
				for (j = 0; j<layer_ob->width; j++) {
					char id = tile_id(layer_ob, j, i);
					TILED_MAP_TILE *tile_ob = tiled_get_tile_for_id(map, id);
					if (!tile_ob)
						continue;

					int tx = j*(tile_ob->tileset->tilewidth);
					int ty = i*(tile_ob->tileset->tileheight);

					int flags = 0;
					if (flipped_horizontally(layer_ob, j, i)) flags |= ALLEGRO_FLIP_HORIZONTAL;
					if (flipped_vertically(layer_ob, j, i)) flags |= ALLEGRO_FLIP_VERTICAL;

					al_draw_bitmap(tile_ob->bitmap, tx, ty, flags);
				}
			}

			layer_item = _al_list_next(map->layers, layer_item);
		}
	}
	else if (!strcmp(map->orientation, "isometric")) {
		fprintf(stderr, "Error: sorry, can't draw isometric maps right now. =(\n");
	}
	else {
		fprintf(stderr, "Error: unknown map orientation: %s\n", map->orientation);
	}

	al_set_target_bitmap(orig_backbuffer);
	
	return map;
}
