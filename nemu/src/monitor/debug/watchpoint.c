#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#include "cpu/reg.h"

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

WP* new_wp(char *str, bool *legal_check) {
	Assert(free_ != NULL, "No empty watchpoint");
	WP *tmp = head, *next = free_;
	free_ = free_->next;
	head = next;
	head->next = tmp;
	
	//head->str = "init";
	head->str = strdup(str);
	strcat(head->str, "\0");
	head->val = expr(str, legal_check);

	return head;
}

void free_wp(int n) {
	if(n >= NR_WP || n < 0) {
		printf("Watchpoint #%d does not exist\n", n);
		return;
	}
	WP *wp = &(wp_pool[n]);
	WP *pre = head, *tmp = free_;
	if(pre == wp) {
		head = head->next;
		free_ = pre;
		free_->next = tmp;
	}
	else {
		while(pre != NULL && pre->next != wp) pre = pre->next;
		if(pre == NULL) printf("Watchpoint #%d does not exist\n", n);
		else {
			WP *next = pre->next->next;
			free_ = pre->next;
			free_->next = tmp;
			pre->next = next;
		}
	}
	return;
}

bool check_wp() {
	WP *pre = head;
	bool changed = false;
	while(pre != NULL) {
		bool legal_check = true;
		uint32_t val = expr(pre->str, &legal_check);
		if(val != pre->val) {
			printf("Hint watchpoint %d at address 0x%08x, expr = %s\n", pre->NO, cpu.eip, pre->str);
			printf("old value = %08x\n", pre->val);
			printf("new value = %08x\n", val);
			pre->val = val;
			changed = true;
		}
		pre = pre->next;
	}
	return changed;
}

void info_wp() {
	printf("NO        Address                 Enable\n");
	WP *pre = head;
	while(pre != NULL) {
		printf("%2d        %-16s        0x%08x\n", pre->NO, pre->str, pre->val);
		pre = pre->next;
	}
	return;
}

