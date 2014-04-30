/*
 * This addon adds Tiled map support to the Allegro game library.
 * Copyright (c) 2012-2014 Damien Radtke - www.damienradtke.com
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *    1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 *    2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 *    3. This notice may not be removed or altered from any source
 *    distribution.
 *
 *                               ---
 *
 * Helper methods for parsing XML with libxml2.
 */

#include "xml.h"

/*
 * Given a parent XML node and the name of all desired children,
 * returns a list of all children with that name
 */
GSList *get_children_for_name(xmlNode *parent, char *name)
{
	GSList *list = NULL;

	if (!parent->children)
		return list;

	xmlNode *child = parent->children->next;
	while (child) {
		if (!strcmp((const char*)child->name, name)) {
			list = g_slist_prepend(list, child);
		}

		child = child->next;
	}

	return list;
}

/*
 * Alternate version that allows two possible names to be specified.
 * Tile/object layers have different node names ("layer" vs "objectgroup"),
 * but their order needs to be preserved. This method is a workaround
 * for that.
 */
GSList *get_children_for_either_name(xmlNode *parent, char *name1, char *name2)
{
	GSList *list = NULL;

	if (!parent->children)
		return list;

	xmlNode *child = parent->children->next;
	while (child) {
		if (!strcmp((const char*)child->name, name1) || !strcmp((const char*)child->name, name2)) {
			list = g_slist_prepend(list, child);
		}

		child = child->next;
	}

	return list;
}

/*
 * Given a parent XML node and the name of the desired child,
 * locates and returns the first child with that name
 */
xmlNode *get_first_child_for_name(xmlNode *parent, char *name)
{
	if (!parent->children)
		return NULL;

	xmlNode *child = parent->children->next;

	while  (child != NULL) {
		if (!strcmp((const char*)child->name, name)) {
			return child;
		} else {
			child = child->next;
		}
	}

	return NULL;
}

/*
 * Gets the value of an attribute from a node
 * The returned value must be copied if it will remain a string
 */
char *get_xml_attribute(xmlNode *node, char *name)
{
	xmlAttr *attrs = node->properties;

	while (attrs != NULL) {
		if (!strcmp((const char*)attrs->name, name))
			return (char *)attrs->children->content;

		attrs = attrs->next;
	}

	return NULL;
}
