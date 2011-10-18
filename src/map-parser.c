#include "map-parser.h"

/*
 * Given a parent XML node and the name of all desired children,
 * returns a list of all children with that name
 */
list get_children_for_name(xmlNode *parent, char *name) {
	list head = NULL;
	xmlNode *child = parent->children->next;

	while (child != NULL) {
		if (!strcmp(child->name, name)) {
			//head = prepend_to_list(&head, child);
			list temp;
			temp = (list)malloc(sizeof(list));
			temp->data = child;
			temp->next = head;
			head = temp;
		}

		child = child->next;
	}

	return head;
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
 */
char *decode_layer_data(xmlNode *data_node) {
	// TODO: get the encoding and compression
	char *data = data_node->children->content;
	int ret = decompress(trim(data), "base64");
	return NULL;
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
	if (DEBUG) printf("Parsing map [%s]\n", filename);
	doc = xmlReadFile(filename, NULL, 0);
	if (!doc) {
		fprintf(stderr, "Failed to parse map data: %s\n", filename);
		return NULL;
	}

	// Get the root element, <map>
	if (DEBUG) printf("Getting root element\n");
	root = xmlDocGetRootElement(doc);

	// Get some basic info
	if (DEBUG) printf("Fetching map attributes\n");
	map = (map_data*)malloc(sizeof(map_data));
	map->width = atoi(get_xml_attribute(root, "width"));
	map->height = atoi(get_xml_attribute(root, "height"));
	map->tile_width = atoi(get_xml_attribute(root, "tilewidth"));
	map->tile_height = atoi(get_xml_attribute(root, "tileheight"));
	map->orientation = copy(get_xml_attribute(root, "orientation"));

	// Construct the list of tilesets
	list tilesets = NULL;

	list tileset_nodes = get_children_for_name(root, "tileset");
	while (tileset_nodes != NULL) {
		map_tileset *set;
		xmlNode *node = (xmlNode*)tileset_nodes->data;
		if (DEBUG) printf("Found tileset\n");

		// fill in required values
		set = (map_tileset*)malloc(sizeof(map_tileset));
		set->firstgid = atoi(get_xml_attribute(node, "firstgid"));
		set->name = copy(get_xml_attribute(node, "name"));
		set->tile_width = atoi(get_xml_attribute(node, "tilewidth"));
		set->tile_height = atoi(get_xml_attribute(node, "tileheight"));

		list images = NULL;
		list image_nodes = get_children_for_name(node, "image");
		while (image_nodes != NULL) {
			xmlNode *img_node = (xmlNode*)image_nodes->data;
			map_tileset_img *img;
			if (DEBUG) printf("Found image\n");

			img = (map_tileset_img*)malloc(sizeof(map_tileset_img));
			img->source = copy(get_xml_attribute(img_node, "source"));
			img->width = atoi(get_xml_attribute(img_node, "width"));
			img->height = atoi(get_xml_attribute(img_node, "height"));

			images = prepend_to_list(images, img);
			image_nodes = image_nodes->next;
		}
		free_list(image_nodes);

		list tiles = NULL;
		list tile_nodes = get_children_for_name(node, "tile");
		while (tile_nodes != NULL) {
			xmlNode *tile_node = (xmlNode*)tile_nodes->data;
			map_tile *t;
			list property_nodes = get_children_for_name(
					get_first_child_for_name(tile_node, "properties"), "property");
			list properties = NULL;
			if (DEBUG) printf("Found tile\n");

			while (property_nodes != NULL) {
				xmlNode *prop_node = (xmlNode*)property_nodes->data;
				property *prop;

				prop = (property*)malloc(sizeof(property));
				prop->name = copy(get_xml_attribute(prop_node, "name"));
				prop->value = copy(get_xml_attribute(prop_node, "value"));
				properties = prepend_to_list(properties, prop);

				property_nodes = property_nodes->next;
			}

			t = (map_tile*)malloc(sizeof(map_tile));
			t->id = atoi(get_xml_attribute(tile_node, "id"));
			t->properties = properties;

			tiles = prepend_to_list(tiles, t);
			tile_nodes = tile_nodes->next;
		}
		free_list(tile_nodes);

		set->images = images;
		set->tiles = tiles;

		// add it to the list and continue on
		tilesets = prepend_to_list(tilesets, set);
		tileset_nodes = tileset_nodes->next;
	}
	free_list(tileset_nodes);

	list layer_nodes = get_children_for_name(root, "layer");
	list layers = NULL;
	while (layer_nodes != NULL) {
		xmlNode *layer_node = (xmlNode*)layer_nodes->data;
		map_layer *layer;
		if (DEBUG) printf("Found layer\n");

		layer = (map_layer*)malloc(sizeof(map_layer));
		layer->name = copy(get_xml_attribute(layer_node, "name"));
		layer->width = atoi(get_xml_attribute(layer_node, "width"));;
		layer->height = atoi(get_xml_attribute(layer_node, "height"));
		layer->data = decode_layer_data(get_first_child_for_name(layer_node, "data"));

		layers = prepend_to_list(layers, layer);
		layer_nodes = layer_nodes->next;
	}
	free_list(layer_nodes);

	// Fill in the map data
	map->tilesets = tilesets;
	map->layers = layers;

	// Free the doc and return
	xmlFreeDoc(doc);
	return map;
}
