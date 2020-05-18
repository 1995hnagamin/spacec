#include "lexer.hpp"
#include "parser.hpp"

Ast *
Parser::parse_top_level_decl() {
  Token *tok = tokens.get();
  if (!tok || tok->get_as_name() != "DefFn") {
    return nullptr;
  }
  tok = tokens.get();
  auto const name = tok->representation();
  tokens.get(); // `(`
  std::vector<std::string> params;
  std::vector<Ast *> types;
  for (tok = tokens.get(); tok && tok->representation() != ")"; tok = tokens.get()) {
    params.push_back(tok.get_as_name());
    tokens.get(); // `:`
    Ast *ty = parse_type();
    if (!ty) {
      return nullptr;
    }
    types.push_back(ty);
  }
  return new DefFnAst(name, params, types, body);
}
