#include <iostream>
#include <vector>

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

#include "ast.hpp"
#include "codegen.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "typechecker.hpp"

void
show_help(std::string const &exec) {
  std::cout << "usage: " << exec << " <filename>" << std::endl;
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

void
show_tokens(std::vector<Token> const &tokens) {
  for (auto &&token : tokens) {
    std::cerr << string_of_tokentype(token.type()) << ": " << token.representation() << std::endl;
  }
}

llvm::Expected<llvm::Target const *>
lookup_target(std::string const &target_triple) {
  std::string error_message_buffer;
  auto const target = llvm::TargetRegistry::lookupTarget(target_triple, error_message_buffer);
  if (!target) {
    return llvm::make_error<llvm::StringError>(
      error_message_buffer, std::make_error_code(std::errc::not_supported));
  }
  return target;
}

llvm::Expected<std::unique_ptr<llvm::raw_fd_ostream>>
create_raw_fd_stream(llvm::StringRef filename, llvm::sys::fs::OpenFlags flags) {
  std::error_code ec;
  auto stream = std::make_unique<llvm::raw_fd_ostream>(filename, ec, flags);
  if (ec) {
    return llvm::errorCodeToError(ec);
  }
  return stream;
}

int
main(int argc, char **argv) {
  if (argc < 2) {
    show_help(argv[0]);
    return 0;
  }
  auto const filename = static_cast<std::string>(argv[1]);

  auto const tokens = LexicalAnalysis(filename);
  Parser parser(tokens);
  auto const tunit = parser.parse_top_level_decl();

  TypeChecker tc;
  tc.traverse_tunit(tunit);

  llvm::LLVMContext ctxt;
  llvm::Module mod(filename, ctxt);
  llvm::IRBuilder<> builder(ctxt);

  CodeGen codegen(ctxt, mod, builder);
  codegen.execute(tunit);
  mod.print(llvm::outs(), nullptr);

  return 0;
}
