#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "list.h"
#include "map.h"
#include "global.h"
#include "util.h"
#include "zlib.h"

#ifndef MAP_PARSER_H
#define MAP_PARSER_H

int get_children_for_name(xmlNode *parent, char *name, list *dest);
xmlNode *get_first_child_for_name(xmlNode *parent, char *name);
char *get_xml_attribute(xmlNode *node, char *name);
map_data *parse_map(const char *filename);

#endif
