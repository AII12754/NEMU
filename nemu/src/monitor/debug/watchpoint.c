#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#include "cpu/reg.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

static bool first_check = true;

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
	first_check = false;
}

WP* new_wp(char *str, bool *legal_check) {
	if(first_check) init_wp_pool();
	Assert(free_->next != NULL, "No empty watchpoint");
	WP *tmp = head->next;
	head->next =  free_->next;
	head->next->next = tmp;
	free_->next = free_->next->next;
	
	head->next->str = str;
	head->next->val = expr(str, legal_check);

	return head->next;
}

void free_wp(int n) {
	if(n > NR_WP || n < 0) printf("Watchpoint #%d does not exist", n);
	WP *wp = find_wp(n);
	WP *pre = head, *tmp = free_->next;
	while(pre->next != NULL && pre->next != wp) pre = pre->next;
	if(pre->next != NULL) printf("Watchpoint #%d does not exist", n);
	free_->next = pre->next;
	free_->next->next = tmp;
	pre->next = pre->next->next;
	return;
}

WP* find_wp(int n) {
	Assert(n >= NR_WP || n < 0, "No such watchpoint");
	return &(wp_pool[n]);
}

bool check_wp() {
	WP *pre = head;
	bool changed = false;
	while(pre->next != NULL) {
		pre = pre->next;
		bool legal_check = true;
		int val = expr(pre->str, &legal_check);
		if(val != pre->val) {
			printf("Hint watchpoint %d at address 0x%08x, expr = %s\n", pre->NO, cpu.eip, pre->str);
			printf("old value = %08x\n", pre->val);
			printf("old value = %08x\n", val);
			pre->val = val;
			changed = true;
		}
	}
	return changed;
}

void info_wp() {
	printf("NO        Address        Enable\n");
	WP *pre = head;
	while(pre->next != NULL) {
		pre = pre->next;
		printf("%-2d        %-16s        0x%08x\n", pre->NO, pre->str, pre->val);
	}
	return;
}

/* TODO: Implement the functionality of watchpoint */


