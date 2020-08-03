#ifndef PARSER_HPP
#define PARSER_HPP

class Parser {
public:
  Parser(TokenStream const &stream): tokens(stream) {
  }
  TranslationUnitAst *parse_top_level_decl();
  Ast *parse_deffn_decl();

  std::vector<Ast *> parse_stmt_seq();
  Ast *parse_stmt();

  Ast *parse_expr();
  Ast *parse_binary_expr_seq();
  Ast *parse_block_expr();
  Ast *parse_primary_expr();
  Ast *parse_integer_literal();
  Ast *parse_ident_expr();
  Ast *parse_if_expr();
  Ast *parse_let_stmt();
  Type *parse_type();

private:
  TokenStream tokens;
};

#endif /* !PARSER_HPP */
