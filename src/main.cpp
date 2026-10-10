#include <iostream>
#include <string>
#include <unordered_set>

bool is_command(const std::string& command);
std::unordered_set<std::string> command_list({"exit"});

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
  return command_list.count(command);
};