#include "type.hpp"
#include "llvm/Support/Casting.h"
#include <cstdlib>
#include <vector>

Type::~Type() {
}

bool
FunctionType::equal(Type *rhs) const {
  auto const rty = llvm::dyn_cast<FunctionType>(rhs);
  if (not rty) {
    return false;
  }
  if (not ret->equal(rty->ret) || get_arity() != rty->get_arity()) {
    return false;
  }
  for (size_t i = 0, len = get_arity(); i < len; ++i) {
    auto const il = get_nth_param(i);
    auto const ir = rty->get_nth_param(i);
    if (not il->equal(ir)) {
      return false;
    }
  }
  return true;
}

bool
IntNType::equal(Type *lhs) const {
  if (auto const lty = llvm::dyn_cast<IntNType>(lhs)) {
    return width == lty->width;
  }
  return false;
}

bool
TyVar::equal(Type *lhs) const {
  if (auto const lty = llvm::dyn_cast<TyVar>(lhs)) {
    return id == lty->id;
  }
  return false;
}
