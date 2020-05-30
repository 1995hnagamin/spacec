#ifndef PARSER_HPP
#define PARSER_HPP

class Parser {
  public:
    Parser(TokenStream const &stream): tokens(stream) {}
    Ast *parse_top_level_decl();
    Ast *parse_deffn_decl();
    Ast *parse_expr();
    Ast *parse_integer_literal();
    Ast *parse_if_stmt();
    Ast *parse_let_stmt();
    Type *parse_type();

  private:
    TokenStream tokens;
};

#endif /* !PARSER_HPP */
