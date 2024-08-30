#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, EQ, NUM, HEX

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
	{"[0-9]+", NUM},							// numbers
	{"0x[0-9a-fA-F]+|0X[0-9a-fA-F]+", HEX},		// HEX numbers
	//{"\\$"}
	{"\\+", '+'},								// plus
	{"==", EQ},									// equal
	{"-", '-'},								// subtract
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
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */

				switch(rules[i].token_type) {
					case NOTYPE:
						break;
					default: 
						strncpy(tokens[nr_token].str , e, substr_len);	// DOUBT
						tokens[nr_token++].type = rules[i].token_type;
						//panic("please implement me");
				}

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

bool check_parentheses(int p, int q) {
	if(tokens[p].type == '(' && tokens[q].type == ')') return true;
	else return false;
}

int find_dominant_op(int p, int q) {
	int op_pos1 = 0, op_pos2 = 0, parentheses_count = 0;
	for(int i = p; i <= q; i++) {
		if(tokens[i].type == '(') parentheses_count++;
		else if(tokens[i].type == ')') parentheses_count--;
		else if(parentheses_count) break;
		else if(tokens[i].type == '+' || tokens[i].type == '-') op_pos1 = i;
		else if(tokens[i].type == '*' || tokens[i].type == '/') op_pos2 = i;
	}
	//return op_pos1 || op_pos2;
	if(!op_pos1) return op_pos2;
	else return op_pos1;
}

uint32_t eval(int p, int q, bool *legal_check) {
	if(p > q) {
		*legal_check = false;
		return 0;
	}
	else if(p == q) {
		uint32_t val = 0;
		if(tokens[p].type == NUM) sscanf(tokens[p].str, "%u", &val);
		else if(tokens[p].type == HEX) sscanf(tokens[p].str, "%x", &val);
		else {
			*legal_check = false;
			return 0;
		}

		//DEBUG
		printf("check num: %u", val);

		return val;
	}
	else if(check_parentheses(p, q)) {

		//DEBUG
		printf("slim parentheses");

		return eval(p + 1, q - 1, legal_check);
	}
	else {
		int op_pos = find_dominant_op(p, q);

		//DEBUG
		printf("dominant op at %d", op_pos);

		if(!op_pos) {
			*legal_check = false;
			return 0;
		}
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
}

uint32_t expr(char *e, bool *legal_check) {
	if(!make_token(e)) {
		*legal_check = false;
		return 0;
	}

	/* TODO: Insert codes to evaluate the expression. */

	uint32_t val = eval(0, nr_token - 1, legal_check);
	//panic("please implement me");
	return val;
}

