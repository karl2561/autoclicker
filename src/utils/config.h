#ifndef CONFIG_H
#define CONFIG_H

#include "constants.h"
#include "conversion.h"

#include <stdatomic.h>

extern atomic_bool start;

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

struct config {
#define X(name, value, desc) int name;
	CFG_KEYS
	CFG_BTNS
#undef X

#define X(name, values, desc) atomic_int name;
	CFG_INTS
#undef X
};

extern struct config cfg;

#define X(name, val, desc) name,
enum cfg_enum { CFG_KEYS CFG_INTS CFG_BTNS CFG_COUNT };
#undef X

typedef enum { CFG_KEY, CFG_BTN, CFG_INT_A } cfg_type_t;

struct cfg_map_t {
	const char *name;
	const char *description;
	void *field;
	cfg_type_t type;
};

extern struct cfg_map_t cfg_map[];
extern size_t cfg_map_length;

extern int convert_field_to_int(int position);

/* Returns a string for the config value, caller must free it */
extern char *get_name(int i);

#endif
