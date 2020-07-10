#ifndef AST_HPP
#define AST_HPP

class Ast {
  public:
    enum class AK {
      DefFn,
      BinaryExpr,
      BlockExpr,
      CallExpr,
      IntegerLiteral,
      IfExpr,
      LetStmt,
      VarRefExpr,
    };

  public:
    Ast(AK k): kind(k) {
    }
    virtual ~Ast() = 0;
    AK get_kind() const {
      return kind;
    }

  private:
    AK const kind;
};

class Type;

class DefFnAst : public Ast {
  public:
    DefFnAst(std::string const &fnname,
        std::vector<std::string> const &paramlist,
        Type *return_type,
        std::vector<Type *> const &typelist,
        Ast *fnbody):
      Ast(AK::DefFn), name(fnname), params(paramlist),
      ret(return_type), ptypes(typelist), body(fnbody)
    {
    }
    static bool classof(Ast const *a) {
      return a->get_kind() == AK::DefFn;
    }
    std::string get_name() const {
      return name;
    }
    size_t get_arity() const {
      return params.size();
    }
    std::string get_nth_name(size_t n) const {
      return params[n];
    }
    Type *get_nth_type(size_t n) const {
      return ptypes[n];
    }
    Type *get_return_type() {
      return ret;
    }
    Ast *get_body() {
      return body;
    }


  private:
    std::string name;
    std::vector<std::string> params;
    Type *ret;
    std::vector<Type *> ptypes;
    Ast *body;
};

class BinOp;

class BinaryExprAst : public Ast {
  public:
    BinaryExprAst(BinOp *binop, Ast *left, Ast *right): Ast(AK::BinaryExpr), op(binop), lhs(left), rhs(right) {}
    static bool classof(Ast const *a) {
      return a->get_kind() == AK::BinaryExpr;
    }
    BinOp *get_op() const {
      return op;
    }
    Ast *get_lhs() {
      return lhs;
    }
    Ast *get_rhs() {
      return rhs;
    }
  private:
    BinOp *op;
    Ast *lhs;
    Ast *rhs;
};

class BlockExprAst : public Ast {
  public:
    BlockExprAst(std::vector<Ast *> const &children): Ast(AK::BlockExpr), stmts(children) {}
    static bool classof(Ast const *a) {
      return a->get_kind() == AK::BlockExpr;
    }
    size_t size() {
      return stmts.size();
    }
    Ast *get_nth_stmt(size_t n) {
      return stmts[n];
    }
  private:
    std::vector<Ast *> stmts;
};

class CallExprAst : public Ast {
  public:
    CallExprAst(Ast *func, std::vector<Ast *> const &arglist): Ast(AK::CallExpr), callee(func), args(arglist) {}
    static bool classof(Ast const *a) {
      return a->get_kind() == AK::CallExpr;
    }
    Ast *get_callee() {
      return callee;
    }
    size_t get_nargs() const {
      return args.size();
    }
    Ast *get_nth_arg(size_t n) {
      return args[n];
    }
  private:
    Ast *callee;
    std::vector<Ast *> args;
};

class IntegerLiteralExpr : public Ast {
  public:
    IntegerLiteralExpr(int v): Ast(AK::IntegerLiteral), val(v) {
    }
    static bool classof(Ast const *a) {
      return a->get_kind() == AK::IntegerLiteral;
    }
    int get_value() const {
      return val;
    }

  private:
    int val;
};

class IfExprAst : public Ast {
  public:
    IfExprAst(Ast *c, Ast *th, Ast *el):
      Ast(AK::IfExpr), cond(c), then(th), els(el)
    {
    }
    static bool classof(Ast const *a) {
      return a->get_kind() == AK::IfExpr;
    }
    Ast *get_cond() {
      return cond;
    }
    Ast *get_then() {
      return then;
    }
    Ast *get_else() {
      return els;
    }
  private:
    Ast *cond;
    Ast *then;
    Ast *els;
};

class LetStmtAst : public Ast {
  public:
    LetStmtAst(std::string const &n, Ast *r): Ast(AK::LetStmt), name(n), rhs(r) {
    }
    static bool classof(Ast const *a) {
      return a->get_kind() == AK::LetStmt;
    }
    std::string get_var_name() const {
      return name;
    }
    Ast *get_init() {
      return rhs;
    }
  private:
    std::string name;
    Ast *rhs;
};

class VarRefExprAst : public Ast {
  public:
    VarRefExprAst(std::string const &n): Ast(AK::VarRefExpr), name(n) {
    }
    static bool classof(Ast const *a) {
      return a->get_kind() == AK::VarRefExpr;
    }
    std::string get_name() const {
      return name;
    }
  private:
    std::string name;
};

#endif /* !AST_HPP */
