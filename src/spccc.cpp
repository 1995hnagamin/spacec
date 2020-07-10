#include <iostream>
#include <vector>

#include "llvm/IR/Value.h"

#include "ast.hpp"
#include "codegen.hpp"
#include "lexer.hpp"
#include "parser.hpp"

void show_help() {
  std::cout << "usage: spccc <filename>" << std::endl;
}

char const *
string_of_tokentype(TokenType t) {
  switch (t) {
    case TokenType::SmallName:
      return "sma";
    case TokenType::CapitalName:
      return "cap";
    case TokenType::Digit:
      return "dig";
    case TokenType::Symbol:
      return "sym";
    default:
      return "!#$";
  }
}

void show_tokens(std::vector<Token> const &tokens) {
  for (auto &&token : tokens) {
    std::cerr << string_of_tokentype(token.type()) << ": "
      << token.representation()
      << std::endl;
  }
}

int main(int argc, char **argv) {
  if (argc < 2) {
    show_help();
    return 0;
  }

  auto const tokens = LexicalAnalysis(argv[1]);
  show_tokens(tokens);
  Parser parser(tokens);
  auto const tunit = parser.parse_top_level_decl();

  CodeGen codegen("null");
  codegen.execute(tunit);

  return 0;
}
