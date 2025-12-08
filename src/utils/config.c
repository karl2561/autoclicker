/**
 * @file config.c
 * @brief Implementation for managing autoclicker configuration.
 */
#include "config.h"

#include <linux/input-event-codes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** @brief Global configuration instance, initialized with default values. */
struct config cfg = {
#define X(name, val, desc) .name = val,
	CFG_KEYS CFG_BTNS CFG_INTS
#undef X
};

/** @brief A map of configuration settings, used for generic access and manipulation. */
struct cfg_map_t cfg_map[] = {
#define X(name, val, desc) {#name, desc, &cfg.name, CFG_KEY},
	CFG_KEYS
#undef X

#define X(name, val, desc) {#name, desc, &cfg.name, CFG_BTN},
		CFG_BTNS
#undef X

#define X(name, val, desc) {#name, desc, &cfg.name, CFG_INT_A},
			CFG_INTS
#undef X
};

/** @brief The number of elements in the `cfg_map` array. */
size_t cfg_map_length = sizeof(cfg_map) / sizeof(cfg_map[0]);

/**
 * @brief Converts a configuration field at a given position in `cfg_map` to an integer.
 * @param position The index in the `cfg_map` array.
 * @return The integer value of the field. For atomic integers, it performs an atomic load.
 */
int convert_field_to_int(int position)
{
	switch (cfg_map[position].type) {
	case CFG_KEY:
	case CFG_BTN:
		return *(int *)cfg_map[position].field;
	case CFG_INT_A:
		return atomic_load((atomic_int *)cfg_map[position].field);
	}
}

/**
 * @brief Strips the prefix from a string (up to the first '_').
 * @param s The input string.
 * @return A new dynamically allocated string with the prefix removed. The caller must free it.
 */
static char *strip_prefix(const char *s)
{
	char *ptr = strchr(s, '_');
	if (!ptr)
		return strdup(s);
	return strdup(ptr + 1);
}

/**
 * @brief Strips the suffix from a string (from the last '_').
 * @param s The input string.
 * @return A new dynamically allocated string with the suffix removed. The caller must free it.
 */
static char *strip_suffix(const char *s)
{
	char *ptr = strrchr(s, '_');
	if (!ptr)
		return strdup(s);
	size_t len = ptr - s;
	char *out = malloc(len + 1);
	if (!out)
		return nullptr;

	memcpy(out, s, len);
	out[len] = '\0';

	return out;
}

/**
 * @brief Replaces all occurrences of a character in a string.
 * @param s The string to modify.
 * @param old The character to replace.
 * @param new The character to replace with.
 */
static void str_replace_char(char *s, char old, char new)
{
	for (; *s; s++)
		if (*s == old)
			*s = new;
}

/**
 * @brief Generates a clean, human-readable name for a configuration item.
 *
 * This function takes an index into the `cfg_map` and generates a name by
 * stripping prefixes/suffixes (e.g., "key_", "_ms") and replacing underscores
 * with spaces.
 *
 * @param i The index of the configuration item in the `cfg_map` array.
 * @return A dynamically allocated string containing the formatted name.
 *         The caller is responsible for freeing this memory. Returns "unknown"
 *         if a name cannot be generated.
 */
char *get_name(int i)
{
	char *name = nullptr;
	if (cfg_map[i].type == CFG_KEY)
		name = strip_prefix(cfg_map[i].name);
	else if (cfg_map[i].type == CFG_INT_A)
		name = strip_suffix(cfg_map[i].name);

	if (!name)
		return strdup("unkown");

	str_replace_char(name, '_', ' ');

	return name;
}
