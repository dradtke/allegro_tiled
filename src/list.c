#include "list.h"

// Prepends data to the list
// Returns a new head of the list
struct node* prepend_to_list(struct node *head, void *data) {
	struct node *temp;
	temp = (struct node*)malloc(sizeof(struct node*));
	temp->data = data;
	temp->next = head;
	return temp;
}

void free_list(struct node *head) {
	while (head != NULL) {
		struct node *temp = head->next;
		//free(head->data);
		free(head);
		head = temp;
	}
}
