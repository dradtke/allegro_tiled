#include "parser.h"

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
static int decode_layer_data(xmlNode *data_node, map_layer *layer)
{
	// TODO: get the encoding and compression
	char *str = trim((char *)data_node->children->content);
	char *data = NULL;
	int flen = 0;
	FILE *tmp = tmpfile();

	decompress(str, tmp);
	fflush(tmp);

	// get file length
	flen = ftell(tmp);

	// read in the file
	rewind(tmp);
	data = (char *)calloc(flen, sizeof(char));
	if (fread(data, sizeof(char), flen, tmp) < flen) {
		fprintf(stderr, "failed to read in map data\n");
		return 1;
	}

	// every tile id takes 4 bytes
	layer->datalen = flen/4;
	layer->data = (char *)calloc(layer->datalen, sizeof(char));
	int x, y, i;
	i = 0;
	for (y = 0; y<layer->height; y++) {
		for (x = 0; x<layer->width; x++) {
			int tileid = 0;
			tileid |= data[i];
			tileid |= data[i+1] << 8;
			tileid |= data[i+2] << 16;
			tileid |= data[i+3] << 24;
			layer->data[i/4] = tileid;
			i += 4;
		}
	}
	//	printf("layer dimensions: %dx%d, data length = %d\n", 
	//			layer->width, layer->height, flen);

	fclose(tmp);
	return 0;
}

/*
 * After all the tiles have been parsed out of their tilesets,
 * create the map's global list of tiles.
 */
