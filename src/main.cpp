// Libraries
#include <iostream>
#include <string>
#include <unordered_set>
#include <filesystem>
#include <vector>

// DEFINES
#define DEBUG false
#ifdef _WIN32
  constexpr char PATH_LIST_SEPARATOR = ';';
#else
  constexpr char PATH_LIST_SEPARATOR = ':';
#endif

// Alias
namespace fs = std::filesystem;

// Global
std::unordered_set<std::string> command_list({"exit", "echo", "type"});

// Function Stub
bool is_executable(fs::perms p);
bool is_command(const std::string& command);
std::string find_command(const std::string& command_name);

// Main

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
      // Built-Ins
      if (parameters == "echo" || parameters == "exit" || parameters == "type") {
        std::cout << parameters << " is a shell builtin\n";
        goto GONEXT;
      };

      std::string filepath = find_command(parameters);
      if(filepath.size() != 0) {
        std::cout << parameters << " is " << filepath << "\n";
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
std::string find_command(const std::string& command_name) {
  std::string PATH = std::getenv("PATH");
  std::vector<fs::path> paths;
  int i = 0;
  while (i < PATH.size()) {
    int j = i;
    while (PATH.at(j) != PATH_LIST_SEPARATOR && j != PATH.size() - 1) {
      j++;
    };

    paths.emplace_back(PATH.substr(i, j - i));
    i = j + 1;
  };

  for (const auto& i : paths) {
    if (fs::exists(i) && fs::is_directory(i)) {
      for (const auto& file : fs::directory_iterator(i)) {
        // If entry is a directory, move on
        if (file.is_directory()) {
          continue;
        };
        // If entry's name does not match what we're looking for, move on
        if (file.path().filename() != command_name) {
          continue;
        };

        // Check permissions of file if it can execute
        bool has_perms = false;
        fs::perms p = fs::status(file).permissions();

        if (is_executable(p));

        return file.path().string();
      };
    };
  };

  return "";
};
bool is_executable(fs::perms p) {
  bool owner = ((p & fs::perms::owner_exec) != fs::perms::none);
  bool group = ((p & fs::perms::group_exec) != fs::perms::none);
  bool others = ((p & fs::perms::others_exec) != fs::perms::none);

  return (owner || group || others);
};