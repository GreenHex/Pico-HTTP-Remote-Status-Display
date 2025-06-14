/**
 * not_resolved_list.c
 * Copyright (c) 2025 MVK
 * 16-May-2025
 *
 * Duplicates not allowed
 *
 */

// #define DEBUG_NOT_RESOLVED

#include "not_resolved_list.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <malloc.h>

static not_res_t *nr_list = NULL;
static not_res_t *last = NULL;
static int num_items = 0;

// extern struct malloc_stats _malloc_stats;

static int init_list(void);
static not_res_t *find_in_list_fwd(int i);
static not_res_t *find_in_list_rev(int i);

// returns number of items in list
int add_to_list(int i)
{

#ifdef DEBUG_NOT_RESOLVED
	printf("add_to_list(%d) | ", i);
#endif // DEBUG_NOT_RESOLVED

	if (nr_list == NULL)
	{
		init_list();
		nr_list->i = i;
		++num_items;

#ifdef DEBUG_NOT_RESOLVED
		print_list();
#endif // DEBUG_NOT_RESOLVED

		return num_items;
	}

	if (find_in_list_rev(i))
		return -1; // not added

	not_res_t *p = malloc(sizeof(not_res_t));
	assert(p);
	memset(p, 0, sizeof(not_res_t));

	p->i = i;
	p->prev = last;

	last = last->next = p;
	++num_items;

#ifdef DEBUG_NOT_RESOLVED
	print_list();
#endif // DEBUG_NOT_RESOLVED

	return num_items;
}

static not_res_t *find_in_list_fwd(int i)
{
	bool found = false;
	not_res_t *a = nr_list;

	while (a)
	{
		if (a->i == i)
		{
			found = true;
			break;
		}
		a = a->next;
	}

	return found ? a : NULL;
}

static not_res_t *find_in_list_rev(int i)
{
	bool found = false;
	not_res_t *a = last;

	while (a)
	{
		if (a->i == i)
		{
			found = true;
			break;
		}
		a = a->prev;
	}

	return found ? a : NULL;
}

// returns number of items in list
bool remove_from_list(int i)
{

#ifdef DEBUG_NOT_RESOLVED
	printf("remove_from_list(%d) | ", i);
#endif // DEBUG_NOT_RESOLVED

	bool is_removed = false;
	not_res_t *a = NULL;

	if (a = find_in_list_rev(i))
	{
		if (a == nr_list) // 1st element
		{
			if (a->next)
			{
				nr_list = a->next;
				nr_list->prev = NULL;
			}
			else
			{
				nr_list = NULL;
				last = NULL;
			}
		}
		else if (a->next == NULL) // last element
		{
			a->prev->next = 0;
			last = a->prev;
		}
		else
		{
			a->prev->next = a->next;
			a->next->prev = a->prev;
		}

		--num_items;

		free(a);

		malloc_trim(0);

		a = NULL;
		is_removed = true;
	}

#ifdef DEBUG_NOT_RESOLVED
	print_list();
#endif // DEBUG_NOT_RESOLVED

	return is_removed;
}

static int init_list(void)
{
	nr_list = malloc(sizeof(not_res_t));
	memset(nr_list, 0, sizeof(not_res_t));

	last = nr_list;

	return 0;
}

int free_list(void)
{

#ifdef DEBUG_NOT_RESOLVED
	printf("free_list() | ");
#endif // DEBUG_NOT_RESOLVED

	int i = 0;
	not_res_t *a = nr_list;

	while (a)
	{
		if (a->next)
		{
			nr_list = a->next;

			free(a);

			a = nr_list;
		}
		else
		{
			free(nr_list);
			a = NULL;
		}

		++i;
	}

	nr_list = NULL;
	last = NULL;
	num_items = 0;

#ifdef DEBUG_NOT_RESOLVED
	print_list();
#endif // DEBUG_NOT_RESOLVED

	malloc_trim(0);

	return i;
}

int num_items_in_list(void)
{
	return num_items;
}

void print_list(void)
{
	if (nr_list)
	{
		not_res_t *a = nr_list;

		while (a)
		{
			// printf("0x%04X ", a->i);
			printf("%d ", a->i);
			a = a->next;
		}
	}
	printf("[%d]\n", num_items);

	// struct mallinfo mi = mallinfo();
	// printf(" Mem. %lu\n", mi.uordblks);

	// printf("Total allocated space (uordblks): %zu\n", m2.uordblks);
	// malloc_stats();
	// printf("Used memory: %d\n", (_malloc_stats.m_allocmem + _malloc_stats.m_small_allocmem));

	return;
}

#if 0
int test(void)
{

	add_to_list(5);
	remove_from_list(5);

	add_to_list(6);
	add_to_list(9);

	// free_list();

	add_to_list(15);

	add_to_list(11);

	add_to_list(1341);

	add_to_list(1411);

	add_to_list(11);

	add_to_list(123);
	add_to_list(53);
	add_to_list(99);
	add_to_list(98);

	remove_from_list(11);
	add_to_list(69);

	remove_from_list(5);
	free_list();
	remove_from_list(12);
	remove_from_list(53);
	add_to_list(10);

	remove_from_list(10);
	add_to_list(101);
	add_to_list(102);
	add_to_list(103);

	add_to_list(129123);
	add_to_list(1238);
	add_to_list(1037);
	add_to_list(1036);
	add_to_list(1035);
	add_to_list(1034);
	add_to_list(1033);
	add_to_list(1032);
	add_to_list(1031);
	remove_from_list(123);

	//
	// remove_from_list(6);

	// add_to_list(123);

	add_to_list(12225);

	add_to_list(25);
	remove_from_list(12225);
	add_to_list(75);

	free_list();
	add_to_list(129123);

	add_to_list(153);
	add_to_list(110);

	free_list();

	remove_from_list(15);
	add_to_list(1314);
	remove_from_list(1411);
	add_to_list(77);
	add_to_list(66);

	add_to_list(98);
	remove_from_list(66);

	remove_from_list(153);
	remove_from_list(123);
	// remove_from_list(110);
	remove_from_list(1341);
	remove_from_list(9);
	remove_from_list(75);
	free_list();
	remove_from_list(69);
	add_to_list(153);
	add_to_list(110);
	remove_from_list(110);
	remove_from_list(129123);
	add_to_list(12321123);

	return 0;
}
#endif

#if 0
int main()
{

#ifdef DEBUG_NOT_RESOLVED
	printf("sizeof(int) %ld\n", sizeof(int));
	printf("sizeof(*) %ld\n", sizeof(void));
	printf("sizeof(not_res_t) %ld\n", sizeof(not_res_t));
	printf("sizeof(not_res_t*) %ld\n", sizeof(not_res_t *));
	printf("sizeof(int*) %ld\n", sizeof(&num_items));
#endif // DEBUG_NOT_RESOLVED

	assert(test() == 0);

	free_list();

	return 0;
}
#endif
