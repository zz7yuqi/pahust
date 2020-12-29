#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ, TK_NUM, TK_HEX, TK_LKH, TK_RKH, TK_PLUS, TK_SUB, TK_MUL, TK_DIV,
  TK_REG, TK_AND, TK_OR, TK_DEREF

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\(", TK_LKH},           // (
  {"\\)", TK_RKH},           // )
  {"\\+", TK_PLUS},         // plus
  {"-", TK_SUB},           // minus
  {"\\*", TK_MUL},         // mul
  {"/", TK_DIV},           // div
  {"0[Xx][0-9a-fA-F]+", TK_HEX},	// hex (must before the TK_NUM)
  {"[0-9]+", TK_NUM},  // number
  {"\\$[a-zA-Z]+", TK_REG},			// register
  {"&&", TK_AND},					// AND
  {"\\|\\|", TK_OR},					// OR
  {"==", TK_EQ}         // equal
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        if(substr_len >= 32){
			    printf("%.*s  The length of the substring is too long.\n", substr_len, substr_start);
			    return false;
		    }

		    if(nr_token >= 32) {
			    printf("The count of tokens(nr_token) is out of the maximum count(32)\n");
			    return false;
		    }

        switch (rules[i].token_type) {
          case TK_NOTYPE:
                          break;
          case TK_HEX:
			    case TK_REG:
          case TK_NUM:
                        tokens[nr_token].type = rules[i].token_type;
                        strncpy(tokens[nr_token].str, substr_start, substr_len);
				                tokens[nr_token].str[substr_len] = '\0';
                        nr_token++;
                        break;
          

          default: //TODO();
                    tokens[nr_token++].type = rules[i].token_type;
                    
                    break;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

// return the priority of the op
int priorityOfOp(int op)
{
	int pri;
	switch(op){
		case TK_OR:
			pri = 0;
			break;
		case TK_AND:
			pri = 1;
			break;
		case TK_EQ:
			pri = 2;
			break;
		case TK_PLUS:
		case TK_SUB :
			pri = 3;
			break;
		case TK_MUL:
		case TK_DIV:
			pri = 4;
			break;
		case TK_DEREF:
			pri = 5;
			break;
		default:
			pri = 10;
	}
	return pri;
}

int comparePriorityOfOp(int op1, int op2){
	int p1 = priorityOfOp(op1);
	int p2 = priorityOfOp(op2);
	if (p1 < p2) return -1;
  else if (p1 == p2) return 0;
  else return 1;
}

bool check_parentheses(int p, int q, bool *legal) {
  Log("check_parentheses begin.");
  int i, pre = -1, last = -1;
  int stack[32];
  int head = 0;

  for (i = p; i <= q; i++) {
    if (tokens[i].type == TK_LKH) {
      stack[head] = i;
      head++;
    }
    else if (tokens[i].type == TK_RKH) {
      if (head <= 0) {
        //bad expr
        *legal = false;
        return false;
      }
      pre = stack[head - 1];    // head >= 0
      last = i;
      head--;
    }
  }

  /* when the stack is empty (head < 0) and pre==p && last == q
   * return true and legal.
   * when the stack is empty but pre != p
   * return false and legal
   */
  // if (head == 0) {
  //   *legal = true;

  //   if (pre == p && last == q) 
  //     return true;
  //   else 
  //     return false;
  // }
  // else {
  //   // '(' > '(' illegal
  //   *legal = false;
  //   return false;
  // }
  *legal = true;
  return false;
}

bool isCulOp(int op) {
  if (op == TK_PLUS || op == TK_SUB || op == TK_MUL || op == TK_DIV ||
      op == TK_OR || op == TK_AND || op == TK_EQ || op == TK_DEREF)
      return true;
  return false;
}

uint32_t eval(int p, int q, bool *legal) {
  Log("Eval begin.");
  int res, i, op;
  if (p > q) {
    Log("In eval, p>q.");
    *legal = false;
    return -1;
  }
  else if (p == q) {
    Log("In eval, p==q.");
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
    if (tokens[p].type != TK_NUM) {
      *legal = false;
      return -1;
    }
    else {
      sscanf(tokens[p].str, "%d", &res);
      *legal = true;
      return res;
    }
  }

  // bool funCheckParenthesesRes = check_parentheses(p, q, legal);
  // if (funCheckParenthesesRes == true) {
  //   Log("check_parentheses is true.");
  //   /* The expression is surrounded by a matched pair of parentheses.
  //    * If that is the case, just throw away the parentheses.
  //    */
  //   return eval(p + 1, q - 1, legal);
  // }
  // else {
  //   Log("check_parentheses is false.");
  //   if (*legal == false) {
  //     return -1;
  //   }
  // }

  Log("To find main oprator.");
  // Find main op.
  int numOfParentheses = 0;
  int preOp = -1;
  op = -1;
  for (i = p; i <= q; i++) {
    if ( isCulOp(tokens[i].type) 
          && (numOfParentheses == 0)) {

      if (op == -1) op = i;
      else if (comparePriorityOfOp(tokens[i].type, tokens[op].type) <= 0) op = i;
    }
    else if (tokens[i].type == TK_LKH) numOfParentheses++;
    else if (tokens[i].type == TK_RKH) numOfParentheses--;
  }

  Log("Main op is found.");
  // Main op is found.
  int val1 = eval(p, op - 1, legal);
  int val2 = eval(op + 1, q, legal);

  if (*legal == false) return -1;

  switch (tokens[op].type)
  {
  case TK_PLUS: *legal = true;
            return (val1 + val2);
            break;
  case TK_SUB: *legal = true;
            return (val1 - val2);
            break;
  case TK_MUL: *legal = true;
            return (val1 * val2);
            break;
  case TK_DIV: *legal = true;
            return (val1 / val2);
            break;
  default:
    *legal = false;
    return -1;
    break;
  }



  return 0;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    //printf("make_token failed!\n");
    *success = false;
    return 0;
  }
  /* TODO: Insert codes to evaluate the expression. */
  //TODO();
  //*success = true;
  return eval(0, nr_token - 1, success);

}


