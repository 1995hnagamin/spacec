#include <string>
#include <vector>

#include "ast.hpp"

#include "binop.hpp"

static
bool bo_higher_than(BO lhs, BO rhs) {
  static bool const gt[4][4] = {
    { 0, 0, 0, 0, },
    { 0, 0, 0, 0, },
    { 1, 1, 0, 0, },
    { 1, 1, 0, 0, },
  };
  auto const l = static_cast<int>(lhs);
  auto const r = static_cast<int>(rhs);
  return gt[l][r];
}

static
bool bo_same(BO lhs, BO rhs) {
  static int constexpr group[] = {
    1, 1,
    2, 2,
  };
  auto const l = static_cast<int>(lhs);
  auto const r = static_cast<int>(rhs);
  return group[l] == group[r];
}

BinOp::~BinOp() {
}

bool
BinOp::higher_than(BinOp const &other) const {
  return bo_higher_than(get_kind(), other.get_kind());
}

bool
BinOp::same(BinOp const &other) const {
  return bo_same(get_kind(), other.get_kind());
}

bool
BinOp::is_left() const {
  static bool constexpr left[] = {
    1, 1, 1, 1,
  };
  return left[static_cast<int>(get_kind())];
}

Ast *
BasicBinOp::create(Ast *lhs, Ast *rhs) const {
  auto const expr = new BinaryExprAst(get_kind(), lhs, rhs);
  return expr;
}

bool
comparable(BinOp const &lhs, BinOp const &rhs) {
  auto const l = lhs.get_kind();
  auto const r = rhs.get_kind();
  return bo_higher_than(l, r)
          || bo_higher_than(r, l)
          || bo_same(l, r);
}
