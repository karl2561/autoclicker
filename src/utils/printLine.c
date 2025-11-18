#include "printLine.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int countChar(const char *s, const char c)
{
	int count = 0;
	for (; *s; s++)
		if (*s == c)
			count++;

	return count;
}

void remove_line(void *data)
{
	struct printed_line *line = (struct printed_line *)data;

	if (!line)
		return;

	if (line->lines > 0) {
		if (line->length > 0)
			line->lines++;
		for (int i = 0; i < line->lines; i++)
			printf("\033[F\033[2K");
	} else {
		for (int i = 0; i < line->length; i++)
			printf("\b \b");
	}
}

struct printed_line *create_line(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	int len = vsnprintf(NULL, 0, fmt, args);
	va_end(args);

	char *buf = malloc(len + 1);
	if (!buf)
		return nullptr;

	va_start(args, fmt);
	vsnprintf(buf, len + 1, fmt, args);
	va_end(args);

	struct printed_line *node = malloc(sizeof(struct printed_line));
	if (!node) {
		free(buf);
		return nullptr;
	}

	const char *last = strrchr(buf, '\n');
	node->text = buf;
	node->lines = countChar(buf, '\n');
	node->length = last ? strlen(last + 1) : strlen(buf);

	printf("%s", buf);
	fflush(stdout);

	return node;
}
