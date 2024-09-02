#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;
	char *str;
	uint32_t val;
	/* TODO: Add more members if necessary */

} WP;

WP* new_wp(char *str, bool *legal_check);

void free_wp(int n);

WP* find_wp(int n);

bool check_wp();

void info_wp();

#endif
