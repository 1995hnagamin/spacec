#include <string>
#include <vector>
#include "ast.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "type.hpp"

Ast *
Parser::parse_top_level_decl() {
  tokens.expect(TokenType::CapitalName, "DefFn");

  auto tok = tokens.get();
  auto const name = tok->representation();

  tokens.expect(TokenType::LParen);
  std::vector<std::string> params;
  std::vector<Type *> types;
  tok = tokens.get();
  while (tok->type() == TokenType::SmallName) {
    params.push_back(tok->get_as_name());
    tokens.expect(TokenType::Symbol, ":");
    Type *ty = parse_type();
    types.push_back(ty);
    tok = tokens.get();
    if (tok->type() == TokenType::Comma) {
      tok = tokens.get();
    }
  }
  tokens.expect(TokenType::RParen);

  tokens.expect(TokenType::LBrace);
  if (tokens.seek()->representation() == "Let") {

  }
  tokens.expect(TokenType::RBrace);
  // return new DefFnAst(name, params, types, body);
  return nullptr;
}

Ast *
Parser::parse_expr() {
  return parse_if_stmt();
}

Ast *
Parser::parse_integer_literal() {
  auto const tok = tokens.get();
  assert(tok->type() == TokenType::Digit);
  return new IntegerLiteralExpr(std::stoi(tok->representation()));
}

Ast *
Parser::parse_if_stmt() {
  tokens.expect(TokenType::CapitalName, "If");
  tokens.expect(TokenType::LParen);
  auto const cond = parse_integer_literal();
  tokens.expect(TokenType::RParen);

  tokens.expect(TokenType::LBrace);
  auto const then = parse_integer_literal();
  tokens.expect(TokenType::RBrace);

  tokens.expect(TokenType::CapitalName, "Else");
  tokens.expect(TokenType::LBrace);
  auto const els = parse_integer_literal();
  tokens.expect(TokenType::RBrace);

  return new IfStmt(cond, then, els);
}

Ast *
Parser::parse_let_stmt() {
  tokens.expect(TokenType::CapitalName, "Let");
  auto const nametok = tokens.expect(TokenType::SmallName);
  tokens.expect(TokenType::Symbol, "=");
  auto const rhs = parse_integer_literal();
  return new LetStmt(nametok->representation(), rhs);
}

Type *
Parser::parse_type() {
  tokens.expect(TokenType::SmallName, "i32");
  return new IntNType(32);
}
