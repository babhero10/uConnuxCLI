#ifndef ARGS_OPT_HPP
#define ARGS_OPT_HPP

#include <string>
#include <vector>

namespace opt_parser {

enum class ArgumentOptions {
  NO_ARG = 0, // No argument.
  REQ_ARG,    // Requred argument.
  OPT_ARG     // Optional argument.
};

const int OK = 0;
const int INVALID_OPTIONS = -1;
const int MISSING_ARGUMENT = -2;

class Option {
private:
  char letter;
  std::string name;
  ArgumentOptions arg_option;
  std::string arg;
  bool found;

public:
  Option(char letter, ArgumentOptions arg_option);
  Option(char letter, std::string name, ArgumentOptions arg_option);
  void set_arg(std::string arg);
  void set_found(bool found);
  char get_letter() const;
  std::string get_name() const;
  ArgumentOptions get_arg_option() const;
  std::string get_arg() const;
  bool get_found() const;
};

class OptionsParser {
private:
  std::vector<Option> options;

public:
  OptionsParser();
  void addOption(char letter, std::string name, ArgumentOptions arg_option);
  void addOption(char letter, ArgumentOptions arg_option);
  Option *findOption(char letter);
  Option *findOption(std::string name);
  int parseOptionsString(std::vector<std::string> const &args);
  const std::vector<Option> &getOptions() const;
};
}; // namespace opt_parser

#endif
