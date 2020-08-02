#include <string>
#include <vector>
#include "ast.hpp"

Ast::~Ast() {
}

ExprAst::~ExprAst() {
}

void
ExprAst::set_type(Type *ty) {
    type = ty;
}

Type *
ExprAst::get_type() const {
    return type;
}
