#include <iostream>

void show_help() {
  std::cout << "usage: spccc <filename>" << std::endl;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    show_help();
    return 0;
  }
  return 0;
}
