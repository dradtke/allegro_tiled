#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "list.h"
#include "map.h"
#include "global.h"
#include "util.h"
#include "map-data-decompresser.h"

#ifndef MAP_PARSER_H
#define MAP_PARSER_H

list get_children_for_name(xmlNode *parent, char *name);
xmlNode *get_first_child_for_name(xmlNode *parent, char *name);
char *get_xml_attribute(xmlNode *node, char *name);
map_data *parse_map(const char *filename);

#endif
