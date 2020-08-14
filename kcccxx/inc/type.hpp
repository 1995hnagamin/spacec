#ifndef TYPE_HPP
#define TYPE_HPP

#include <vector>

class Type {
public:
  enum class TK {
    Bool,
    Function,
    IntN,
    Unit,
    Slice,
    TyVar,
  };

public:
  Type(TK k): kind(k) {
  }
  virtual ~Type() = 0;
  TK get_kind() const {
    return kind;
  }
  virtual bool equal(Type *lhs) const {
    return get_kind() == lhs->get_kind();
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
  bool equal(Type *) const override;

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

class FunctionType : public Type {
public:
  FunctionType(Type *retty, std::vector<Type *> paramlist):
      Type(TK::Function), ret(retty), params(paramlist) {
  }
  static bool classof(Type const *t) {
    return t->get_kind() == TK::Function;
  }
  bool equal(Type *) const override;
  size_t get_arity() const {
    return params.size();
  }
  Type *get_return_type() const {
    return ret;
  }
  Type *get_nth_param(size_t i) const {
    return params[i];
  }

private:
  Type *ret;
  std::vector<Type *> params;
};

class UnitType : public Type {
public:
  UnitType(): Type(TK::Unit) {
  }
  static bool classof(Type const *t) {
    return t->get_kind() == TK::Unit;
  }
};

class SliceType : public Type {
public:
  explicit SliceType(Type *el): Type(TK::Slice), elem(el) {
  }
  static bool classof(Type const *t) {
    return t->get_kind() == TK::Slice;
  }
  Type *get_elem_type() const {
    return elem;
  }

private:
  Type *elem;
};

class TyVar : public Type {
public:
  TyVar(size_t type_id): Type(TK::TyVar), id(type_id) {
  }
  static bool classof(Type const *t) {
    return t->get_kind() == TK::TyVar;
  }
  bool equal(Type *) const override;

private:
  size_t id;
};

#endif /* !TYPE_HPP */
