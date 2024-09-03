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
	if(n >= NR_WP || n < 0) printf("Watchpoint #%d does not exist\n", n);
	WP *wp = &(wp_pool[n]);
	Log("1");
	WP *pre = head, *tmp = free_;
	Log("1");
	if(pre == wp) {
		Log("1");
		head = head->next;
		Log("1");
		free_ = pre;
		Log("1");
		free_->next = tmp;
		Log("1");
	}
	else {
		while(pre != NULL && pre->next != wp) pre = pre->next;
		if(pre == NULL) printf("Watchpoint #%d does not exist\n", n);
		else {
			WP *next = free_;
			free_ = pre->next;
			free_->next = next;
			pre->next = pre->next->next;
		}
	}
	return;
}

bool check_wp() {
	Log("1");
	WP *pre = head;
	Log("1");
	bool changed = false;
	Log("1");
	while(pre != NULL) {
		Log("11");
		bool legal_check = true;
		Log("11");
		uint32_t val = expr(pre->str, &legal_check);
		Log("11");
		if(val != pre->val) {
			Log("11");
			printf("Hint watchpoint %d at address 0x%08x, expr = %s\n", pre->NO, cpu.eip, pre->str);
			printf("old value = %08x\n", pre->val);
			printf("new value = %08x\n", val);
			pre->val = val;
			changed = true;
		}
		pre = pre->next;
		Log("11");
	}
	Log("1");
	return changed;
}

void info_wp() {
	Log("1");
	printf("NO        Address                 Enable\n");
	Log("1");
	WP *pre = head;
	Log("1");
	while(pre != NULL) {
		Log("3");
		printf("%2d        %-16s        0x%08x\n", pre->NO, pre->str, pre->val);
		pre = pre->next;
	}
	Log("1");
	return;
}

/* TODO: Implement the functionality of watchpoint */


