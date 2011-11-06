#include "list.h"

/*
 * Prepends an item to the list
 * Returns a new head for the list
 */
list prepend_to_list(list head, void *data) {
	list temp = (list)malloc(sizeof(struct node));
	temp->data = data;
	temp->next = head;
	return temp;
}

/*
 * Frees a list
 */
void free_list(list head) {
	list temp;
	while (head != NULL) {
		temp = head->next;
		// TODO: get this to work, because I think it might be needed
		//free(head->data);
		free(head);
		head = temp;
	}
}
