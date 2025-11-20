#include "src/utils/create_pattern.h"
#include "src/utils/slist.h"

#include <stdio.h>
#include <stdlib.h>

int main()
{
	struct slist *head = create_pattern();
	if (!head)
		exit(1);
	struct slist *body = head;
	struct Movement *element;
	unsigned count = 0;
	while (body) {
		element = body->data;
		printf("x: %5d, y: %5d\n", element->x, element->y);
		count++;
		body = body->next;
	}
	printf("Count: %d\n", count);
	slist_delete(&head, remove_item);

	return 0;
}
