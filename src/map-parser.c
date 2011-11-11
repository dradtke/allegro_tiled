#include "map-parser.h"

/*
 * Given a parent XML node and the name of all desired children,
 * returns a list of all children with that name
 */
int get_children_for_name(xmlNode *parent, char *name, list *dest){
	int count = 0;
	(*dest) = NULL;
	xmlNode *child = parent->children->next;

	while (child != NULL) {
		if (!strcmp(child->name, name)) {
			//dest = prepend_to_list(&children, child);
			list temp;
			temp = (list)malloc(sizeof(struct node));
			temp->data = child;
			temp->next = (*dest);
			(*dest) = temp;
			count++;
		}

		child = child->next;
	}

	return count;
}

/*
 * Given a parent XML node and the name of the desired child,
 * locates and returns the first child with that name
 */
xmlNode *get_first_child_for_name(xmlNode *parent, char *name) {
	xmlNode *child = parent->children->next;

	while  (child != NULL) {
		if (!strcmp(child->name, name))
			return child;

		child = child->next;
	}

	return NULL;
}

/*
 * Gets the value of an attribute from a node
 * The returned value must be copied if it will remain a string
 */
char *get_xml_attribute(xmlNode *node, char *name) {
	xmlAttr *attrs = node->properties;

	while (attrs != NULL) {
		if (!strcmp(attrs->name, name))
			return attrs->children->content;

		attrs = attrs->next;
	}
	
	return NULL;
}

/*
 * Decodes map data from a <data> node
 * Saves the decoded and decompressed data to dest
 */
int decode_layer_data(xmlNode *data_node, map_layer *layer) {
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
map_data *parse_map(const char *filename) {
	xmlDoc *doc;
	xmlNode *root;
	map_data *map;

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

	// get all the tilesets
	list tileset_list;
	int tileset_count = get_children_for_name(root, "tileset", &tileset_list);
	debug("Found %d tilesets", tileset_count);
	map_tileset_list *tmp_tilesets = (map_tileset_list*)malloc(sizeof(map_tileset_list));
	tmp_tilesets->length = tileset_count;
	tmp_tilesets->list = (map_tileset*)malloc(sizeof(map_tileset[tmp_tilesets->length]));

	int i = tileset_count - 1;
	while (tileset_list != NULL) {
		xmlNode *node = (xmlNode*)tileset_list->data;
		tmp_tilesets->list[i].firstgid = atoi(get_xml_attribute(node, "firstgid"));
		tmp_tilesets->list[i].name = copy(get_xml_attribute(node, "name"));
		tmp_tilesets->list[i].tile_width = atoi(get_xml_attribute(node, "tilewidth"));
		tmp_tilesets->list[i].tile_height = atoi(get_xml_attribute(node, "tileheight"));
		//tmp_tilesets->list[i].images = NULL;
		tmp_tilesets->list[i].tiles = NULL;
		debug("Tileset [name = '%s', firstgid = '%d']",
				tmp_tilesets->list[i].name, tmp_tilesets->list[i].firstgid);

		// for each tileset, get its images
		list image_list;
		int image_count = get_children_for_name(node, "image", &image_list);
		debug("Found %d images", image_count);
		map_image_list *tmp_images = (map_image_list*)malloc(sizeof(map_image_list));
		tmp_images->length = image_count;
		tmp_images->list = (map_image*)malloc(sizeof(map_image[tmp_images->length]));

		int j = image_count - 1;
		while (image_list != NULL) {
			xmlNode *_node = (xmlNode*)image_list->data;
			tmp_images->list[j].source = copy(get_xml_attribute(_node, "source"));
			tmp_images->list[j].width = atoi(get_xml_attribute(_node, "width"));
			tmp_images->list[j].height = atoi(get_xml_attribute(_node, "height"));
			debug("Image [source = '%s', width = '%d', height = '%d']",
					tmp_images->list[j].source, tmp_images->list[j].width, tmp_images->list[j].height);
			image_list = image_list->next;
			j--;
		}

		tmp_tilesets->list[i].images = tmp_images;

		// get the tiles
		list tile_list;
		int tile_count = get_children_for_name(node, "tile", &tile_list);
		debug("Found %d tiles", tile_count);
		map_tile_list *tmp_tiles = (map_tile_list*)malloc(sizeof(map_tile_list));
		tmp_tiles->length = tile_count;
		tmp_tiles->list = (map_tile*)malloc(sizeof(map_tile[tmp_tiles->length]));

		j = tile_count - 1;
		while (tile_list != NULL) {
			xmlNode *_node = (xmlNode*)tile_list->data;
			tmp_tiles->list[j].id = atoi(get_xml_attribute(_node, "id"));
			debug("Tile [id = '%d']", tmp_tiles->list[j].id);

			list prop_list;
			int prop_count = get_children_for_name(get_first_child_for_name(_node, "properties"),
					"property", &prop_list);
			debug("Found %d properties", prop_count);
			map_prop_list *tmp_props = (map_prop_list*)malloc(sizeof(map_prop_list));
			tmp_props->length = prop_count;
			tmp_props->list = (map_property*)malloc(sizeof(map_property[tmp_props->length]));

			int k = prop_count - 1;
			while (prop_list != NULL) {
				xmlNode *__node = (xmlNode*)prop_list->data;
				tmp_props->list[k].name = copy(get_xml_attribute(__node, "name"));
				tmp_props->list[k].value = copy(get_xml_attribute(__node, "value"));
				debug("Property [name = '%s', value = '%s']",
						tmp_props->list[k].name, tmp_props->list[k].value);
				prop_list = prop_list->next;
				k--;
			}

			tmp_tiles->list[j].properties = tmp_props;

			tile_list = tile_list->next;
			j--;
		}

		tmp_tilesets->list[i].tiles = tmp_tiles;

		// continue looping
		tileset_list = tileset_list->next;
		i--;
	}
	// save the list in map
	map->tilesets = tmp_tilesets;

	// Free the doc and return
	xmlFreeDoc(doc);
	return map;
}
