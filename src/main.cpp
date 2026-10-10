#include <iostream>
#include <string>
#include <unordered_set>

#define DEBUG false

bool is_command(const std::string& command);
std::unordered_set<std::string> command_list({"exit", "echo", "type"});

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  std::string command;
  std::string parameters;

  do {
    std::cout << "$ ";
    std::getline(std::cin, command);
    for (int i = 0; i < command.size(); i++) {
      if (command.find(' ') != std::string::npos) {
        parameters = command.substr(command.find(' ') + 1);
        command = command.substr(0, command.find(' '));
      };
    };

    if (DEBUG) {
      std::cout << "TEST_COMMAND: " << command << "\n";
      std::cout << "TEST_PARAMETERS: " << parameters << "\n";
    };


    if (!is_command(command)) {
      std::cout << command << ": command not found\n";
      goto GONEXT;
    };

    if (command == "echo") {
      std::cout << parameters << std::endl;
    };
    if (command == "type") {
      if (parameters == "echo" || parameters == "exit" || parameters == "type") {
        std::cout << parameters << " is a shell builtin\n";
      }
      else if(is_command(parameters)) {
        std::cout << "Not specified\n";
      }
      else {
        std::cout << parameters << ": not found\n";
      };
    };

    GONEXT:

    std::cin.clear();

  } while (command != "exit");
};


bool is_command(const std::string& command) {
  return command_list.count(command);
};