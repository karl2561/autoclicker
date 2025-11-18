#ifndef RECORD_PATTERN_H
#define RECORD_PATTERN_H

struct Movement {
	int x_change;
	int y_change;
};

[[nodiscard]] struct Movement *createInput(int x, int y);
[[nodiscard]] extern struct slist *create_pattern();

void remove_item(void *data);

#endif
