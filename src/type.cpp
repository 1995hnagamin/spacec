#include <cstdlib>
#include "type.hpp"
#include "llvm/Support/Casting.h"

Type::~Type() {
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
