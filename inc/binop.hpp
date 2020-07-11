#ifndef BINOP_HPP
#define BINOP_HPP

class Ast;

enum class BO {
  Plus,
  Minus,
  Mult,
  Div,
  Lt,
  Gt,
};

class BinOp {
  public:
    BinOp(BO op): kind(op) {
    }
    virtual ~BinOp() = 0;
    BO get_kind() const {
      return kind;
    }
    virtual Ast *create(Ast *lhs, Ast *rhs) = 0;
    bool higher_than(BinOp const &other) const;
    bool same(BinOp const &other) const;

    bool is_left() const;
    bool is_right() const {
      return !is_left();
    }

  private:
    BO const kind;
};

class BasicBinOp : public BinOp {
  public:
    BasicBinOp(BO op): BinOp(op) {
    }
    static bool classof(BinOp const *a) {
      auto const op = a->get_kind();
      return op == BO::Plus
        || op == BO::Minus
        || op == BO::Mult
        || op == BO::Div
        || op == BO::Lt
        || op == BO::Gt;
    }
    Ast *create(Ast *, Ast *) override;
};

bool
comparable(BinOp const &lhs, BinOp const &rhs);

#endif /* !BINOP_HPP */
