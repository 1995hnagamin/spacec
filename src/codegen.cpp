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
  pimpl->themod.print(llvm::outs(), nullptr);
  return true;
}

llvm::Value *
CodeGen::generate_expr(Ast *body) {
  using llvm::dyn_cast;
  if (auto const ife = dyn_cast<IfExprAst>(body)) {
    return generate_if_expr(ife);
  }
  if (auto const num = dyn_cast<IntegerLiteralExpr>(body)) {
    return generate_integer_literal(num);
  } else if (auto const bin = dyn_cast<BinaryExprAst>(body)) {
    return generate_binary_expr(bin);
  }
  llvm_unreachable("not implemented");
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
CodeGen::generate_if_expr(IfExprAst *ife) {
  auto const zero = llvm::ConstantInt::get(
      llvm::Type::getInt32Ty(pimpl->thectxt), 0);
  auto const cond = generate_expr(ife->get_cond());
  auto const flag = pimpl->thebuilder.CreateICmpNE(cond, zero);

  auto const func = pimpl->thebuilder.GetInsertBlock()->getParent();

  auto thenBB = llvm::BasicBlock::Create(pimpl->thectxt, "then", func);
  auto elseBB  = llvm::BasicBlock::Create(pimpl->thectxt, "else");
  auto const mergeBB = llvm::BasicBlock::Create(pimpl->thectxt, "ifcont");
  pimpl->thebuilder.CreateCondBr(flag, thenBB, elseBB);

  pimpl->thebuilder.SetInsertPoint(thenBB);
  auto const thenV = generate_expr(ife->get_then());

  pimpl->thebuilder.CreateBr(mergeBB);
  thenBB = pimpl->thebuilder.GetInsertBlock();

  func->getBasicBlockList().push_back(elseBB);
  pimpl->thebuilder.SetInsertPoint(elseBB);
  auto const elseV = generate_expr(ife->get_else());
  pimpl->thebuilder.CreateBr(mergeBB);
  elseBB = pimpl->thebuilder.GetInsertBlock();

  func->getBasicBlockList().push_back(mergeBB);
  pimpl->thebuilder.SetInsertPoint(mergeBB);
  auto phi = pimpl->thebuilder.CreatePHI(
      llvm::Type::getInt32Ty(pimpl->thectxt), 2, "iftmp");

  phi->addIncoming(thenV, thenBB);
  phi->addIncoming(elseV, elseBB);
  return phi;
}

llvm::Value *
CodeGen::generate_integer_literal(IntegerLiteralExpr *num) {
  auto const type = llvm::Type::getInt32Ty(pimpl->thectxt);
  return llvm::ConstantInt::get(type, num->get_value());
}
