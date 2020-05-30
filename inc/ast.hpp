#ifndef AST_HPP
#define AST_HPP

class Ast {
  public:
    enum class AK {
      DefFn,
      BinaryExpr,
      CallExpr,
      IntegerLiteral,
      IfStmt,
      LetStmt,
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

enum class BO {
  Plus,
  Minus,
  Mult,
  Div,
};

class BinaryExprAst : public Ast {
  public:
    BinaryExprAst(BO binop, Ast *left, Ast *right): Ast(AK::BinaryExpr), op(binop), lhs(left), rhs(right) {}
    static bool classof(Ast const *a) {
      return a->get_kind() == AK::BinaryExpr;
    }
    BO get_op() const {
      return op;
    }
    Ast *get_lhs() {
      return lhs;
    }
    Ast *get_rhs() {
      return rhs;
    }
  private:
    BO op;
    Ast *lhs;
    Ast *rhs;
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

class IfStmt : public Ast {
  public:
    IfStmt(Ast *c, Ast *th, Ast *el):
      Ast(AK::IfStmt), cond(c), then(th), els(el)
    {
    }
    static bool classof(Ast const *a) {
      return a->get_kind() == AK::IfStmt;
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

class LetStmt : public Ast {
  public:
    LetStmt(std::string const &n, Ast *r): Ast(AK::LetStmt), name(n), rhs(r) {
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

#endif /* !AST_HPP */
