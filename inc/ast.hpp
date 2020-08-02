#ifndef AST_HPP
#define AST_HPP

class Ast {
  public:
    enum class AK {
      TranslationUnit,
      DefFn,
      BinaryExpr,
      BlockExpr,
      BoolLiteral,
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

class ExprAst : public Ast {
  public:
    ExprAst() = delete;
    ExprAst(ExprAst const &) = delete;
    ExprAst(ExprAst &&) = delete;
    ExprAst &operator=(ExprAst const &) = delete;
    ExprAst &operator=(ExprAst &&) = delete;
    virtual ~ExprAst() = 0;

    explicit ExprAst(AK kind): Ast(kind) {
    }

    static bool classof(Ast const *a) {
      auto constexpr lb = static_cast<int>(AK::BinaryExpr);
      auto constexpr ub = static_cast<int>(AK::VarRefExpr);

      auto const num = static_cast<int>(a->get_kind());
      return lb <= num && num <= ub;
    }

    void set_type(Type *);
    Type *get_type() const;

  private:
    Type *type;
};

class BinOp;

class BinaryExprAst : public ExprAst {
  public:
    BinaryExprAst(BinOp *binop, Ast *left, Ast *right):
      ExprAst(AK::BinaryExpr), op(binop), lhs(left), rhs(right) {}
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

class BlockExprAst : public ExprAst {
  public:
    BlockExprAst(std::vector<Ast *> const &children):
      ExprAst(AK::BlockExpr), stmts(children) {}
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

class BoolLiteralExprAst : public ExprAst {
  public:
    BoolLiteralExprAst(bool v): ExprAst(AK::BoolLiteral), value(v) {
    }
    static bool classof(Ast const *a) {
      return a->get_kind() == AK::BoolLiteral;
    }
    bool get_value() const {
      return value;
    }
  private:
    bool value;
};

class CallExprAst : public ExprAst {
  public:
    CallExprAst(Ast *func, std::vector<Ast *> const &arglist):
      ExprAst(AK::CallExpr), callee(func), args(arglist) {}
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

class IntegerLiteralExpr : public ExprAst {
  public:
    IntegerLiteralExpr(int v):
      ExprAst(AK::IntegerLiteral), val(v) {
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

class IfExprAst : public ExprAst {
  public:
    IfExprAst(Ast *c, Ast *th, Ast *el):
      ExprAst(AK::IfExpr), cond(c), then(th), els(el)
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

class LetStmtAst : public ExprAst {
  public:
    LetStmtAst(std::string const &n, Ast *r):
      ExprAst(AK::LetStmt), name(n), rhs(r) {
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

class TranslationUnitAst : public Ast {
  public:
    TranslationUnitAst(std::vector<Ast *> const &fn): Ast(AK::TranslationUnit), funcs(fn) {
    }
    static bool classof(Ast const *a) {
      return a->get_kind() == AK::TranslationUnit;
    }
    size_t size() const {
      return funcs.size();
    }
    Ast *get_nth_func(size_t n) {
      return funcs[n];
    }
  private:
    std::vector<Ast *> funcs;
};

class VarRefExprAst : public ExprAst {
  public:
    VarRefExprAst(std::string const &n):
      ExprAst(AK::VarRefExpr), name(n) {
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
