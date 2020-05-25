#include <iostream>
#include <vector>
#include "ast.hpp"
#include "lexer.hpp"
#include "parser.hpp"

void show_help() {
  std::cout << "usage: spccc <filename>" << std::endl;
}

void show_tokens(std::vector<Token> const &tokens) {
  for (auto &&token : tokens) {
    std::cout << token.representation() << std::endl;
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
  parser.parse_let_stmt();
  return 0;
}
