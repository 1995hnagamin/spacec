#ifndef LEXER_HPP
#define LEXER_HPP

enum class TokenType {
  SmallName,
  CapitalName,
  Digit,
  LParen,
  RParen,
  LBracket,
  RBracket,
  LBrace,
  RBrace,
  Comma,
  Symbol,
  Eof
};

class BadGetterException : std::domain_error {
  public:
    BadGetterException(std::string const & cause);
};

class Token {
  public:
    Token(TokenType type, std::string const & literal);
    TokenType type() const;
    std::string representation() const;
    std::string get_as_name() const;
    int get_as_integer() const;
  private:
    TokenType token_type;
    std::string literal;
};

class TokenStream {
  public:
    TokenStream(std::vector<Token> const & tokens);
    Token *get();
    Token *seek();
    void expect(TokenType);
    void expect(TokenType, char const *);
  private:
    std::vector<Token> stream;
    size_t idx;
};

std::vector<Token>
LexicalAnalysis(std::string const &filename);

#endif /* !LEXER_HPP */
