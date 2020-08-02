#ifndef TYPECHECKER_HPP
#define TYPECHECKER_HPP

class DefFnAst;
class IfExprAst;
class IntegerLiteral;
class LetStmtAst;
class TranslationUnitAst;

class Type;
class TypeCheckerImpl;

class TypeChecker {
  public:
    explicit TypeChecker();
    ~TypeChecker();
    void traverse_tunit(TranslationUnitAst *);
    Type *traverse_decl(Ast *);
    Type *traverse_deffn(DefFnAst *);

    Type *traverse_expr(Ast *);
    Type *traverse_binary_expr(BinaryExprAst *);
    Type *traverse_block_expr(BlockExprAst *);
    Type *traverse_call_expr(CallExprAst *);
    Type *traverse_if_expr(IfExprAst *);
    Type *traverse_integer_literal(IntegerLiteralExpr *);
    Type *traverse_let_stmt(LetStmtAst *);
    Type *traverse_var_ref(VarRefExprAst *);
  private:
    std::unique_ptr<TypeCheckerImpl> pimpl;
};

#endif /* !TYPECHECKER_HPP */
