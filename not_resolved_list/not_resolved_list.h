/**
 * not_resolved_list.h
 * Copyright (c) 2025 MVK
 * 16-May-2025
 *
 */

#ifndef _NOT_RESOLVED_H_
#define _NOT_RESOLVED_H_

#include <stdbool.h>

typedef struct not_res_t
{
	int i;
	struct not_res_t *prev;
	struct not_res_t *next;
} not_res_t;

int add_to_list(int i);
bool remove_from_list(int i);
int free_list(void);
int num_items_in_list(void);
void print_list(void);

#endif // _NOT_RESOLVED_H_
