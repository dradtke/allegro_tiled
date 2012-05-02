#include <allegro5/tiled.h>
#include "internal.h"

/*
 * Given a parent XML node and the name of all desired children,
 * returns a list of all children with that name
 */
_AL_LIST *get_children_for_name(xmlNode *parent, char *name)
{
	_AL_LIST *list = _al_list_create();
	xmlNode *child = parent->children->next;

	while (child != NULL) {
		if (!strcmp((const char*)child->name, name))
			_al_list_push_back(list, child);

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
	xmlNode *child = parent->children->next;

	while  (child != NULL) {
		if (!strcmp((const char*)child->name, name))
			return child;

		child = child->next;
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
