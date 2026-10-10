#include <iostream>
#include <string>

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  do {
    std::cout << "$ ";
    std::string command;
    std::cin >> command;

    if (command.size()) {
      std::cout << command << ": command not found\n";
    };
  } while (true);
};
