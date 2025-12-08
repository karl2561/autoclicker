#include "config.h"

#include <linux/input-event-codes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct config cfg = {
#define X(name, val, desc) .name = val,
	CFG_KEYS CFG_BTNS CFG_INTS
#undef X
};

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

size_t cfg_map_length = sizeof(cfg_map) / sizeof(cfg_map[0]);

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

static char *strip_prefix(const char *s)
{
	char *ptr = strchr(s, '_');
	if (!ptr)
		return strdup(s);
	return strdup(ptr + 1);
}

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

static void str_replace_char(char *s, char old, char new)
{
	for (; *s; s++)
		if (*s == old)
			*s = new;
}

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
