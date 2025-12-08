/**
 * @file config.h
 * @brief Manages configuration settings for the autoclicker.
 *
 * This file defines the structures and macros for managing application
 * settings, including keybindings, click intervals, and other parameters.
 */
#ifndef CONFIG_H
#define CONFIG_H

#include "constants.h"
#include "conversion.h"

#include <stdatomic.h>

#define CFG_KEYS                                                               \
	X(key_quit, KEY_ESC, "exit the program or close a menu")               \
	X(key_create_pattern, KEY_F1, "create a new pattern")                  \
	X(key_play_pattern, KEY_F2, "play saved pattern")                      \
	X(key_show_config, KEY_F3, "show this info screen")                    \
	X(key_start, KEY_F5, "start the autoclicker")                          \
	X(key_change_interval, KEY_F6, "change click interval")                \
	X(key_menu, KEY_F7, "change keybindings")                              \
	X(key_timer, KEY_F9, "start autoclicker for a specified duration")     \
	X(key_amount, KEY_F10, "click for X times")

#define CFG_INTS X(interval_ms, 100, "interval between clicks (in ms)")
#define CFG_BTNS X(key_pressed, BTN_LEFT, "key used for clicking")

/**
 * @struct config
 * @brief Holds all configuration settings for the autoclicker.
 *
 * Uses X-macros to define its members from `CFG_KEYS`, `CFG_BTNS`, and `CFG_INTS`.
 */
struct config {
#define X(name, value, desc) int name;
	CFG_KEYS
	CFG_BTNS
#undef X

#define X(name, values, desc) atomic_int name;
	CFG_INTS
#undef X
};

/** @brief Global instance of the configuration struct. */
extern struct config cfg;

#define X(name, val, desc) name,
/**
 * @enum cfg_enum
 * @brief Enumeration of all configuration items for indexing.
 *
 * Generated using X-macros.
 */
enum cfg_enum { CFG_KEYS CFG_INTS CFG_BTNS CFG_COUNT };
#undef X

/**
 * @enum cfg_type_t
 * @brief Defines the type of a configuration field.
 */
typedef enum {
	CFG_KEY,   /**< A keyboard key binding. */
	CFG_BTN,   /**< A mouse button binding. */
	CFG_INT_A, /**< An atomic integer value. */
} cfg_type_t;

/**
 * @struct cfg_map_t
 * @brief Maps a configuration setting to its properties.
 *
 * This struct is used to create a map of all configuration settings,
 * allowing them to be accessed generically.
 */
struct cfg_map_t {
	const char *name;       /**< The programmatic name of the setting. */
	const char *description;/**< A human-readable description. */
	void *field;            /**< A pointer to the field in the `cfg` struct. */
	cfg_type_t type;        /**< The type of the configuration field. */
};

/** @brief An array that maps all configuration settings. */
extern struct cfg_map_t cfg_map[];
/** @brief The number of elements in `cfg_map`. */
extern size_t cfg_map_length;

/**
 * @brief Converts a configuration field to an integer value.
 *
 * Handles different field types, including atomic integers.
 * @param position The index of the configuration field in `cfg_map`.
 * @return The integer value of the configuration field.
 */
extern int convert_field_to_int(int position);

/**
 * @brief Gets a user-friendly name for a configuration setting.
 * @param i The index of the configuration field in `cfg_map`.
 * @return A dynamically allocated string with the name. The caller must free it.
 */
extern char *get_name(int i);

#endif
