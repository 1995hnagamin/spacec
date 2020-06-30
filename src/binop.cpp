#include <string>
#include <vector>

#include "ast.hpp"

#include "binop.hpp"

BinOp::~BinOp() {
}

Ast *
BasicBinOp::create(Ast *lhs, Ast *rhs) const {
  auto const expr = new BinaryExprAst(get_kind(), lhs, rhs);
  return expr;
}
