#include "xml.h"

/*
 * Given a parent XML node and the name of all desired children,
 * returns a list of all children with that name
 */
_AL_VECTOR *get_children_for_name(xmlNode *parent, char *name)
{
	_AL_VECTOR *vec = (_AL_VECTOR*)malloc(sizeof(_AL_VECTOR));
	xmlNode *child = parent->children->next;

	_al_vector_init(vec, sizeof(xmlNode*));

	while (child != NULL) {
		if (!strcmp((const char*)child->name, name))
			_al_vector_append_array(vec, 1, &child);

		child = child->next;
	}

	return vec;
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
