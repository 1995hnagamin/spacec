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
#include "type.hpp"
#include <array>

class CodeGenImpl {
public:
  CodeGenImpl(llvm::LLVMContext &ctxt, llvm::Module &mod, llvm::IRBuilder<> &builder):
      thectxt(ctxt), themod(mod), thebuilder(builder) {
  }

  llvm::LLVMContext &thectxt;
  llvm::Module &themod;
  llvm::IRBuilder<> &thebuilder;

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
  auto const alloca = entry.CreateAlloca(val->getType(), 0, name);
  register_val(name, alloca);

  return alloca;
}

CodeGen::CodeGen(llvm::LLVMContext &ctxt, llvm::Module &mod, llvm::IRBuilder<> &builder):
    pimpl(new CodeGenImpl(ctxt, mod, builder)) {
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

static llvm::Type *
generate_llvm_type(CodeGenImpl *pimpl, Type *type) {
  using llvm::dyn_cast;
  if (auto const boolty = dyn_cast<BoolType>(type)) {
    return llvm::IntegerType::getInt1Ty(pimpl->thectxt);
  }
  if (auto const intty = dyn_cast<IntNType>(type)) {
    return llvm::IntegerType::get(pimpl->thectxt, intty->get_width());
  }
  llvm_unreachable("not implemented");
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
  if (auto const bl = dyn_cast<BoolLiteralExprAst>(body)) {
    return generate_bool_literal(bl);
  }
  if (auto const call = dyn_cast<CallExprAst>(body)) {
    return generate_call_expr(call);
  }
  if (auto const decl = dyn_cast<DeclStmtAst>(body)) {
    return generate_decl_stmt(decl);
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
  if (auto const oseq = dyn_cast<OctetSeqLiteralAst>(body)) {
    return generate_octet_seq_literal(oseq);
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
    case BO::Eq:
      return pimpl->thebuilder.CreateICmpEQ(lhs, rhs);
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
CodeGen::generate_bool_literal(BoolLiteralExprAst *bl) {
  auto const type = llvm::Type::getInt1Ty(pimpl->thectxt);
  return llvm::ConstantInt::get(type, bl->get_value());
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
CodeGen::generate_decl_stmt(DeclStmtAst *decl) {
  auto const fnt = llvm::cast<FunctionType>(decl->get_type());

  std::vector<llvm::Type *> llparams;
  for (size_t i = 0, arity = fnt->get_arity(); i < arity; ++i) {
    auto const llt = generate_llvm_type(pimpl, fnt->get_nth_param(i));
    llparams.push_back(llt);
  }
  auto const llfnt = llvm::FunctionType::get(
    generate_llvm_type(pimpl, fnt->get_return_type()), llparams, false /* not variadic */);
  auto const fn = llvm::Function::Create(
    llfnt, llvm::Function::ExternalLinkage, llvm::Twine(decl->get_var_name()), pimpl->themod);
  pimpl->register_val(decl->get_var_name(), fn);
  return llvm::UndefValue::get(llvm::Type::getVoidTy(pimpl->thectxt));
}

llvm::Value *
CodeGen::generate_function_definition(DefFnAst *def) {
  auto const arity = def->get_arity();
  std::vector<llvm::Type *> param_types;
  for (size_t i = 0; i < arity; ++i) {
    auto const type = generate_llvm_type(pimpl, def->get_nth_type(i));
    param_types.push_back(type);
  }
  llvm::FunctionType *fn_type = llvm::FunctionType::get(
    generate_llvm_type(pimpl, def->get_return_type()), param_types, false /* not variadic */);
  llvm::Function *fn = llvm::Function::Create(
    fn_type, llvm::Function::ExternalLinkage, llvm::Twine(def->get_name()), pimpl->themod);
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
  auto const cond = generate_expr(ife->get_cond());

  auto const func = pimpl->thebuilder.GetInsertBlock()->getParent();

  auto thenBB = llvm::BasicBlock::Create(pimpl->thectxt, "then", func);
  auto elseBB = llvm::BasicBlock::Create(pimpl->thectxt, "else");
  auto const mergeBB = llvm::BasicBlock::Create(pimpl->thectxt, "ifcont");
  pimpl->thebuilder.CreateCondBr(cond, thenBB, elseBB);

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
  auto const type = generate_llvm_type(pimpl, ife->get_type());
  auto phi = pimpl->thebuilder.CreatePHI(type, 2, "iftmp");

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

static llvm::Constant *
create_global_octet_seq_ptr(CodeGenImpl *pimpl, std::string const &data) {
  // See llvm::IRBuilderBase::CreateGlobalStringPtr()
  auto const strval = llvm::ConstantDataArray::getString(pimpl->thectxt, data, false /* no \0 */);
  auto global = new llvm::GlobalVariable(
    pimpl->themod,
    strval->getType(),
    true /* is constant */,
    llvm::GlobalVariable::PrivateLinkage,
    strval,
    "oseq");
  global->setUnnamedAddr(llvm::GlobalVariable::UnnamedAddr::Global);
  global->setAlignment(llvm::Align(1));

  auto const zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(pimpl->thectxt), 0);
  std::array<llvm::Constant *, 2> indices{
    zero, // &strval : [N x i8]*
    zero, // &strval[0] : i8*
  };
  return llvm::ConstantExpr::getInBoundsGetElementPtr(global->getValueType(), global, indices);
}

llvm::Value *
CodeGen::generate_octet_seq_literal(OctetSeqLiteralAst *oseq) {
  auto const content = oseq->get_content();
  auto const pai8 = create_global_octet_seq_ptr(pimpl, content);
  llvm::ArrayRef<llvm::Type *> slice_member_type{
    pai8->getType(),
    llvm::Type::getInt32Ty(pimpl->thectxt) // FIXME
  };
  auto const slice_t = llvm::StructType::create(pimpl->thectxt, slice_member_type, "oseq_t");

  llvm::ArrayRef<llvm::Constant *> members{
    pai8, llvm::ConstantInt::get(llvm::Type::getInt32Ty(pimpl->thectxt), content.size())};
  auto const val = llvm::ConstantStruct::get(slice_t, members);

  return val;
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
