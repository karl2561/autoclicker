#ifndef SLIST_H
#define SLIST_H

struct slist {
	struct slist *next;
	void *data;
};

/* Looks at the first element */
[[nodiscard]] void *slist_peek(struct slist *head);

/* Takes the head of an slist, prepends data at the start, returns new head */
[[nodiscard]] struct slist *slist_push(struct slist *head, void *data);

/* Deletes the last element of the slist, returns a pointer to the new head */
[[nodiscard]] struct slist *slist_pop(
	struct slist *head, void (*func)(void *data));

/* Takes the tail of an slist and appends data at the tail, returns new tail */
[[nodiscard]] struct slist *slist_append(struct slist *tail, void *data);

/* Deletes the slist completely and all its data */
void slist_delete(struct slist **head_ptr, void (*func)(void *data));

/* Takes a head pointer,  iterates through the list, using func on each data */
void slist_forEach(struct slist **head_ptr, void (*func)(void *data));

#endif
