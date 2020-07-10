#ifndef CODEGEN_HPP
#define CODEGEN_HPP

class Ast;
class BinaryExprAst;
class DefFnAst;
class IntegerLiteralExpr;

class CodeGenImpl;

class CodeGen {
  public:
    CodeGen(std::string const &module_id);
    ~CodeGen();
    bool execute(Ast *translation_unit);

    llvm::Value *generate_expr(Ast *);
    llvm::Value *generate_binary_expr(BinaryExprAst *);
    llvm::Value *generate_block_expr(BlockExprAst *);
    void generate_function_definition(DefFnAst *);
    llvm::Value *generate_if_expr(IfExprAst *);
    llvm::Value *generate_integer_literal(IntegerLiteralExpr *);
    llvm::Value *generate_let_stmt(LetStmtAst *);

  private:
    CodeGenImpl *pimpl;
};

#endif /* !CODEGEN_HPP */
