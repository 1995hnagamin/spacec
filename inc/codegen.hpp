#ifndef CODEGEN_HPP
#define CODEGEN_HPP

class Ast;
class DefFnAst;
class IntegerLiteralExpr;

class CodeGenImpl;

class CodeGen {
  public:
    CodeGen(std::string const &module_id);
    ~CodeGen();
    bool execute(Ast *translation_unit);

    llvm::Value *generate_expr(Ast *);
    void generate_function_definition(DefFnAst *);
    llvm::Value *generate_integer_literal(IntegerLiteralExpr *);

  private:
    CodeGenImpl *pimpl;
};

#endif /* !CODEGEN_HPP */
