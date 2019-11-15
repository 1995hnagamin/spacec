#include <fstream>
#include <string>
#include <vector>
#include <exception>
#include "lexer.hpp"

BadGetterException::BadGetterException(std::string const & cause):
  std::domain_error(cause)
{}

Token::Token(TokenType type, std::string const & token_literal):
  token_type(type),
  literal(token_literal)
{}

TokenType Token::type() const {
  return token_type;
}

std::string Token::representation() const {
  return literal;
}

std::string Token::get_as_name() const {
  switch (token_type) {
    case TokenType::SmallName:
    case TokenType::CapitalName:
      return literal;
    default:
      throw BadGetterException("Not a name");
  }
}

int Token::get_as_integer() const {
  switch (token_type) {
    case TokenType::Digit:
      return std::stoi(literal);
    default:
      throw BadGetterException("Not an integer");
  }
}

TokenStream::TokenStream(const std::vector<Token>& tokens):
  stream(tokens)
{}



struct char_stream {
  explicit char_stream() {}
  bool is_valid() {
    return index <= cur_line.length();
  }
  bool open(std::string const &filename) {
    ifs.open(filename);
    if (!ifs) {
      return false;
    }
    std::getline(ifs, cur_line);
    index = 0;
    return true;
  }
  char get_next_token() {
    if (index < cur_line.length()) {
      return cur_line[index++];
    }
    if (!ifs) {
      return '\0';
    }
    std::getline(ifs, cur_line);
    index = 0;
    return cur_line[0];
  }
  std::ifstream ifs;
  std::string cur_line;
  size_t index;
};
