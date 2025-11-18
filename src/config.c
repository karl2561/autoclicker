#include "config.h"
#include "utils/conversion.h"

#include <linux/input-event-codes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

constexpr char FILE_PATH[] = ".config";

void insert_from_config(int position, char *line);

atomic_bool start = false;
atomic_bool quit = false;

struct config cfg = {
#define X(name, val) .name = val,
	CFG_KEYS
#undef X

#define X(name, val) .name = val,
		CFG_INTS
#undef X
};

struct cfg_map_t cfg_map[] = {
#define X(name, val) {#name, &cfg.name, CFG_KEY},
	CFG_KEYS
#undef X
	{"interval_ms", &cfg.interval_ms, CFG_INT_A},
};

size_t cfg_map_length = sizeof(cfg_map) / sizeof(cfg_map[0]);

int convert_field_to_int(int position)
{
	if (cfg_map[position].type == CFG_KEY)
		return *(int *)cfg_map[position].field;
	else if (cfg_map[position].type == CFG_INT_A)
		return atomic_load((atomic_int *)cfg_map[position].field);

	return 0;
}

static void insert_from_configs(int position, char *line)
{
	int length = strlen(cfg_map[position].name);
	if (strncmp(line, cfg_map[position].name, length))
		return;

	int tmp_value;
	// returns 1 if it read one int successfully.
	// returns 0 if it saw something, but couldn't parsse it
	// returns EOF (usually -1) -> end of input or empty line
	if (sscanf(line + length + 1, "%d", &tmp_value) != 1)
		return;

	if (cfg_map[position].type == CFG_KEY)
		*(int *)cfg_map[position].field = tmp_value;
	else if (cfg_map[position].type == CFG_INT_A)
		atomic_store((atomic_int *)cfg_map[position].field, tmp_value);
}

int read_config()
{
	FILE *fconf = fopen(FILE_PATH, "r");
	if (!fconf)
		return -1;

	char line[MAX_LINE_LENGTH];
	while (fgets(line, sizeof(line), fconf))
		for (size_t i = 0; i < cfg_map_length; i++)
			insert_from_configs(i, line);

	fclose(fconf);
	return 0;
}

int write_config()
{
	FILE *fconf = fopen(FILE_PATH, "w");
	if (!fconf)
		return -1;

	for (size_t i = 0; i < cfg_map_length; i++) {
		int value = convert_field_to_int(i);
		fprintf(fconf, "%s=%d\n", cfg_map[i].name, value);
	}

	fclose(fconf);
	return 0;
}

char *strip_prefix(const char *s)
{
	char *ptr = strchr(s, '_');
	if (!ptr)
		return strdup(s);
	return strdup(ptr + 1);
}

char *strip_suffix(const char *s)
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

void str_replace_char(char *s, char old, char new)
{
	for (; *s; s++)
		if (*s == old)
			*s = new;
}

char *get_name(int i)
{
	char *name;
	if (cfg_map[i].type == CFG_KEY)
		name = strip_prefix(cfg_map[i].name);
	else if (cfg_map[i].type == CFG_INT_A)
		name = strip_suffix(cfg_map[i].name);
	else
		return strdup("unkown");

	if (!name)
		return strdup("unkown");

	str_replace_char(name, '_', ' ');

	return name;
}

void print_line_perm(int i)
{
	char *name = get_name(i);
	int value;
	switch (cfg_map[i].type) {
	case CFG_KEY:
		value = *(int *)cfg_map[i].field;
		printf("Press %s to %s\n", kctc(value), name);
		break;
	case CFG_INT_A:
		value = convert_field_to_int(i);
		printf("Current %s: %dms\n", name, value);
		break;
	}

	free(name);
}
