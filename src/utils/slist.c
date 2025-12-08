/**
 * @file slist.c
 * @brief Implementation of a generic, singly-linked list.
 */
#include "slist.h"

#include <stdlib.h>

/**
 * @brief Returns the data from the head of the list without removing it.
 * @param head The head of the list.
 * @return A void pointer to the data, or `nullptr` if the list is empty.
 */
void *slist_peek(struct slist *head)
{
	return head != nullptr ? head->data : nullptr;
}

/**
 * @brief Adds a new element to the beginning of the list.
 * @param head The current head of the list.
 * @param data The data to add.
 * @return The new head of the list. Returns `nullptr` on allocation failure.
 */
struct slist *slist_push(struct slist *head, void *data)
{
	struct slist *const new_head = malloc(sizeof(struct slist));
	if (!new_head)
		return nullptr;

	new_head->next = head;
	new_head->data = data;

	return new_head;
}

/**
 * @brief Removes the first element from the list and frees its resources.
 * @param head The head of the list.
 * @param func A function pointer to be called to handle the `data` of the removed node (e.g., freeing it).
 * The node's `data` pointer itself is also freed.
 * @return A pointer to the new head of the list.
 */
struct slist *slist_pop(struct slist *head, void (*func)(void *data))
{
	if (!head)
		return nullptr;
	struct slist *next = head->next;

	func(head->data);

	free(head->data);
	free(head);

	return next;
}

/**
 * @brief Appends a new element to the end of the list.
 * @param tail The current tail of the list. Can be `nullptr` if the list is empty.
 * @param data The data to add.
 * @return The new tail of the list. Returns `nullptr` on allocation failure.
 */
struct slist *slist_append(struct slist *tail, void *data)
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

/**
 * @brief Deletes all elements in the list and frees their resources.
 * @param head_ptr A pointer to the head of the list. The head pointer will be set to `nullptr`.
 * @param func A function pointer to be called to handle the `data` of each node.
 */
void slist_delete(struct slist **head_ptr, void (*func)(void *data))
{
	if (!head_ptr || !*head_ptr)
		return;

	struct slist *head = *head_ptr;
	while ((head = slist_pop(head, func)))
		;
	*head_ptr = nullptr;
}

/**
 * @brief Iterates through the list and applies a function to each element's data.
 * @param head_ptr A pointer to the head of the list.
 * @param func The function to apply to each data element.
 */
void slist_forEach(struct slist **head_ptr, void (*func)(void *data))
{
	if (!head_ptr || !*head_ptr)
		return;
	struct slist *head = *head_ptr;
	while (head) {
		func(head->data);
		head = head->next;
	}
}
