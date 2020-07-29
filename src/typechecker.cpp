#include <map>
#include <iostream>
#include <string>
#include <vector>
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"
#include "ast.hpp"
#include "binop.hpp"
#include "type.hpp"

#include "typechecker.hpp"

class TypeCheckerImpl {
  public:
    Type *lookup_tyenv(std::string const &name) const;
    void push_tyenv();
    void pop_tyenv();
    void register_type(std::string const &name, Type *ty);

    using tymap = std::map<std::string, Type *>;
    std::vector<tymap> tyenv;
};

Type *
TypeCheckerImpl::lookup_tyenv(std::string const &name) const {
  for (auto iter = rbegin(tyenv); iter != rend(tyenv); ++iter) {
    auto const kv = iter->find(name);
    if (kv != iter->end()) {
      return kv->second;
    }
  }
  return nullptr;
}

void
TypeCheckerImpl::push_tyenv() {
  tyenv.push_back(tymap());
}

void
TypeCheckerImpl::pop_tyenv() {
  tyenv.pop_back();
}

void
TypeCheckerImpl::register_type(std::string const &name, Type *ty) {
  tyenv.back()[name] = ty;
}

TypeChecker::TypeChecker(): pimpl(new TypeCheckerImpl) {
}

TypeChecker::~TypeChecker() = default;

void
TypeChecker::traverse_tunit(TranslationUnitAst *tunit) {
  pimpl->push_tyenv();
  for (size_t i = 0, len = tunit->size(); i < len; ++i) {
    auto const def = llvm::cast<DefFnAst>(tunit->get_nth_func(i));
    auto const fnty = traverse_deffn(def);
    pimpl->register_type(def->get_name(), fnty);
  }
  pimpl->pop_tyenv();
}

Type *
TypeChecker::traverse_decl(Ast *) {
}

Type *
TypeChecker::traverse_deffn(DefFnAst *def) {
  pimpl->push_tyenv();
  std::vector<Type *> params;
  for (size_t i = 0, len = def->get_arity(); i < len; ++i) {
    auto const name = def->get_nth_name(i);
    auto const ty = def->get_nth_type(i);
    pimpl->register_type(name, ty);
    params.push_back(ty);
  }
  auto const retty = def->get_return_type();
  auto const fnty = new FunctionType(retty, params);
  pimpl->register_type(def->get_name(), fnty);

  auto const body = llvm::cast<BlockExprAst>(def->get_body());
  auto const bodyty = traverse_block_expr(body);
  if (not retty->equal(bodyty)) {
    llvm::report_fatal_error("return type mismatch");
  }
  pimpl->pop_tyenv();
  return fnty;
}

Type *
TypeChecker::traverse_expr(Ast *expr) {
  using llvm::dyn_cast;
  if (auto const bin = dyn_cast<BinaryExprAst>(expr)) {
    return traverse_binary_expr(bin);
  }
  if (auto const block = dyn_cast<BlockExprAst>(expr)) {
    return traverse_block_expr(block);
  }
  if (auto const call = dyn_cast<CallExprAst>(expr)) {
    return traverse_call_expr(call);
  }
  if (auto const ife = dyn_cast<IfExprAst>(expr)) {
    return traverse_if_expr(ife);
  }
  if (auto const let = dyn_cast<LetStmtAst>(expr)) {
    return traverse_let_stmt(let);
  }
  llvm_unreachable("not implemented");
}

Type *
TypeChecker::traverse_binary_expr(BinaryExprAst *bin) {
  switch (bin->get_op()->get_kind()) {
    case BO::Eq:
    case BO::Lt:
    case BO::Gt:
    {
      auto const lty = traverse_expr(bin->get_lhs());
      auto const rty = traverse_expr(bin->get_rhs());
      if (!llvm::isa<IntNType>(lty) || !llvm::isa<IntNType>(rty)) {
        llvm::report_fatal_error("must be integer");
      }
      return new BoolType;
    }
    case BO::Plus:
    case BO::Minus:
    case BO::Mult:
    case BO::Div:
    {
      auto const lty = traverse_expr(bin->get_lhs());
      auto const rty = traverse_expr(bin->get_rhs());
      if (!llvm::isa<IntNType>(lty) || !llvm::isa<IntNType>(rty)) {
        llvm::report_fatal_error("must be integer");
      }
      return new IntNType(32);
    }
  }
  llvm_unreachable("not implemented");
}

Type *
TypeChecker::traverse_block_expr(BlockExprAst *block) {
  size_t len = block->size();
  if (len == 0) {
    return new UnitType;
  }
  pimpl->push_tyenv();
  for (size_t i = 0; i + 1 < len; ++i) {
    auto const expr = block->get_nth_stmt(i);
    auto const ty = traverse_expr(expr);
    if (not llvm::isa<UnitType>(ty)) {
      llvm::report_fatal_error("must be unit");
    }
  }
  auto const ty = traverse_expr(block->get_nth_stmt(len-1));
  pimpl->pop_tyenv();
  return ty;
}

Type *
TypeChecker::traverse_call_expr(CallExprAst *call) {
  auto const callee = call->get_callee();
  auto const fnty = llvm::dyn_cast<FunctionType>(traverse_expr(callee));
  if (not fnty) {
    llvm::report_fatal_error("must be function");
  }
  auto const arity = fnty->get_arity();
  if (arity != call->get_nargs()) {
    llvm::report_fatal_error("wrong number of arguments");
  }
  for (size_t i = 0; i < arity; ++i) {
    auto const ty = traverse_expr(call->get_nth_arg(i));
    if (not ty->equal(fnty->get_nth_param(i))) {
      llvm::report_fatal_error("wrong argument");
    }
  }
  return fnty->get_return_type();
}

Type *
TypeChecker::traverse_if_expr(IfExprAst *ife) {
  auto const condty = traverse_expr(ife->get_cond());
  if (not llvm::isa<BoolType>(condty)) {
    llvm::report_fatal_error("must be bool");
  }
  auto const thenty = traverse_expr(ife->get_then());
  auto const elsety = traverse_expr(ife->get_else());
  if (not thenty->equal(elsety)) {
    llvm::report_fatal_error("then and else must be the same type");
  }
  return thenty;
}

Type *
TypeChecker::traverse_let_stmt(LetStmtAst *let) {
  auto const var = let->get_var_name();
  auto const ty = traverse_expr(let->get_init());
  pimpl->register_type(var, ty);
  return new UnitType;
}

Type *
TypeChecker::traverse_var_ref(VarRefExprAst *var) {
  return pimpl->lookup_tyenv(var->get_name());
}
