#ifndef CONFIG_H
#define CONFIG_H

#include <stdatomic.h>
#include <stddef.h>

#define MAX_LINE_LENGTH 128

extern atomic_bool start;
extern atomic_bool quit;

#define CFG_KEYS                                                               \
	X(key_quit, KEY_ESC)                                                   \
	X(key_start, KEY_F5)                                                   \
	X(key_change_interval, KEY_F6)                                         \
	X(key_menu, KEY_F7)                                                    \
	X(key_pattern, KEY_F8)                                                 \
	X(key_timer, KEY_F9)                                                   \
	X(key_amount, KEY_F10)                                                 \
	X(key_pressed, BTN_LEFT)

#define CFG_INTS X(interval_ms, 100)

struct config {
#define X(name, value) int name;
	CFG_KEYS
#undef X

#define X(name, values) atomic_int name;
	CFG_INTS
#undef X
};
extern struct config cfg;

#define X(name, val) name,
enum cfg_enum { CFG_KEYS CFG_INTS CFG_COUNT };
#undef X

typedef enum { CFG_KEY, CFG_INT_A } cfg_type_t;

struct cfg_map_t {
	const char *name;
	void *field;
	cfg_type_t type;
};

extern struct cfg_map_t cfg_map[];
extern size_t cfg_map_length;

extern int convert_field_to_int(int position);
extern int read_config();
extern int write_config();

/* Returns a string for the config value, caller must free it */
extern char *get_name(int i);

/* Prints a line to stdouout. */
extern void print_line_perm(int i);

#endif
