#ifndef CODEGEN_HPP
#define CODEGEN_HPP

class Ast;
class BinaryExprAst;
class DefFnAst;
class IntegerLiteralExpr;

class CodeGenImpl;

class CodeGen {
public:
  CodeGen(llvm::LLVMContext &ctxt, llvm::Module &mod, llvm::IRBuilder<> &builder);
  ~CodeGen();
  bool execute(Ast *translation_unit);

  llvm::Value *generate_expr(Ast *);
  llvm::Value *generate_binary_expr(BinaryExprAst *);
  llvm::Value *generate_block_expr(BlockExprAst *);
  llvm::Value *generate_bool_literal(BoolLiteralExprAst *);
  llvm::Value *generate_call_expr(CallExprAst *);
  llvm::Value *generate_function_definition(DefFnAst *);
  llvm::Value *generate_if_expr(IfExprAst *);
  llvm::Value *generate_integer_literal(IntegerLiteralExpr *);
  llvm::Value *generate_let_stmt(LetStmtAst *);
  llvm::Value *generate_var_ref(VarRefExprAst *);

private:
  CodeGenImpl *pimpl;
};

#endif /* !CODEGEN_HPP */
