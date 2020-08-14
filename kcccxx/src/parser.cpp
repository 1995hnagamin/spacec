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

TranslationUnitAst *
Parser::parse_top_level_decl() {
  std::vector<Ast *> funcs;
  while (tokens.seek()->type() != TokenType::Eof) {
    auto const fn = parse_deffn_decl();
    funcs.push_back(fn);
  }
  return new TranslationUnitAst(funcs);
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

  Ast *body = parse_block_expr();
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
  if (repr == "Decl") {
    return parse_decl_stmt();
  }
  return parse_expr();
}

Ast *
Parser::parse_expr() {
  return parse_binary_expr_seq();
}

static bool
is_binop_token(Token *tok) {
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
  } else if (repr == "=") {
    return new BasicBinOp(BO::Eq);
  } else if (repr == "<") {
    return new BasicBinOp(BO::Lt);
  } else if (repr == ">") {
    return new BasicBinOp(BO::Gt);
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
      auto const rhs = outstk.top();
      outstk.pop();
      auto const lhs = outstk.top();
      outstk.pop();
      outstk.push(t->create(lhs, rhs));
    }
    opstk.push(op);

    auto const next = parse_primary_expr();
    outstk.push(next);
  }
  while (!opstk.empty()) {
    auto const op = opstk.top();
    opstk.pop();
    auto const rhs = outstk.top();
    outstk.pop();
    auto const lhs = outstk.top();
    outstk.pop();
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
Parser::parse_decl_stmt() {
  tokens.expect(TokenType::CapitalName, "Decl");
  auto const nametok = tokens.expect(TokenType::SmallName);
  tokens.expect(TokenType::Symbol, ":");
  auto const ty = parse_type();
  return new DeclStmtAst(nametok->representation(), ty);
}

Ast *
Parser::parse_primary_expr() {
  auto const tok = tokens.seek();
  switch (tok->type()) {
    case TokenType::Digit:
      return parse_integer_literal();
    case TokenType::LParen: {
      tokens.expect(TokenType::LParen);
      auto const expr = parse_expr();
      tokens.expect(TokenType::RParen);
      return expr;
    }
    case TokenType::LBrace:
      return parse_block_expr();
    case TokenType::CapitalName: {
      auto const head = tok->representation();
      if (head == "False") {
        tokens.advance();
        return new BoolLiteralExprAst(false);
      }
      if (head == "If") {
        return parse_if_expr();
      }
      if (head == "Oc") {
        return parse_octet_seq_literal();
      }
      if (head == "True") {
        tokens.advance();
        return new BoolLiteralExprAst(true);
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
  if (tokens.seek()->type() != TokenType::LParen) {
    return var;
  }

  // function call
  tokens.expect(TokenType::LParen);
  std::vector<Ast *> args;
  while (tokens.seek()->type() != TokenType::RParen) {
    if (args.size() > 0) {
      tokens.expect(TokenType::Comma);
    }
    auto const arg = parse_expr();
    args.push_back(arg);
  }
  tokens.expect(TokenType::RParen);
  return new CallExprAst(var, args);
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
  auto const rhs = parse_expr();
  return new LetStmtAst(nametok->representation(), rhs);
}

static std::string
read_octet_seq_literal(std::string const &repr) {
  return repr.substr(1, repr.size() - 2);
}

Ast *
Parser::parse_octet_seq_literal() {
  tokens.expect(TokenType::CapitalName, "Oc");
  auto const literal = tokens.expect(TokenType::DoubleQuoted);
  auto const content = read_octet_seq_literal(literal->representation());
  return new OctetSeqLiteralAst(content);
}

Type *
Parser::parse_type() {
  auto const tok = tokens.seek();
  switch (tok->type()) {
    case TokenType::SmallName: {
      tokens.expect(TokenType::SmallName, "i32");
      return new IntNType(32);
    }
    case TokenType::CapitalName: {
      auto const head = tok->representation();
      if (head == "Bool") {
        return new BoolType;
      }
      if (head == "Fr") {
        return parse_fn_type();
      }
    }
    default:
      llvm_unreachable("not implemented");
  }
}

Type *
Parser::parse_fn_type() {
  tokens.expect(TokenType::CapitalName, "Fr");

  tokens.expect(TokenType::LParen);
  std::vector<Type *> types;
  auto tok = tokens.seek();
  while (tok->type() != TokenType::RParen) {
    auto const ty = parse_type();
    types.push_back(ty);
    tok = tokens.get();
    if (tok->type() == TokenType::Comma) {
      tok = tokens.get();
    }
  }
  tokens.expect(TokenType::Symbol, "->");
  auto const retty = parse_type();
  return new FunctionType(retty, types);
}
