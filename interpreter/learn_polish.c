/*
dummy json grammar:
string : [a-zA-Z0-9]
number: [0-9]+ | [0-9]+ "." [0-9]+
boolen: "true" | "false"
null: "null"
value_type: string | number | boolean | null | array | json
k_v_pair: string ":" value_type
array:  "[" value_type ("," value_type )* "]"
json: "{" k_v_pair  ("," k_v_pair)* "}"
*/
#include "parser_combinator/mpc.h"
#include <editline.h>
#include <stdio.h>
#include <string.h>

/* Use operator string to see which operation to perform */
long eval_op(long x, char *op, long y) {
  if (strcmp(op, "+") == 0) {
    return x + y;
  }
  if (strcmp(op, "-") == 0) {
    return x - y;
  }
  if (strcmp(op, "*") == 0) {
    return x * y;
  }
  if (strcmp(op, "/") == 0) {
    return x / y;
  }
  return 0;
}

/*
Ast looks like that:
    root
    |
    +--  operator (child[1])
    |
    +-- expr | number | regex (child[2])
    |
    +-- expr (child[3])
        |
        +-- operator (sub-child[1])
        |
        +-- expr | number | regex (sub-child[1])
        |
        +-- expr | ... and so on

*/
long eval(mpc_ast_t *ast) {
  /* base case: if it is number return it */
  if (strstr(ast->tag, "number")) {
    return atol(ast->contents);
  }
  /* the op is always in second child */
  char *op = ast->children[1]->contents;

  long x = eval(ast->children[2]);

  /* Iterate the remaining children and combining. */
  int i = 3;
  while (strstr(ast->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(ast->children[i]));
    i++;
  }

  return x;
}
/* polish notation grammar parser */
int main(void) {
  mpc_parser_t *Operator = mpc_new("operator");
  mpc_parser_t *Number = mpc_new("number");
  mpc_parser_t *Expr = mpc_new("expr");
  mpc_parser_t *Polish = mpc_new("polish");

  mpca_lang(MPCA_LANG_DEFAULT,
            R"(

        operator: '+' | '-' | '*' | '/' | '%' ;
        number: /-?[0-9]+(\.[0-9]+)?/  ;
        expr: <number> | '(' <operator> <expr>+ ')' ;
        polish: /^/ <operator> <expr>+ /$/ ;

      )",
            Operator, Number, Expr, Polish);

  puts("PLN PARSER");
  puts("Press Ctrl+c to exit");

  while (1) {
    char *input = readline("PLN parser > ");
    /* try parse user input */
    mpc_result_t res;
    if (mpc_parse("<stdin>", input, Polish, &res)) {
      long result = eval(res.output);
      printf("%li\n", result);
      mpc_ast_delete(res.output);
    } else {
      mpc_err_print(res.error);
      mpc_err_delete(res.error);
    }
    add_history(input);
    free(input);
  }
  mpc_cleanup(4, Operator, Number, Expr, Polish);
}
