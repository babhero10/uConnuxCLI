#include "../include/args_opt.hpp"
#include <string>

namespace opt_parser {

/** Option class  **/
Option::Option(char letter, ArgumentOptions arg_option)
    : letter(letter), name(""), arg_option(arg_option), arg(""), found(false) {}

Option::Option(char letter, std::string name, ArgumentOptions arg_option)
    : letter(letter), name(name), arg_option(arg_option), arg(""),
      found(false) {}

void Option::set_found(bool found) { this->found = found; }

void Option::set_arg(std::string arg) { this->arg = arg; }

char Option::get_letter() const { return letter; }

std::string Option::get_name() const { return name; }

ArgumentOptions Option::get_arg_option() const { return arg_option; }

std::string Option::get_arg() const { return arg; }

bool Option::get_found() const { return found; }

/** OptionParser class  **/
OptionsParser::OptionsParser() {}

Option *OptionsParser::findOption(char letter) {
  for (Option &opt : options) {
    if (opt.get_letter() == letter) {
      return &opt;
    }
  }
  return nullptr;
}

Option *OptionsParser::findOption(std::string name) {
  for (Option &opt : options) {
    if (opt.get_name() == name) {
      return &opt;
    }
  }
  return nullptr;
}

void OptionsParser::addOption(char letter, std::string name,
                              ArgumentOptions arg_option) {
  Option opt(letter, name, arg_option);
  options.push_back(opt);
}

void OptionsParser::addOption(char letter, ArgumentOptions arg_option) {
  Option opt(letter, arg_option);
  options.push_back(opt);
}

const std::vector<Option> &OptionsParser::getOptions() const { return options; }

int OptionsParser::parseOptionsString(std::vector<std::string> const &args) {
  size_t i;
  int last_index = 0;

  for (Option &option : options) {
    option.set_arg("");
    option.set_found(false);
  }

  for (i = 1; i < args.size(); i++) {
    if (args[i].rfind("--", 0) == 0) {

      if (args[i].length() <= 3) {
        return INVALID_OPTIONS;
      }

      std::string name = args[i].substr(2);
      Option *option = findOption(name);
      if (option == nullptr) {
        return INVALID_OPTIONS;
      } else {
        option->set_found(true);
        if (option->get_arg_option() == ArgumentOptions::REQ_ARG) {
          if (i + 1 == args.size()) {
            return MISSING_ARGUMENT;
          }
          option->set_arg(args[++i]);
        }
      }
    } else if (args[i].rfind("-", 0) == 0) {

      if (args[i].length() != 2) {
        return INVALID_OPTIONS;
      }

      Option *option = findOption(args[i][1]);
      if (option == nullptr) {
        return INVALID_OPTIONS;
      } else {
        option->set_found(true);
        if (option->get_arg_option() == ArgumentOptions::REQ_ARG) {
          if (i + 1 == args.size()) {
            return MISSING_ARGUMENT;
          }
          option->set_arg(args[++i]);
        }
      }
    } else {
      break;
    }

    last_index++;
  }

  return last_index;
}

} // namespace opt_parser
