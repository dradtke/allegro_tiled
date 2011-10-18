#include "list.h"

/*
 * Prepends an item to the list
 * Returns a new head for the list
 */
list prepend_to_list(list head, void *data) {
	list temp;
	temp = (list)malloc(sizeof(list));
	temp->data = data;
	temp->next = head;
	return temp;
}

/*
 * Frees a list
 */
void free_list(list head) {
	while (head != NULL) {
		list temp = head->next;
		// TODO: get this to work, because I think it might be needed
		//free(head->data);
		free(head);
		head = temp;
	}
}
