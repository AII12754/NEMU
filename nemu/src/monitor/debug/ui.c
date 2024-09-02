#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char *rl_gets()
{
	static char *line_read = NULL;

	if (line_read)
	{
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read)
	{
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args)
{
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args)
{
	return -1;
}

static int cmd_si(char *args)
{
	char *arg = strtok(args, " ");
	uint32_t n;
	if (arg == NULL)
	{
		n = 1;
	}
	else
	{
		n = atoi(arg);
	}
	cpu_exec(n);
	return 0;
}

static int cmd_info(char *args)
{
	char *arg = strtok(args, " ");

	switch(*arg) {
		case 'r':
			for(int i = 0; i < 8; i++) printf("%-16s%-16u0X%08X\n", regsl[i], reg_l(i), reg_l(i));
			printf("%-16s%-16u0X%08X\n", "eip", cpu.eip, cpu.eip);
			break;
		case 'w':
			Log("2");
			info_wp();
			break;
		default:
			printf("illegal parameter");
	}
	return 0;
}

static int cmd_p(char *args)
{
	bool legal_check = true;
	uint32_t val = expr(args, &legal_check);
	if(!legal_check) printf("illegal expression!\n");
	else printf("%s = 0x%08x(%u)\n", args, val, val);
	return 0;
}

static int cmd_x(char *args)
{
	char *arg[2];
	arg[0] = strtok(args, " ");
	args += strlen(arg[0]) + 1;
	arg[1] = strtok(args, " ");
	int len = atoi(arg[0]);
	uint32_t addr = (uint32_t)strtol(arg[1], NULL, 16);

	uint32_t data;
	printf("0X%08X: ", addr);
	for (int i = 0; i < len; i++)
	{
		data = swaddr_read(addr, 4);
		if (!(i % 4) && i) printf("\n0X%08X: ", addr);
		printf("0X%08X ", data);
		addr += 4;
	}
	printf("\n");

	return 0;
}

static int cmd_w(char *args)
{
	bool legal_check = true;
	WP *wp = new_wp(args, &legal_check);
	if(legal_check) printf("Set watchpoint #%d\n", wp->NO);
	else {
		free_wp(wp->NO);
		printf("illegal expression!\n");
	}
	return 0;
}

static int cmd_d(char *args)
{
	char *arg = strtok(args, " ");
	int n;
	n = atoi(arg);
	free_wp(n);
	return 0;
}

static int cmd_bt(char *args)
{
	// TODO
	return 0;
}

static int cmd_help(char *args);

static struct
{
	char *name;
	char *description;
	int (*handler)(char *); // 函数指针
} cmd_table[] = {
	{"help", "Display informations about all supported commands", cmd_help},
	{"c", "Continue the execution of the program", cmd_c},
	{"q", "Exit NEMU", cmd_q},
	{"si", "execute next N instructions and then stop", cmd_si},
	{"info", "print the status of registers or the watchpoints", cmd_info},
	{"p", "print the value of the exprission", cmd_p},
	{"x", "print the following N * 4 bits in HEX", cmd_x},
	{"w", "when the exprission changes, the program will stop", cmd_w},
	{"d", "delete the watchpoint numbered N", cmd_d},
	{"bt", "print the stack frame chain", cmd_bt}

	/* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args)
{
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if (arg == NULL)
	{
		/* no argument given */
		for (i = 0; i < NR_CMD; i++)
		{
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else
	{
		for (i = 0; i < NR_CMD; i++)
		{
			if (strcmp(arg, cmd_table[i].name) == 0)
			{
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop()
{
	while (1)
	{
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if (cmd == NULL)
		{
			continue;
		}

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1; // 此处已去掉首个空格
		if (args >= str_end)
		{
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for (i = 0; i < NR_CMD; i++)
		{
			if (strcmp(cmd, cmd_table[i].name) == 0)
			{
				if (cmd_table[i].handler(args) < 0)
				{
					return;
				}
				break;
			}
		}

		if (i == NR_CMD)
		{
			printf("Unknown command '%s'\n", cmd);
		}
	}
}
