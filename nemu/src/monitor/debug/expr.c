#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, EQ, NUM, HEX, NEG, DEREF, REF, AND, OR, NOT, NEQ, REG

	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE},							// spaces

	{"0[xX][0-9a-fA-F]+", HEX},					// HEX numbers
	{"[0-9]+", NUM},							// numbers
	{"\\$[a-zA-Z]+", REG},						// registers

	{"&&", AND},									// and
	{"\\|\\|", OR},										// or
	{"==", EQ},										// equal
	{"!=", NEQ},									// not equal
	{"!", NOT},										// not
	{"&", REF},										// reference
	{"\\*\\B", DEREF},								// dereference
	{"\\+", '+'},								// plus
	{"-", '-'},									// subtract
	//{"\\< -\\B", NEG},							// NEG
	{"\\*", '*'},								// multiply
	{"/", '/'},									// devide
	{"\\(", '('},								// left parenthese
	{"\\)", ')'}								// right parenthese

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */

				switch(rules[i].token_type) {
					case NOTYPE:
						break;
					default: 
						//for(int i = 0; i < substr_len; i++) tokens[nr_token].str[i] = *(e + position + i);
						strncpy(tokens[nr_token].str , e + position, substr_len);	// DOUBT
						tokens[nr_token].str[substr_len] = '\0';
						tokens[nr_token].type = rules[i].token_type;
						nr_token++;
						//panic("please implement me");
				}

				position += substr_len;

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}

bool check_parentheses(int p, int q, bool *legal_check) {
	int parentheses_count = 0;
	for(int i = p; i <= q; i++) {			// TODO:此处重复执行，考虑优化
		if(parentheses_count < 0) *legal_check = false;
		switch(tokens[i].type) {
			case '(': 
				parentheses_count++;
				break;
			case ')': 
				parentheses_count--;
				break;
		}
	}
	if(parentheses_count) *legal_check = false;
	if(tokens[p].type == '(' && tokens[q].type == ')') return true;
	else return false;
}

int find_dominant_op(int p, int q) {
	int op_pos[4] = {0}, parentheses_count = 0;
	for(int i = p; i <= q; i++) {
		switch(tokens[i].type) {
			case '(': 
				parentheses_count++;
				break;
			case ')': 
				parentheses_count--;
				break;
			case EQ:
			case NEQ:
				if(!parentheses_count) op_pos[0] = i;
				break;
			case AND:
			case OR:
				if(!parentheses_count) op_pos[1] = i;
				break;
			case '+': 
			case '-':
				if(!parentheses_count) op_pos[2] = i;
				break;
			case '*':
			case '/':
				if(!parentheses_count) op_pos[3] = i;
				break;
		}
	}

	for(int i = 0; i < 4; i++) if(op_pos[i]) return op_pos[i];
	return 0;
}

uint32_t eval(int p, int q, bool *legal_check) {
	if(*legal_check && p > q) {
		*legal_check = false;
		return 0;
	}
	else if(p == q) {
		uint32_t val = 0;
		if(tokens[p].type == NUM) sscanf(tokens[p].str, "%u", &val);
		else if(tokens[p].type == HEX) sscanf(tokens[p].str, "%x", &val);
		else if(tokens[p].type == REG) {
			for(int i = 0; i <= 8; i++) {
				if(i == 8) {
					*legal_check = false;
					return 0;
				}
				else if(strcmp(tokens[p].str + 1, regsl[i])) {
					//sscanf(reg_l(i), "%u", &val);
					val = reg_l(i);
					break;
				}
				else if(strcmp(tokens[p].str + 1, regsw[i])) {
					//sscanf((uint32_t)reg_w(i), "%u", &val);
					val = reg_w(i);
					break;
				}
				else if(strcmp(tokens[p].str + 1, regsb[i])) {
					//sscanf((uint32_t)reg_b(i), "%u", &val);
					val = reg_b(i);
					break;
				}
			}
		}
		else {
			*legal_check = false;
			return 0;
		}

		//DEBUG
		Log("check num at %d : %u", p, val);

		return val;
	}
	else if(check_parentheses(p, q, legal_check)) {

		//DEBUG
		Log("slim parentheses");

		return eval(p + 1, q - 1, legal_check);
	}
	else if(*legal_check) {
		//单目运算符处理
		if(p + 1 == q) {
			uint32_t val = 0;
			swaddr_t addr = 0;
			switch (tokens[p].type)
			{
			case NOT:
				val = eval(q, q, legal_check);
				return !val;
			case REF:
				// TODO: implement the REF operator
			case DEREF:
				addr = (swaddr_t)eval(q, q, legal_check);
				val = swaddr_read(addr, 4);
				return val;
			case NEG:
				// TODO: implement the NEG operator
				break;
			}
		}

		int op_pos = find_dominant_op(p, q);

		//DEBUG
		Log("dominant op at %d", op_pos);

		if(!op_pos) *legal_check = false;
		uint32_t val1 = eval(p, op_pos - 1, legal_check);
		uint32_t val2 = eval(op_pos + 1, q, legal_check);

		if(!*legal_check) return 0;
		switch(tokens[op_pos].type) {
			case '+':
				return val1 + val2;
			case '-':
				return val1 - val2;
			case '*':
				return val1 * val2;
			case '/':
				return val1 / val2;
			default:
				*legal_check = false;
				return 0;
		}
	}
	return 0;
}

uint32_t expr(char *e, bool *legal_check) {
	if(!make_token(e)) {
		*legal_check = false;
		return 0;
	}

	// recognize dereference and negative
	for(int i = 0; i < nr_token - 1; i++) {
		if(tokens[i].type == '*' && (!i || !(tokens[i - 1].type == NUM 
		|| tokens[i - 1].type == REG || tokens[i - 1].type == HEX))) {
			tokens[i].type = DEREF;
		}
		if(tokens[i].type == '-' && (!i || !(tokens[i - 1].type == NUM 
		|| tokens[i - 1].type == REG || tokens[i - 1].type == HEX))) {
			tokens[i].type = NEG;
		}
	}

	//DEBUG
	for(int i = 0; i < nr_token; i++) Log("%s", tokens[i].str);

	/* TODO: Insert codes to evaluate the expression. */

	uint32_t val = eval(0, nr_token - 1, legal_check);
	//panic("please implement me");
	return val;
}

