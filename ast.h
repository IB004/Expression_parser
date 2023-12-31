/* ast.h */

#pragma once
#ifndef _LLP_AST_H_
#define _LLP_AST_H

#include <inttypes.h>
#include <stdio.h>

struct AST {
  enum AST_type { AST_BINOP, AST_UNOP, AST_LIT } type;
  union {
    struct binop {
      enum binop_type { BIN_PLUS, BIN_MINUS, BIN_MUL, BIN_DIV } type;
      struct AST *left, *right;
    } as_binop;
    struct unop {
      enum unop_type { UN_NEG } type;
      struct AST *operand;
    } as_unop;
    struct literal {
      int64_t value;
    } as_literal;
  };
};

struct AST *newnode(struct AST ast);
struct AST _lit(int64_t value);
struct AST *lit(int64_t value);
struct AST _unop(enum unop_type type, struct AST *operand);
struct AST *unop(enum unop_type type, struct AST *operand);
struct AST _binop(enum binop_type type, struct AST *left, struct AST *right);
struct AST *binop(enum binop_type type, struct AST *left, struct AST *right);

void print_ast(FILE *f, struct AST *ast);
int64_t calc_ast(struct AST *ast);
void p_print_ast(FILE *f, struct AST *ast);



#define DECLARE_BINOP(fun, code)                                             \
static struct AST *fun(struct AST *left, struct AST *right) {                \
  return binop(BIN_##code, left, right);                                     \
}
DECLARE_BINOP(add, PLUS)
DECLARE_BINOP(mul, MUL)
DECLARE_BINOP(sub, MINUS)
DECLARE_BINOP(divide, DIV)
#undef DECLARE_BINOP

#define DECLARE_UNOP(fun, code)                                              \
static struct AST *fun(struct AST *operand) {                                \
  return unop(UN_##code, operand);                                           \
}
DECLARE_UNOP(neg, NEG)
#undef DECLARE_UNOP






#define RIGHT calc_ast(node->as_binop.right)
#define LEFT calc_ast(node->as_binop.left)
#define OPER calc_ast(node->as_unop.operand)

#define DECLARE_CALC(code, exp)                                        			\
static int64_t CALC_##code(struct AST *node) {                							\
  return (exp);                                     												\
}
DECLARE_CALC(PLUS, LEFT + RIGHT)
DECLARE_CALC(MINUS, LEFT - RIGHT)
DECLARE_CALC(MUL, LEFT * RIGHT)
DECLARE_CALC(DIV, LEFT / RIGHT)
DECLARE_CALC(NEG, (-1) * OPER)
#undef DECLARE_CALC

#undef RIGHT
#undef LEFT
#undef OPER

#endif
