// Libraries
#include <iostream>
#include <string>
#include <unordered_set>
#include <filesystem>
#include <vector>
// #include <cstdlib>

// DEFINES
#ifdef _WIN32
  constexpr char PATH_LIST_SEPARATOR = ';';
  constexpr char PATH_DIRECTORY_KEYWORD = '\\';
#else
  constexpr char PATH_LIST_SEPARATOR = ':';
  constexpr char PATH_DIRECTORY_KEYWORD = '/';
#endif
#define SHELL_CHARACTER "$";

// Alias
namespace fs = std::filesystem;

// Global
std::unordered_set<std::string> command_list({"exit", "echo", "type", "pwd", "cd"});
fs::path CURRENT_PATH(fs::current_path());

// Function Stub
bool is_executable(fs::perms p);
bool isBuiltIn(const std::string& command);
std::string find_command(const std::string& command_name);

// Main

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  std::string command;
  std::string parameters;


  do {
    std::cout << SHELL_CHARACTER;
    std::cout << " ";
    std::getline(std::cin, command);
    
    if (command.find(' ') != std::string::npos) {
      parameters = command.substr(command.find(' ') + 1);
      command = command.substr(0, command.find(' '));
    };

    if (!isBuiltIn(command)) {
      std::string custom_command = find_command(command);
      if (custom_command.size() > 0) {
        command.append(" " + parameters);
        std::system(command.c_str());
      }
      else {
        std::cout << command << ": command not found\n";
      };
    };

    if (command == "echo") {
      // Break Parameters
      std::vector<std::string> brokenParams;

      for (int i = 0; i < parameters.size(); i++) {
        if (parameters.at(i) == '\'') {
          int j = i + 1;
          while (j < parameters.size() && parameters.at(j) != '\'') {
            j++;
          };
          i++;
          std::string tempadder = parameters.substr(i, j - i);
          if ((j + 1) < parameters.size() && parameters.at(j + 1) == ' ') {
            tempadder.push_back(' ');
          };
          brokenParams.emplace_back(tempadder);
          i = j;
        }
        else if (parameters.at(i) == '"') {
          int j = i + 1;
          while (j < parameters.size() && parameters.at(j) != '"') {
            j++;
          };
          i++;
          std::string tempadder = parameters.substr(i, j - i);
          if ((j + 1) < parameters.size() && parameters.at(j + 1) == ' ') {
            tempadder.push_back(' ');
          };
          brokenParams.emplace_back(tempadder);
          i = j;
        }
        else if (parameters.at(i) == ' ') {
          continue;
        }
        else if (parameters.at(i) == '\\') {
          if (i + 1 < parameters.size()) {
            std::string tempadder = "";
            tempadder.push_back(parameters.at(i + 1));
            brokenParams.emplace_back(tempadder);
          };
          i++;
        }
        else {
          int j = i;
          while (j < parameters.size() && parameters.at(j) != ' ' && parameters.at(j) != '\'' && parameters.at(j) != '"') {
            j++;
          };

          std::string tempadder = parameters.substr(i, j - i);
          if (j < parameters.size() && parameters.at(j) == ' ') {
            tempadder.push_back(' ');
          };
          brokenParams.emplace_back(tempadder);
          i = j;
        };
      };

      for (int i = 0; i < brokenParams.size(); i++) {
        std::cout << brokenParams.at(i);
      };

      std::cout << std::endl;
    };
    if (command == "type") {
      // Built-Ins
      if (isBuiltIn(parameters)) {
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
    if (command == "pwd") {
      std::string pathAsString = CURRENT_PATH; 
      std::cout << pathAsString << "\n";
    };

    if (command == "cd") {
      fs::path PATH = parameters;
      
      if (fs::exists(PATH) && (parameters.find("..") == std::string::npos && parameters.find("./") && std::string::npos && parameters.find("~") == std::string::npos)) {
        CURRENT_PATH = PATH;
        goto GONEXT;
      };

      PATH = CURRENT_PATH;
      std::string res;
      std::string cleanedParameters = parameters;

      do {
        char keyword = cleanedParameters.at(0);
        if (keyword == '~') {
          std::string conversion = std::getenv("HOME");
          PATH = conversion;
          cleanedParameters = cleanedParameters.substr(1);
        }
        else if (keyword == '.') {
          // Parent Directory
          if (cleanedParameters.substr(0, 3) == "../" || cleanedParameters.substr(0, 2) == "..") {
            fs::path HOME = std::getenv("HOME");
            if (PATH != HOME.parent_path()) {              
              PATH = PATH.parent_path();
            };

            if (cleanedParameters.substr(0, 3) == "../") {
              cleanedParameters = cleanedParameters.substr(3);
            }
            else {
              cleanedParameters = cleanedParameters.substr(2);
            };
          }
          // Check if current directory (can only be ./ or something within)
          else if (cleanedParameters.substr(0, 2) == "./") {
            cleanedParameters = cleanedParameters.substr(2);
          };
        }
        else if (keyword == PATH_DIRECTORY_KEYWORD) {
          cleanedParameters = cleanedParameters.substr(1);
          fs::path tempCheck = CURRENT_PATH / cleanedParameters;
          if (fs::exists(tempCheck)) {
            CURRENT_PATH = tempCheck;
            goto GONEXT;
          };
        }
        else {
          if (cleanedParameters.find(PATH_DIRECTORY_KEYWORD) != std::string::npos) {
            PATH = PATH / cleanedParameters.substr(0, cleanedParameters.find(PATH_DIRECTORY_KEYWORD));
            cleanedParameters = cleanedParameters.substr(cleanedParameters.find(PATH_DIRECTORY_KEYWORD) + 1);
          }
          else {
            PATH = PATH / cleanedParameters;
            cleanedParameters = "";
          };
        };
      } while (cleanedParameters.size() != 0);

      if (!fs::exists(PATH)) {
        std::cout << command << ": " << parameters << ": No such file or directory\n";
        goto GONEXT;
      };

      res = PATH;
      char lastCharacter = res.at(res.size() - 1);
      while (lastCharacter == '.' || lastCharacter == '/' || lastCharacter == '\\') {
        res.erase(res.size() - 1);
        lastCharacter = res.at(res.size() - 1);
      };
      CURRENT_PATH = res;
    };

    GONEXT:

    std::cin.clear();

  } while (command != "exit");
};


bool isBuiltIn(const std::string& command) {
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

        if (!is_executable(p)) {
          continue;
        };

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