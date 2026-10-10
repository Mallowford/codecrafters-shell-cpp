#include <iostream>
#include <string>

bool is_command(const std::string& command);

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  std::string command;

  do {
    std::cout << "$ ";
    std::cin >> command;

    if (!is_command(command)) {
      std::cout << command << ": command not found\n";
    };
  } while (command != "exit");
};


bool is_command(const std::string& command) {
  if (command == "exit") {
    return true;
  };

  return false;
};