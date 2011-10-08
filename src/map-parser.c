#include "list.h"
#include "map-parser.h"

// map_data
typedef struct {
	// ???: orientation?
	int width;
	int height;
	int tile_width;
	int tile_height;
} map_data;

// tileset
typedef struct {
	int firstgid;
	char *name;
	int tile_width;
	int tile_height;
} tileset;

tileset *tileset_list;

node *get_nodes_for_name(xmlNode *parent, char *name) {
	node *head = NULL;
	xmlNode *child = parent->children->next;

	while (child != NULL) {
		if (!strcmp(child->name, name)) {
			// insert child into the list
			node *temp;
			temp = (node*)malloc(sizeof(node));
			temp->data = child;
			temp->next = head;
			head = temp;
		}

		child = child->next;
	}

	return head;
}

int parse_map(const char *filename) {
	xmlDoc *doc;
	xmlNode *root;

	// read in the data file
	doc = xmlReadFile(filename, NULL, 0);
	if (!doc) {
		fprintf(stderr, "Failed to parse map data: %s\n", filename);
		return 1;
	}

	// <map>
	root = xmlDocGetRootElement(doc);

	node *tileset = get_nodes_for_name(root, "tileset");
	while (tileset != NULL) {
		xmlNode *xnode = (xmlNode*)tileset->data;
		printf("found: %s\n", xnode->name);

		tileset = tileset->next;
	}

	// Free the doc
	xmlFreeDoc(doc);
	return 0;
}
