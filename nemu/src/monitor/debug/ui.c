#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args);

static int cmd_info(char *args);

static int cmd_p(char *args);

static int cmd_x(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands.", cmd_help },
  { "c", "Continue the execution of the program.", cmd_c },
  { "q", "Exit NEMU.", cmd_q },
  { "si", "Execute N instructions, then stop the program", cmd_si },
  { "info", "Print register information when \"info r\", print watchpoints when \"info w\".", cmd_info},
  { "p", "Calculate the value of the expression EXPR.", cmd_p},
  { "x", "Calculate the value of the expression EXPR, and output N consecutive 4 bytes starting from EXPR in hexadecimal form.", cmd_x },
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_si(char *args) {
  int N;
  /* N is 1 when args is NULL */
  if (args == NULL) N = 1;
  else {
    /* N == 0 when args is unknown */
    if (sscanf(args, "%d", &N) < 1) N = 0;
  }

  if (N > 0){
    cpu_exec(N);
    //printf("execute %d steps!\n", N);
  }
  else {
    printf("error args[%s] of command si\n", args);
  }

  return 0;
}

static int cmd_info(char * args){
  /* print reg_info when argument is r 
   * print watchpoint_info when argument is w
   */
  if (args == NULL){
    printf("error args.\n");
  }
  else if (strcmp(args, "r") == 0){
    isa_reg_display();
  }
  else if (strcmp(args, "w") == 0){
    printWP();
  }
  else {
    printf("error args.\n");
  }

  return 0;
}

static int cmd_p(char *args)
{
	if(args == NULL) return 0;
	bool success = true;
	uint32_t value = expr(args, &success);
	if(success){
		printf("0x%08x\n", value);
	}
  else {
    printf("wrong EXPR.\n");
  }
	return 0;
}

static int cmd_x(char *args){
	char *arg = strtok(NULL, " ");
	if(arg == NULL) {
    printf("wrong args.\n");
    cmd_help("x");
    return 0;
  }
	int n = 0, i;
	sscanf(arg, "%d", &n);

  char *exprInCmd = arg + strlen(arg) + 1;

	bool success = true;
	uint32_t res = expr(exprInCmd, &success);
	if(!success) {
    printf("wrong expr.\n");
    return 0;
  }
	for(i = 0; i < n; i++){
		printf("0x%08x: 0x%08x\n ", res, vaddr_read(res, 4));
		res += 4;
	}
	return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    /* position of the last character of the command */
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    /* no args in command */
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
