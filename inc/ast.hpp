#ifndef AST_HPP
#define AST_HPP

class Ast {
  public:
    enum class AK {
      DefFn,
      BinaryExpr,
      CallExpr
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

class DefFnAst : public Ast {
  public:
    DefFnAst(std::string const &fnname, std::vector<std::string> const &paramlist, std::vector<Type> const &typelist, Ast *fnbody):
      name(fnname), params(paramlist), ptypes(typelist), body(fnbody)
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
    Ast *get_nth_type(size_t n) const {
      return ptypes[n];
    }
    Ast *get_body() {
      return body;
    }


  private:
    std::string name;
    std::vector<std::string> params;
    std::vector<Ast *> ptypes;
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

#endif /* !AST_HPP */
