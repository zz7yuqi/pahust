#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ, TK_NUM

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
  {"\\(", '('},           // (
  {"\\)", ')'},           // )
  {"\\+", '+'},         // plus
  {"-", '-'},           // minus
  {"\\*", '*'},         // mul
  {"/", '/'},           // div
  {"-?[1-9][0-9]*", TK_NUM},  // number
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

        switch (rules[i].token_type) {
          case TK_NOTYPE:
                          break;
          case '(':  
                    tokens[nr_token++].type = '(';
                    break;
          case ')':  
                    tokens[nr_token++].type = ')';
                    break;
          case '+':  
                    tokens[nr_token++].type = '+';
                    break;
          case '-':  
                    tokens[nr_token++].type = '-';
                    break;
          case '*':  
                    tokens[nr_token++].type = '*';
                    break;
          case '/':  
                    tokens[nr_token++].type = '/';
                    break;
          case TK_NUM:
                        tokens[nr_token++].type = TK_NUM;
                        if (substr_len > 32) substr_len = 32;

                        for (i = 0; i < substr_len; i++) 
                          tokens[nr_token].str[i] = substr_start[i];
                        
                        break;
          case TK_EQ:
                      tokens[nr_token++].type = TK_EQ;
                      break;
          default: //TODO();
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

bool check_parentheses(int p, int q, bool *legal) {
  int i, pre = -1, last = -1;
  int stack[32];
  int head = -1;

  for (i = p; i <= q; i++) {
    if (tokens[i].type == '(') {
      stack[++head] = i;
    }
    else if (tokens[i].type == ')') {
      if (head <= -1) {
        //bad expr
        *legal = false;
        return false;
      }
      pre = stack[head];    // head >= 0
      last = i;
      head--;
    }
  }

  /* when the stack is empty (head < 0) and pre==p && last == q
   * return true and legal.
   * when the stack is empty but pre != p
   * return false and legal
   */
  if (head < 0) {
    *legal = true;

    if (pre == p && last == q) 
      return true;
    else 
      return false;
  }
  else {
    // '(' > '(' illegal
    *legal = false;
    return false;
  }

  return false;
}

uint32_t eval(int p, int q, bool *legal) {
  int res, i, op;
  if (p > q) {
    *legal = false;
    return -1;
  }
  else if (p == q) {
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
  else if (check_parentheses(p, q, legal) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(p + 1, q - 1, legal);
  }
  else {
    if (*legal == false) return -1;

    // Find main op.
    int numOfParentheses = 0;
    char preOp = 0;
    op = p;
    for (i = p; i <= q; i++) {
      if ( (tokens[i].type == '+' || tokens[i].type == '-') 
            && (numOfParentheses == 0)) {

        preOp = tokens[i].type;
        op = i;
      }
      else if ( (tokens[i].type == '*' || tokens[i].type == '/') 
            && (numOfParentheses == 0)) {
              
        if (preOp == 0 || preOp == '*' || preOp == '/') {
          preOp = tokens[i].type;
          op = i;
        }
      }
      else if (tokens[i].type == '(') numOfParentheses++;
      else if (tokens[i].type == ')') numOfParentheses--;
    }
    
    int val1 = eval(p, op - 1, legal);
    int val2 = eval(op + 1, q, legal);

    if (*legal == false) return -1;

    switch (tokens[op].type)
    {
    case '+': *legal = true;
              return (val1 + val2);
              break;
    case '-': *legal = true;
              return (val1 - val2);
              break;
    case '*': *legal = true;
              return (val1 * val2);
              break;
    case '/': *legal = true;
              return (val1 / val2);
              break;
    default:
      *legal = false;
      return -1;
      break;
    }

  }

  return 0;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  //TODO();
  return eval(0, nr_token - 1, success);

  return 0;
}


