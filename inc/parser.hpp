#ifndef PARSER_HPP
#define PARSER_HPP

class Parser {
  public:
    Parser(TokenStream const &stream): tokens(stream) {}
    Ast *parse_top_level_decl();

  private:
    TokenStream tokens;
};

#endif /* !PARSER_HPP */
