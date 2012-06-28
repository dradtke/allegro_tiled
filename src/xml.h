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

#ifndef _XML_H
#define _XML_H

#include <allegro5/allegro.h>
#include <allegro5/tiled.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

_AL_LIST *get_children_for_name(xmlNode *parent, char *name);
xmlNode *get_first_child_for_name(xmlNode *parent, char *name);
char *get_xml_attribute(xmlNode *node, char *name);

#endif
