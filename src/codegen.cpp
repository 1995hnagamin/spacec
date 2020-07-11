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

    using varmap = std::map<std::string, llvm::Value *>;
    std::vector<varmap> vartab;
    llvm::Value *lookup_vartab(std::string const &name) const;
    void push_vartab();
    void pop_vartab();
    void register_val(std::string const &name, llvm::Value *val);
    llvm::AllocaInst *register_auto_var(std::string const &name, llvm::Value *val);
};

llvm::Value *
CodeGenImpl::lookup_vartab(std::string const &name) const {
  for (auto iter = vartab.rbegin(); iter != vartab.rend(); ++iter) {
    for (auto &&kv : *iter) {
      if (kv.first == name) {
        return kv.second;
      }
    }
  }
  return nullptr;
}

void
CodeGenImpl::push_vartab() {
  vartab.push_back(varmap());
}

void
CodeGenImpl::pop_vartab() {
  vartab.pop_back();
}

void
CodeGenImpl::register_val(std::string const &name, llvm::Value *val) {
  vartab.back()[name] = val;
}

llvm::AllocaInst *
CodeGenImpl::register_auto_var(std::string const &name, llvm::Value *val) {
  auto const fn = thebuilder.GetInsertBlock()->getParent();
  llvm::IRBuilder<> entry(&fn->getEntryBlock(), fn->getEntryBlock().begin());
  auto const alloca = entry.CreateAlloca(llvm::Type::getInt32Ty(thectxt), 0, name);
  register_val(name, alloca);

  return alloca;
}

CodeGen::CodeGen(std::string const &modid):
  pimpl(new CodeGenImpl(modid)) {
}

CodeGen::~CodeGen() {
  delete pimpl;
}

bool
CodeGen::execute(Ast *prog) {
  pimpl->push_vartab();

  auto const tunit = llvm::dyn_cast<TranslationUnitAst>(prog);
  for (size_t i = 0, len = tunit->size(); i < len; ++i) {
    auto const defun = llvm::dyn_cast<DefFnAst>(tunit->get_nth_func(i));
    generate_function_definition(defun);
  }

  pimpl->pop_vartab();

  return true;
}

void
CodeGen::display_llvm_ir(llvm::raw_ostream &s) {
  pimpl->themod.print(s, nullptr);
}

llvm::Value *
CodeGen::generate_expr(Ast *body) {
  using llvm::dyn_cast;
  if (auto const bin = dyn_cast<BinaryExprAst>(body)) {
    return generate_binary_expr(bin);
  }
  if (auto const block = dyn_cast<BlockExprAst>(body)) {
    return generate_block_expr(block);
  }
  if (auto const call = dyn_cast<CallExprAst>(body)) {
    return generate_call_expr(call);
  }
  if (auto const ife = dyn_cast<IfExprAst>(body)) {
    return generate_if_expr(ife);
  }
  if (auto const let = dyn_cast<LetStmtAst>(body)) {
    return generate_let_stmt(let);
  }
  if (auto const num = dyn_cast<IntegerLiteralExpr>(body)) {
    return generate_integer_literal(num);
  }
  if (auto const var = dyn_cast<VarRefExprAst>(body)) {
    return generate_var_ref(var);
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
    case BO::Lt:
      return pimpl->thebuilder.CreateICmpSLT(lhs, rhs);
    case BO::Gt:
      return pimpl->thebuilder.CreateICmpSGT(lhs, rhs);
  }
  llvm_unreachable("not implemented");
}

llvm::Value *
CodeGen::generate_block_expr(BlockExprAst *block) {
  pimpl->push_vartab();
  auto const func = pimpl->thebuilder.GetInsertBlock()->getParent();
  auto BB = llvm::BasicBlock::Create(pimpl->thectxt, "block", func);
  pimpl->thebuilder.CreateBr(BB);
  pimpl->thebuilder.SetInsertPoint(BB);

  std::vector<llvm::Value *> seq;
  for (size_t i = 0, len = block->size(); i < len; ++i) {
    auto const ast = block->get_nth_stmt(i);
    auto const val = generate_expr(ast);
    seq.push_back(val);
  }

  BB = pimpl->thebuilder.GetInsertBlock();
  pimpl->pop_vartab();
  return seq.back();
}

llvm::Value *
CodeGen::generate_call_expr(CallExprAst *call) {
  auto const fn = generate_expr(call->get_callee());

  std::vector<llvm::Value *> args;
  for (size_t i = 0, len = call->get_nargs(); i < len; ++i) {
    auto const expr = generate_expr(call->get_nth_arg(i));
    args.push_back(expr);
  }
  return pimpl->thebuilder.CreateCall(fn, args, "calltmp");
}

llvm::Value *
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
  pimpl->register_val(def->get_name(), fn);

  llvm::BasicBlock *BB = llvm::BasicBlock::Create(pimpl->thectxt, "entry", fn);
  pimpl->thebuilder.SetInsertPoint(BB);

  pimpl->push_vartab();
  size_t i = 0;
  for (auto AI = fn->arg_begin(); i < arity; ++i, ++AI) {
    auto const name = def->get_nth_name(i);
    AI->setName(name);
    pimpl->register_val(name, AI);
  }

  auto const val = generate_expr(def->get_body());
  pimpl->thebuilder.CreateRet(val);

  pimpl->pop_vartab();
  llvm::verifyFunction(*fn);

  return fn;
}


llvm::Value *
CodeGen::generate_if_expr(IfExprAst *ife) {
  auto const zero = pimpl->thebuilder.getInt1(false);
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

llvm::Value *
CodeGen::generate_let_stmt(LetStmtAst *let) {
  auto const val = generate_expr(let->get_init());

  auto const name = let->get_var_name();
  auto const alloca = pimpl->register_auto_var(name, val);
  pimpl->thebuilder.CreateStore(val, alloca);

  return alloca;
}

llvm::Value *
CodeGen::generate_var_ref(VarRefExprAst *var) {
  auto const name = var->get_name();
  auto const val = pimpl->lookup_vartab(name);
  if (auto const alloc = llvm::dyn_cast<llvm::AllocaInst>(val)) {
    return pimpl->thebuilder.CreateLoad(alloc, name);
  }
  return val;
}