static void cache_tile_list(map_data *map)
{
	map->tiles = create_list(0);
	_AL_LIST_ITEM *tileset_item = _al_list_front(map->tilesets);

	while (tileset_item != NULL) {
		map_tileset *tileset_ob = _al_list_item_data(tileset_item);
		_AL_LIST_ITEM *tile_item = _al_list_front(tileset_ob->tiles);

		while (tile_item != NULL) {
			map_tile *tile_ob = _al_list_item_data(tile_item);
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
map_data *parse_map(const char *dir, const char *filename)
{
	// first move into the directory
	ALLEGRO_PATH *cwd = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
	al_join_paths(cwd, al_create_path(dir));
	if (!al_change_directory(al_path_cstr(cwd, ALLEGRO_NATIVE_PATH_SEP))) {
		printf("Failed to change directory.");
	}
	
	xmlDoc *doc;
	xmlNode *root;
	map_data *map;

	unsigned i, j, k;

	// Read in the data file
	debug("parsing map [%s]", filename);
	doc = xmlReadFile(filename, NULL, 0);
	if (!doc) {
		fprintf(stderr, "failed to parse map data: %s\n", filename);
		return NULL;
	}

	// Get the root element, <map>
	debug("Getting root element");
	root = xmlDocGetRootElement(doc);

	// Get some basic info
	debug("Fetching map attributes");
	map = (map_data*)malloc(sizeof(map_data));
	map->width = atoi(get_xml_attribute(root, "width"));
	map->height = atoi(get_xml_attribute(root, "height"));
	map->tile_width = atoi(get_xml_attribute(root, "tilewidth"));
	map->tile_height = atoi(get_xml_attribute(root, "tileheight"));
	map->orientation = copy(get_xml_attribute(root, "orientation"));

	// Get the tilesets
	_AL_VECTOR *tilesets = get_children_for_name(root, "tileset");
	map->tilesets = create_list(_al_vector_size(tilesets));
	for (i = 0; i<_al_vector_size(tilesets); i++) {
		xmlNode **tileset_node = _al_vector_ref(tilesets, i);
		map_tileset *tileset_ob = (map_tileset*)malloc(sizeof(map_tileset));
		tileset_ob->firstgid = atoi(get_xml_attribute(*tileset_node, "firstgid"));
		tileset_ob->tilewidth = atoi(get_xml_attribute(*tileset_node, "tilewidth"));
		tileset_ob->tileheight = atoi(get_xml_attribute(*tileset_node, "tileheight"));
		tileset_ob->name = copy(get_xml_attribute(*tileset_node, "name"));

		// Get this tileset's image
		xmlNode *image_node = get_first_child_for_name(*tileset_node, "image");
		map_image *image_ob = (map_image*)malloc(sizeof(map_image));
		image_ob->width = atoi(get_xml_attribute(image_node, "width"));
		image_ob->height = atoi(get_xml_attribute(image_node, "height"));
		image_ob->source = copy(get_xml_attribute(image_node, "source"));
		image_ob->bitmap = al_load_bitmap(image_ob->source);

		tileset_ob->image = image_ob;

		// Get this tileset's tiles
		_AL_VECTOR *tiles = get_children_for_name(*tileset_node, "tile");
		tileset_ob->tiles = create_list(_al_vector_size(tiles));
		for (j = 0; j<_al_vector_size(tiles); j++) {
			xmlNode **tile_node = _al_vector_ref(tiles, j);
			map_tile *tile_ob = (map_tile*)malloc(sizeof(map_tile));
			tile_ob->id = tileset_ob->firstgid + atoi(get_xml_attribute(*tile_node, "id"));
			tile_ob->tileset = tileset_ob;
			tile_ob->bitmap = NULL;

			// Get this tile's properties
			_AL_VECTOR *properties = get_children_for_name(
					get_first_child_for_name(*tile_node, "properties"),
					"property");
			tile_ob->properties = create_list(_al_vector_size(properties));
			for (k = 0; k<_al_vector_size(properties); k++) {
				xmlNode **prop_node = _al_vector_ref(properties, k);
				map_property *prop_ob = (map_property*)malloc(sizeof(map_property));
				prop_ob->name = copy(get_xml_attribute(*prop_node, "name"));
				prop_ob->value = copy(get_xml_attribute(*prop_node, "value"));
				_al_list_push_back(tile_ob->properties, prop_ob);
			}

			_al_list_push_back(tileset_ob->tiles, tile_ob);
		}

		_al_vector_free(tiles);
		_al_list_push_back(map->tilesets, tileset_ob);
	}

	_al_vector_free(tilesets);

	// Create the map's master list of tiles
	cache_tile_list(map);

	// Get the layers
	_AL_VECTOR *layers = get_children_for_name(root, "layer");
	map->layers = create_list(_al_vector_size(layers));
	for (i = 0; i<_al_vector_size(layers); i++) {
		xmlNode **layer_node = _al_vector_ref(layers, i);
		map_layer *layer_ob = (map_layer*)malloc(sizeof(map_layer));
		layer_ob->name = copy(get_xml_attribute(*layer_node, "name"));
		layer_ob->width = atoi(get_xml_attribute(*layer_node, "width"));
		layer_ob->height = atoi(get_xml_attribute(*layer_node, "height"));
		layer_ob->map = map;
		decode_layer_data(get_first_child_for_name(*layer_node, "data"), layer_ob);

		// Create any missing tile objects
		for (j = 0; j<layer_ob->height; j++) {
			for (k = 0; k<layer_ob->width; k++) {
				char id = get_tile_id(layer_ob, k, j);

				if (id == 0)
					continue;

				map_tile *tile_ob = get_tile_for_id(map, id);
				if (!tile_ob) {
					// wasn't defined in the map file, presumably
					// because it had no properties
					tile_ob = (map_tile*)malloc(sizeof(map_tile));
					tile_ob->id = id;
					tile_ob->properties = _al_list_create();
					tile_ob->tileset = NULL;
					tile_ob->bitmap = NULL;

					// locate its tilemap
					_AL_LIST_ITEM *tileset_item = _al_list_front(map->tilesets);
					while (tileset_item != NULL) {
						map_tileset *tileset_ob = _al_list_item_data(tileset_item);
						if (tileset_ob->firstgid <= id) {
							if (!tile_ob->tileset || tileset_ob->firstgid > tile_ob->tileset->firstgid) {
								tile_ob->tileset = tileset_ob;
							}
						}
						tileset_item = _al_list_next(map->tilesets, tileset_item);
					}

					_al_list_push_back(map->tiles, tile_ob);
				}

				// create this tile's bitmap if it hasn't been yet
				if (!tile_ob->bitmap) {
					map_tileset *tileset = tile_ob->tileset;
					int id = tile_ob->id - tileset->firstgid;
					int width = tileset->image->width / tileset->tilewidth;
					int x = (id % width) * tileset->tilewidth;
					int y = (id / width) * tileset->tileheight;
					tile_ob->bitmap = al_create_sub_bitmap(
							tileset->image->bitmap,
							x, y,
							tileset->tilewidth,
							tileset->tileheight);
				}
			}
		}

		_al_list_push_back(map->layers, layer_ob);
	}

	_al_vector_free(layers);
	
	// Free the doc and return
	xmlFreeDoc(doc);
	return map;
}
