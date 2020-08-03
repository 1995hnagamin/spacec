#include <string>
#include <vector>

#include "ast.hpp"

#include "binop.hpp"

static bool
bo_higher_than(BO lhs, BO rhs) {
  // clang-format off
  static bool constexpr gt[7][7] = {
    { 0, 0, 0, 0, 1, 1, 1, },
    { 0, 0, 0, 0, 1, 1, 1, },
    { 1, 1, 0, 0, 1, 1, 1, },
    { 1, 1, 0, 0, 1, 1, 1, },
    { 0, 0, 0, 0, 0, 0, 0, },
    { 0, 0, 0, 0, 0, 0, 0, },
    { 0, 0, 0, 0, 0, 0, 0, },
  };
  // clang-format on
  auto const l = static_cast<int>(lhs);
  auto const r = static_cast<int>(rhs);
  return gt[l][r];
}

static bool
bo_same(BO lhs, BO rhs) {
  // clang-format off
  static int constexpr group[] = {
    1, 1,
    2, 2,
    0, 0, 0,
  };
  // clang-format on
  auto const l = static_cast<int>(lhs);
  auto const r = static_cast<int>(rhs);
  if (group[l] == 0 || group[r] == 0) {
    return false;
  }
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
  // clang-format off
  static bool constexpr left[] = {
    1, 1, 1, 1,
    1, 1, 1,
  };
  // clang-format on
  return left[static_cast<int>(get_kind())];
}

Ast *
BasicBinOp::create(Ast *lhs, Ast *rhs) {
  auto const expr = new BinaryExprAst(this, lhs, rhs);
  return expr;
}

bool
comparable(BinOp const &lhs, BinOp const &rhs) {
  auto const l = lhs.get_kind();
  auto const r = rhs.get_kind();
  return bo_higher_than(l, r) || bo_higher_than(r, l) || bo_same(l, r);
}
