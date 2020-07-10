#include <cassert>
#include <stack>
#include <string>
#include <vector>

#include "llvm/Support/ErrorHandling.h"

#include "ast.hpp"
#include "binop.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "type.hpp"

Ast *
Parser::parse_top_level_decl() {
  return parse_deffn_decl();
}

Ast *
Parser::parse_deffn_decl() {
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
  assert(tok->type() == TokenType::RParen);

  tokens.expect(TokenType::Symbol, "->");
  Type *retty = parse_type();

  tokens.expect(TokenType::LBrace);
  Ast *body = parse_expr();
  tokens.expect(TokenType::RBrace);
  return new DefFnAst(name, params, retty, types, body);
}

std::vector<Ast *>
Parser::parse_stmt_seq() {
  std::vector<Ast *> seq;
  auto const hd = parse_stmt();
  seq.push_back(hd);

  while (tokens.seek()->type() == TokenType::Semicolon) {
    tokens.expect(TokenType::Semicolon);
    auto const stmt = parse_stmt();
    seq.push_back(stmt);
  }
  return seq;
}

Ast *
Parser::parse_stmt() {
  auto const repr = tokens.seek()->representation();
  if (repr == "Let") {
    return parse_let_stmt();
  }
  return parse_expr();
}

Ast *
Parser::parse_expr() {
  return parse_binary_expr_seq();
}

static bool
is_binop_token(Token* tok) {
  return tok->type() == TokenType::Symbol;
}

static BinOp *
get_binop(Token *tok) {
  auto const repr = tok->representation();
  if (repr == "+") {
    return new BasicBinOp(BO::Plus);
  } else if (repr == "-") {
    return new BasicBinOp(BO::Minus);
  } else if (repr == "*") {
    return new BasicBinOp(BO::Mult);
  } else if (repr == "/") {
    return new BasicBinOp(BO::Div);
  }
  llvm_unreachable("operator not implemented");
}

Ast *
Parser::parse_binary_expr_seq() {
  std::stack<Ast *> outstk;
  std::stack<BinOp *> opstk;
  auto const hd = parse_primary_expr();
  outstk.push(hd);
  while (is_binop_token(tokens.seek())) {
    auto const op = get_binop(tokens.get());
    while (!opstk.empty()) {
      auto const t = opstk.top();
      if (!comparable(*t, *op)) {
        std::abort();
      }
      if (op->higher_than(*t) || (op->same(*t) && op->is_right())) {
        break;
      }
      opstk.pop(); // == t
      auto const rhs = outstk.top(); outstk.pop();
      auto const lhs = outstk.top(); outstk.pop();
      outstk.push(t->create(lhs, rhs));
    }
    opstk.push(op);

    auto const next = parse_primary_expr();
    outstk.push(next);
  }
  while (!opstk.empty()) {
    auto const op = opstk.top(); opstk.pop();
    auto const rhs = outstk.top(); outstk.pop();
    auto const lhs = outstk.top(); outstk.pop();
    outstk.push(op->create(lhs, rhs));
  }
  assert(outstk.size() == 1);
  return outstk.top();
}

Ast *
Parser::parse_block_expr() {
  tokens.expect(TokenType::LBrace);
  auto const stmts = parse_stmt_seq();
  tokens.expect(TokenType::RBrace);
  return new BlockExprAst(stmts);
}

Ast *
Parser::parse_primary_expr() {
  auto const tok = tokens.seek();
  switch (tok->type()) {
    case TokenType::Digit:
      return parse_integer_literal();
    case TokenType::LParen:
      {
        tokens.expect(TokenType::LParen);
        auto const expr = parse_expr();
        tokens.expect(TokenType::RParen);
        return expr;
      }
    case TokenType::LBrace:
      return parse_block_expr();
    case TokenType::CapitalName:
      {
        auto const head = tok->representation();
        if (head == "If") {
          return parse_if_expr();
        }
      }
    case TokenType::SmallName:
      return parse_ident_expr();
    default:
      llvm_unreachable("not implemented");
  }
}

Ast *
Parser::parse_integer_literal() {
  auto const tok = tokens.get();
  assert(tok->type() == TokenType::Digit);
  return new IntegerLiteralExpr(std::stoi(tok->representation()));
}

Ast *
Parser::parse_ident_expr() {
  auto const tok = tokens.get();
  auto const var = new VarRefExprAst(tok->representation());
  return var;
}

Ast *
Parser::parse_if_expr() {
  tokens.expect(TokenType::CapitalName, "If");
  auto const cond = parse_expr();

  tokens.expect(TokenType::CapitalName, "Then");
  auto const then = parse_expr();

  tokens.expect(TokenType::CapitalName, "Else");
  auto const els = parse_expr();

  return new IfExprAst(cond, then, els);
}

Ast *
Parser::parse_let_stmt() {
  tokens.expect(TokenType::CapitalName, "Let");
  auto const nametok = tokens.expect(TokenType::SmallName);
  tokens.expect(TokenType::Symbol, "=");
  auto const rhs = parse_integer_literal();
  return new LetStmtAst(nametok->representation(), rhs);
}

Type *
Parser::parse_type() {
  tokens.expect(TokenType::SmallName, "i32");
  return new IntNType(32);
}
