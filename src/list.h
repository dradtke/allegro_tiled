#include <stdlib.h>

#ifndef LIST_H
#define LIST_H

typedef struct node *list;

struct node {
	void *data;
	list next;
};

list prepend_to_list(list node, void *data);

#endif
