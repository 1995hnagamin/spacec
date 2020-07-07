#include "llvm/ADT/APInt.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/Casting.h"

#include "ast.hpp"
#include "binop.hpp"
#include "codegen.hpp"

class CodeGenImpl {
  public:
    CodeGenImpl(std::string const &modid):
      thectxt(),
      themod(modid, thectxt),
      thebuilder(thectxt) {
    }

    llvm::LLVMContext thectxt;
    llvm::Module themod;
    llvm::IRBuilder<> thebuilder;
};

CodeGen::CodeGen(std::string const &modid):
  pimpl(new CodeGenImpl(modid)) {
}

CodeGen::~CodeGen() {
  delete pimpl;
}

bool
CodeGen::execute(Ast *tunit) {
  auto const defun = llvm::dyn_cast<DefFnAst>(tunit);
  generate_function_definition(defun);
  pimpl->themod.print(llvm::errs(), nullptr);
  return true;
}

llvm::Value *
CodeGen::generate_expr(Ast *body) {
  using llvm::dyn_cast;
  if (auto const num = dyn_cast<IntegerLiteralExpr>(body)) {
    return generate_integer_literal(num);
  }
}

llvm::Value *
CodeGen::generate_binary_expr(BinaryExprAst *bin) {
  auto const lhs = generate_expr(bin->get_lhs());
  auto const rhs = generate_expr(bin->get_rhs());
  switch (bin->get_op()->get_kind()) {
    case BO::Plus:
      return pimpl->thebuilder.CreateAdd(lhs, rhs);
    case BO::Minus:
      return pimpl->thebuilder.CreateSub(lhs, rhs);
    case BO::Mult:
      return pimpl->thebuilder.CreateMul(lhs, rhs);
    case BO::Div:
      return pimpl->thebuilder.CreateSDiv(lhs, rhs);
  }
}

void
CodeGen::generate_function_definition(DefFnAst *def) {
  auto const arity = def->get_arity();
  std::vector<llvm::Type *> param_types(
      arity,
      llvm::Type::getInt32Ty(pimpl->thectxt));
  llvm::FunctionType *fn_type = llvm::FunctionType::get(
      llvm::Type::getInt32Ty(pimpl->thectxt),
      param_types,
      false /* not variadic */);
  llvm::Function *fn = llvm::Function::Create(
      fn_type,
      llvm::Function::ExternalLinkage,
      llvm::Twine(def->get_name()),
      pimpl->themod);

  llvm::BasicBlock *BB = llvm::BasicBlock::Create(pimpl->thectxt, "entry", fn);
  pimpl->thebuilder.SetInsertPoint(BB);

  auto const val = generate_expr(def->get_body());
  pimpl->thebuilder.CreateRet(val);

  llvm::verifyFunction(*fn);
}

llvm::Value *
CodeGen::generate_integer_literal(IntegerLiteralExpr *num) {
  auto const type = llvm::Type::getInt32Ty(pimpl->thectxt);
  return llvm::ConstantInt::get(type, num->get_value());
}
