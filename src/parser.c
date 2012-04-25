#include "parser.h"

/*
 * Decodes map data from a <data> node
 * Saves the decoded and decompressed data to dest
 */
int decode_layer_data(xmlNode *data_node, map_layer *layer)
{
	// TODO: get the encoding and compression
	char *str = trim(data_node->children->content);
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
		return -1;
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
 * Parses a map file
 * Given the path to a map file, returns a new map struct
 * The struct must be freed once it's done being used
 */
map_data *parse_map(const char *filename)
{
	// TODO: update cwd and move into the map file's directory
	// this makes it easier to locate image references
	
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
	map->tilesets = _al_list_create_static(tilesets->_size);
	for (i = 0; i<tilesets->_size; i++) {
		xmlNode **tileset_node = _al_vector_ref(tilesets, i);
		map_tileset *tileset_ob = (map_tileset*)malloc(sizeof(map_tileset));
		tileset_ob->firstgid = atoi(get_xml_attribute(*tileset_node, "firstgid"));
		tileset_ob->tilewidth = atoi(get_xml_attribute(*tileset_node, "tilewidth"));
		tileset_ob->tileheight = atoi(get_xml_attribute(*tileset_node, "tileheight"));
		tileset_ob->name = copy(get_xml_attribute(*tileset_node, "name"));

		// Get this tileset's images
		_AL_VECTOR *images = get_children_for_name(*tileset_node, "image");
		tileset_ob->images = _al_list_create_static(images->_size);
		for (j = 0; j<images->_size; j++) {
			xmlNode **image_node = _al_vector_ref(images, j);
			map_image *image_ob = (map_image*)malloc(sizeof(map_image));
			image_ob->width = atoi(get_xml_attribute(*image_node, "width"));
			image_ob->height = atoi(get_xml_attribute(*image_node, "height"));
			image_ob->source = copy(get_xml_attribute(*image_node, "source"));
			image_ob->bitmap = al_load_bitmap(image_ob->source);
			_al_list_push_back(tileset_ob->images, image_ob);
		}

		_al_vector_free(images);

		// Get this tileset's tiles
		_AL_VECTOR *tiles = get_children_for_name(*tileset_node, "tile");
		tileset_ob->tiles = _al_list_create_static(tiles->_size);
		for (j = 0; j<tiles->_size; j++) {
			xmlNode **tile_node = _al_vector_ref(tiles, j);
			map_tile *tile_ob = (map_tile*)malloc(sizeof(map_tile));
			tile_ob->id = atoi(get_xml_attribute(*tile_node, "id"));

			// Get this tile's properties
			_AL_VECTOR *properties = get_children_for_name(
					get_first_child_for_name(*tile_node, "properties"),
					"property");
			tile_ob->properties = _al_list_create_static(properties->_size);
			for (k = 0; k<properties->_size; k++) {
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
	
	// TODO: free all created _AL_VECTOR objects
	// Free the doc and return
	xmlFreeDoc(doc);
	return map;
}
