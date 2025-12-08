/**
 * @file slist.h
 * @brief A generic, singly-linked list implementation.
 */
#ifndef SLIST_H
#define SLIST_H

/**
 * @struct slist
 * @brief A node in a singly-linked list.
 */
struct slist {
	struct slist *next; /**< Pointer to the next node in the list. */
	void *data;         /**< Pointer to the data held by this node. */
};

/**
 * @brief Returns the data from the head of the list without removing it.
 * @param head The head of the list.
 * @return A void pointer to the data, or `nullptr` if the list is empty.
 */
[[nodiscard]] void *slist_peek(struct slist *head);

/**
 * @brief Adds a new element to the beginning of the list.
 * @param head The current head of the list.
 * @param data The data to add.
 * @return The new head of the list. Returns `nullptr` on allocation failure.
 */
[[nodiscard]] struct slist *slist_push(struct slist *head, void *data);

/**
 * @brief Removes the first element from the list and frees its resources.
 * @param head The head of the list.
 * @param func A function pointer to be called to free the `data` of the removed node.
 * @return A pointer to the new head of the list.
 */
[[nodiscard]] struct slist *slist_pop(
	struct slist *head, void (*func)(void *data));

/**
 * @brief Appends a new element to the end of the list.
 * @param tail The current tail of the list. Can be `nullptr` if the list is empty.
 * @param data The data to add.
 * @return The new tail of the list. Returns `nullptr` on allocation failure.
 */
[[nodiscard]] struct slist *slist_append(struct slist *tail, void *data);

/**
 * @brief Deletes all elements in the list and frees their resources.
 * @param head_ptr A pointer to the head of the list. The head pointer will be set to `nullptr`.
 * @param func A function pointer to be called to free the `data` of each node.
 */
void slist_delete(struct slist **head_ptr, void (*func)(void *data));

/**
 * @brief Iterates through the list and applies a function to each element's data.
 * @param head_ptr A pointer to the head of the list.
 * @param func The function to apply to each data element.
 */
void slist_forEach(struct slist **head_ptr, void (*func)(void *data));

#endif
