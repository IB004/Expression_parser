/* main.c */

#include <string.h>

#include "ast.h"
#include "ring.h"
#include "tokenizer.h"

void ast_print(struct AST ast) { print_ast(stdout, &ast); }
void token_print(struct token token) { printf("%s(%" PRId64 ")", TOKENS_STR[token.type], token.value); }



DECLARE_RING(oper, struct token)
DEFINE_RING(oper, struct token)
DEFINE_RING_PRINT(oper, token_print)

DECLARE_RING(ast, struct AST*)
DEFINE_RING(ast, struct AST*)
//DEFINE_RING_PRINT(ast, ast_print)

//DECLARE_RING(token, struct token)	//declared in "tokenizer.h"
DEFINE_RING(token, struct token)
DEFINE_RING_PRINT(token, token_print)

#define RETURN_ERROR(code, msg) return printf(msg), code

static const int PRIORITY[] = {
    [TOK_PLUS] = 1, 
    [TOK_MINUS] = 1, 
    [TOK_MUL] = 5, 
    [TOK_DIV] = 5, 
    [TOK_NEG] = 5, 
    [TOK_OPEN]  = 0, 
    [TOK_CLOSE] = 0, 
    [TOK_LIT] = 100,
    [TOK_END] = 0, 
    [TOK_ERROR] = 100
};
 
static const int TOKEN_TO_AST[] = {
    [TOK_PLUS] = BIN_PLUS, 
    [TOK_MINUS] = BIN_MINUS, 
    [TOK_MUL] = BIN_MUL, 
    [TOK_DIV] = BIN_DIV, 
    [TOK_NEG] = UN_NEG
};

static int token_is_unop(struct token token){
	return (token.type == TOK_NEG);
}

static void exit_error(int code, char* msg){
	fprintf(stderr, "%s \n", msg);
	exit(code);
}

static void exec_top(struct ring_oper* opers, struct ring_ast* asts){
	struct token top = ring_oper_pop(&opers);
	struct AST* new;
	if (token_is_unop(top)){
		struct AST* operand = ring_ast_pop(&asts);
		if (operand->type != AST_LIT){ exit_error(-1, "Incorrect expression!"); }
		new = unop(TOKEN_TO_AST[top.type], operand);
	}
	else{
		struct AST* right = ring_ast_pop(&asts);
		struct AST* left = ring_ast_pop(&asts);
		if (right->type != AST_LIT || left->type != AST_LIT){ exit_error(-1, "Incorrect expression!"); }
		new = binop(TOKEN_TO_AST[top.type], left, right);
	}
	ring_ast_push(&asts, new);
}

void exec_until(enum token_type const end_type, struct ring_oper* opers, struct ring_ast* asts){
	while(1){
		struct token top = ring_oper_last(opers);
		if (top.type == end_type){ ring_oper_pop(&opers); break; }
		if (top.type == TOK_END){ exit_error(-1, "Incorrect expression!"); }
		exec_top(opers, asts);
  }
}

static void process_one_token(struct token next, struct ring_oper* opers, struct ring_ast* asts){
	struct token top = ring_oper_last(opers);
	if(next.type == TOK_LIT){
		ring_ast_push(&asts, lit(next.value));
		return;
	}
	if(next.type == TOK_OPEN){
		ring_oper_push(&opers, next);
		return;
	} 
	if(next.type == TOK_CLOSE){
		exec_until(TOK_OPEN, opers, asts);
		return;
	} 
	
	if (PRIORITY[next.type] > PRIORITY[top.type]){
		ring_oper_push(&opers, next);
	}
	else{
		exec_top(opers, asts);
		ring_oper_push(&opers, next);
	}
}

struct AST *build_ast(char *str)
{
  struct ring_token *tokens = tokenize(str);
  if (tokens == NULL)
    RETURN_ERROR(NULL, "Tokenization error.\n");
  ring_token_print(tokens);
  
  struct token end = {TOK_END, 0};
  struct ring_oper* opers = ring_oper_create(end);
  struct ring_ast* asts = ring_ast_create(binop(BIN_PLUS, NULL, NULL));
  
  while(ring_token_first(tokens).type != TOK_END){
  	struct token next = ring_token_pop_top(&tokens);
		token_print(next), printf("\n");
		process_one_token(next, opers, asts);
		ring_oper_print(opers), printf("\n");
  }
  exec_until(TOK_END, opers, asts);


  ring_token_free(&tokens);

  return ring_ast_pop(&asts);
}



int main()
{
  const int MAX_LEN = 1024;
  char str[MAX_LEN];
  if (fgets(str, MAX_LEN, stdin) == NULL)
    RETURN_ERROR(0, "Input is empty.");

  if (str[strlen(str) - 1] == '\n')
    str[strlen(str) - 1] = '\0';

  struct AST *ast = build_ast(str);

  if (ast == NULL)
    printf("AST build error.\n");
  else
  {
    print_ast(stdout, ast);
    printf("\n\n%s = %" PRId64 "\n", str, calc_ast(ast));
    p_print_ast(stdout, ast);
    printf(" = %" PRId64 "\n", calc_ast(ast));    
  }

  return 0;
}
