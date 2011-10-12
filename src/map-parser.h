#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "list.h"
#include "map.h"
#include "global.h"

#ifndef MAP_PARSER_H
#define MAP_PARSER_H

struct node *get_nodes_for_name(xmlNode *parent, char *name);
xmlNode *get_first_node_for_name(xmlNode *parent, char *name);
char *get_xml_attribute(xmlNode *node, char *name);
map_data *parse_map(const char *filename);

#endif
