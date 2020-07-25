#ifndef TYPE_HPP
#define TYPE_HPP

class Type {
  public:
    enum class TK {
      Bool,
      IntN,
      Unit,
      TyVar,
    };

  public:
    Type(TK k): kind(k) {
    }
    virtual ~Type() = 0;
    TK get_kind() const {
      return kind;
    }

  private:
    TK const kind;
};

class IntNType : public Type {
  public:
    IntNType(int w): Type(TK::IntN), width(w) {
    }
    static bool classof(Type const *t) {
      return t->get_kind() == TK::IntN;
    }
    int get_width() const {
      return width;
    }

  private:
    int const width;
};

class BoolType : public Type {
  public:
    BoolType(): Type(TK::Bool) {
    }
    static bool classof(Type const *t) {
      return t->get_kind() == TK::Bool;
    }
};

class UnitType : public Type {
  public:
    UnitType(): Type(TK::Unit) {
    }
    static bool classof(Type const *t) {
      return t->get_kind() == TK::Unit;
    }
};

class TyVar : public Type {
  public:
    TyVar(size_t type_id): Type(TK::TyVar), id(type_id) {
    }
    static bool classof(Type const *t) {
      return t->get_kind() == TK::TyVar;
    }
  private:
    size_t id;
};

#endif /* !TYPE_HPP */
