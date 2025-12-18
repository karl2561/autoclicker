/**
 * @file array_t.h
 * @brief A generic, array implementation.
 */
#ifndef ARRAY_T_H
#define ARRAY_T_H

#include <stddef.h>

/**
 * @struct array_t
 * @brief A generic array.
 */
typedef struct {
	void *data;  /**< Pointer to the saved data. */
	size_t size; /**< Size of one element. */
	size_t len;  /**< Length of the array. */
	size_t cap;  /**< Capacity of the array. */
} array_t;

/**
 * @brief initializes the array
 * @param size of the elements stored in the array
 * @return mallocated array, nullptr on failure. Caller must free.
 */
[[nodiscard]] array_t *array_init(size_t data_size);

/**
 * @brief frees the array after use.
 * @param array to free.
 * @return nullptr
 */
void *array_free(array_t *a);

/**
 * @brief increases size of array if necessary. length is not changed.
 * @param array which size to increase
 * @param new length of the array
 * @return -1 on failure, 1 on success, 0 if nothing was done
 */
int array_increase_size(array_t *a, size_t new_size);

/**
 * @brief appends element to array
 * @param array to which to append to
 * @param element to append to array
 * @return -1 on failure, 0 on success
 */
int array_push(array_t *a, void *new_element);

/**
 * @brief Iterates through the list and applies a function to each element.
 * @param Array to iterate over
 * @param Function to apply to each data element.
 */
void array_forEach(array_t *a, void (*func)(void *data));

#endif /* ARRAY_T_H */
