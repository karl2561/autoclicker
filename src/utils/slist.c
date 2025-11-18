#include "slist.h"

#include <stdlib.h>

[[nodiscard]] void *slist_peek(struct slist *head)
{
	return head != nullptr ? head->data : nullptr;
}

[[nodiscard]] struct slist *slist_push(struct slist *head, void *data)
{
	struct slist *const new_head = malloc(sizeof(struct slist));
	if (!new_head)
		return nullptr;

	new_head->next = head;
	new_head->data = data;

	return new_head;
}

[[nodiscard]] struct slist *slist_pop(
	struct slist *head, void (*func)(void *data))
{
	if (!head)
		return nullptr;
	struct slist *next = head->next;

	func(head->data);

	free(head->data);
	free(head);

	return next;
}

[[nodiscard]] struct slist *slist_append(struct slist *tail, void *data)
{
	struct slist *const new_node = malloc(sizeof(struct slist));
	if (!new_node)
		return nullptr;

	new_node->data = data;
	new_node->next = nullptr;

	if (tail)
		tail->next = new_node;

	return new_node;
}

void slist_delete(struct slist **head_ptr, void (*func)(void *data))
{
	if (!head_ptr || !*head_ptr)
		return;

	struct slist *head = *head_ptr;
	while ((head = slist_pop(head, func)))
		;
	*head_ptr = nullptr;
}
