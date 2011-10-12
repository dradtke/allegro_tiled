#include <stdlib.h>

#ifndef LIST_H
#define LIST_H

struct node {
	void *data;
	struct node *next;
};

struct node* prepend_to_list(struct node *node, void *data);

#endif
