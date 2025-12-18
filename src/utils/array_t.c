/**
 * @file array_t.c
 * @brief Implementation of a generic array type.
 */
#include "array_t.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief initializes the array
 * @param size of the elements stored in the array
 * @return mallocated array, nullptr on failure. Caller must free.
 */
array_t *array_init(size_t data_size)
{
	array_t *a = malloc(sizeof(array_t));
	if (!a)
		return nullptr;

	a->len = 0;
	a->cap = 4;
	a->size = data_size;
	a->data = malloc(a->size * a->cap);
	if (!a->data) {
		free(a);
		return nullptr;
	}
	return a;
}

/**
 * @brief frees the array after use.
 * @param array to free.
 * @return nullptr
 */
void *array_free(array_t *a)
{
	if (!a)
		return nullptr;
	free(a->data);
	free(a);
	return nullptr;
}

/**
 * @brief increases size of array if necessary. length is not changed.
 * @param array which size to increase
 * @param new length of the array
 * @return -1 on failure, 1 on success, 0 if nothing was done
 */
int array_increase_size(array_t *a, size_t new_size)
{
	if (new_size < a->cap)
		return 0;

	size_t cap = a->cap;
	while (cap <= new_size)
		cap <<= 1;

	void *tmp = realloc(a->data, a->size * cap);
	if (!tmp) {
		perror("Failed to realloc size");
		return -1;
	}

	a->cap = cap;
	a->data = tmp;
	return 1;
}

/**
 * @brief appends element to array
 * @param array to which to append to
 * @param element to append to array
 * @return -1 on failure, 0 on success
 */
int array_push(array_t *a, void *new_element)
{
	if (!a || !new_element)
		return -1;

	if (a->len >= a->cap && array_increase_size(a, a->len) < 0) {
		return -1;
	}

	void *target = (char *)a->data + a->len * a->size;
	memcpy(target, new_element, a->size);
	a->len++;

	return 0;
}

/**
 * @brief Iterates through the list and applies a function to each element.
 * @param Array to iterate over
 * @param Function to apply to each data element.
 */
void array_forEach(array_t *a, void (*func)(void *data))
{
	if (!a)
		return;

	for (size_t i = 0; i < a->len; i++)
		func(&a->data[i]);
}
