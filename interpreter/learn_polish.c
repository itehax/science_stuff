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

/* polish notation grammar parser */
int main(void) {
  mpc_parser_t *Operator = mpc_new("operator");
  mpc_parser_t *Number = mpc_new("number");
  mpc_parser_t *Expr = mpc_new("expr");
  mpc_parser_t *Polish = mpc_new("polish");

  mpca_lang(MPCA_LANG_DEFAULT,
            R"(

        operator: '+' | '-' | '*' | '/' ;
        number: /-?[0-9]+/ ;
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
      mpc_ast_print(res.output);
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
