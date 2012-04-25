#include <allegro5/allegro.h>
#include <allegro5/internal/aintern_vector.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "map.h"

#ifndef XML_H
#define XML_H

_AL_VECTOR *get_children_for_name(xmlNode *parent, char *name);
xmlNode *get_first_child_for_name(xmlNode *parent, char *name);
char *get_xml_attribute(xmlNode *node, char *name);

#endif
