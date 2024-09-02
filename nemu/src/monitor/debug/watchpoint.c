#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}

WP* new_wp() {
	Assert(free_->next != NULL, "No empty watchpoint");
	WP *tmp = head->next;
	head->next =  free_->next;
	head->next->next = tmp;
	free_->next = free_->next->next;
	return head->next;
}

void free_wp(WP *wp) {
	WP *pre = head, *tmp = free_->next;
	while(pre->next != NULL && pre->next != wp) pre = pre->next;
	Assert(pre->next != NULL, "No such watchpoint in use");
	free_->next = pre->next;
	free_->next->next = tmp;
	pre->next = pre->next->next;
	return;
}

/* TODO: Implement the functionality of watchpoint */


